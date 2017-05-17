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
    // Cgi 초기화 문장 출력
    fprintf(stdout, "Content-type: text/html\n\n");
    // 매개변수 분석 
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

CgiInit Cgi::cgiInit_;        // Cgi 객체 생성시 딱 한번 실행되는 함수

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
     //폼->콘솔 출력
     qForm_.htmlPrintForm(stdout);
     char * val =  getValue(formEntry_, "QUERY"); // 입력문장을 읽는다.
     if(val==NULL)
         throw KupError("[ERR] ParseCgi::processQuery(): 입력폼이 맞지 않습니다.");
     string snt(val);
     if(snt.empty())
         throw KupError("[ERR] ParseCgi::processQuery(): 입력 에러 - 입력된 문장이 없습니다.");
   
     string::reverse_iterator rit=snt.rbegin();
     if(*rit!='.' && *rit!='?' && *rit!='!')  snt+='.';
     
     bool b_t_suc_f_fail= kup_.doKup(snt.c_str(), tree_);  // 파싱
     
     if(b_t_suc_f_fail)
     {
         //몇가지 치장.문서 영역 시작 설정하고 한 줄 그어 줌
          fprintf(stdout,"<html>\n<body>\n<hr>\n");    
          // 트리->콘솔 출력 
          kto_.htmlTreeGraph(stdout, tree_);  
          // 해당 트리를 파일에 쓰기 
          FILE * pf = fopen(ParseCgi::tmpTreeFile_, "wt");
          if(pf!=NULL)
          {
               kto_.bracket(pf, tree_);  
               fclose(pf);
          }
          else
               throw KupError("[ERR] ParseCgi::processQuery(): Tree->File Output Error");
          // 마무리
          fprintf(stdout,"</body>\n</html>\n\n");
     }
     else
           throw KupError("[ERR] ParseCgi::processQuery(): 파싱 에러 - 문장이 너무 길거나 파싱에 실패했습니다");
        
         
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