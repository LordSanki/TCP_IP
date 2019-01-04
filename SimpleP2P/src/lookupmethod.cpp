#include <include/lookupmethod.h>
#include <include/common.h>
#include <sstream>

using namespace std;

LookupMethod::LookupMethod(const Request &_req){
  Method::request(_req);
  rfc(getRFC());
  title(getTitle());
  version(getVersion());
  port(getPort());
  host(getHost());
}

LookupMethod::LookupMethod(const Response &_resp)
            : ListMethod(_resp) {
}


string LookupMethod::request(){
  stringstream ss;
  ss << "LOOKUP RFC " << rfc() << " " << getLocalVersion() << "\r\n";
  ss << "Host: " << getLocalHost() << "\r\n";
  ss << "Port: " <<  uploadPortNo() << "\r\n";
  ss << "Title: " << title() << "\r\n";
  return ss.str();
}



