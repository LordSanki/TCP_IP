#ifndef __RESPONSE_H__
#define __RESPONSE_H__
#include <string>
#include <include/request.h>

class Response{

protected:
    std::string m_data;
    std::string m_version;
    std::string m_status;
    Request::Tokens m_tokens;
    std::string getVersion();
    std::string getStatus();
public:
    Response(std::string resp);
    Response();
    std::string data() { return m_data; }

    std::string version() { return m_version; }
    std::string status() { return m_status; }
    void version(std::string version) { m_version = version; }
    void status(std::string status) { m_status = status; }
    Request::Tokens & tokens(){ return m_tokens; }
};



#endif // RESPONSE_H
