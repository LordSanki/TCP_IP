#include <iostream>
#include <cstring>
#include <cstdlib>
#include <include/Socket.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <exception>
#include <errno.h>
#include <unistd.h>

using namespace std;

void terror(std::string s, int e = -1){
  if(e == -1)
    cerr<<s<<endl;
  else
    cerr<<s<<" \tErrorNo:"<<e<<endl;
  exit(0);
}

ServerSocket::ServerSocket(int listen_port){
  
  sockaddr_in sin_me;

  m_sock = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(m_sock == -1){
    terror("Unable to create socket");
  }
  bzero(&m_sin, sizeof(m_sin));

  bzero(&m_sin, sizeof(m_sin));

  sin_me.sin_family = AF_INET;
  sin_me.sin_port = htons(listen_port);
  sin_me.sin_addr.s_addr = htonl(INADDR_ANY);

  //bind socket to port
  if( bind(m_sock , (struct sockaddr*)&sin_me, sizeof(sin_me) ) == -1){
    terror("Unable to bind socket to port", errno);
  }
}

ClientSocket::ClientSocket(std::string server_addr, int server_port){
  m_sock = ::socket(AF_INET, SOCK_DGRAM|SOCK_NONBLOCK, IPPROTO_UDP);
  if(m_sock == -1){
    terror("Unable to create socket");
  }
  bzero(&m_sin, sizeof(m_sin));
  m_sin.sin_family = AF_INET;
  m_sin.sin_port = htons(server_port);
  int ret = inet_aton(server_addr.c_str(), &m_sin.sin_addr);
  
  if(ret == 0){
    server_addr = hostname_to_ip(server_addr);
    ret = inet_aton(server_addr.c_str(), &m_sin.sin_addr);
    if(ret == 0){
      terror("Invalid server address expecting a.b.c.d format");
    }
  }
}

Socket::Socket(){
  m_sock = -1;
}

Socket::~Socket(){
  if(m_sock != -1){
    ::close(m_sock);
  }
}

void Socket::operator<<(const Packet &p)
{
  do{
    int bytes_sent = ::sendto(m_sock, p.buff, p.size,0, (struct sockaddr*)&m_sin, sizeof(m_sin));
    if(bytes_sent == -1){
      if(errno != EWOULDBLOCK && errno != EAGAIN){
        terror("Unable to send data", errno);
      }
    }
    else{
      break;
    }
    usleep(10*1000);
  }while(errno == EWOULDBLOCK || errno == EAGAIN);
}

#define MAX_BUF_LEN 2048
void Socket::operator>>(Packet &p)
{
  uchar *buff = (uchar*)malloc(MAX_BUF_LEN);
  socklen_t slen = (socklen_t)sizeof(m_sin);
  int bytes_read = recvfrom(m_sock, buff, MAX_BUF_LEN, 0, (struct sockaddr *) &m_sin, &slen);
  if(bytes_read == -1){
    free(buff);
    p.buff = NULL;
    p.size = 0;
    if(errno == EWOULDBLOCK || errno == EAGAIN){
      return;
    }
    else{
      terror("Unable to receive data", EWOULDBLOCK);
    }
  }
  p.buff = (void*)buff;
  p.size = bytes_read;
}

