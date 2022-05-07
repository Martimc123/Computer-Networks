#include "../auxiliar_code/aux_functions.h"
#include "../socket_operations/socket_functs.h"
#include "commands.h"
#include <stdlib.h>
#include <unordered_map>

//Executes the commands from the statement
void Client(std::string DSIP,std::string DSport)
{
    char message[MAX_MESSAGE], line[MAX_LINE];
    char command[16], var1[241], var2[128];
    std::string logUID, logPass, num_grps;
    std::unordered_map<int,group> groups;
    std::string AGID;

    //Socket variables
    int client,client_tcp, err;
    struct addrinfo hintsClient;
    struct addrinfo hintsClientTCP;
    struct addrinfo *resClient;
    struct sockaddr_in addr;

    //ssize_t n;

    setsocketUDP(client,hintsClient);

    //gets the ip address and port of the server
    err = getaddrinfo(DSIP.c_str(), DSport.c_str(), &hintsClient, &resClient);
    if (err != 0)
        ThrowError("Error: getting address info of AS server\n");

    while (1)
    {
        //sets up client TCP socket
        setsocketTCP(client_tcp,hintsClientTCP);

        //clears the message address space
        memset(message, '\0', sizeof(message));
        
        //gets the command line from the stdin and parses the line
        fgets(line, sizeof(line), stdin);
        sscanf(line, "%s %s %s", command,var1,var2);
        std::string option1(var1);
        std::string option2(var2);

        
        //registers a UID in the server
        if (strcmp(command, "reg") == 0)
        {
            command_reg((char*)"REG",option1,option2,client,resClient,addr);
        }
        
        //unregisters a UID from the server
        else if (strcmp(command, "unregister") == 0 || strcmp(command, "unr") == 0)
        {
            command_reg((char*)"UNR",option1,option2,client,resClient,addr);
        }
        
        //logs in a UID
        else if (strcmp(command, "login") == 0)
        {
            command_log(logUID,logPass,option1,option2,client,resClient,addr);
        }

        //logs out the logged in UID
        else if (strcmp(command, "logout") == 0)
        {
            command_logout(logUID,logPass,client,resClient,addr);
        }

        //displays the logged UID
        else if (strcmp(command, "showuid") == 0 || strcmp(command, "su") == 0)
        {
            std::cout << "Logged user: " << logUID << '\n';
        }

        //ends session with server and exits
        else if (strcmp(command, "exit") == 0)
        {
            if (logUID.compare("") != 0 && logPass.compare("") != 0)
                command_logout(logUID,logPass,client,resClient,addr);
            close(client);
            close(err);
            close(client_tcp);
            exit(EXIT_SUCCESS);
        }

        //displays the available groups in the server
        else if (strcmp(command, "groups") == 0 || strcmp(command, "gl") == 0)
        {
            command_gl(num_grps,client,resClient,addr);
        }

        //subscribes to a group
        else if (strcmp(command, "subscribe") == 0 || strcmp(command, "s") == 0)
        {
            std::string gid(var1);
            std::string gname(var2);
            command_subscribe(gid,num_grps,logUID,gname,groups,client,resClient,addr);
        }

        //unsubscribes a UID from a group
        else if (strcmp(command, "unsubscribe") == 0 || strcmp(command, "u") == 0)
        {
            std::string gid(var1);
            std::string gname(groups.at(atoi(var1)).GNAME);
            command_unsubscribe(groups,AGID,gid,gname,logUID,client,resClient,addr);
        }

        //displays a list with the groups subscribed
        else if (strcmp(command, "my_groups") == 0 || strcmp(command, "mgl") == 0)
        {
            command_mgl(groups,logUID,client,resClient,addr);
        }

        //selects an existing GID as the active GID
        else if (strcmp(command, "select") == 0 || strcmp(command, "sag") == 0)
        {
            std::string gid(var1);
            command_select(groups,AGID,gid,logUID);
        }

        //displays active GID
        else if (strcmp(command, "showgid") == 0 || strcmp(command, "sg") == 0)
        {
            command_sg(groups,AGID);
        }

        //selects an existing GID as the active GID
        else if (strcmp(command, "ulist") == 0 || strcmp(command, "ul") == 0)
        {
            command_ul(groups,AGID,client_tcp,resClient);
        }

        //selects an existing GID as the active GID
        else if (strcmp(command, "post") == 0)
        {
            std::string str(line);
            std::string gname(groups.at(atoi(AGID.c_str())).GNAME);
            command_post(groups,str,logUID,gname,AGID,client_tcp,resClient);
        }

        //retrieves up to 20 messages from a respective group
        else if (strcmp(command, "retrieve") == 0 || strcmp(command, "r") == 0)
        {
            std::string MID(var1);
            command_RTV(MID,logUID,AGID,client_tcp,resClient);
        }
    }
}
