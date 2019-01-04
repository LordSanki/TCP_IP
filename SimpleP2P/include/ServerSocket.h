// Definition of the ServerSocket class

#ifndef ServerSocket_class
#define ServerSocket_class

#include "Socket.h"
#include <string>

class ServerSocket : private Socket
{
 public:

  ServerSocket ( int port );
  ServerSocket (){};
  virtual ~ServerSocket();

  const ServerSocket& operator << ( const std::string& ) const;
  const ServerSocket& operator >> ( std::string& ) const;
  void raw_send(const void *buf, unsigned int size) const;
  void raw_recv(void **buf, unsigned int &size) const;
  void accept ( ServerSocket& );
  std::string get_client_addr();
  std::string get_self_addr();
  int getPortNum();
};


#endif
