/*
Author:Ryan - David Reyes
*/
#ifndef TCPIPNIX_H_
#define TCPIPNIX_H_
#include <unistd.h>

/*
   The socket that you do data operations on if you're the client and
   have used connectToHost is servSock.

   If you're the server the socket you use for data operations is
   the one returned from acceptConnection().
   */

class TCP
{
  public:
    TCP();
    ~TCP();
    unsigned int connectToHost    (const char * IPAddress, const int portNo);
    bool listenToPort             (const int portNo);
    unsigned int acceptConnection ();

    int sendData    (unsigned int writeTo,  char * data, const int len);
    int receiveData (unsigned int readFrom, char * data, const int len);

    /*
       The folowing functions send and receive data
       framed with a 4 byte packet that specifies
       the size of that data
       */

    int sendFramedData    (unsigned int writeTo, char * data, const int len);
    int receiveFramedData (unsigned int readFrom, char * data);

    int getSocket         () const {return server_socket_;}
    int closeSocket       (unsigned int socket) {return close(socket);}

  private:
    unsigned int server_socket_; //socket handle
};

#endif // TCPIPNIX_H_
