#include <include/response.h>
#include <include/common.h>

using namespace std;

Response::Response(string resp){
  m_tokens = Request::tokenize(resp);
  status(getStatus());
  version(getVersion());
}

Response::Response(){
  status("200 OK");
  version(getLocalVersion());
}

std::string Response::getStatus(){
  return combine_vector(m_tokens[0],1);
}

std::string Response::getVersion(){
  return m_tokens[0][0];
}
