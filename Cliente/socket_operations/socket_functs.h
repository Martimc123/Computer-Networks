#ifndef SOCKET_FUNCTS_H
#define SOCKET_FUNCTS_H

#include "../client_constants.h"
#include <string>

void setsocketTCP(int &client_tcp,struct addrinfo &hintsClientTCP); //used in client
void setsocketUDP(int &client,struct addrinfo &hintsClient); //used in client
int TimerON(int sd);
void sendmsgUDP(char *message,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr,std::string &response);
//void sendmsgTCP(char *message,int type,int &client_tcp,std::string &status,std::string &response);
void sendmsgTCPUL(char *message,int &client_tcp,std::string &status);
void sendmsgTCPPST(char *message,int &client_tcp,std::string &status);
void sendmsgTCPRTV(char *message,int &client_tcp,std::string &status,std::string &response);

#endif