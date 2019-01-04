#ifndef LOOKUPMETHOD_H
#define LOOKUPMETHOD_H

#include <include/listmethod.h>

class LookupMethod: public ListMethod{

public:
  LookupMethod(const Request &_req);
  LookupMethod(const Response &_resp);
  LookupMethod() {}
  std::string request();
};
#endif // LOOKUPMETHOD_H
