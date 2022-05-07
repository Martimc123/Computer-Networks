#include "aux_functions.h"
#include <unistd.h>
#include <fcntl.h>

void ParseArgs(int argc, char *argv[],std::string &DSIP,std::string &DSport)
{
    //Gets the IP and Port from the command line using getopt
    int option;
    int i = 1;
    while ((option = getopt(argc, argv, "n:p:")) != -1)
    {
        switch (option)
        {
        case 'n':
            //IP found and assgins it to the Port variable
            i += 2;
            DSIP = std::string(argv[i-1]);
            break;
        case 'p':
            //Port found and assgins it to the Port variable
            i += 2; 
            DSport = std::string(argv[i-1]);
            break;
        default:
            break;
        }
    }
    
    //If the IP is omitted
    if (DSIP.size() == 0)
    {
        char hostclient[256];
        int hostname;
        //It gets the machine IP
        hostname = gethostname(hostclient,sizeof(hostclient));
        if (hostname == -1) {
            perror("Error: in gethostname");
            exit(1);
        }
        DSIP = std::string(hostclient);
    }

    //If port is omitted it assigns the GGPORT to the port
    if (DSport.size() == 0)
        DSport  = std::string(GPPORT);
}

//Parses responses received from the server and returns 1 in success and 0 in unsuccess
int ParseResponse(std::string res,const char *gid,const char *gname)
{
    //parses register response from server
    if (res.compare("RRG OK\n") == 0)
    {
        std::cout << "User successfully registered" << '\n';
        return 1;
    }
    else if (res.compare("RRG DUP\n") == 0)
    {
        std::cout << "User already registered" << '\n';
        return 0;
    }
    else if (res.compare("RRG NOK\n") == 0)
    {
        std::cout << "Couldn't register user" << '\n';
        return 0;
    }

    //parses unregister response from server
    if (res.compare("RUN OK\n") == 0)
    {
        std::cout << "User successfully unregistered" << '\n';
        return 1;
    }
    else if (res.compare("RUN NOK\n") == 0)
    {
        std::cout << "Couldn't unregister user" << '\n';
        return 0;
    }

    //parses login response from server
    if (res.compare("RLO OK\n") == 0)
    {
        std::cout << "You are now logged in" << '\n';
        return 1;
    }
    else if (res.compare("RLO NOK\n") == 0)
    {
        std::cout << "Couldn't log in" << '\n';
        return 0;
    }

    //parses logout response from server
    if (res.compare("ROU OK\n") == 0)
    {
        std::cout << "You are now logged out" << '\n';
        return 1;
    }
    else if (res.compare("ROU NOK\n") == 0)
    {
        std::cout << "Couldn't log out: userID or password invalid" << '\n';
        return 0;
    }

    //parses unsubscribe response from server
    if (res.compare("RGU OK\n") == 0)
    {
        std::cout << "Unsubscribed: "<< gid << " – " << '\"' << gname << '\"' <<'\n';
        return 1;
    }
    else if (res.compare("RGU E_USR\n") == 0)
    {
        std::cout << "Couldn't unsubscribe group: invalid user" << '\n';
        return 0;
    }
    else if (res.compare("RGU E_GRP\n") == 0)
    {
        std::cout << "Couldn't unsubscribe group: invalid group" << '\n';
        return 0;
    }
    else if (res.compare("RGU NOK\n") == 0)
    {
        std::cout << "Couldn't unsubscribe group" << '\n';
        return 0;
    }
    
    if (res.compare("RUL OK\n") == 0)
    {
        std::cout << "List of users assgined to group " << gid << ':' << '\n' << "* ";
        return 1;
    }
    else if (res.compare("RUL NOK\n") == 0)
    {
        std::cout << "Group doesn't exist" << '\n';
        return 0;
    }
    else
        return 0;
}

//Parses responses received from the server and returns 1 in success and 0 in unsuccess
int ParseResponseUL(std::string res,const char *gid,const char *gname)
{
    //parses ulist response from server
    if (res.compare("RUL NOK\n") == 0)
    {
        std::cout << "Group doesn't exist" << '\n';
        return 0;
    }
    else
    {
        std::cout << "List of users assgined to group " << gid << " – " << gname << ":\n* ";
        return 1;
    }
    
}

int ParseResponseSub(std::string res,const char *gid,const char *gname)
{
    //parses response from server
    if (res.compare("RGS OK\n") == 0)
    {
        std::cout << "New group subscribed: "<< gid << " – " << '\"' << gname << '\"' <<'\n';
        return 1;
    }
    else if (res.compare("RGS E_USR\n") == 0)
    {
        std::cout << "Couldn't create and subscribe to group: invalid user ID" <<'\n';
        return 0;
    }
    else if (res.compare("RGS E_GRP\n") == 0)
    {
        std::cout << "Couldn't create and subscribe to group: invalid group ID" <<'\n';
        return 0;
    }
    else if (res.compare("RGS E_GNAME\n") == 0)
    {
        std::cout << "Couldn't create and subscribe to group: invalid group name" <<'\n';
        return 0;
    }
    else if (res.compare("RGS E_FULL\n") == 0)
    {
        std::cout << "Couldn't create and subscribe to group: server full" <<'\n';
        return 0;
    }
    else if (res.compare("RGS NOK\n") == 0)
    {
        std::cout << "Couldn't create and subscribe to group: invalid info" <<'\n';
        return 0;
    }
    else
    {
        std::cout << "New group created and subscribed: "<< gid << " – " << '\"' << gname << '\"' <<'\n';
        return 1;
    }
}

int ParseResponseGL(std::string res)
{
    //parses response from server
    if (res.compare("RGL 0\n") == 0)
    {
        std::cout << "Can't display groups: No groups available to subscribe" <<'\n';
        return 0;
    }
    else
        return 1;
}

int ParseResponseMGL(std::string res)
{
    //parses response from server
    if (res.compare("RGM 0\n") == 0)
    {
        std::cout << "Can't display groups: No groups subscribed" <<'\n';
        return 0;
    }
    else if (res.compare("RGM E_USR\n") == 0)
    {
        std::cout << "Can't display groups: No user logged in or invalid user" <<'\n';
        return 0;
    }
    else
        return 1;
}

int ParseResponsePST(std::unordered_map<int,group> &groups,std::string res,const char *gid,const char *gname)
{
    //parses response from server
    if (res.compare("RPT NOK\n") == 0)
    {
        std::cout << "Couldn't post message: invalid data" <<'\n';
        return 0;
    }
    else
    {
        char num[5];
        groups.at(atoi(gid)).num_msgs = atoi(res.substr(5,res.size()-1).c_str());
        sprintf(num,"%d",groups.at(atoi(gid)).num_msgs++);
        std::cout << "posted message number " << num << " to group " << gid << " – \"" << gname << "\"\n";
        return 1;
    }
}

int ParseResponseRTV(std::string res)
{
    //parses response from server
    if (res.compare("RRT OK\n") == 0)
    {
        return 1;
    }
    else if (res.compare("RRT EOF\n") == 0)
    {
        std::cout << "Couldn't retrieve messages: no messages to display" <<'\n';
        return 0;
    }
    else if (res.compare("RRT NOK\n") == 0)
    {
        std::cout << "Couldn't retrieve messages: invalid info in request" <<'\n';
        return 0;
    }
    else
        return 0;
}

//Parses error with a specific message
void ThrowError(const char* message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

//Finds a user from the group assigned users array
int find_uid(char (*users)[6],char* uid)
{
    //iterates trough the users array
    for (int i=0;i<MAX_USERS;i++)
    {
        //if uid is equal to any string in users
        if (strcmp(users[i],uid) == 0)
        //user found
            return 1;
    }

    //user not found
    return 0;
}

//Removes a user from the group assigned users array
void remove_user(char (*users)[6],char uid[])
{
    //iterates trough the users array
    for (int i=0;i<MAX_USERS;i++)
    {
        //if uid is equal to any string in users
        if (strcmp(users[i],uid) == 0)
        //user found and eliminates uid by changing it to a ""
            strcpy(users[i],"");
    }
}

//Prints an array of messages of a group
void print_msgs(msg messages[],int n)
{
    int i;
    //iterates through the an array and prints the messages contained in it 
    for (i = 0; i < n; i++) {
        if (strcmp(messages[i].fname,"") == 0)
            std::cout << messages[i].MID << " – \""<< messages[i].text << "\";"<<'\n';
        else
            std::cout << messages[i].MID << " – \""<< messages[i].text << "\"; file stored: "<< messages[i].fname << " with " << messages[i].fsize << " bytes\n";
    }
}

//Empties a string
void empty_str(char str[],int len)
{
    for (int i=0; i<len;i++)
        str[i]=0;
}

//Prints an array of users assigned to a group
void printarray(char (*users)[6],int n)
{
    int i;
    //iterates through the an array and prints the users assigned to it 
    for (i = 0; i < n; i++) {
        std::cout << users[i] << ' ';
    }
    std::cout << '\n';
}

//Prints the groups from the groups map for the mgl command
void printgroups(std::unordered_map<int,group> &groups)
{
    //iterates through the map and prints the GID, the GNAME and the users assigned to that group 
    for (auto x : groups)
    {
      std::cout << "GroupID: " << x.second.GID << " | Group Name: " << x.second.GNAME << '\n';  
    }
}


//Prints all the info from the groups from the groups map
void printmap(std::unordered_map<int,group> &groups)
{
    //iterates through the map and prints the GID, the GNAME and the users assigned to that group 
    for (auto x : groups)
    {
      std::cout << "GroupID: " << x.second.GID << " Group Name: " << x.second.GNAME << "-" << x.second.num_users << "-";
      
      //prints the users assigned to the respective group
      printarray(x.second.UIDS,x.second.num_users);
    }
}

//Adds a group in the group map or assigns a group to a respective UID
void add_groups(char* str,std::unordered_map<int,group> &groups,std::string logUID)
{
    char* token;
    char* rest = str;

    //removes the RGM and number of groups from the buffer
    strtok_r(rest, " ", &rest);
    strtok_r(rest, " ", &rest);
    int i = 0;
    int j = 0;
    group *gp = new group;
    
    //parses through the line, getting the GID and GNAME
    while ((token = strtok_r(rest, " ", &rest)))
    {
        //GID found and copies it to gp name
        if (i==0)
        {
            strcpy(gp->GID,token);
            i++;
            j++;
        }
        
        //GNAME found
        else if (i==1)
        {
            //copies GNAME found and it the logged user to gp and 
            //assumes its num users 1 since the group doesnt exist
            strcpy(gp->GNAME,token);
            gp->num_users=1;
            strcpy(gp->UIDS[0],logUID.c_str());
            
            //group found and it already exists
            if (groups.find(atoi(gp->GID)) != groups.end() && find_uid(groups.at(atoi(gp->GID)).UIDS,(char*)logUID.c_str()) == 1)
            {}
            
            //group found but logged user isnt assigned to it
            else if (groups.find(atoi(gp->GID)) != groups.end() && find_uid(groups.at(atoi(gp->GID)).UIDS,(char*)logUID.c_str()) == 0)
            {
                //gets the number of users the group has and increments it
                //and assigns the user to the group
                int i = groups.at(atoi(gp->GID)).num_users;
                strcpy(groups.at(atoi(gp->GID)).UIDS[i],logUID.c_str());
                groups.at(atoi(gp->GID)).num_users++;   
            }
            
            //group not found (found MID) then adds the created group to the groups map
            else
            {
                groups[atoi(gp->GID)]=*gp;
            }
            j++;
            i++;
        }
        
        //found MID so ignors it and changes i to 0 since the next token
        //is the GID of the next group
        else
        {
            i=0;
        }
    }
}

//Adds users in the group users array in the group map
void add_users(char* str,std::string &AGID,std::unordered_map<int,group> &groups)
{
    char* token;
    char* rest = str;

    //removes the RGM and number of groups and Gname from the buffer
    int i = 0;
    
    //parses through the line, getting the UID's subscribed
    while ((token = strtok_r(rest, " ", &rest)))
    {
        //UID found and copies it to group
        strcpy(groups.at(atoi(AGID.c_str())).UIDS[i++],token);
    }
    groups.at(atoi(AGID.c_str())).num_users=i;
}

//parses string contaning messages and assigns them to their respective groups
void split_msgs(std::unordered_map<int,group> &groups,char* AGID,char* str)
{
    char* token;
    char* rest = str;
    msg menssagem;
    int i=0;

    //parses through the lines until there's no more lines
    while ((token = strtok_r(rest, "\n", &rest)))
    {
        //found a line containing info about the text
        if (token[1] != '/')
        {
            parse_msgs(token,1,menssagem);
            if (rest[1] != '/' || rest == NULL)
            {
                empty_str(menssagem.data,strlen(menssagem.data));
                empty_str(menssagem.fname,strlen(menssagem.fname));
                empty_str(menssagem.fsize,strlen(menssagem.fsize));
                groups.at(atoi(AGID)).messages[i++] = menssagem;
                groups.at(atoi(AGID)).num_msgs++;
                //print_msg(groups.at(atoi(AGID)).messages[i++],1);
            }
        }
        else if (token[1] == '/')
        {
            parse_msgs(token,2,menssagem);
            groups.at(atoi(AGID)).messages[i++] = menssagem;
            groups.at(atoi(AGID)).num_msgs++;
            //print_msg(groups.at(atoi(AGID)).messages[i++],1);
        }
    }
}

//parses through a message line and assigns it to a variable
void parse_msgs(char* str,int type,msg &mensagem)
{
    char* token;
    char* rest = str;

    //parses lines contaning info about the text message
    if (type == 1)
    {
        token = strtok_r(rest, " ", &rest);
        strcpy(mensagem.MID,token);

        token = strtok_r(rest, " ", &rest);
        strcpy(mensagem.UID,token);

        token = strtok_r(rest, " ", &rest);
        strcpy(mensagem.textsize,token);

        std::string resto(rest);
        strcpy(mensagem.text,resto.substr(0,resto.size()).c_str());
    }

    //parses lines contaning info about the file
    else if (type == 2)
    {
        token = strtok_r(rest, " ", &rest);
        token = strtok_r(rest, " ", &rest);
        strcpy(mensagem.fname,token);
        token = strtok_r(rest, " ", &rest);
        strcpy(mensagem.fsize,token);
        std::string resto(rest);
        strcpy(mensagem.data,resto.substr(0,resto.size()).c_str());   
    }
}

void split_str(std::string str,std::string &res,std::string &status)
{
    std::string nstr;
    
    char* token;
    char* rest = (char*)str.c_str();

    //removes the RGM and number of groups from the buffer
    for (int i=0;i<3;i++)
    {
        token = strtok_r(rest, " ", &rest);
        nstr += token;
        nstr += " ";
    }
    nstr+="\n";
    
    status = nstr;
    res = std::string(rest);
}

//calculates the next MID given an MID
std::string calc_next(std::string num)
{
    std::string res;
    char mid[6];
    sprintf(mid,"%04d",atoi(num.c_str())+1);
    res = std::string(mid);
    return res;
}

//gets text/data of a message 
std::string busca_texto(char* str,char* num,std::string &text,int &flag,std::string &tok)
{
    char* token;
    char* rest = str;

    //searches for words until it finds a / or the next MID
    while ((token = strtok_r(rest, " ", &rest)))
    {
        //found a /, meaning that it already parsed the text of a message
        if ((strcmp(token,"/") == 0))
        {
            flag=1;
            break;
        }

        //found an MID, meaning that it already parsed the data of a message
        //or the text of a message that doesnt contain any files attached
        else if (strcmp(token,num) == 0)
        {
            tok = std::string(token);
            flag=2;
            break;
        }

        //parses the text by joining the words being found in a string
        else
        {
            text += token;
            text+= " ";
        }
    }
    return std::string(rest);
}

//parses response from the server for the retrieve command
void parseMessagesRTV(char* str,char* agid,std::unordered_map<int,group> &groups,int limit)
{
    char* mid;
    char* uid;
    char* textsize;
    std::string text;
    std::string num; //MID,of the response, from the next message used for checking the response
    std::string remainder; // used to save the rest of the response while parsing
    std::string toknum; //MID,of the response, from the next message used for pasing the response
    int flag; //used to check if a message has or not a file included
    char* rest = str;
    int i=0; //records the index of the messages being parsed
    
    //parses RTV response from the server
    do
    {
        //if (i == limit)
            //break;
        std::cout << limit;

        msg message;
        
        //gets the MID of the message
        mid = strtok_r(rest, " ", &rest); 
        strcpy(message.MID,mid);

        //in the, since the last MID remained, after the 1st strtok
        //the rest will be an whitespace since the response had already been
        //parsed, so it breaks the cicle, due to not being needed more parsing
        if (strcmp(rest," ") == 0)
            break;

        //gets the User (UID) of the person that sent the message
        uid = strtok_r(rest, " ", &rest);
        strcpy(message.UID,uid);

        //gets the size of the text of the message sent
        textsize = strtok_r(rest, " ", &rest);
        strcpy(message.textsize,textsize);
        
        //calculates the next MID in order to find the end of the message,
        //in case, the message doesn't include any files attached
        num = calc_next(std::string(mid));

        //parses the rest of the response until it gets the text of the message sent
        //and finds whether the message, has or not, a file sent
        remainder = busca_texto(rest,(char*)num.c_str(),text,flag,toknum);
        strcpy(message.text,(char*)text.substr(0,text.size()-1).c_str());
        text.clear();
        rest = (char*)remainder.c_str();
        
        //case there's a file sent the flag is 1 meaning that
        //the message has a file attached
        if (flag == 1)
        {
            char* fname;
            char* fsize;
            std::string data;

            //gets the name of the file attached to the message
            fname = strtok_r(rest, " ", &rest);
            strcpy(message.fname,fname);
            
            //gets the size of the file attached to the message
            fsize = strtok_r(rest, " ", &rest);
            strcpy(message.fsize,fsize);
            
            //parses the rest of the response until it gets the data of the file attached
            //to the message sent and finds the next MID in order to know when there's no
            //more data to parse
            remainder = busca_texto(rest,(char*)num.c_str(),data,flag,toknum);
            
            //removes the \n from the data
            data=data.substr(0,data.size()-2);
            
            //returns the next MID to the rest of the response due to it being extracted
            //with strtok during the parsing of the data of the file
            toknum+=" ";
            remainder = toknum + remainder;
            rest = (char*)remainder.c_str();
            strcpy(message.data,(char*)data.c_str());

            //adds the message to the groups map since the message is fully parsed
            groups.at(atoi(agid)).messages[i++] = message;
            groups.at(atoi(agid)).num_msgs++;
        }

        //case there's not a file attached to message sent,
        //the flag is 2 meaning that the message doesn't have a file attached
        else if (flag == 2)
        {
            //clears the info about the data of the last message in case there is
            //info remaining from the last parsing
            strcpy(message.fname,"");
            strcpy(message.fsize,"");
            strcpy(message.data,"");

            //adds the message to the groups map since it already has been parsed
            //before reaching this branch of the loop
            groups.at(atoi(agid)).messages[i++] = message;
            groups.at(atoi(agid)).num_msgs++;
            
            //returns the next MID to the rest of the response for the reason
            //stated before
            toknum+=" ";
            remainder = toknum + remainder;
            rest = (char*)remainder.c_str();
        }
    }while(strcmp(rest,"") !=0);

    //removes \n from the last word of the text in case the message doesn't have
    //a file attached and from the data, in case, the message has attached a file
    if (flag == 1)
    {
        std::string message = groups.at(atoi(agid)).messages[i-1].data;
        groups.at(atoi(agid)).messages[i-1].data[message.size()-1]='\0';
    }
    if (flag == 2)
    {
        std::string message = groups.at(atoi(agid)).messages[i-1].text;
        groups.at(atoi(agid)).messages[i-1].text[message.size()-1]='\0';
    }
}

//creates files generated in the Retrieve command
void creates_files(msg messages[],int n)
{
    for (int i=0;i<n;i++)
    {
        if( strcmp(messages[i].fname,"") != 0)
        {
            std::ofstream fileOut(messages[i].fname);
            fileOut << messages[i].data;
            fileOut.close();
        }
    }
}

/*void ParseMSGRTV2(char* str,char* agid,std::unordered_map<int,group> &groups)
{
    char *rest = str;
    char* mid;
    char* uid;
    char* textsize;
    std::string text;
    char* fname;
    char* fsize;
    std::string data("");

    do
    {
        msg message;
        mid = strtok_r(rest, " ", &rest);
        strcpy(message.MID,mid);

        std::cout << "|"<<message.MID<<"|\n";
        
        uid = strtok_r(rest, " ", &rest);
        strcpy(message.UID,uid);
        
        std::cout << "|"<<message.UID<<"|\n";

        textsize = strtok_r(rest, " ", &rest);
        strcpy(message.textsize,textsize);
        
        std::cout << "|"<<message.textsize<<"|\n";

        int i;
        for (i=0;i<atoi(textsize);i++)
        {
            text+=rest[i];
        }
        
        std::cout << "|"<<message.text<<"|\n";

        strcpy(rest,std::string(rest).substr(i).c_str());
        

        char* token;
        token =strtok_r(rest, " ", &rest);
        
        if (strcmp(token,"/") == 0)
        {    
            fname = strtok_r(rest, " ", &rest);
            strcpy(message.fname,fname);
            std::cout << "|"<<message.fname<<"|\n";
            fsize = strtok_r(rest, " ", &rest);
            strcpy(message.fsize,fsize);
            std::cout << "|"<<message.fsize<<"|\n";
            std::cout << "AQUI 1\n";
            
            data = std::string(rest).substr(0,atoi(fsize));
            
            std::cout << "caminho 1\n";

            //strcpy(message.data,data.c_str());
            std::cout << "caminho t1\n";
            int len;
            int nfile,nwritten;
            len = atoi(fsize);
            nfile = open(fname,O_WRONLY | O_CREAT,0644);
            
            nwritten = write(nfile,data.c_str(),data.size());
                
            close(nfile);
            std::cout << "caminho t4\n";
            
            //groups.at(atoi(agid)).messages[++i] = message;
            std::cout << "caminho t5\n";
            //groups.at(atoi(agid)).num_msgs++;
            std::cout << "caminho t6\n";
            std::cout << "caminho 2\n";
            strcpy(fname,"");
            strcpy(fsize,"");
            data.clear();
            std::cout << "caminho 3\n";
            int pos = std::string(rest).find("\n");
            strcpy(rest,std::string(rest).substr(pos+1).c_str());
            std::cout << "caminho 4\n";
        }
        else
        {
            std::string resto(token);
            resto+=" ";
            resto+=rest;
            strcpy(rest,resto.c_str());
            //groups.at(atoi(agid)).messages[i++] = message;
            //groups.at(atoi(agid)).num_msgs++;
        }
        text.clear();
    }while(strcmp(rest,"\n") != 0);
}*/

void get_data(int fsize,char* fname,char* rest,int &client_tcp)
{
    int len = fsize-1;//current len of the file
    //int rbytes; //size of the remainder of the buffer including files data
    int nfile;
    ssize_t nwritten,nread;
    nfile = open(fname,O_WRONLY | O_CREAT,0644);
    if (len<512)
    {
        int i;
        for (i=512;i>0;i--)
        {
            if (rest[i] == '\n')
                break;
        }
        i++;
        int bytestow = 512-i;
        nwritten = write(nfile,rest,bytestow);
    }
    else
    {
        nwritten = write(nfile,rest,512);
        len -= nwritten;
    }
    while(len>0)
    {
        char buffer[512];
        nread=read(client_tcp,buffer,512);
        if(nread==-1)exit(1);
        else if(nread==0)break;//closed by peer
        nwritten = write(nfile, buffer, nread);
        len-=nwritten;
    }
}