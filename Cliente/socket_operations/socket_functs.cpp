#include "socket_functs.h"
#include "../auxiliar_code/aux_functions.h"

//Sets up the client UDP socket
void setsocketUDP(int &client,struct addrinfo &hintsClient)
{
    //creates UDP socket
    client = socket(AF_INET, SOCK_DGRAM, 0);
    if (client == -1)
        ThrowError("Error: in creating UDP client socket\n");

    //clears the bytes of the hintsClient address and assigns its family and socket type (UDP)
    memset(&hintsClient, 0, sizeof(hintsClient));
    hintsClient.ai_family = AF_INET;
    hintsClient.ai_socktype = SOCK_DGRAM;

}

//Sets up the client TCP socket
void setsocketTCP(int &client_tcp,struct addrinfo &hintsClientTCP)
{
    //creates TCP socket
    client_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if (client_tcp == -1)
        ThrowError("Error: in creating TCP client socket\n");

    //clears the bytes of the hintsClient address and assigns its family and socket type (TCP)
    memset(&hintsClientTCP, 0, sizeof(hintsClientTCP));
    hintsClientTCP.ai_family = AF_INET;
    hintsClientTCP.ai_socktype = SOCK_STREAM;
}

int TimerON(int sd)
{
    struct timeval tmout;
    memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
    tmout.tv_sec=2; /* Wait for 15 sec for a reply from server. */
    return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tmout,sizeof(struct timeval)));
}

int TimerOFF(int sd)
{
    struct timeval tmout;
    memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
    return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tmout,sizeof(struct timeval)));
}

void sendmsgUDP(char *message,int &client,struct addrinfo* &resClient,struct sockaddr_in &addr,std::string &response)
{   
    ssize_t n;
    socklen_t addrlen;
    char buffer[MAX_BUFFER];
    bzero(buffer,MAX_BUFFER*sizeof(char));
    //sends a message to server
    n = sendto(client, message, strlen(message), 0, resClient->ai_addr, resClient->ai_addrlen);
    if (n == -1)
        ThrowError("Error: in sending a message to the server\n");
        
    addrlen = sizeof(addr);

    n=TimerON(client);
    if (n == -1)
        ThrowError("Error: in activating timer\n");
        
    //receives message a from the server
    n = recvfrom(client, buffer,MAX_BUFFER, 0, (struct sockaddr *)&addr, &addrlen);
    if (n == -1) /*error*/
    {

        //if a packet is lost, it retransmits the message from the client to the server
        //after a timeout of 15 seconds
        std::cout << "Error: Message lost, retransmiting message\n";
        n = sendto(client, message, strlen(message), 0, resClient->ai_addr, resClient->ai_addrlen);
        if (n == -1)
            ThrowError("Error: in sending a message to the server\n");
        std::cout << "Retransmited message has been sent\n";
    }
    n=TimerOFF(client);
    if (n == -1)
        ThrowError("Error: in deactivating timer\n");

    response = std::string(buffer);
    bzero(buffer,MAX_BUFFER*sizeof(char));
}

void sendmsgTCPRTV(char *message,int &client_tcp,std::string &status,std::string &response)
{
    ssize_t n1,n2;
    char bufferTCP[MAX_BUFFER];
    char bufferTCP2[MAX_BUFFER];

    //sends a message to the server
    n1 = write(client_tcp,message, strlen(message));
    if (n1 == -1)
        ThrowError("Error: in sending a message to the server\n");
            
    //recives a message from the server
    n1 = read(client_tcp,bufferTCP,MAX_BUFFER);
    if (n1 == -1)
        ThrowError("Error: in reading the buffer from the server\n");

    response[n1]='\0';

    n2 = read(client_tcp,bufferTCP2,MAX_BUFFER);
    if (n2 == -1)
        ThrowError("Error: in reading the buffer from the server\n");
    
    response[n2] ='\0';

    status = std::string(bufferTCP);
    response = std::string(bufferTCP2);
}

void sendmsgTCPUL(char *message,int &client_tcp,std::string &status)
{
    ssize_t n;
    char bufferTCP[MAX_BUFFER];
    
    //sends a message to the server
    n = write(client_tcp,message, strlen(message));
    if (n == -1)
        ThrowError("Error: in sending a message to the server\n");
            
    //recives a message from the server
    n = read(client_tcp,bufferTCP,MAX_BUFFER);
    if (n == -1)
        ThrowError("Error: in reading the buffer from the server\n");
    bufferTCP[n]='\0';
    status = std::string(bufferTCP);
}

void sendmsgTCPPST(char *message,int &client_tcp,std::string &status)
{
    ssize_t n;
    char bufferTCP[MAX_BUFFER];
    
    //sends a message to the server
    n = write(client_tcp,message, strlen(message));
    if (n == -1)
        ThrowError("Error: in sending a message to the server\n");
            
    //recives a message from the server
    n = read(client_tcp,bufferTCP,MAX_BUFFER);
    if (n == -1)
        ThrowError("Error: in reading the buffer from the server\n");
    
    status = std::string(bufferTCP);
}

/*void sendmsgPOST_imag(std::string message,char *fname,int &client_tcp,std::string &status)
{
    struct stat file;
    ssize_t nbytes,nleft,nwritten,nread;
        //char* ptr;

        //char buffer[128];
        //ptr=buffer;
    stat(fname, &file);
    nbytes= file.st_size;
        
    FILE *fp = fopen(fname, "rb");

    int n1;
    n1 = write(client_tcp,message.c_str(),message.size());
    if (n1 == -1)
        ThrowError("Error: in sending a message to the server\n");
    
    char buffer[128];
    nleft=nbytes;
    int size = 128;
    std::cout << "Bytes left: " << nleft << '\n';
    while(nleft>0)
    {
        if (nleft < size)
            size=nleft;
        nread=fread(buffer,sizeof(unsigned char),size,fp);
        if (nread==0) break;
        nwritten=write(client_tcp,buffer,size);
        std::cout << "Bytes written: " << nwritten << '\n';
        nleft-=nwritten;
        std::cout << "Bytes left: " << nleft << '\n';
    }
    write(client_tcp,"\n",strlen("\n"));

    char bufferTCP[MAX_BUFFER];
    n1 = read(client_tcp,bufferTCP,MAX_BUFFER);
    if (n1 == -1)
        ThrowError("Error: in reading the buffer from the server\n");
    
    status = std::string(bufferTCP);
}*/