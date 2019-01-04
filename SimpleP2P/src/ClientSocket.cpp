// Implementation of the ClientSocket class

#include "ClientSocket.h"
#include "SocketException.h"


ClientSocket::ClientSocket ( std::string host, int port )
{
  if ( ! Socket::create() )
    {
      throw SocketException ( "Could not create client socket." );
    }

  if ( ! Socket::connect ( host, port ) )
    {
      throw SocketException ( "Could not bind to port." );
    }

}


const ClientSocket& ClientSocket::operator << ( const std::string& s ) const
{
  if ( ! Socket::send ( s ) )
    {
      throw SocketException ( "Could not write to socket." );
    }

  return *this;

}


const ClientSocket& ClientSocket::operator >> ( std::string& s ) const
{
  if ( ! Socket::recv ( s ) )
    {
      throw SocketException ( "Could not read from socket." );
    }

  return *this;
}

void ClientSocket::raw_send(const void *buf, unsigned int size) const 
{
  if ( ! Socket::send ( buf, size ) )
  {
    throw SocketException ( "Could not read from socket." );
  }
}

void ClientSocket::raw_recv(void **buf, unsigned int &size) const 
{
  if ( ! Socket::recv ( buf, size ) )
  {
    throw SocketException ( "Could not read from socket." );
  }
}

std::string ClientSocket::get_self_addr(){
  return Socket::get_self_addr();
}
