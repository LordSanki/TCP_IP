#include <include/method.h>
#include <cstdlib>
#include <include/common.h>
using namespace std;

int Method::getPort(){
  Request::Tokens &tk = req.tokens();
  for(unsigned int i=0; i< tk.size(); i++){
    if(tk[i][0] == "Port:"){
      return atoi(tk[i][1].c_str());
    }
  }
  return 0;
}

string Method::getTitle(){
  Request::Tokens &tk = req.tokens();
  for(unsigned int i=0; i< tk.size(); i++){
    if(tk[i][0] == "Title:"){
      return combine_vector(tk[i],1);
    }
  }
  return string();
}

std::string Method::getHost(){
  Request::Tokens &tk = req.tokens();
  for(unsigned int i=0; i< tk.size(); i++){
    if(tk[i][0] == "Host:"){
      return tk[i][1];
    }
  }
  return string();
}

std::string Method::getRFC(){
  Request::Tokens &tk = req.tokens();
  int l = tk[0].size();
  if(l - 2 >= 0){
    return tk[0][l-2];
  }
  else{
    return string();
  }
}

std::string Method::getVersion(){
  return (req.tokens())[0].back();
}

