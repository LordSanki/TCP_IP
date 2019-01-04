#ifndef __P2P_SERVER_H__
#define __P2P_SERVER_H__
#include <map>
#include <list>
#include <string>
#include <include/addmethod.h>
#include <include/listmethod.h>
#include <include/lookupmethod.h>
#include <include/getmethod.h>

#include <include/ServerSocket.h>
class Server {
  public:
    Server();
    void run();

  private:
    typedef std::string PeerID;
    struct Metadata{
      int port;
      std::string host;
      PeerID id;
      std::string title;
    };
    typedef std::list < Metadata > DataList;
    typedef std::pair < DataList *, DataList::iterator > RFCPtr;
    typedef std::list < RFCPtr > RFCList;
    typedef std::map < PeerID , RFCList > PeerHash;
    typedef std::map < std::string, DataList > RFCHash;
    PeerHash *activePeers;
    RFCHash *availableRFC;

    void run_login_server();
    void run_db_server(ServerSocket &server);
    void run_query_server(ServerSocket &sock, const PeerID pid);
    std::string resolve_query(const std::string &query, const PeerID &peer);
    void listRFC(ListMethod &lm);
    void addRFC(AddMethod &am, const PeerID &peer);
    void lookupRFC(LookupMethod &lm);
};


#endif //__P2P_SERVER_H__

