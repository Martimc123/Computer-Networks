#ifndef CLIENT_CONSTANTS_H
#define CLIENT_CONSTANTS_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

//Constants
#define IP "tejo.tecnico.ulisboa.pt"
#define PORT "58011"
#define GPPORT "58048"
#define MAX_MESSAGE 1048576
#define MAX_LINE 320
#define MAX_BUFFER 1048576
#define MAX_USERS 32


//Structs

//Structure that represents a message
typedef struct msg{
    char MID[5];
    char UID[6];
    char textsize[11];
    char text[241];
    char fname[26];
    char fsize[11];
    char data[1025];
}msg;

//Structure that represents a group
typedef struct
{
    int num_users;
    char GNAME[26],GID[3];
    char UIDS[MAX_USERS][6];
    int num_msgs;
    msg messages[20];
}group;


#endif //client_constants.h