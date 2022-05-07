#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <unordered_map>
#include "../client_constants.h"

void Client(std::string DSIP,std::string DSport);
void command_reg(char* opt,std::string uid,std::string pass,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr);
void command_log(std::string &loguid,std::string &logpass,std::string uid,std::string pass,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr);
void command_logout(std::string &loguid,std::string &logpass,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr);
void command_gl(std::string &num,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr);
void command_subscribe(std::string &gid,std::string &num_grps,std::string &logUID,std::string &gname,std::unordered_map<int,group> &groups,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr);
void command_unsubscribe(std::unordered_map<int,group> &groups,std::string &agid,std::string &gid,std::string &gname,std::string &logUID,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr);
void command_mgl(std::unordered_map<int,group> &groups,std::string &logUID,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr);
void command_select(std::unordered_map<int,group> &groups,std::string &agid,std::string gid,std::string logUID);
void command_sg(std::unordered_map<int,group> groups,std::string agid);
void command_ul(std::unordered_map<int,group> groups,std::string agid,int &client_tcp,struct addrinfo* &resClient);
void command_post(std::unordered_map<int,group> &groups,std::string str,std::string logUID,std::string gname,std::string agid,int &client_tcp,struct addrinfo* &resClient);
void command_RTV(std::string MID,std::string logUID,std::string agid,int &client_tcp,struct addrinfo* &resClient);

#endif //commands.h