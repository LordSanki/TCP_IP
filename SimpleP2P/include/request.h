#ifndef __REQUEST_H__
#define __REQUEST_H__

#include<string>
#include <vector>

class Request{

public:
  enum RequestMethod{
    none,
    P2S_ADD,
    P2S_LOOKUP,
    P2S_LIST
  };
  typedef std::vector< std::vector<std::string> > Tokens;
protected:
  RequestMethod m_meth;
  std::string m_host;
  std::string m_data;
  std::string m_version;
  Tokens tk;

private:
  std::string getMethod();
  std::string getHost();
  RequestMethod str2enum(std::string m);

public:
  static Tokens tokenize(std::string data);
  Tokens & tokens() { return tk; }
  Request() {}
  Request(std::string data);
  std::string data() { return m_data; }
  void data(std::string d) { m_data = d; }
  std::string version() { return m_version; }
  void version(std::string d) { m_version = d; }
  RequestMethod method() { return m_meth; }
  void method(RequestMethod m) { m_meth = m; }
  std::string host() { return m_host; }
  void host(std::string h) { m_host = h; }
};

#endif //__REQUEST_H__
