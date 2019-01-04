#ifndef LISTMETHOD_H
#define LISTMETHOD_H

#include <include/method.h>
#include <vector>

class ListMethod : public Method{
  private:
    std::vector <Method::RFCData> vdata;

  public:
    ListMethod (const Request & _req);
    ListMethod (const Response & _resp);
    ListMethod () {}

    void append(std::string rfc, std::string title, std::string host, int port);

    const std::vector <Method::RFCData> & data() { return vdata; }
    std::string response();
    std::string request();
};

#endif // LISTRESPONSE_H
