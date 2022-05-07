#ifndef AUX_FUNCTIONS_H
#define AUX_FUNCTIONS_H

#include "../client_constants.h"
#include <signal.h>
#include <string>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <sys/stat.h>

//Prototypes of auxiliar functions used in client, commands or in main
void ParseArgs(int argc, char *argv[],std::string &DSIP,std::string &DSport); //used in main
int ParseResponse(std::string res,const char *gid,const char *gname);
int ParseResponseSub(std::string res,const char *gid,const char *gname);
int ParseResponseGL(std::string res);
int ParseResponseMGL(std::string res);
int ParseResponseUL(std::string res,const char *gid,const char *gname);
int ParseResponsePST(std::unordered_map<int,group> &groups,std::string res,const char *gid,const char *gname);
int ParseResponseRTV(std::string res);
void ThrowError(const char* message);
int find_uid(char (*users)[6],char* uid);
void remove_user(char (*users)[6],char uid[]);
void split_buffer(char* str,int type,std::unordered_map<int,group> &groups,std::string logUID,std::string AGID);
void add_groups(char* str,std::unordered_map<int,group> &groups,std::string logUID);
void add_users(char* str,std::string &AGID,std::unordered_map<int,group> &groups);
int TimerOFF(int sd);
void split_msgs(std::unordered_map<int,group> &groups,char* AGID,char* str);
void split_str(std::string str,std::string &res,std::string &status);
void parse_msgs(char* str,int type,msg &mensagem);
void print_msgs(msg messages[],int n);
void empty_str(char str[],int len);
void printarray(char (*users)[6],int n);
void printgroups(std::unordered_map<int,group> &groups);
void printmap(std::unordered_map<int,group> &groups);
std::string calc_next(std::string num);
std::string busca_texto(char* str,char* num,std::string &text,int &flag,std::string &tok);
void parseMessagesRTV(char* str,char* agid,std::unordered_map<int,group> &groups,int limit);
void ParseMSGRTV2(char* str,char* agid,std::unordered_map<int,group> &groups);
void creates_files(msg messages[],int n);
void get_data(int fsize,char* fname,char* rest,int &client_tcp);

#endif //aux_functions.h

