#ifndef __ADDMETHOD_H__
#define __ADDMETHOD_H__

#include <include/method.h>

class AddMethod : public Method{

  public:
    AddMethod (const Request & _req);
    AddMethod (const Response & _resp);
    AddMethod () {}

    std::string response();
    std::string request();
};


#endif // __ADDMETHOD_H__
