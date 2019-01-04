#include <include/request.h>
#include <include/common.h>

using namespace std;
void print_tokens(Request::Tokens &tk){
  cout << "===========Printing Tokens===========" << endl;
  for(unsigned int i=0; i<tk.size(); i++){
    for(unsigned int j=0; j<tk[i].size(); j++){
      cout<<tk[i][j] << " ";
    }
    cout << endl;
  }
  cout << "========done=========" << endl;
}

Request::Request(string d)
{
  m_data = d;

  m_meth = str2enum( getMethod() );

  tk = tokenize(m_data);
  //print_tokens(tk);

  m_host = getHost();

  m_version = tk[0].back();
}

Request::RequestMethod Request::str2enum(std::string t){
  if(t == "ADD") return P2S_ADD;
  if(t == "LIST") return P2S_LIST;
  if(t == "LOOKUP") return P2S_LOOKUP;
  return none;
}

string Request::getMethod(){
  int place = m_data.find_first_of(" ");
  string temp = m_data.substr(0,place);
  return temp;
}


string Request::getHost(){
  for(unsigned int i=0; i<tk.size(); i++){
    if(tk[i][0] == "Host:"){
      return tk[i][1];
    }
  }
  return string();
}

/*
string Request::getVersion(){

    int tempPosition_first = data.find("RFC ")+4;
    int startposition =data.find(" ",tempPosition_first+1)+1;
    int endposition=data.find("\r\n",startposition+1);
    string temp =data.substr(startposition,endposition-startposition);
    return temp;
}
*/

Request::Tokens Request::tokenize(string _d){
  string d = _d;
  Tokens t;
  while(d.empty() == false){
    unsigned long l = d.find_first_of("\r\n");
    if(l == string::npos) break;
    string d1 = d.substr(0, l);
    if(l+2 >= d.length())
//    if(l+2 >= string::npos)
      d.clear();
    else
      d = d.substr(l+2,string::npos);
    vector<string> d1v;
    while(d1.empty() == false){
      unsigned long l1 = d1.find_first_of(" ");
      string crop = d1.substr(0,l1);
      d1v.push_back(crop);
      if(l1 == string::npos) break;
      d1 = d1.substr(l1+1, string::npos);
    }
    t.push_back(d1v);
  }
  return t;
}


