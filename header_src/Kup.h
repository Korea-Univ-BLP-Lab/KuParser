#if !defined (KUP_H)
#define KUP_H

#pragma warning (disable:4786)

#include "KupError.h"
#include "KomaWrap.h"
#include "StaticRule.h"
#include "Lexicon.h"
#include "DynamicRule.h"
#include "ReadTree.h"
#include "WriteTree.h"
#include "ReadTree.h"
#include "ParseTree.h"

#include<cstdio>
#include<vector>
#include<ostream>
#include<utility>
#include<string>
#include<functional>
#include<cassert>
#include<list>
#include<stdexcept>
#include<typeinfo>

using std::runtime_error;
using std::pair;
using std::string;
using std::ostream;
using std::vector;
using std::list;
using std::binary_function;

namespace ku_parser
{
//forwards
class KomaWrap;    
    
// typedefs
typedef pair<string, string> t_ccpair;  // Kup의 정적 멤버가 쓰기 위한 타입. 내부에 있으면 안 된다.


class StringPair
{
public:
     StringPair(const string & one, const string & two):ssp_(one, two){}
     t_ccpair     ssp_;
     friend ostream & operator<<(ostream & ost, const StringPair & ssp);
};

typedef vector<StringPair>  t_sspairs;


class StrMapTable
{
public:
     struct TblCmp: binary_function<StringPair, const char*, bool>
     {
          bool operator()(const StringPair & r_ccp, const char * str) const
          {        
               return (strcmp(r_ccp.ssp_.first.c_str(), str)==0);
          }
     };
public:
     StrMapTable(const char* tableFile, const char * delimpattern="\t->\t")
          throw (KupError);
     const char* getMappedStr(const char* str);
private:
      t_sspairs     sspairs_;
};

class Kup
{
public:
     
     Kup(const char * resourceRoot=NULL,             // NULL일 경우엔 현재 디렉토리
          bool b_verbose=false,
         bool b_parseWithLx=false) throw (KupError);   // 파싱시 렉시콘을 쓸 것인가
     ~Kup();
     bool doKup(const char * sent, 
               Phrases & tree,
               TaggingMethod posTagMethd=PATH_BASED_TAGGING); 

     friend const char * convertPosTag(const char * src, StrMapTable * p_posCvtTable);
public:
     StrMapTable    *posCvtTable;                      // 품사변환 테이블   
private:
     KomaWrap  kmw_;
     cLexicon		*lexicon;						// 어휘사전(학습=true/false,파일이름=출력/입력,기준빈도)
	cStaticRule	*sRule;						// 결정규칙(어휘사전)
	cDynamicRule	*dRule;						// 학습규칙(학습=true/false,파일이름=출력/입력)
	cParseTree	*parse;						// 구문분석(결정규칙,학습규칙) 
	
     int            size;
private:
     bool      b_parseWithLex_;    // 파싱시 렉시콘을 쓸것인가의 여부
     bool      b_verbose_;
   //  Words	words;			// 형태소  분석정보
private:
    void finalize();
};


typedef pair<string, int>     t_cell;
typedef list<t_cell>          t_cell_con;
typedef vector<t_cell_con>    t_tree_output;

class KupTrOut
{
public:
     struct HtmlTreeOut_TD: public binary_function<t_cell, FILE*, bool>
     {
          bool operator()(const t_cell & cell, FILE *pf) const;
     };

     struct HtmlTreeOut:public binary_function<t_cell_con, FILE*, bool>
     {
          bool operator()(const t_cell_con & cell_con, FILE * pf) const;
     };
public:
     void bracket(FILE* p_stream, Phrases & tree);
     void htmlTreeGraph(FILE* p_stream, Phrases & tree);
private:
     cWriteTree     write_;						// 트리출력
private:
     void tree_recurse(t_tree_output & tro, Phrases * tree, Words *words);
     int tree_recurse_real(t_tree_output & tro, int idx_lvl, aPhrase * phr, Words *words);
     void tree_nonRecurse(t_tree_output & tro, Phrases * tree, Words *words);
     string htmlPrintOneNode(aPhrase * phr, Words *words);
};

class KupTrIn
{
public:
     KupTrIn():sRule_(NULL),read_(&sRule_){}
     bool bracketRead(const char * filename, Phrases & tree);
private:
     cStaticRule    sRule_; // 이게 먼저 선언됐으므로 이게 먼저 초기화됨
     cReadTree      read_;
};


}    // namespace ku_parser

using ku_parser::KupTrIn;
using ku_parser::KupTrOut;
using ku_parser::Kup;
using ku_parser::KupError;
using ku_parser::t_ccpair;
using ku_parser::t_sspairs;
using ku_parser::yj_err;
using ku_parser::StrMapTable;
using ku_parser::StringPair;
using ku_parser::t_cell;
using ku_parser::t_cell_con;
using ku_parser::t_tree_output;
using ku_parser::convertPosTag;

#endif