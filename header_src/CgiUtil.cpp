#include "CgiUtil.h"
#include<fstream>
#include<iostream>
#include<iterator>
#include<string>

using std::ifstream;
using std::istreambuf_iterator;
using std::string;
using std::cerr;

QueryForm::QueryForm(const char * formFile):form_("")
{
     ifstream ifs(formFile);
     if (ifs.is_open())
     {
          istreambuf_iterator<char> isbit_be(ifs);
          istreambuf_iterator<char> isbit_en;

          form_.assign(isbit_be, isbit_en);
     }
 }

void 
QueryForm::htmlPrintForm(FILE * fout)
{
     fprintf(fout, "%s", this->form_.c_str());
}

///////////////////////////////////////////////////////////////////////////////
//
CgiInit::CgiInit()
{ 
    // Cgi �ʱ�ȭ ���� ���
    fprintf(stdout, "Content-type: text/html\n\n");
    // �Ű����� �м� 
    getEntry_GET(inParam_); 
}

CgiInit::~CgiInit()
{
   for(int i = 0; (inParam_[i].Value!=NULL && inParam_[i].Name!=NULL);++i)
   {
       free(inParam_[i].Name);
       free(inParam_[i].Value);
   }
}

CgiInit Cgi::cgiInit_;        // Cgi ��ü ������ �� �ѹ� ����Ǵ� �Լ�

Cgi::Cgi(const char * formFile)
:tree_(),qForm_(formFile), formEntry_(&Cgi::cgiInit_.inParam_[0])
{
     
}
///////////////////////////////////////////////////////////////////////////////
//

const char* ParseCgi::tmpTreeFile_="./__bracketTree.tmp";

ParseCgi::ParseCgi(const char * formFile, const char * dataDir) throw (KupError)
: Cgi(formFile), kup_(dataDir, true, true), kto_()
{
     
}

void 
ParseCgi::processQuery() throw (KupError)
{
     //��->�ܼ� ���
     qForm_.htmlPrintForm(stdout);
     char * val =  getValue(formEntry_, "QUERY"); // �Է¹����� �д´�.
     if(val==NULL)
         throw KupError("[ERR] ParseCgi::processQuery(): �Է����� ���� �ʽ��ϴ�.");
     string snt(val);
     if(snt.empty())
         throw KupError("[ERR] ParseCgi::processQuery(): �Է� ���� - �Էµ� ������ �����ϴ�.");
   
     string::reverse_iterator rit=snt.rbegin();
     if(*rit!='.' && *rit!='?' && *rit!='!')  snt+='.';
     
     bool b_t_suc_f_fail= kup_.doKup(snt.c_str(), tree_);  // �Ľ�
     
     if(b_t_suc_f_fail)
     {
         //��� ġ��.���� ���� ���� �����ϰ� �� �� �׾� ��
          fprintf(stdout,"<html>\n<body>\n<hr>\n");    
          // Ʈ��->�ܼ� ��� 
          kto_.htmlTreeGraph(stdout, tree_);  
          // �ش� Ʈ���� ���Ͽ� ���� 
          FILE * pf = fopen(ParseCgi::tmpTreeFile_, "wt");
          if(pf!=NULL)
          {
               kto_.bracket(pf, tree_);  
               fclose(pf);
          }
          else
               throw KupError("[ERR] ParseCgi::processQuery(): Tree->File Output Error");
          // ������
          fprintf(stdout,"</body>\n</html>\n\n");
     }
     else
           throw KupError("[ERR] ParseCgi::processQuery(): �Ľ� ���� - ������ �ʹ� ��ų� �Ľ̿� �����߽��ϴ�");
        
         
}

///////////////////////////////////////////////////////////////////////////////
//

TreeCgi::TreeCgi(const char * formFile)
: Cgi(formFile), kti_(), kto_()
{
}

void 
TreeCgi::processQuery() throw (KupError)
{
}