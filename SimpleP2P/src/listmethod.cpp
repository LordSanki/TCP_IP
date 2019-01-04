#include <include/listmethod.h>
#include <include/common.h>
#include <sstream>
#include <cstdlib>
using namespace std;

ListMethod::ListMethod(const Request &_req){
  Method::request(_req);
  host(getHost());
  port(getPort());
  version(getVersion());
}

void ListMethod::append(std::string rfc, std::string title, std::string host, int port){
  Method::RFCData d;
  d.rfc = rfc; d.title = title; d.host = host; d.port = port;
  vdata.push_back(d);
}

ListMethod::ListMethod(const Response & _resp){
  resp = _resp;
  version(resp.version());
  if(resp.status() == "200 OK"){
    Request::Tokens &tk = resp.tokens();
    for(unsigned int i=1; i<tk.size(); i++){
      Method::RFCData d;
      unsigned int len = tk[i].size();
      if(len >= 4){
        d.rfc = tk[i][1];
        d.port = atoi(tk[i][len-1].c_str());
        d.host = tk[i][len-2];
        vector<std::string> vs = tk[i];
        vs.pop_back(); vs.pop_back();
        d.title = combine_vector( vs, 2 );
        vdata.push_back(d);
      }
    }
  }
}

std::string ListMethod::request(){
  stringstream ss;
  ss << "LIST ALL " << getLocalVersion() << "\r\n" ;
  ss << "Host: " << getLocalHost() << "\r\n" ;
  ss << "Port: " << uploadPortNo() << "\r\n" ;
  ss << "\r\n" ;
  return ss.str();
}

std::string ListMethod::response(){
  stringstream ss;
  ss << getLocalVersion() << " " << resp.status() << "\r\n\r\n";
  for( unsigned int i=0; i<vdata.size(); i++){
    ss << "RFC " << vdata[i].rfc << " " << vdata[i].title << " ";
    ss << vdata[i].host << " " << vdata[i].port << "\r\n";
  }
  return ss.str();
}

