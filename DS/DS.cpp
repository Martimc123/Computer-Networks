
#include "ds_const.h"

int main(int argc, char **argv)
{
    n_group = 0;
    ParseArgs(argc, argv);
    printf("%s\n", DSport);

    TCP_Server();
    UDP_Server();

    select();
    // printf("select\n");
    freeaddrinfo(res);
    close(UDP_fd);
    freeaddrinfo(resTCPserver);
    close(TCP_fd);
}

void ParseArgs(int argc, char *argv[])
{
    int option;
    int index = 0;
    while ((option = getopt(argc, argv, "p:v:")) != -1)
    {

        switch (option)
        {
        case 'p':
            index += 2;
            strcpy(DSport, argv[index]);
            break;
        case 'v':
            index += 2;
            verbose_mode = 1;
            break;
        default:
            break;
        }
    }

    if (strlen(DSport) == 0)
        strcpy(DSport, PORT);
}

void UDP_Server()
{

    UDP_fd = socket(AF_INET, SOCK_DGRAM, 0); /* UDP socket */
    if (UDP_fd == -1)                        /*error*/
        exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      /*IPv4*/
    hints.ai_socktype = SOCK_DGRAM; /* UDP socket */
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(NULL, PORT, &hints, &res);
    if (errcode != 0) /*error*/
        exit(1);

    n = bind(UDP_fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);
}

void TCP_Server()
{

    if ((TCP_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { /*error*/
        perror("ERR: in socket of Server TCP");
        exit(1);
    }

    memset(&hintsTCPserver, 0, sizeof(hintsTCPserver));

    hintsTCPserver.ai_family = AF_INET;
    hintsTCPserver.ai_socktype = SOCK_STREAM;
    hintsTCPserver.ai_flags = AI_PASSIVE;

    if ((errcode = getaddrinfo(NULL, PORT, &hintsTCPserver, &resTCPserver)) != 0)
    { /*error*/
        perror("ERR: in creating socket of Server TCP");
        exit(1);
    }
    if (bind(TCP_fd, resTCPserver->ai_addr, resTCPserver->ai_addrlen) == -1)
    { /*error*/
        perror("ERR: in binding socket of Server TCP");
        exit(1);
    }
    if (listen(TCP_fd, 5) == -1)
    { /*error*/
        perror("ERR: in listen Server TCP");
        exit(1);
    }
}

void select()
{
    int max_fd = 0, val = 0;
    while (1)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(UDP_fd, &rfds);
        FD_SET(TCP_fd, &rfds);
        max_fd = max(UDP_fd, TCP_fd);
        val = select(max_fd + 1, &rfds, NULL, NULL, &tv);
        if (val < 0)
        {
            perror("ERRO: erro select\n");
            exit(1);
        }
        else if (val == 0)
        {
            perror("ERRO: erro timeout\n");
            exit(1);
        }

        for (; val; val--)
        {
            if (FD_ISSET(UDP_fd, &rfds))
            {
                UDP_Server_Response();
            }
            if (FD_ISSET(TCP_fd, &rfds))
            {
                addrTCPserverlen = sizeof(addrTCPserver);
                newfdTCP = accept(TCP_fd, (struct sockaddr *)&addrTCPserver, &addrTCPserverlen);
                if (newfdTCP == -1)
                {
                    perror("Error in accepting client connection");
                    exit(1);
                }
                TCP_Server_Response();
            }
        }

        FD_CLR(UDP_fd, &rfds);
        FD_CLR(TCP_fd, &rfds);
    }
}
/*----------------------------------------UDP---------------------------------------------------------------*/
void UDP_Server_Sendto()
{

    n = sendto(UDP_fd, UDP_sendMSG, strlen(UDP_sendMSG), 0, (struct sockaddr *)&addr, addrlen);
    if (n == -1)
    {
        perror("ERROR:in UDP sendto\n");
        exit(1);
    }
}

void UDP_Server_Recvfrom()
{

    addrlen = sizeof(addr);
    n = recvfrom(UDP_fd, buffer, 128, 0, (struct sockaddr *)&addr, &addrlen);
    if (n == -1)
    {
        perror("ERROR: in UDP recvfrom\n");
        exit(1);
    }

    memcpy(UDP_recMSG, buffer, strlen(buffer));

    write(1, "received: ", 10);
    write(1, buffer, n);
}

void UDP_Server_Response()
{

    UDP_Server_Recvfrom();
    if (!strncmp(UDP_recMSG, "REG", 3))
    {
        UDP_Register_User();
    }
    else if (!strncmp(UDP_recMSG, "UNR", 3))
    {
        UDP_Unregister_User();
    }
    else if (!strncmp(UDP_recMSG, "LOG", 3))
    {
        UDP_Login_User();
    }
    else if (!strncmp(UDP_recMSG, "OUT", 3))
    {
        UDP_Logout_User();
    }
    else if (!strncmp(UDP_recMSG, "GLS", 3))
    {
        UDP_List_Groups();
    }
    else if (!strncmp(UDP_recMSG, "GSR", 3))
    {
        UDP_Subscribe_Groups();
    }
    else if (!strncmp(UDP_recMSG, "GUR", 3))
    {
        UDP_Unsubscribe_Groups();
    }
    else if (strncmp(UDP_recMSG, "GLM", 3) == 0)
    {
        UDP_List_My_Groups();
    }
}

/*------------------------------TCP----------------------------------------------------------*/

void TCP_Server_Response()
{
    ssize_t nread;
    nread = read(newfdTCP, TCP_recMSG, 14);
    if (nread == -1)
    { // error
        perror("ERROR: in server TCP read");
        exit(1);
    }
    TCP_recMSG[nread] = '\0';

    if (!strncmp(TCP_recMSG, "ULS", 3))
    {
        TCP_List_User_Subscribed();
    }
    else if (!strncmp(TCP_recMSG, "PST", 3))
    {
        TCP_Post();
    }
    else if (!strncmp(TCP_recMSG, "RTV", 3))
    {
        TCP_Retrieve();
    }
}
/*-------------------------------CMD-USERS---------------------------------------------------------------------*/
void UDP_Register_User()
{
    sscanf(&UDP_recMSG[4], "%s %s", received_UID, received_password);
    if (is_ok_UID(received_UID) && is_ok_password(received_password))
    {
        if (has_user_registered(received_UID) == -1)
        {
            create_dir_and_file_user(received_UID, received_password);
            if (verbose_mode == 1)
            {
                cout << "UID=" << received_UID << ": new user" << endl;
            }
            strcpy(UDP_sendMSG, "RRG OK\n");
        }
        else
        {
            perror("ERRO: já existe\n");
            strcpy(UDP_sendMSG, "RRG DUP\n");
        }
    }
    else
    {
        perror("ERRO: uid pass\n");
        strcpy(UDP_sendMSG, "RRG NOK\n");
    }
    UDP_Server_Sendto();
}

void create_dir_and_file_user(char *received_UID, char *received_password)
{
    chdir("USERS");
    int len = 5 + 1 + 4 + 4 + 5 + 1;
    char name_userPass[len];
    sprintf(name_userPass, "%s/%s_pass.txt", received_UID, received_UID);
    int check = mkdir(received_UID, 0777);
    if (!check)
    {
        printf("Folder created\n");
        FILE *fp = fopen(name_userPass, "w+");
        fwrite(received_password, sizeof(received_password), 1, fp);
        fclose(fp);
    }
    else
    {
        printf("Unable to create folder\n");
        exit(1);
    }
    chdir("..");
}

int has_user_registered(char *UID)
{
    int id = -1;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("USERS")) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (!strcmp(ent->d_name, UID))
            {
                id = 1;
            }
        }
    }
    else
    {
        perror("ERROR: do not have directory USERS\n");
    }
    return id;
}

bool is_ok_UID(char *UID)
{
    bool is_ok = false;
    if (strlen(UID) == 5)
    {
        for (int i = 0; i < 5; i++)
        {
            if (UID[i] >= '0' && UID[i] <= '9')
            {
                is_ok = true;
            }
        }
    }
    return is_ok;
}

bool is_ok_password(char *pass)
{
    bool is_ok = false;
    if (strlen(pass) == 8)
    {
        for (int i = 0; i < 8; i++)
        {
            if ((pass[i] >= '0' && pass[i] <= '9') || isalpha(pass[i]))
            {
                is_ok = true;
            }
        }
    }
    return is_ok;
}

void UDP_Unregister_User()
{
    sscanf(&UDP_recMSG[4], "%s %s", received_UID, received_password);
    int id = has_user_registered(received_UID);
    if (id != -1)
    {
        if (check_password(received_UID, received_password))
        {
            remove_dir(received_UID, received_password);
            strcpy(UDP_sendMSG, "RUN OK\n");
        }
        else
        {
            perror("ERRO: pass not good\n");
            strcpy(UDP_sendMSG, "RUN NOK\n");
        }
    }
    else
    {
        perror("ERRO: not exist\n");
        strcpy(UDP_sendMSG, "RUN NOK\n");
    }
    UDP_Server_Sendto();
}

bool check_password(char *received_UID, char *received_password)
{
    bool check = false;
    char name[26];
    sprintf(name, "USERS/%s/%s_pass.txt", received_UID, received_UID);
    FILE *fp = fopen(name, "r");
    if (fp)
    {
        char pass[8];
        fscanf(fp, "%s", pass);
        if (!strcmp(pass, received_password))
            check = true;
    }
    fclose(fp);

    return check;
}

void remove_dir(char *received_UID, char *received_password)
{
    chdir("USERS");
    int len = 5;
    if (nftw(received_UID, rmFiles, 10, FTW_DEPTH | FTW_MOUNT | FTW_PHYS) < 0)
    {
        perror("ERROR: nftw");
        exit(1);
    }
    chdir("..");
}
static int rmFiles(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
    if (remove(pathname) < 0)
    {
        perror("ERROR: remove");
        return -1;
    }
    return 0;
}

void UDP_Login_User()
{
    sscanf(&UDP_recMSG[4], "%s %s", received_UID, received_password);
    int id = has_user_registered(received_UID);
    if (id != -1)
    {

        if (check_password(received_UID, received_password)) // check pass
        {
            create_file(received_UID);
            if (verbose_mode == 1)
            {
                cout << "UID=" << received_UID << ": login ok" << endl;
            }
            strcpy(UDP_sendMSG, "RLO OK\n");
        }
        else
        {
            perror("ERRO: pass not good\n");
            strcpy(UDP_sendMSG, "RLO NOK\n");
        }
    }
    else
    {
        perror("ERRO: not exist\n");
        strcpy(UDP_sendMSG, "RLO NOK\n");
    }
    UDP_Server_Sendto();
}

bool check_exist_login(char *UID)
{
    chdir("USERS");
    chdir(UID);
    bool check = false;
    char name[21];
    sprintf(name, "%s_login.txt", UID);

    if (access(name, F_OK) == 0)
    {
        check = true;
    }

    chdir("..");
    chdir("..");
    return check;
}

void create_file(char *received_UID)
{
    chdir("USERS");
    int len = 5 + 1 + 5 + 1 + 9;
    char name_userPass[len];
    sprintf(name_userPass, "%s/%s_login.txt", received_UID, received_UID);

    FILE *fp = fopen(name_userPass, "w+");
    fclose(fp);
    chdir("..");
}

void UDP_Logout_User()
{
    sscanf(&UDP_recMSG[4], "%s %s", received_UID, received_password);
    int id = has_user_registered(received_UID);
    if (id != -1)
    {
        if (check_password(received_UID, received_password)) // check pass
        {
            if (check_exist_login(received_UID) == true) // check login
            {
                delete_file(received_UID);
                strcpy(UDP_sendMSG, "ROU OK\n");
            }
            else
            {
                perror("ERRO: is not login user\n");
                strcpy(UDP_sendMSG, "ROU NOK\n");
            }
        }
        else
        {
            perror("ERRO: pass not good\n");
            strcpy(UDP_sendMSG, "ROU NOK\n");
        }
    }
    else
    {
        perror("ERRO: not exist\n");
        strcpy(UDP_sendMSG, "ROU NOK\n");
    }
    UDP_Server_Sendto();
}

void delete_file(char *received_UID)
{
    chdir("USERS");
    int len = 5 + 1 + 5 + 1 + 9;
    char name_userLog[len];
    sprintf(name_userLog, "%s/%s_login.txt", received_UID, received_UID);
    if (remove(name_userLog) == 0)
    {
        printf("The file is deleted successfully.");
    }
    else
    {
        printf("The file is not deleted.");
    }
    chdir("..");
}

/*-------------------------------CMD-GROUPS---------------------------------------------------------------------*/
void UDP_List_Groups()
{
    FILE *fp;
    char GIDname[30];
    int i = 0, j, ngr = 0, numMensagens;
    all_group *all_groups = (all_group *)malloc(sizeof(all_group) * 99);

    DIR *dir;
    struct dirent *ent;
    int count = 0;
    if ((dir = opendir("GROUPS")) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_name[0] == '.')
                continue;
            if (strlen(ent->d_name) > 2)
                continue;

            strcpy(all_groups[i].GID, ent->d_name);
            sprintf(GIDname, "GROUPS/%s/%s_name.txt", ent->d_name, ent->d_name);
            fp = fopen(GIDname, "r");
            if (fp)
            {
                fscanf(fp, "%24s", all_groups[i].Gname);
                fclose(fp);
            }

            chdir("GROUPS");
            DIR *dir2, *dir3;
            struct dirent *ent2;
            if ((dir2 = opendir(all_groups[i].GID)) != NULL)
            { // 01
                chdir(all_groups[i].GID);

                if ((dir3 = opendir("MSG")) != NULL)
                {
                    chdir("MSG");
                    numMensagens = 0;
                    while ((ent2 = readdir(dir3)) != NULL)
                    {
                        if (strcmp(ent2->d_name, ".") == 0 || strcmp(ent2->d_name, "..") == 0)
                            continue;

                        numMensagens++;
                        strcpy(all_groups[i].MID, ent2->d_name);
                    }
                    if (numMensagens == 0)
                    {
                        strcpy(all_groups[i].MID, "0000");
                    }
                    // printf("%d %s\n", numMensagens, all_groups[i].MID);
                    all_groups[i].n_mensagens = numMensagens;
                    chdir("..");
                }
                closedir(dir3);
                chdir("..");
            }
            closedir(dir2);
            chdir("..");
            ++i;
            if (i == 99)
                break;

            count++;
        }
    }
    else
    {
        perror("ERROR: do not have directory GROUP\n");
    }

    if (count == 0)
    {
        sprintf(UDP_sendMSG, "RGL %d", count);
    }
    else
    {
        strcat(UDP_sendMSG, "RGL");
        strcat(UDP_sendMSG, " ");
        char number[3];
        sprintf(number, "%d", count);
        strcat(UDP_sendMSG, number);
        for (j = 0; j < count; j++)
        {
            strcat(UDP_sendMSG, " ");
            strcat(UDP_sendMSG, all_groups[j].GID);
            strcat(UDP_sendMSG, " ");
            strcat(UDP_sendMSG, all_groups[j].Gname);
            strcat(UDP_sendMSG, " ");
            strcat(UDP_sendMSG, all_groups[j].MID);
        }
    }
    strcat(UDP_sendMSG, "\n");
    UDP_Server_Sendto();
}

void UDP_Subscribe_Groups()
{
    sscanf(&UDP_recMSG[4], "%s %s %s", received_UID, received_GID, received_Gname);
    if (check_exist_login(received_UID) == true)
    {
        if (is_ok_GID(received_GID))
        {
            if (is_ok_Gname(received_Gname))
            {
                if (strcmp(received_GID, "00"))
                { // subs
                    if (check_group_created(received_GID) && check_Gname(received_GID, received_Gname))
                    {

                        create_UID_file_group(received_UID, received_GID);
                        strcpy(UDP_sendMSG, "RGS OK\n");
                    }
                    else
                    {
                        strcpy(UDP_sendMSG, "RGS E_GNAME\n");
                    }
                }
                else if (!strcmp(received_GID, "00") && n_group < 99)
                { // create new group
                    char GID[3];

                    if (n_group < 9)
                    {
                        sprintf(GID, "0%d", n_group + 1);
                    }
                    else
                    {
                        sprintf(GID, "%d", n_group + 1);
                    }
                    create_dir_and_file_group(GID, received_Gname);
                    if (verbose_mode == 1)
                    {
                        cout << "UID=" << received_UID << ": new_group: " << received_GID << " - " << received_Gname << endl;
                    }
                    n_group++;
                    sprintf(UDP_sendMSG, "RGS NEW %s\n", GID);
                }
                else
                {
                    strcpy(UDP_sendMSG, "RGS E_FULL\n");
                }
            }
            else
            {
                strcpy(UDP_sendMSG, "RGS E_GNAME\n");
            }
        }
        else
        {
            strcpy(UDP_sendMSG, "RGS E_GRP\n");
        }
    }
    else
    {
        perror("ERRO: user not exist or not login\n");
        strcpy(UDP_sendMSG, "RGS E_USR\n");
    }
    UDP_Server_Sendto();
}

bool check_Gname(char *received_GID, char *received_Gname)
{
    bool check = false;

    char name[21];
    sprintf(name, "GROUPS/%s/%s_name.txt", received_GID, received_GID);

    FILE *fp = fopen(name, "r");
    char Gname[24];
    fscanf(fp, "%24s", Gname);
    if (!strcmp(Gname, received_Gname))
    {
        check = true;
    }
    fclose(fp);

    return check;
}

void create_UID_file_group(char *UID, char *GID)
{
    chdir("GROUPS");
    int len = 2 + 1 + 5 + 4;
    char name_uid_file[len];
    sprintf(name_uid_file, "%s/%s.txt", GID, UID);

    FILE *fp = fopen(name_uid_file, "w");
    fclose(fp);
    chdir("..");
}

void create_dir_and_file_group(char *GID, char *received_Gname)
{
    chdir("GROUPS");
    int len = 5 + 1 + 8 + 4 + 5 + 1;
    char name_groupPass[len];
    sprintf(name_groupPass, "%s/%s_name.txt", GID, GID);

    int check = mkdir(GID, 0777);
    if (!check)
    {
        FILE *fp = fopen(name_groupPass, "w+");
        fwrite(received_Gname, 24, 1, fp);
        fclose(fp);
    }
    else
    {
        printf("Unable to create folder\n");
        exit(1);
    }
    chdir(GID);
    int check1 = mkdir("MSG", 0777);
    if (!check1)
    {
        printf("Folder created\n");
    }
    else
    {
        printf("Unable to create folder\n");
        exit(1);
    }
    chdir("..");

    chdir("..");
}

bool check_group_created(char *GID)
{
    chdir("GROUPS");
    bool check = false;

    if (access(GID, X_OK) == 0)
    {
        check = true;
    }
    chdir("..");

    return check;
}

bool is_ok_GID(char *GID)
{
    bool is_ok = false;
    if (strlen(GID) == 2)
    {
        for (int i = 0; i < 2; i++)
        {
            if (GID[i] >= '0' && GID[i] <= '9')
            {
                is_ok = true;
            }
        }
    }
    return is_ok;
}
bool is_ok_Gname(char *Gname)
{
    bool is_ok = false;
    if (strlen(Gname) < 24)
    {
        for (int i = 0; i < 24; i++)
        {
            if (isalpha(Gname[i]) || (Gname[i] == '-') || (Gname[i] == '_'))
            {
                is_ok = true;
            }
        }
    }
    return is_ok;
}

void UDP_Unsubscribe_Groups()
{
    sscanf(&UDP_recMSG[4], "%s %s", received_UID, received_GID);
    if (check_exist_login(received_UID) == true)
    {
        if (check_group_created(received_GID))
        {
            if (check_User_subscribed_group(received_UID, received_GID))
            {
                delete_uid_file_in_GROUPS(received_UID, received_GID);
                strcpy(UDP_sendMSG, "RGU OK\n");
            }
            else
            {
                strcpy(UDP_sendMSG, "RGU NOK\n");
            }
        }
        else
        {
            strcpy(UDP_sendMSG, "RGU E_GRP\n");
        }
    }
    else
    {
        strcpy(UDP_sendMSG, "RGU E_USR\n");
    }
    UDP_Server_Sendto();
}
void delete_uid_file_in_GROUPS(char *received_UID, char *received_GID)
{
    chdir("USERS");
    int len = 3 + 5 + 4;
    char name_user[len];
    sprintf(name_user, "%s/%s.txt", received_GID, received_UID);
    if (remove(name_user) == 0)
    {
        printf("The file is deleted successfully.");
    }
    else
    {
        printf("The file is not deleted.\n");
    }
    chdir("..");
}

bool check_User_subscribed_group(char *received_UID, char *received_GID)
{
    chdir("GROUPS");
    bool check = false;
    char fname[12];
    sprintf(fname, "%s/%s.txt", received_GID, received_UID);

    if (access(fname, F_OK) == 0)
    {
        check = true;
    }
    chdir("..");
    return check;
}
void UDP_List_My_Groups()
{
    sscanf(&UDP_recMSG[4], "%s", received_UID);
    int i = 0, numGroupsSubs = 0, numMensagens, j;
    FILE *fp;
    char GIDname[30];
    all_group *all_groups = (all_group *)malloc(sizeof(all_group) * 99);

    if (is_ok_UID(received_UID) == true || check_exist_login(received_UID) == true)
    {
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir("GROUPS")) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                if (ent->d_name[0] == '.')
                    continue;
                if (strlen(ent->d_name) > 2)
                    continue;

                char Gname_temp[24];
                sprintf(GIDname, "GROUPS/%s/%s_name.txt", ent->d_name, ent->d_name);
                fp = fopen(GIDname, "r");
                if (fp)
                {
                    fscanf(fp, "%24s", Gname_temp);
                    fclose(fp);
                }

                chdir("GROUPS");
                DIR *dir2, *dir3;
                struct dirent *ent2, *aux;
                char GID_temp[3];
                strcpy(GID_temp, ent->d_name);
                if ((dir2 = opendir(ent->d_name)) != NULL)
                { // 01
                    chdir(ent->d_name);
                    while ((aux = readdir(dir2)) != NULL) // ler 01,msg
                    {
                        char *nome = new char[12];
                        sprintf(nome, "%s.txt", received_UID);

                        if (strcmp(aux->d_name, nome) == 0)
                        {
                            strcpy(all_groups[numGroupsSubs].GID, GID_temp);
                            strcpy(all_groups[numGroupsSubs].Gname, Gname_temp);

                            if ((dir3 = opendir("MSG")) != NULL)
                            {
                                chdir("MSG");
                                numMensagens = 0;
                                while ((ent2 = readdir(dir3)) != NULL)
                                {
                                    if (strcmp(ent2->d_name, ".") == 0 || strcmp(ent2->d_name, "..") == 0)
                                        continue;
                                    numMensagens++;
                                    strcpy(all_groups[numGroupsSubs].MID, ent2->d_name);
                                }
                                if (numMensagens == 0)
                                {
                                    strcpy(all_groups[numGroupsSubs].MID, "0000");
                                }
                                all_groups[numGroupsSubs].n_mensagens = numMensagens;

                                chdir("..");
                            }
                            closedir(dir3);
                            //}
                            numGroupsSubs++;
                        }
                        ++i;
                    }
                    chdir("..");
                }
                closedir(dir2);
                chdir("..");

                if (i == 99)
                    break;
            }
        }
        else
        {
            perror("ERROR: do not have directory GROUP\n");
        }
        if (numGroupsSubs == 0)
        {
            strcpy(UDP_sendMSG, "RGM 0\n");
        }
        else
        {
            sprintf(UDP_sendMSG, "RGL %d", numGroupsSubs);

            for (j = 0; j < numGroupsSubs; j++)
            {
                strcat(UDP_sendMSG, " ");
                strcat(UDP_sendMSG, all_groups[j].GID);
                strcat(UDP_sendMSG, " ");
                strcat(UDP_sendMSG, all_groups[j].Gname);
                strcat(UDP_sendMSG, " ");
                strcat(UDP_sendMSG, all_groups[j].MID);
            }
            strcat(UDP_sendMSG, "\n");
        }
    }
    else
    {
        strcpy(UDP_sendMSG, "RGM E_USR\n");
    }
    // printf("grupos sub: %d\n",numGroupsSubs);

    UDP_Server_Sendto();
}

/*--------------------------------------TCP-------------------------------*/

bool is_group_ok(char *GID)
{
    bool group_ok = false;
    chdir("GROUPS");

    if (access(GID, X_OK) == 0)
    {
        group_ok = true;
    }

    chdir("..");
    return group_ok;
}

void TCP_List_User_Subscribed()
{
    sscanf(&TCP_recMSG[4], "%s", received_GID);
    DIR *dir;
    FILE *fp;
    struct dirent *ent;
    char Gname_temp[24];
    char GIDname[40];
    all_user *user = (all_user *)malloc(sizeof(user) * 100000);
    int uid = 0;
    ssize_t nwrite, nleft;
    if (is_group_ok(received_GID) == true)
    {

        if ((dir = opendir("GROUPS")) != NULL)
        {
            sprintf(GIDname, "GROUPS/%s/%s_name.txt", received_GID, received_GID);
            fp = fopen(GIDname, "r");
            if (fp)
            {
                fscanf(fp, "%24s", Gname_temp);
                fclose(fp);
            }

            chdir("GROUPS");
            DIR *dir2;
            struct dirent *ent2;
            if ((dir2 = opendir(received_GID)) != NULL)
            {

                chdir(received_GID);
                while ((ent2 = readdir(dir2)) != NULL) // ler 01,msg
                {
                    if (strcmp(ent2->d_name, ".") == 0 || strcmp(ent2->d_name, "..") == 0)
                        continue;
                    char name[11];
                    sprintf(name, "%s_name.txt", received_GID);
                    if (!(!strcmp(ent2->d_name, "MSG") || !strcmp(ent2->d_name, name) || !strcmp(ent2->d_name, "core")))
                    {
                        strncpy(user[uid].UID, ent2->d_name, 5);
                        uid++;
                    }
                }
                chdir("..");
            }
            closedir(dir2);
            chdir("..");
        }
        else
        {
            perror("ERROR: do not have directory GROUP\n");
        }
    }
    else
    {
        strcmp(TCP_sendMSG, "RUL NOK\n");
    }
    int k;

    strcat(TCP_sendMSG, "RUL");
    strcat(TCP_sendMSG, " ");
    strcat(TCP_sendMSG, "OK");
    strcat(TCP_sendMSG, " ");
    strcat(TCP_sendMSG, Gname_temp);

    for (k = 0; k < uid; k++)
    {
        strcat(TCP_sendMSG, " ");
        strcat(TCP_sendMSG, user[k].UID);
    }
    strcat(TCP_sendMSG, "\n");

    TCP_write();
    close(newfdTCP);
}

void TCP_write()
{
    ssize_t messagelen = 0, nleft, nwritten;

    messagelen = strlen(TCP_sendMSG);
    nleft = messagelen;
    while (nleft > 0)
    {
        nwritten = write(newfdTCP, TCP_sendMSG, messagelen);
        if (nwritten == -1)
        { /*error*/
            perror("Erro in TCP write\n");
            exit(1);
        }
        nleft -= nwritten;
    }
}

char *save_message(char *received_UID, char *received_GID, int Tsize, char *text, char *Fname, int Fsize, int flag)
{
    DIR *d, *d2, *d3;
    struct dirent *ent, *ent2;
    ssize_t nread;
    char *MID_r;
    strcpy(MID_r, "");
    int n_mensagens = 0;

    if ((d = opendir("GROUPS")) != NULL)
    {
        chdir("GROUPS");
        if ((d2 = opendir(received_GID)) != NULL)
        {
            chdir(received_GID);
            if ((d3 = opendir("MSG")) != NULL)
            {
                while ((ent2 = readdir(d3)) != NULL)
                {
                    n_mensagens++;
                    ;
                }

                if (n_mensagens == 0)
                    strcpy(MID_r, "0000");
                else
                {
                    sprintf(MID_r, "%04d", n_mensagens);
                }

                chdir("MSG");

                mkdir(MID_r, 007);
                chdir(MID_r);

                FILE *fp = fopen("T E X T.txt", "w");
                fwrite(text, Tsize, 1, fp);
                fclose(fp);

                FILE *fp2 = fopen("A U T H O R.txt", "w");
                fwrite(received_UID, sizeof(received_UID), 1, fp2);
                fclose(fp2);

                if (flag == 1)
                {
                    FILE *fp3 = fopen(Fname, "r");
                    while (Fsize > 0)
                    {
                        nread = read(newfdTCP, TCP_recMSG, 1);
                        if (nread == -1) /*error*/
                            exit(1);

                        if (nread == 0)
                            break;

                        fwrite(TCP_recMSG, 1, nread, fp3);
                        Fsize -= nread;
                    }
                    fclose(fp3);
                }

                chdir("..");
                chdir("..");
            }
            closedir(d3);
            chdir("..");
        }
        closedir(d2);
        chdir("..");
    }
    closedir(d);
    return MID_r;
}

void TCP_Post()
{
    ssize_t nread, nwrite, nleft;
    char Tsize[3];
    char text[241];
    char Fname[30], Fsize[11];
    int numMensagens = 0;
    char *MID_r;
    sscanf(&TCP_recMSG[4], "%s %s %s %s %s %s", received_UID, received_GID, Tsize, text, Fname, Fsize);

    MID_r = save_message(received_UID, received_GID, atoi(Tsize), text, Fname, atoi(Fsize), 0);

    if (strlen(Fname) == 0)
    {
        if (strlen(MID_r) == 0)
        {
            sprintf(TCP_sendMSG, "RPT NOK");
        }
        else
        {
            sprintf(TCP_sendMSG, "RPT %s", MID_r);
        }
    }
    else
    {
        memset(TCP_recMSG, 0, MAX_BUFFER);
        MID_r = save_message(received_UID, received_GID, atoi(Tsize), text, Fname, atoi(Fsize), 1);
        strcpy(TCP_sendMSG, MID_r);
    }
    strcat(TCP_sendMSG, "\n");
    nleft = strlen(TCP_sendMSG);
    while (nleft > 0)
    {
        nwrite = write(newfdTCP, TCP_sendMSG, sizeof(TCP_sendMSG));
        if (nwrite == -1) /*error*/
            exit(1);

        nleft -= nwrite;
    }
    if (verbose_mode == 1)
    {
        cout << "UID=" << received_UID << ": post group " << received_GID << ": " << text << " " << Fname << endl;
    }
}

void TCP_Retrieve()
{
}