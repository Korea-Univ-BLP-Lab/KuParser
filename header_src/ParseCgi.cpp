#include "CgiUtil.h"

int main()
{

     try
     {
          ParseCgi pc("./index.html", "../KuParserDeploy/bin/data");
          pc.processQuery();
     }
     catch(KupError & ke)
     {
          //몇가지 치장.문서 영역 시작 설정하고 한 줄 그어 줌
          fprintf(stdout,"<html>\n<body>\n<hr>\n<div align=\"center\">"); 
          fprintf(stdout,"<font color=#ff0000>%s</font>", typeid(ke).name());
          fprintf(stdout, "<br><B>%s</B><br>\n", ke.what());
         fprintf(stdout, "<div>\n", ke.what());
          // 마무리
          fprintf(stdout,"</body>\n</html>\n\n");
     }
     return 0;
}