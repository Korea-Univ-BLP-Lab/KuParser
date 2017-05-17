#if !defined (__KUP_ERROR_H__)
#define __KUP_ERROR_H__

#include<cstdio>
#include<cassert>
#include<stdexcept>
#include<string>

using std::runtime_error;
using std::string;

namespace ku_parser
{

inline void yj_err(const char* errMesg)
{
#ifdef _DEBUG
     fprintf(stderr,"%s\n",errMesg);
#endif
     assert(false);
     exit(1); 
}

// class
class KupError : public runtime_error
{
public:
     KupError(const char * msg): runtime_error(string(msg)){}
};

} // namespace ku_parser

using ku_parser::KupError;

#endif