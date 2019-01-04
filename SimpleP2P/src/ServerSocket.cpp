// Implementation of the ServerSocket class

#include "ServerSocket.h"
#include "SocketException.h"
#include <sstream>

ServerSocket::ServerSocket ( int port )
{
  if ( ! Socket::create() )
    {
      throw SocketException ( "Could not create server socket." );
    }

  if ( ! Socket::bind ( port ) )
    {
      throw SocketException ( "Could not bind to port." );
    }

  if ( ! Socket::listen() )
    {
      throw SocketException ( "Could not listen to socket." );
    }

}

ServerSocket::~ServerSocket()
{
}


const ServerSocket& ServerSocket::operator << ( const std::string& s ) const
{
  if ( ! Socket::send ( s ) )
    {
      throw SocketException ( "Could not write to socket." );
    }

  return *this;

}


const ServerSocket& ServerSocket::operator >> ( std::string& s ) const
{
  if ( ! Socket::recv ( s ) )
    {
      throw SocketException ( "Could not read from socket." );
    }

  return *this;
}

void ServerSocket::accept ( ServerSocket& sock )
{
  if ( ! Socket::accept ( sock ) )
    {
      throw SocketException ( "Could not accept socket." );
    }
}

std::string ServerSocket::get_client_addr(){
  const sockaddr_in addr = Socket::get_client_addr();
  std::stringstream ret;
  ret << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port);
  return ret.str();
}

std::string ServerSocket::get_self_addr(){
  return Socket::get_self_addr();
}

void ServerSocket::raw_send(const void *buf, unsigned int size) const 
{
  if ( ! Socket::send ( buf, size ) )
  {
    throw SocketException ( "Could not read from socket." );
  }
}

void ServerSocket::raw_recv(void **buf, unsigned int &size) const
{
  if ( ! Socket::recv ( buf, size ) )
  {
    throw SocketException ( "Could not read from socket." );
  }
}

int ServerSocket::getPortNum(){
  int p = Socket::getPortNum();
  if(p < 1){
    throw SocketException("Unable to get port address");
  }
  return p;
}

