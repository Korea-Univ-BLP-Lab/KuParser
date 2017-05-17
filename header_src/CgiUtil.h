#if !defined (__CGI_UTIL_H__)
#define __CGI_UTIL_H__

#include "Kup.h"
#include "FormUtil.h"

namespace cgi_util
{

// classes
class QueryForm
{
public:
     QueryForm(const char * formFile);
     void htmlPrintForm(FILE * out);
private:
     string form_;
};

struct CgiInit
{
public:
     CgiInit();
     ~CgiInit();
    
public:
     FormEntry    inParam_[MAX_ENTRIES]; 
};

class Cgi
{
public:
     Cgi(const char * formFile);
     virtual void processQuery() throw (KupError){}
protected:
     Phrases        tree_;
     QueryForm      qForm_;
     FormEntry *    formEntry_;
protected:
     static CgiInit      cgiInit_;    
};

class ParseCgi : public Cgi
{
public:
     ParseCgi(const char * formFile, const char * dataDir) throw (KupError);
     void processQuery() throw (KupError);
     friend class TreeCgi;
public:
     static const char * tmpTreeFile_;
private:
     Kup       kup_;
     KupTrOut  kto_;
};

class TreeCgi : public Cgi
{
public:
     TreeCgi(const char * formFile);
     void processQuery() throw (KupError);
private:
     KupTrIn   kti_;
     KupTrOut  kto_;
};

} // namespace cgi_util

using cgi_util::CgiInit;
using cgi_util::Cgi;
using cgi_util::QueryForm;
using cgi_util::ParseCgi;
using cgi_util::TreeCgi;

#endif //__CGI_UTIL_H__