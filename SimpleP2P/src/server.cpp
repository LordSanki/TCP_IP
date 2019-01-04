#include <iostream>
#include <cstdlib>

#include <include/server.h>
#include <include/ClientSocket.h>
#include <include/SocketException.h>

#include <include/common.h>
using namespace std;

Server::Server(){

}

void Server::run() {
  cout<<"Server Started"<<endl;
  int p = -1;
  try {
    ServerSocket server ( 0 );
    db_server_port(server.getPortNum());
    p = fork();
    if(p != 0){
      run_db_server(server);
    }
  }
  catch(SocketException &e){
  }
  if(p == 0){
    run_login_server();
  }
}

void Server::run_login_server(){
  try
  {
    // Create the socket on advertised client port and start listening to it
    ServerSocket server ( SERVER_PORT );

    // accepting connection forever
    while ( true )
    {
      // create a new socket object
      ServerSocket new_sock;
      // accept incoming connection into newly created socket object
      // this is done so that the server socket is able to keep listening for new connections
      // and not be locked into one connections
      server.accept ( new_sock );
      const PeerID pid = server.get_client_addr();
      if( fork() ){
        continue;
      }
      else{ // Request server
        run_query_server(new_sock, pid);
        // exit from request server
        return;
      }
    }
  }
  catch ( SocketException& e )
  {
    std::cerr << "Login Server Shutdown: " << e.description() << "\nExiting.\n";
  }
}

void Server::run_db_server(ServerSocket & server){

  activePeers = new PeerHash();
  availableRFC = new RFCHash();

  try {
    // Create the socket on internal db port and start listening to it

    // accept connection forever
    while ( true ) {
      ServerSocket new_sock;
      // accept incoming connection into newly created socket object
      // this is done so that the server socket is able to keep listening for new connections
      // and not be locked into one connections
      server.accept ( new_sock );
      // serve one request per connection attempt
      try {
        string request_at_db_server;
        string response_from_db_server;
        PeerID pid;
        new_sock >> pid;
        new_sock << "OK"; 
        // accept query
        new_sock >> request_at_db_server;
        //DBG1(request_at_db_server);
        // process query
        // Server/Client Pair Registration
        /////////PeerID pid = server.get_client_addr();
        if(request_at_db_server.substr(0,3) == string("REG")) {
          // Register new server process and client
          if(request_at_db_server.substr(4,1) == string("A")){
            (*activePeers)[ pid ] = RFCList();
            response_from_db_server = "Added\r\n";
          }
          // unregister server process and client
          else if(request_at_db_server.substr(4,1) == string("D")){
            RFCList &rlist = (*activePeers)[ pid ];
            for(RFCList::iterator it = rlist.begin(); it != rlist.end();
                it++){
              RFCPtr ptr = *it;
              (ptr.first)->erase(ptr.second);
            }
            activePeers->erase( pid );
            response_from_db_server = "Deleted\r\n";
          }
        }
        else{
          response_from_db_server = resolve_query( request_at_db_server, pid );
        }
        //DBG1(response_from_db_server);
        // return result
        new_sock << response_from_db_server;
      }
      catch ( SocketException& ) {}
    }
  }
  catch ( SocketException& e )
  {
    std::cerr << "DB Server Shutdown: " << e.description() << "\nExiting.\n";
  }
  delete activePeers;
  delete availableRFC;
}

void Server::run_query_server(ServerSocket &sock, const PeerID pid){
  try
  {
    // Register with DB Server
    try{
      ClientSocket db_socket ( DB_SERVER_ADDRESS, db_server_port() );
      string data; 
      db_socket << pid;
      db_socket >> data;
      if(data == "OK"){
        data = "REG A\r\n";
        db_socket << data;
        db_socket >> data;
      }
      else{
        throw SocketException("Unable to login");
      }
    }
    catch ( SocketException& e) {
      cerr << "Query Server Unable to open DB: "<< e.description() << "\nExiting.\n";
      return;
    }
    while ( true )
    {
      std::string client_query;
      std::string server_response;
      // receive request from client
      sock >> client_query;
      print_req(client_query);
      // create connection with db server
      ClientSocket db_socket ( DB_SERVER_ADDRESS, db_server_port() );
      try
      {
        db_socket << pid;
        db_socket >> server_response;
        if(server_response != "OK"){ throw SocketException("unable to login"); }
        // forward request to db server
        db_socket << client_query;
        // accept reply from db server
        db_socket >> server_response;
      }
      catch ( SocketException& e) {
        cerr << "Query Server error with DB connection: "<< e.description() << "\nExiting.\n";
        return;
      }
      print_reply(server_response);
      // forward reply to client
      sock << server_response;
    }
  }
  catch ( SocketException& e) {
    //cerr << "Query Server shutdown: "<< e.description() << "\nExiting.\n";
  }
  // unregister with DB Server
  {
    try{
      ClientSocket db_socket ( DB_SERVER_ADDRESS, db_server_port() );
      string data;
      db_socket << pid;
      db_socket >> data;
      data = "REG D \r\n";
      db_socket << data;
      db_socket >> data;
    }
    catch ( SocketException& e) {
      cerr << "Query Server Unable to close DB: "<< e.description() << "\nExiting.\n";
    }
  }
}

std::string Server::resolve_query(const std::string &query, const PeerID &peer){

  Request req = Request(query);
  string response;
  switch(req.method()){

    case Request::P2S_ADD: 
      {
        AddMethod am = AddMethod(req); 
        addRFC(am, peer);
        response = am.response();
        break;
      }

    case Request::P2S_LIST: 
      {
        ListMethod lm = ListMethod(req);
        listRFC(lm);
        response = lm.response();
        break;
      }
    case Request::P2S_LOOKUP:
      {
        LookupMethod lm = LookupMethod(req);
        lookupRFC(lm);
        response = lm.response();
        break;
      }
    case Request::none:
    default:
      break;
  }

  // read query type
  return response;
}

void Server::addRFC(AddMethod &am, const PeerID &peer){
  RFCPtr rptr;
  if(availableRFC->find(am.rfc()) == availableRFC->end()){
    DataList dl; Metadata d;
    d.id = peer; d.port = am.port(); d.host = am.host(); d.title = am.title();
    dl.push_back(d);
    (*availableRFC)[am.rfc()] = dl;
    rptr = RFCPtr(&((*availableRFC)[am.rfc()]), (*availableRFC)[am.rfc()].begin());
  }
  else{
    Metadata d;
    d.id = peer; d.port = am.port(); d.host = am.host(); d.title = am.title();
    (*availableRFC)[am.rfc()].push_back(d);
    rptr = RFCPtr(&((*availableRFC)[am.rfc()]), --((*availableRFC)[am.rfc()].end()));
  }
  (*activePeers)[peer].push_back(rptr);
}

void Server::lookupRFC(LookupMethod &lm){
  if(availableRFC->find(lm.rfc()) == availableRFC->end()){
    lm.status("404 Not Found");
    return;
  }
  DataList &dl = (*availableRFC)[lm.rfc()];
  for(DataList::iterator it = dl.begin();
      it != dl.end(); it++){
    Metadata &d = (*it);
    lm.append(lm.rfc(), d.title, d.host, d.port);
  }
}

void Server::listRFC(ListMethod &lm){

  for(RFCHash::iterator it = availableRFC->begin(); 
        it != availableRFC->end(); it++){
    DataList &dl = it->second;
    for(DataList::iterator dlit = dl.begin();
        dlit != dl.end(); dlit++){
      Metadata &d = (*dlit);
      lm.append(it->first, d.title, d.host, d.port);
    }
  }
}

