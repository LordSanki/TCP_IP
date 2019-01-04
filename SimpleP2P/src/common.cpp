#include <include/common.h>
#include <cstdio>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <unistd.h>
#include <fstream>
#include <time.h>
using namespace std;

static int upload_port;
static int db_port;
static std::string pversion = string("P2P-CI/1.0");
static std::string HostName;
static std::string * OSName = NULL;
static std::string server_addr = string("localhost");

bool copyFileOnDisk(string s, string d){
  ifstream source(s.c_str(), ios::binary);
  ofstream dest(d.c_str(), ios::binary);
  if(source.is_open() && dest.is_open()){
    dest << source.rdbuf();

    source.close();
    dest.close();
    return true;
  }
  else{
    return false;
  }
}

std::string getLocalHost(){
  if(HostName.empty()){
    char temp[100] = {0};
    gethostname(temp, 99);
    HostName = string(temp);
  }
  return HostName;
}

void setLocalHost(std::string s){
  HostName = s;
}

std::string getLocalOS(){
  if(OSName) return *OSName;
  FILE* f = popen("lsb_release -ds", "r");
  char temp[1000] = {0};
  // read entire line
  if(temp == fgets(temp, 1000, f)){};
  pclose(f);
  OSName = new string(temp);
  // remove new line
  *OSName = OSName->substr(0,OSName->length()-1);
  return *OSName;
}

std::string getLocalVersion(){
  return pversion;
}

int server_port(){
  return SERVER_PORT;
}

int db_server_port(){
  return db_port;
}

void db_server_port(int p){
  db_port = p;
}

std::string server_address(){
  return server_addr;
}

void server_address(string s){
  server_addr = s;
}

std::string db_server_address(){
  return string(DB_SERVER_ADDRESS);
}

int uploadPortNo(){
  return upload_port;
}

void uploadPortNo(int p){
  upload_port = p;
}
void print_reply(string s){
  cout << "\n===== Reply =====" <<endl;
  cout << s;
  cout << "==================="<<endl;
}

void print_req(string s){
  cout << "\n===== Request =====" <<endl;
  cout << s;
  cout << "==================="<<endl;
}

std::string trim(const std::string& str)
{
  const std::string& whitespace = " \t\r\n";
  unsigned long strBegin = str.find_first_not_of(whitespace);
  if (strBegin == std::string::npos)
    return ""; // no content

  unsigned long strEnd = str.find_last_not_of(whitespace);
  unsigned long strRange = strEnd - strBegin + 1;

  return str.substr(strBegin, strRange);
}

std::string combine_vector(const std::vector <std::string>& v, int start){
  string s;
  for(unsigned int i=start; i<v.size(); i++){
    s = s + v[i];
    s = s + string(" ");
  }
  return trim(s);
}

std::string getLocalDateTime(){
  time_t t = time(0);
  return trim(string(asctime(gmtime(&t)))) + string(" GMT");
}

vector<string> hostname_to_ip(string hostname)
{
  struct hostent *he;
  struct in_addr **addr_list;
  int i;
  vector<string> vs;
  if ( (he = gethostbyname( hostname.c_str() ) ) == NULL) 
  {
    return vs;
  }

  addr_list = (struct in_addr **) he->h_addr_list;

  for(i = 0; addr_list[i] != NULL; i++) 
  {
    //Return the first one;
    vs.push_back( string(inet_ntoa(*addr_list[i]) ) );
  }

  return vs;
}


