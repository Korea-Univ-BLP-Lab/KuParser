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
          //��� ġ��.���� ���� ���� �����ϰ� �� �� �׾� ��
          fprintf(stdout,"<html>\n<body>\n<hr>\n<div align=\"center\">"); 
          fprintf(stdout,"<font color=#ff0000>%s</font>", typeid(ke).name());
          fprintf(stdout, "<br><B>%s</B><br>\n", ke.what());
         fprintf(stdout, "<div>\n", ke.what());
          // ������
          fprintf(stdout,"</body>\n</html>\n\n");
     }
     return 0;
}