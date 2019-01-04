#ifndef __GETMETHOD_H__
#define __GETMETHOD_H__
#include <include/method.h>

class GetMethod: public Method {

  private:
    void * m_buf;
    unsigned int m_size;
    char *m_resp_buf;
    unsigned int m_resp_size;
    std::string m_date;
    std::string m_lastModified;
    std::string m_contentType;
    unsigned int m_contentLength;
    std::string resp_msg; 
  private:
    void create_response_buf( char ** buff, unsigned int &size);
    int getContentLength();
    std::string getContentType();
    std::string getLastModified();
    std::string getDate();
    std::string getOS();

  public:
    GetMethod (const Request & _req);
    GetMethod (char *buff, unsigned int size);
    GetMethod ();
    ~GetMethod(); 
    std::string request();
    void * response_buf();
    unsigned int response_size();
    std::string response_msg() { return resp_msg; }
    void read_file(std::string f);
    void write_file(std::string f);
    int file_size() { return m_size; }
    int contentLength(){ return m_contentLength; }
    std::string contentType() { return m_contentType; }
    std::string date() { return m_date; }
    std::string lastModified() { return m_lastModified; }
    void contentLength(int l){ m_contentLength = l; }
    void contentType(std::string s) { m_contentType = s; }
    void date(std::string s) { m_date = s; }
    void lastModified(std::string s) { m_lastModified = s; }

};

#endif // GETMETHOD_H
