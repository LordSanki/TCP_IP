#include <include/getmethod.h>
#include <include/common.h>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

using namespace std;
GetMethod::GetMethod(){
  m_resp_size = 0;
  m_resp_buf = NULL;
  m_buf = NULL;
  m_size = 0;
}

GetMethod::GetMethod(const Request &_req){
  m_resp_size = 0;
  m_resp_buf = NULL;
  m_buf = NULL;
  m_size = 0;

  Method::request(_req);
  host(getHost());
  port(getPort());
  version(getVersion());
  rfc(getRFC());
  //OS(getOS());
}

GetMethod::GetMethod(char *buff, unsigned int size){
  m_resp_size = 0;
  m_resp_buf = NULL;
  m_buf = NULL;
  m_size = 0;

  unsigned int text_len=0;
  int num_lf = 0;
  for(; text_len < size; text_len++){
    if(buff[text_len] == '\n'){
      break;
    }
  }
  resp_msg = string(buff, text_len+1);
  resp = Response(resp_msg);
  if(resp.status() == "200 OK"){
    text_len=0;
    num_lf = 6;
    for(; text_len < size; text_len++){
      if(buff[text_len] == '\n'){
        num_lf --;
        if(num_lf == 0) break;
      }
    }
    resp_msg = string(buff, text_len+1);
    resp = Response(resp_msg);
    //OS(getOS());
    status(resp.status());
    version(resp.version());
    date(getDate());
    lastModified(getLastModified());
    contentLength(getContentLength());
    contentType(getContentType());

    m_size = size - (text_len+1);
    m_buf = malloc(m_size);
    memcpy(m_buf, &(buff[text_len+1]), m_size);
  }
}

GetMethod::~GetMethod(){
  if(m_buf){
    free(m_buf);
  }
  if(m_resp_buf){
    free(m_resp_buf);
  }
}
std::string GetMethod::request() {
  stringstream ss;
  ss << "GET RFC " << rfc() << " " << getLocalVersion() << "\r\n";
  ss << "Host: " << getLocalHost() << "\r\n";
  ss << "OS: " << getLocalOS() << "\r\n";
  return ss.str();
}

void GetMethod::create_response_buf(char **buff, unsigned int& size){
  stringstream ss;
  ss << getLocalVersion() << " " << resp.status() << "\r\n";
  if(resp.status() == "200 OK"){
    ss << "Date: " << getLocalDateTime() << "\r\n";
    ss << "OS: " << getLocalOS() << "\r\n";
    ss << "Last-Modified: " << lastModified() << "\r\n";
    ss << "Content-Length: " << contentLength() << "\r\n";
    ss << "Content-Type: " << contentType() << "\r\n";
    //DBG1(ss.str());
    int len = ss.str().length() + m_size;
    *buff = (char*)malloc(len);
    memcpy(*buff, ss.str().c_str(), ss.str().length());
    size = len;
    char *b = *buff;
    memcpy(&b[ss.str().length()], m_buf, m_size);
  }
  else{
    int len = ss.str().length();
    *buff = (char*)malloc(len);
    memcpy(*buff, ss.str().c_str(), ss.str().length());
    size = len;
  }
}
void * GetMethod::response_buf(){
  if(m_resp_buf == NULL){
    create_response_buf(&m_resp_buf, m_resp_size);
  }
  return m_resp_buf;
}

unsigned int GetMethod::response_size(){
  if(m_resp_size == 0){
    create_response_buf(&m_resp_buf, m_resp_size);
  }
  return m_resp_size;
}

void GetMethod::write_file(std::string fname){
  FILE *f = fopen(fname.c_str(), "wb+");
  if(m_buf == NULL || m_size <= 0) return;
  fwrite(m_buf, 1, m_size, f);
  fclose(f);
}

void GetMethod::read_file(string fname){
  struct stat stbuf;
  int fd;

  fd = open(fname.c_str(), O_RDONLY);
  if (fd == -1) {
    DBG1("Unable to open file for reading");
    DBG1(fname);
    return;
  }

  if ((fstat(fd, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode))) return;
  m_size = stbuf.st_size;
  lastModified( trim(string(asctime(gmtime(&stbuf.st_mtime))))+string(" GMT") );
  close(fd);
  FILE * f = fopen(fname.c_str(), "r");
  if(f == NULL) return;

  m_buf = malloc(m_size);

  int size_read = fread(m_buf, 1, m_size, f);
  if((unsigned int)size_read != m_size) DBG2("Unable to read file");
  fclose(f);
  contentLength(m_size);
  contentType("text/plain");
}

std::string GetMethod::getOS(){
  Request::Tokens &tk = resp.tokens();
  for(unsigned int i=0; i<tk.size(); i++){
    if((!tk[i].empty()) && tk[i][0] == "OS:"){
      return combine_vector(tk[i], 1);
    }
  }
  return string();
}

std::string GetMethod::getDate(){
  Request::Tokens &tk = resp.tokens();
  for(unsigned int i=0; i<tk.size(); i++){
    if((!tk[i].empty()) &&tk[i][0] == "Date:"){
      return combine_vector(tk[i], 1);
    }
  }
  return string();
}

std::string GetMethod::getLastModified(){
  Request::Tokens &tk = resp.tokens();
  for(unsigned int i=0; i<tk.size(); i++){
    if((!tk[i].empty()) &&tk[i][0] == "Last-Modified:"){
      return combine_vector(tk[i], 1);
    }
  }
  return string();
}

int GetMethod::getContentLength(){
  Request::Tokens &tk = resp.tokens();
  for(unsigned int i=0; i<tk.size(); i++){
    if((!tk[i].empty()) &&tk[i][0] == "Content-Length:"){
      return atoi(combine_vector(tk[i], 1).c_str());
    }
  }
  return 0;
}

std::string GetMethod::getContentType(){
  Request::Tokens &tk = resp.tokens();
  for(unsigned int i=0; i<tk.size(); i++){
    if((!tk[i].empty()) &&tk[i][0] == "Content-Type:"){
      return combine_vector(tk[i], 1);
    }
  }
  return string();
}

