#if !defined (RSC_PATH_H)
#define RSC_PATH_H

#include <string>
using std::string;

namespace kwk_simple_type
{

class RscPath
{
public:
     RscPath(const char * roo=NULL): root_(roo==NULL?"":roo)
     {
          if(roo==NULL) root_=".";
          if(*(root_.rbegin())!='/' || *(root_.rbegin())!= '\\')
               root_+='/';
     }
     inline const char * filePath(const char *pat)
     {
          path_ = root_+pat;
          return path_.c_str();
     }
private:
     string root_;
     string path_;
};

} // namespace kwk_simple_type

using kwk_simple_type::RscPath;

#endif