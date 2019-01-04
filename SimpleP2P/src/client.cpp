#include <string>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <signal.h>

#include <include/ClientSocket.h>
#include <include/SocketException.h>
#include <include/ServerSocket.h>

#include <sys/prctl.h>
#include <include/common.h>
#include <include/client.h>

using namespace std;

Client::Client(std::string path){
  m_rfc_dir = path;
  checkRFCDir();
}

void Client::checkRFCDir(){
  struct stat sb;

  if (stat(m_rfc_dir.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
  {
    scanForFiles();
  }
  else{
    string cmd = string("mkdir -p ");
    cmd += m_rfc_dir;
    int ret = system(cmd.c_str());
    if(ret != 0) DBG2("MKDIR failed");
  }
}

void Client::scanForFiles(){
  DIR *dir;
  struct dirent *ent;
  m_local_rfc.clear();
  if ((dir = opendir (m_rfc_dir.c_str())) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      if( ent->d_type==DT_REG){
        m_local_rfc.push_back( Client::RFC_Info(m_rfc_dir + string("/") + string(ent->d_name)) );
      }
    }
    closedir (dir);
  }
  else{
    cout << "Unable to open directory " << m_rfc_dir << endl <<"EXITING !!!"<<endl;
    exit(0);
  }
}

void Client::advertizeLocalRFC(ClientSocket &conn){
  for(unsigned int i=0; i<m_local_rfc.size(); i++){
    addRequest(conn,m_local_rfc[i]); 
  }
}

void Client::addRequest(ClientSocket &conn, Client::RFC_Info r){
  AddMethod am;
  am.rfc(r.rfc);
  am.title(r.title);
  string s = am.request();
  print_req(s);
  conn << s;
  conn >> s;
  print_reply(s);
}

LookupMethod Client::lookupRequest(ClientSocket &conn, vector<string> &opts){
  LookupMethod lm;
  lm.rfc(opts[0]);
  if(opts.size() > 1)
    lm.title(opts[1]);
  string s = lm.request();
  print_req(s);
  conn << s;
  conn >> s;
  opts.clear();
  print_reply(s);
  return LookupMethod(Response(s));
}

void Client::listRequest(ClientSocket &conn){

  ListMethod lm;
  string s = lm.request();
  print_req(s);
  conn << s;
  conn >> s;
  print_reply(s);
}

void Client::listLocal(){
  stringstream ss;
  for(unsigned int i=0; i<m_local_rfc.size(); i++){
    ss<<m_local_rfc[i].rfc << " " <<m_local_rfc[i].title << endl;
  }
  print_reply(ss.str());
}

bool Client::isFilePresent(string filename){
  ifstream infile(filename.c_str());
  if (infile.good()){
    return true;
  }else {
    return false;
  }
}

Client::RFC_Info Client::copyFile(vector <string> &opts){
  RFC_Info r = RFC_Info(opts[0], opts[1], opts[2]);
  r.filename = m_rfc_dir + string("/") + r.filename;
  
  if(copyFileOnDisk(opts[2], r.filename)){
    scanForFiles();
  }
  else{
    DBG1(r.filename);
    DBG1(opts[2]);
    DBG2("failed to copy file");
  }
  return r;
}

void Client::getResponse(GetMethod &gm){
  RFC_Info *r = NULL;
  for(unsigned int i=0; i<m_local_rfc.size(); i++){
    if(m_local_rfc[i].rfc == gm.rfc()){
      r = &(m_local_rfc[i]);
      break;
    }
  }
  if(r == NULL){
    gm.status("404 Not Found");
  }
  else{
    gm.read_file(r->filename);
  }
}

void Client::getRequest(ClientSocket &conn, std::vector<string> &opts){
  GetMethod gm;
  string host, title;
  int port;
  if(opts.size() > 3 ){
    gm.rfc(opts[0]);
    gm.title(opts[1]);
    host = opts[2];
    port = atoi(opts[3].c_str());
  }
  else{
    LookupMethod lm = lookupRequest(conn, opts);
    if(lm.data().size() > 0){
      host = lm.data()[0].host;
      port = lm.data()[0].port;
      gm.title(lm.data()[0].title);
      gm.rfc(lm.data()[0].rfc);
    }
    else{
      return;
    }
  }
  string s = gm.request();
  print_req(s);
  try{
    char * buf = NULL;
    unsigned int size;
    ClientSocket p2p ( host, port );
    p2p << s;
    p2p.raw_recv((void**)&buf, size);
    GetMethod gm2 = GetMethod(buf, size);
    free(buf);
    if(gm2.status() == "200 OK"){
      if(gm2.contentLength() != gm2.file_size()){
        DBG1(gm2.contentLength());
        DBG1(gm2.file_size());
      }
      RFC_Info rinfo = RFC_Info(gm.rfc(), gm.title(), string(".txt"));
      rinfo.filename = m_rfc_dir + string("/") + rinfo.filename;
      gm2.write_file(rinfo.filename);
      scanForFiles();
      print_reply(gm2.response_msg());
      addRequest(conn, rinfo);
    }
    else{
      print_reply(gm2.response_msg());
    }
  }
  catch(SocketException &e){
  }
}

void Client::run(Client::ClientCallback func){
  int pid = -1;
  signal(SIGCHLD, SIG_IGN);
  // upload socket scope
  try{
    ServerSocket server ( 0 );
    uploadPortNo(server.getPortNum());
    pid = fork();
    if(pid == 0){
      run_upload_server(server);
    }
  }
  catch(SocketException &e){}
  // upload socket unbound for regular client
  if( pid != 0 ){
    // Register with DB Server
    try{
      // login to network here
      ClientSocket connection ( server_address(), server_port() );
      setLocalHost(connection.get_self_addr()); 
      // inform the server about local rfc
      advertizeLocalRFC(connection);

      bool forever = true;
      vector<std::string> opts;
      // forever loop
      while(forever){
        Method m;
        // get command from user
        switch( func(opts) ){
          case LIST_REMOTE:
            listRequest(connection);
            break;
          case LIST_LOCAL:
            listLocal();
            break;
          case GET:
            getRequest(connection, opts);
            break;
          case LOOKUP:
            m = lookupRequest(connection, opts);
            break;
          case ADD:
            addRequest(connection, copyFile(opts));
            break;
          case EXIT:
            forever = false;
            continue;
            break;
          default:
            continue;
            break;
        }
      }
    }
    catch ( SocketException& e) {
      cerr << "Connection closed: "<< e.description() << "\nExiting.\n";
      return;
    }
  }
}

void Client::run_upload_server(ServerSocket &server) {
  prctl(PR_SET_PDEATHSIG, SIGKILL);
  // run upload server
  try
  {
    // Create the socket on advertised client port and start listening to it
    // signal kernel not to keep any zombie proceses and reap them on exit
    
    // accepting connection forever
    while ( true )
    {
      try{
        ServerSocket new_sock;
        server.accept ( new_sock );
        string req;
        new_sock >> req;
        GetMethod gm = GetMethod(req);
        scanForFiles();
        getResponse(gm);
        new_sock.raw_send( gm.response_buf(), gm.response_size() );
      }
      catch(SocketException &e){}
    }
  }
  catch ( SocketException& e )
  {
    std::cerr << "Client Upload Server Shutdown: " << e.description() << "\nExiting.\n";
  }

}


