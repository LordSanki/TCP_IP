#ifndef __SOCKET_H__
#define __SOCKET_H__
#include <string>
#include <sys/socket.h>
#include <include/Common.h>
#include <arpa/inet.h>

class Socket
{
  protected:
    int m_sock;
    struct sockaddr_in m_sin;
  
  public:
  Socket();
  ~Socket();
  void operator<<(const Packet &p);
  void operator>>(Packet &p);
};

class ClientSocket : public Socket {
  public:
    ClientSocket(std::string server_addr, int server_port);
};

class ServerSocket : public Socket {
  public:
    ServerSocket(int listen_port);
};

#endif //__SOCKET_H__
