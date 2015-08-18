/*
Author:Ryan - David Reyes
*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "tcpipnix.h"
const int INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;

/*
   const int SCK_VERSION1 = 0x0101;
   const int SCK_VERSION2 = 0x0202;

#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3

#define AF_INET 2

#define IPPROTO_TCP 6
*/

TCP::TCP()
{
  //ctor
}

TCP::~TCP()
{
  if (server_socket_)
    close(server_socket_);
  //dtor
}

unsigned int TCP::connectToHost(const char* IPAddress, const int portNo)
{
  struct sockaddr_in target;

  //fill out info needed to initialize a socket
  //address internet
  target.sin_family = AF_INET;

  //port to conect on (host to network)
  target.sin_port = htons(portNo);
  target.sin_addr.s_addr = inet_addr(IPAddress);  //target ip

  //create the socket
  server_socket_ = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket_ == INVALID_SOCKET)
    return false;       //couldn't create the socket


  if ( connect( server_socket_,
        (struct sockaddr*)&target,
        sizeof(target)) == SOCKET_ERROR)
    return false;       //couldnt connect

  else
    return server_socket_;
}

bool TCP::listenToPort(const int portNo)
{
  struct sockaddr_in addr;

  addr.sin_family = AF_INET;

  addr.sin_port = htons(portNo);
  addr.sin_addr.s_addr = htonl (INADDR_ANY);
  //assign port to this socket

  //accept a connection from any IP using INADDR_ANY
  //or just pass inet_addr("0.0.0.0")
  //specify a specific ip address to watch for only that address

  server_socket_ = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (server_socket_ == INVALID_SOCKET)
    return false;

  if (bind( server_socket_,
        (struct sockaddr*) &addr,
        sizeof(addr)) == SOCKET_ERROR)

    return false;
  // will happen if you try to bind to same socket multiple times

  //listen to the port with max connections possible
  //function does not return until a connection request is made
  // u_long iMode=1;
  //ioctlsocket(s,FIONBIO,&iMode);

  listen(server_socket_, SOMAXCONN);
  return true;
}

unsigned int TCP::acceptConnection()
{
  unsigned int TempSock = SOCKET_ERROR;
  TempSock = accept(server_socket_, NULL, NULL);
  return TempSock;
}

int TCP::sendData(unsigned int writeTo, char * data, const int len)
{
  return send(writeTo, data, len, 0);
}

int TCP::receiveData(unsigned int readFrom, char * buffer, const int len)
{
  return recv(readFrom, buffer, len, 0);
}

//Sends a 4 byte packed integer. Sending one byte at a time
//is more reliable than sending an int in one shot.
int TCP::sendFramedData(unsigned int writeTo, char* data, const int len)
{
  char lenBuff[4] = {0};

  lenBuff[0] = (unsigned char) len;
  lenBuff[1] = (unsigned char) (len >> 8);
  lenBuff[2] = (unsigned char) (len >> 16);
  lenBuff[3] = (unsigned char) (len >> 24);

  this->sendData(writeTo, lenBuff, 4);
  return this->sendData(writeTo, data, len);
}


int TCP::receiveFramedData(unsigned int readFrom, char * data)
{
  char lenBuff[4];
  char * dataBuff;
  int lenPrefix;
  int totalRead;
  int currentRead;

  //priming read:
  currentRead = totalRead = this->receiveData(readFrom, lenBuff, 4);

  //read if and while not enough data received, until all data arrives
  while (currentRead > 0 && totalRead < 4)
  {
    currentRead = this -> receiveData(  readFrom,
        (lenBuff + totalRead),
        (4 - totalRead));
    totalRead += currentRead;
  }

  //dereferenced casted int pointer to char array
  lenPrefix = *(int *) lenBuff;
  //reset current and total read
  currentRead = 0;
  totalRead = 0;

  //dataBuff = new char [lenPrefix];

  //priming read:
  currentRead = totalRead = this->receiveData(readFrom, data, lenPrefix);

  //read if and while not enough data received, until all data arrives
  while (currentRead > 0 && totalRead < lenPrefix)
  {
    currentRead = this -> receiveData(  readFrom,
        (data + totalRead),
        (lenPrefix - totalRead));
    totalRead += currentRead;
  }


  //delete [] dataBuff;
  //dtor
  return totalRead;
}
