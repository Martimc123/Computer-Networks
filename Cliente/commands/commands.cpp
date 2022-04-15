#include "commands.h"
#include "../socket_operations/socket_functs.h"
#include "../auxiliar_code/aux_functions.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>


//Sends a message to the server to register on unregister a user
//depending on the option REG (to resgister) and UNR (to unresgister)
void command_reg(char* opt,std::string uid,std::string pass,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr)
{
    std::string message;
    std::string reg(opt);
    std::string res;
    
    //copies the the upmost arguments from the line and sends a 
    //message with them to the server 
    message = reg+' '+uid+' '+pass+'\n';
    sendmsgUDP((char*)message.c_str(),client,resClient,addr,res);

    //Parses response from server
    ParseResponse(res,"","");
}

//Sends a message to the server to login a user
void command_log(std::string &loguid,std::string &logpass,std::string uid,std::string pass,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr)
{
    std::string message;
    std::string log("LOG");
    std::string res;
    
    if (loguid.compare("") == 0)
    {
        //copies the the upmost arguments from the line and sends a 
        //message with them to the server 
        message = log+' '+uid+' '+pass+'\n';
        sendmsgUDP((char*)message.c_str(),client,resClient,addr,res);

        int i;
        //Parses response from server
        i = ParseResponse(res,"","");
        if (i == 1)
        {
            loguid = uid;
            logpass = pass;
        }
    }
    else
        std::cout << "You are already logged in user " << loguid <<'\n';
}

//Sends a message to the server to login a user
void command_logout(std::string &loguid,std::string &logpass,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr)
{
    std::string message;
    std::string out("OUT");
    std::string res;
    
    //copies the the upmost arguments from the line and sends a 
    //message with them to the server 
    message = out+' '+loguid+' '+logpass+'\n';
    sendmsgUDP((char*)message.c_str(),client,resClient,addr,res);

    int i;
    //Parses response from server
    i = ParseResponse(res,"","");
    if (i == 1)
    {
        loguid = std::string("");
        logpass = std::string("");
    }
}

//Sends a message to the server to login a user
void command_gl(std::string &num,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr)
{
    std::string message("GLS\n");
    std::string res;
    
    //copies the the upmost arguments from the line and sends a 
    //message with them to the server 
    sendmsgUDP((char*)message.c_str(),client,resClient,addr,res);
    int i;
    //Parses response from server
    i = ParseResponseGL(res);
    if (i == 1)
    {
        std::cout << res;
        char* token;
        char* rest = (char*)res.c_str();
        strtok_r(rest, " ", &rest);
        token = strtok_r(rest, " ", &rest);
        num = std::string(token);
        res.clear();
    }
}

//Sends a message to the server to subscribe or create a group
void command_subscribe(std::string &gid,std::string &num_grps,std::string &logUID,std::string &gname,std::unordered_map<int,group> &groups,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr)
{
    std::string res;
    //new group created in the server
    if (gid.compare("00") == 0)
    {
        //updates overall number of groups in the server
        char ngp[3];
        sprintf(ngp, "%d",atoi(num_grps.c_str()));
        num_grps = std::string(ngp);

        std::string message("GSR ");
        message += logUID+' '+'0'+'0'+' '+gname+'\n';
        
        //adds new group to the group map
        sendmsgUDP((char*)message.c_str(),client,resClient,addr,res);
        int i;
        i = ParseResponseSub(res,num_grps.c_str(),gname.c_str());
        if (i == 1)
        {
            group gp;
            gp.num_users=0;
            strcpy(gp.GID,ngp);
            strcpy(gp.GNAME,gname.c_str());
            strcpy(gp.UIDS[gp.num_users++],(char*)logUID.c_str());
            groups[atoi(gp.GID)]=gp;
        }
    }
    else
    {
        //copies the GID to the created group variable
        group Sub_gp;
        strcpy(Sub_gp.GID,gid.c_str());
        strcpy(Sub_gp.GNAME,gname.c_str());
        int j=0; //flag to check if group was already created and stored in groups

        //Group found (already exists) and logged UID assigned to it
        if (groups.find(atoi(Sub_gp.GID)) != groups.end() && find_uid(groups.at(atoi(Sub_gp.GID)).UIDS,(char*)logUID.c_str()) == 1)
        {
            Sub_gp = groups.at(atoi(Sub_gp.GID));
            j=1;
        }


        //Group found (already exists) and logged UID not assigned to it
        else if (groups.find(atoi(Sub_gp.GID)) != groups.end() && find_uid(groups.at(atoi(Sub_gp.GID)).UIDS,(char*)logUID.c_str()) == 0)
        {
            //gets the group and increments the number of assigned users and assigns
            //the current logged user to the respective group
            int i = groups.at(atoi(Sub_gp.GID)).num_users;
            strcpy(groups.at(atoi(Sub_gp.GID)).UIDS[i],logUID.c_str());
            groups.at(atoi(Sub_gp.GID)).num_users++;
            Sub_gp = groups.at(atoi(Sub_gp.GID));
            j=1;
        }
        
        //sends a message to the server with the created and subscribed group
        std::string message("GSR ");
        message += logUID+' '+Sub_gp.GID+' '+Sub_gp.GNAME+'\n';
        sendmsgUDP((char*)message.c_str(),client,resClient,addr,res);

        int i;
        //Parses response from server
        i = ParseResponseSub(res,Sub_gp.GID,Sub_gp.GNAME);
        if (i == 1 && j==0)
        {
            //group not found (group doesn't exist)
            //creates group and adds it into the group map
            strcpy(Sub_gp.UIDS[Sub_gp.num_users++],logUID.c_str());
            groups[atoi(Sub_gp.GID)]=Sub_gp;
        }   
    }
}

//Sends a message to the server to unsubscribe a group
void command_unsubscribe(std::unordered_map<int,group> &groups,std::string &agid,std::string &gid,std::string &gname,std::string &logUID,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr)
{
    std::string res;
    //unsubscribes active GID from the logged user
    if (gid.compare(agid) == 0)
    { 
        agid = std::string("");
    }

    //sends message to the server saying that the group has been unsubscribed
    std::string message("GUR ");
    message += logUID+' '+gid+'\n';
    int i;   
    sendmsgUDP((char*)message.c_str(),client,resClient,addr,res);
    i = ParseResponse(res,gid.c_str(),gname.c_str());
    if (i==1)
    {
        //group found
        if (groups.find(atoi(gid.c_str())) != groups.end())
        {
            //unassigns logged user from the group
            remove_user(groups.at(atoi(gid.c_str())).UIDS,(char*)logUID.c_str());
            groups.at(atoi(gid.c_str())).num_users--;
        }
    }
}

//displays a list of the current subscribed groups
void command_mgl(std::unordered_map<int,group> &groups,std::string &logUID,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr)
{
    //sends a message to the sever asking for the subscribed groups
    std::string message("GLM ");
    std::string ans;
    message += logUID+'\n';   
    sendmsgUDP((char*)message.c_str(),client,resClient,addr,ans);
    int i;
    i = ParseResponseMGL(ans);
    if (i == 1)
    {
        //parses through the buffer with the subscribed groups and creates or
        //assigns users to groups
        add_groups((char*)ans.c_str(),groups,logUID);
                
        //prints map to confirm if every group was parsed
        printgroups(groups);
    }
}

//selects a group from the group map as the active group
void command_select(std::unordered_map<int,group> &groups,std::string &agid,std::string gid,std::string logUID)
{
    //finds if logged user is subscribed to the respective group
    //if so, it declares that group as current active group
    if (groups.find(atoi(gid.c_str())) != groups.end())
    {
        if (find_uid(groups.at(atoi(gid.c_str())).UIDS,(char*)logUID.c_str()) == 1)
        {
            //gets the selected group (if it exists)
            group sub_gp = groups.at(atoi(gid.c_str()));
            agid = gid;
            std::cout << "Group "<< agid << " – \"" << sub_gp.GNAME <<"\" is now the active group\n";
        }
    }
    else
        std::cout << "Can't select group: Group "<< agid <<  " doesn't exist\n";
}

//displays the current active group
void command_sg(std::unordered_map<int,group> groups,std::string agid)
{
    std::cout << "Current active group: Group " << agid << " – "<< groups.at(atoi(agid.c_str())).GNAME << '\n';
}

//displays the list of users subscribed to the current active group
void command_ul(std::unordered_map<int,group> groups,std::string agid,int &client_tcp,struct addrinfo* &resClient)
{
    std::string ans;
    std::string status;
    //connects the client socket to the server
    int r;
    r=connect(client_tcp,resClient->ai_addr,resClient->ai_addrlen);
    if (r == -1)
        ThrowError("Error: in connecting TCP client socket\n");
            
    //sends a message to the sever asking for the subscribed groups
    std::string message("ULS ");
    message += agid+'\n';

    sendmsgTCPUL((char*)message.c_str(),client_tcp,status);

    ssize_t n;
    char buffer[MAX_BUFFER];
    
    //recives a message from the server
    n = read(client_tcp,buffer,MAX_BUFFER);
    if (n == -1)
        ThrowError("Error: in reading the buffer from the server\n");
    buffer[n-1] = '\0';
    ans = std::string(buffer);
    
    std::string res = status+ans;
    split_str(res,ans,status);

    int i;
    i = ParseResponseUL(status,agid.c_str(),groups.at(atoi(agid.c_str())).GNAME);
    if (i==1)
    {
        //assigns users to the group
        add_users((char*)ans.c_str(),agid,groups);
        printarray(groups.at(atoi(agid.c_str())).UIDS,groups.at(atoi(agid.c_str())).num_users);
    }

    //closes TCP connection
    close(client_tcp);    
}

//posts a message to the current active group
void command_post(std::unordered_map<int,group> &groups,std::string str,std::string logUID,std::string gname,std::string agid,int &client_tcp,struct addrinfo* &resClient)
{
    std::string ans;
    std::string status;
    std::string message("PST ");
    char textsize[10];
    char datasize[10];

    //parses the input line to get the text and file name
    std::string token1, token2, token3;
    std::istringstream(str) >> token1 >> std::quoted(token2) >> token3;

    //connects the client socket to the server
    int r;
    r=connect(client_tcp,resClient->ai_addr,resClient->ai_addrlen);
    if (r == -1)
        ThrowError("Error: in connecting TCP client socket\n");

    //Case if file is also sent
    if (token3.compare("") != 0)
    {
        //opens file in binary in case its an image
        struct stat file;
        ssize_t nbytes,nleft,nwritten,nread;
        
        stat(token3.c_str(), &file);
        nbytes= file.st_size;
        
        FILE *fp = fopen(token3.c_str(), "rb");

        sprintf(textsize,"%ld",token2.size());
        sprintf(datasize,"%ld",file.st_size);

        sprintf(textsize,"%ld",token2.size());
        
        //sets up a message to the server with the file info and the respective text message
        message += logUID+' '+agid+' '+textsize+' '+token2+' '+token3+' '+datasize+' ';
        int n1;
        n1 = write(client_tcp,message.c_str(),message.size());
        if (n1 == -1)
            ThrowError("Error: in sending a message to the server\n");
    
        unsigned char buffer[512];
        nleft=nbytes;
        int size = 512;
        while(nleft>0)
        {
            /*if (nleft < size)
                size=nleft;*/
            nread=fread(buffer,sizeof(unsigned char),size,fp);
            if (nread==0) break;
            nwritten=write(client_tcp,buffer,nread);
            if(nwritten == -1)
                ThrowError("Error: in sending bytes of file to the server\n");
            nleft-=nread;
        }
        
        char bufferTCP[MAX_BUFFER];
        n1 = read(client_tcp,bufferTCP,MAX_BUFFER);
        if (n1 == -1)
            ThrowError("Error: in reading the buffer from the server\n");
    
        status = std::string(bufferTCP);
    }
    else
    {
        sprintf(textsize,"%ld",token2.size());
        //sets up a message to the sever with the text message
        message += logUID+' '+agid+' '+textsize+' '+token2+'\n';
        sendmsgTCPPST((char*)message.c_str(),client_tcp,status);
    }

    //sends message to the server and posts result
    //std::cout << message;
    ParseResponsePST(groups,status,agid.c_str(),gname.c_str());
    
    //closes TCP connection
    close(client_tcp);
}


//retrieves up to 20 messages from the server assigned to the current active group
void command_RTV(std::string MID,std::string logUID,std::string agid,int &client_tcp,struct addrinfo* &resClient)
{
    std::string ans;
    std::string status;
    std::string message("RTV ");
    char *rest;
    
    //connects the client socket to the server
    int r;
    r=connect(client_tcp,resClient->ai_addr,resClient->ai_addrlen);
    if (r == -1)
        ThrowError("Error: in connecting TCP client socket\n");

    message += logUID+' '+agid+' '+MID+'\n';

    //sends message to the server and posts result
    //sendmsgTCPRTV((char*)message.c_str(),client_tcp,status,ans);
    
    ssize_t n1,n2;
    char bufferTCP[512];
    char bufferTCP2[512];

    //sends a message to the server
    n1 = write(client_tcp,message.c_str(), strlen(message.c_str()));
    if (n1 == -1)
        ThrowError("Error: in sending a message to the server\n");
            
    //recives a message from the server
    n1 = read(client_tcp,bufferTCP,512);
    if (n1 == -1)
        ThrowError("Error: in reading the buffer from the server\n");

    bufferTCP[n1]='\0';
    status = std::string(bufferTCP) + '\n';

    //char* rest2;
    rest = bufferTCP2;
    int flag=0;
    std::string num;

    msg msg2;

    while ( (n2 = read(client_tcp,bufferTCP2,512)) != 0)
    {
        if (n2 == -1)
            ThrowError("Error: in reading the buffer from the server\n");

        bufferTCP2[n2]=0;
        
        rest = bufferTCP2;
        
        if (flag == 1)
        {
            std::string remainder = std::string(num) + " ";
            remainder+= rest;
            rest = (char*)remainder.c_str();
                
        }
        
        while(strcmp(rest,"\n") != 0 || strcmp(rest,"") != 0)
        {
            char* ptr;
            char* mid;
            char* uid;
            char* textsize;
            std::string text;
            char* fname;
            char* fsize;
    
            mid = strtok_r(rest, " ", &rest);
            strcpy(msg2.MID,mid);
            num = calc_next(std::string(mid));
            uid = strtok_r(rest, " ", &rest);
            strcpy(msg2.UID,uid);
            textsize = strtok_r(rest, " ", &rest);
            strcpy(msg2.textsize,textsize);
            flag=1;
            
            int i;
            ptr=rest;
            for (i=0;i<atoi(textsize);i++)
            {
                text+=rest[i];
                ptr++;
            }
            strcpy(msg2.text,text.c_str());
            rest=ptr;
            char* token;
            token =strtok_r(rest, " ", &rest);
            if (strcmp(token,"/") != 0)
            {
                if (strcmp(fname,"") != 0) strcpy(fname,"");
                if (strcmp(fsize,"") != 0) strcpy(fsize,"");
                std::string remainder;
                remainder+= std::string(token) + " ";
                remainder+= rest;
                rest = (char*)remainder.c_str();
                //rest2=rest;            
            }
        
            else if (strcmp(token,"/") == 0)
            {
                fname = strtok_r(rest, " ", &rest);
                strcpy(msg2.fname,fname);
                fsize = strtok_r(rest, " ", &rest);
                strcpy(msg2.fsize,fsize);
                get_data(atoi(fsize),fname,rest,client_tcp);
                std::string num = calc_next(std::string(mid));
                break;
            }
            text.clear();
        }
    }
    
    status = status.substr(0,6) + '\n';
    
    int i;
    i = ParseResponseRTV(status);
    if (i==1)
    {
        std::cout << msg2.MID << " – \""<< msg2.text << "\"; file stored: "<< msg2.fname << " with " << msg2.fsize << " bytes\n";
    }

    //closes TCP connection
    close(client_tcp);
}