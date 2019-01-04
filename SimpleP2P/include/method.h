#ifndef __METHOD_H__
#define __METHOD_H__
#include <include/request.h>
#include <include/response.h>


class Method{
  protected:
    Request req;
    Response resp;

    struct RFCData{
      std::string rfc;
      int port;
      std::string title;
      std::string host;
    };
    RFCData data;
    std::string ver;
    void request(const Request & _req) { req = _req; }
    int getPort();
    std::string getTitle();
    std::string getHost();
    std::string getRFC();
    std::string getVersion();

  public:
    void version(std::string v) { ver = v; }
    std::string version() { return ver; }
    void title(std::string t) { data.title = t; }
    std::string title() { return data.title; }
    void rfc(std::string r) { data.rfc = r; }
    std::string rfc() { return data.rfc; }
    void port(int p) { data.port = p; }
    int port() { return data.port; }
    std::string host() { return data.host; }
    void host(std::string h) { data.host = h; }
    void status(std::string s) { resp.status(s); }
    std::string status() { return resp.status(); }
};


#endif //__METHOD_H__
