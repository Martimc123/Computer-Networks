#ifndef DS_CONST_H
#define DS_CONST_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <map>
#include <ftw.h>

using namespace std;

//#define IP "localhost"
#define PORT "58048"
//#define PORT "81"

#define MAX_BUFFER 1024

char DSport[20];

char buffer[MAX_BUFFER];
char bufferToPrint[MAX_BUFFER];

char UDP_recMSG[MAX_BUFFER];
char UDP_sendMSG[MAX_BUFFER];

char TCP_recMSG[MAX_BUFFER];
char TCP_sendMSG[MAX_BUFFER];

char received_UID[6], received_password[9], received_GID[3], received_Gname[25];

int UDP_fd, errcode;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;

int TCP_fd, newfdTCP;
struct addrinfo hintsTCPserver, *resTCPserver;
struct sockaddr_in addrTCPserver;
socklen_t addrTCPserverlen;
// source_info *fds;

int verbose_mode = 1;

struct timeval tv = {900, 0};

typedef struct
{
    char GID[3];
    char Gname[25];
    char MID[5];
    int n_mensagens;

} all_group;

typedef struct
{
    char UID[6];
} all_user;

int n_group;

void ParseArgs(int argc, char *argv[]);
/* Function Server with TCP connection and UDP conection, select and execute them*/
void select();

/* UDP conection*/
void UDP_Server();
void UDP_Server_Recvfrom();
void UDP_Server_Sendto();

/* UDP command*/
void UDP_Server_Response();

/* UDP following the reg command, the User application sends the user ID UID and the selected password pass for registration at the DS server*/
void UDP_Register_User();
/* Check if UID is correct return true
 * Inputs: - UID
 */
bool is_ok_UID(char *received_UID);
/* Check if pass is correct return true
 * Inputs: - pass
 */
bool is_ok_password(char *received_password);
/* Check if UID is registered return 1
 * Inputs: - UID
 */
int has_user_registered(char *received_UID);
/* Create the directory with received UID and file with UID_pass.txt
 * Inputs: - UID, pass
 */
void create_dir_and_file_user(char *received_UID, char *received_password);

/* UDP following the unregister command, the User application asks the DS to unregister the user with UID and whose password is pass.*/
void UDP_Unregister_User();
/* Check if password is registered and correct return 1
 * Inputs: - UID and pass
 */
bool check_password(char *received_UID, char *received_password);
/* Check if UID is logined return 1
 * Inputs: - UID
 */
bool check_exist_login(char *received_UID);
/* Remove the directory with the name of UID
 * Inputs: - UID, pass
 */
void remove_dir(char *received_UID, char *received_password);
/* Remove Directory if it is not empty, the function ignore*/
static int rmFiles(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb);

/* UDP following the login command, the User application sends the DS server a message with the user’s UID and password pass for validation.*/
void UDP_Login_User();
/* Create file with the name of UID_login.txt
 * Inputs: - UID
 */
void create_file(char *received_UID);

/* UDP following the logout command, the User application sends the DS server a message with the user’s UID and password pass for validation.*/
void UDP_Logout_User();
/* Delete file with the name of UID_login.txt
 * Inputs: - UID
 */
void delete_file(char *received_UID);

/* UDP ollowing the groups command, the User application sends the DS a request asking for the list of existing groups.*/
void UDP_List_Groups();
/* Check group has created return true
 * Inputs: - GID
 */
bool check_group_created(char *received_GID);
/* Check GID is valid return true
 * Inputs: - GID
 */
bool is_ok_GID(char *GID);
/* Check Gname is valid return true
 * Inputs: - GID
 */
bool is_ok_Gname(char *Gname);

/*UDP following the subscribe command, the User application sends the DS a request asking for user UID to join the group with GID and GName. If GID = 00 this corresponds to a request to create a new group with name GName.*/
void UDP_Subscribe_Groups();
/* Create directory with the name GID and file with GID_name.txt(and save Gname)
 * Inputs: - GID and Gname
 */
void create_dir_and_file_group(char *GID, char *received_Gname);
/* Create file UID in directory GID
 * Inputs: -UID and GID
 */
void create_UID_file_group(char *UID, char *GID);
/* Check GID_name.txt exists in directory GID return true
 * Inputs: - GID and Gname
 */
bool check_Gname(char *received_GID, char *received_Gname);

/*UDP following the unsubscribe command, the User application sends the DS a request asking for user UID to unsubscribe the group GID.*/
void UDP_Unsubscribe_Groups();
/* Check UID.txt exists in directory GID return true
 * Inputs: - GID and Gname
 */
bool check_User_subscribed_group(char *received_UID, char *received_GID);
/* Delete UID.txt in directory Gid
 * Inputs: - GID and UID
 */
void delete_uid_file_in_GROUPS(char *received_UID, char *received_GID);

/*UDP following the my_groups command, the User application sends the DS a request asking for the list of groups to which user UID has already subscribed.*/
void UDP_List_My_Groups();

/*TCP connection*/
void TCP_Server();
void TCP_write();
/*TCP command*/
void TCP_Server_Response();
/*TCP following the ulist command, the User application opens a TCP connection with the DS server and asks for the list of users subscribed to group GID.*/
void TCP_List_User_Subscribed();
/*TCP following the post command, the User application opens a TCP connection
with the DS server to send a text message and optionally also a file. The text message text contains Tsize characters.*/
void TCP_Post();
/* Save the received text or file in a directory GID->UID
 * Inputs: - GID,UID,Tsize,text,Fname,Fsize,flag
 */
char *save_message(char *received_UID, char *received_GID, int Tsize, char *text, char *Fname, int Fsize, int flag);

void TCP_Retrieve();

#endif