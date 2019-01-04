#include <include/addmethod.h>
#include <sstream>
#include <include/common.h>

using namespace std;

AddMethod::AddMethod(const Request &_req)
{
  Method::request(_req);
  port(getPort());
  title(getTitle());
  rfc(getRFC());
  version(getVersion());
  host(getHost());
}

AddMethod::AddMethod(const Response &_resp){
  resp = _resp;  
}

string AddMethod::request(){
  stringstream ss;
  ss<<"ADD RFC "<<rfc()<<" "<<getLocalVersion()<<"\r\n";
  ss<<"Host: "<<getLocalHost()<<"\r\n";
  ss<<"Port: "<<uploadPortNo()<<"\r\n";
  ss<<"Title: "<<title()<<"\r\n";
  return ss.str();
}

string AddMethod::response(){
  stringstream ss;
  ss<<getLocalVersion()<<" "<<resp.status() << "\r\n";
  if(resp.status() == string("200 OK")){
    ss<<"RFC "<<rfc()<<" "<<title()<<" "<<host()<<" "<<port()<<"\r\n";
  }
  return ss.str();
}

