#include "Kup.h"
#include"RscPath.h"

#include<cassert>
#include<string>
#include<algorithm>
#include<functional>
#include<iostream>
#include<fstream>
#include<iterator>
using std::find;
using namespace std;

#define LexiconFileNLX      "parser_lexicon_NLX.txt"
#define LexiconFileLX      "parser_lexicon_LX.txt"
#define DynaRuleFileNLX     "parser_rule_NLX.txt"
#define DynaRuleFileLX     "parser_rule_LX.txt"
#define PosCvtFile       "parser_poscvt.txt"
 
Kup::Kup(const char * resourceRot, bool b_verbose, bool b_parseWithLx) throw (KupError)
:lexicon(NULL),sRule(NULL),dRule(NULL), parse(NULL),posCvtTable(NULL),
kmw_(resourceRot,b_verbose),b_verbose_(b_verbose),b_parseWithLex_(b_parseWithLx)

{
     sRule	= new cStaticRule( NULL );				// 결정규칙(어휘사전)
	parse	= new cParseTree(sRule, NULL );			// 구문분석(결정규칙,학습규칙) 
     
     try
     {
          size=112;
          RscPath rpth(resourceRot);
          lexicon= new cLexicon( false, 
               (char*) rpth.filePath(b_parseWithLx==false?LexiconFileNLX:LexiconFileLX), 
               size); // 어휘사전(학습=true/false,파일이름=출력/입력,기준빈도)
          sRule->newLexicon( lexicon ); 	// 어휘사전 새로설정 


          if(b_verbose_){
          fprintf(stderr, "$ Loading KupLexicon..OK\n");
          }

	     dRule  = new cDynamicRule( false,
               (char*) rpth.filePath(b_parseWithLx==false?DynaRuleFileNLX:DynaRuleFileLX)
               );        // 학습규칙(학습=true/false,파일이름=출력/입력)  
          parse->newDynamicRule(dRule); 	// 학습규칙 새로 설정 

          if(b_verbose_){
               fprintf(stderr, "$ Loading KupDynaRules..OK\n");
          }

          posCvtTable = new StrMapTable(rpth.filePath(PosCvtFile));

          
          if(b_verbose_)
          {
           fprintf(stderr, "$ Loading PosCvtTable..OK\n");
          }
     }
     catch(runtime_error & rte)
     {
          finalize();
          operator delete(this); 
          string msg_tmp="- (<rcved by>"; msg_tmp+=__FILE__;
          msg_tmp+=")"; msg_tmp+=rte.what();
          throw KupError(msg_tmp.c_str());
     }


     if(!(sRule && parse && lexicon && dRule))
          throw KupError("[ERR] Kup::Kup() : Parser Resource Setting Error.");
   //memset(&words, '\0',sizeof(Words));	
}

void 
Kup::finalize()
{
    if(b_verbose_)
         fprintf(stderr,"- Finalizing Kup Object..OK\n");
     delete sRule;
	delete parse;
     delete lexicon;
     delete dRule;
     delete posCvtTable;
}

Kup::~Kup()
{
     finalize();
}

// 성공하면 true, 실패하면 false
bool 
Kup::doKup(const char * sent, Phrases & tree, TaggingMethod tagMeth)
{
      // 간단한 문장 전처리(빈 문자열이면 리턴)
     string snt(sent);
     if(snt.empty()) return false;
     
     tree.clear();
     
      // 품사 태깅
     int num_eoj = this->kmw_.tagPos(snt.c_str(), tagMeth);
     if(MAX_WRD_CNT < num_eoj || num_eoj<=0) 
          return false;
     
     // 단말노드 세팅 
     for (int i=0; i<kmw_.eojNum_; i++)
	{
          strcpy(tree.words.one[i].raw, kmw_.Wrd[i]);
          if(b_verbose_){
               fprintf(stderr,"%s\t",tree.words.one[i].raw);
          }
          tree.words.one[i].pos[0]='\0';
		for (int j=0; kmw_.Result[i][j]; j++) 
          {
	          strcat(tree.words.one[i].pos, convertPosTag(kmw_.Result[i][j], posCvtTable));
               if(b_verbose_){
                    fprintf(stderr,"%s\n",tree.words.one[i].pos);
               }
		}
	}

     // 구문 분석 (성공하면 true, 아니면 false)																// 평가단계2: 실험 집합 평가 			
    bool b_t_succ_f_fail=false;
    b_t_succ_f_fail = parse->tree( &(tree.words), &tree );
    return b_t_succ_f_fail;
}

const char *
ku_parser::convertPosTag(const char * src, StrMapTable * p_posCvtTable)
{
    static string converted;
    string KuPos;
    converted.reserve(strlen(src)*2);
    converted.erase();
    for(const char *pp = src; *pp!='\0'; ++pp)
    {
    //      //기본: 무조건 복사
        if(*pp=='/') // 품사 조건 
        {
             // 튕겨져 나갈 조건: [/]/, SS[/]
             if(!(*(pp+1)=='/' || *(pp-1)=='S'))
             {
               //+까지 찾고, 역시 자질구레한 처리.
               const char * pplus = pp; while(*pplus!='+' && *pplus!='\0') ++pplus; if(*(pplus+1)=='+') ++pplus;
               KuPos.assign(pp+1,pplus); // POS 처리
               const char * mappedPos = p_posCvtTable->getMappedStr(KuPos.c_str());

               // 변환품사 문자열 세팅
               converted+='/';  
               converted+=mappedPos; // Ku POS -> Sejong POS로 변환
               converted+=" "; // 소영이 코드를 위해 공백을 하나 더 넣어준다  
               
               pp+=KuPos.length(); // 일단 품사 길이만큼 이동한다. 루프 끝에서 한번 더 증가되니까 
               continue;  
             }
        }
        
        converted+=*pp;
        if(*pp=='+')
             converted +=" "; // 더하기 기호였다면, 소영이코드를 위해 하나 더 넣어준다.
    }
    
    return converted.c_str();
}
///////////////////////////////////////////////////////////////////////////////
//
ostream & 
ku_parser::operator<<(ostream & ost, const StringPair &  ssp)
{
     ost<<ssp.ssp_.first<<" -> "<<ssp.ssp_.second;
     return ost;
}

StrMapTable::StrMapTable(const char* tableFile, const char * delimpattern)
throw (KupError)
{
     // 파일을 읽어서 테이블에 적재 
     ifstream ifs(tableFile); 
     if(!(ifs.is_open())) 
          throw KupError("[ERR] StrMapTable: tablefile open error");

     string line;
     while(std::getline(ifs, line))
     {
          if(line.empty()) 
               continue;
          if(line.find("//")==0) 
               continue;

          int ipos=0;
          ipos = line.find_first_of(delimpattern, ipos);
          if(ipos==string::npos) 
               throw KupError("[ERR] StrMaptable: entry pattern error");
          string first = line.substr(0,ipos);
          if(first.empty())
               throw KupError("[ERR] StrMaptable: First string not found");
          ipos +=strlen(delimpattern);
          string second = line.substr(ipos, string::npos); 
          if(second.empty())
               throw KupError("[ERR] StrMaptable: Second string not found");

          sspairs_.push_back(StringPair(first, second));
     }
#ifdef _DEBUG
   //  copy(sspairs_.begin(), sspairs_.end(), ostream_iterator<StringPair>(cout, "\n"));
#endif
}

const char*
StrMapTable::getMappedStr(const char* str)
{
     
     t_sspairs::iterator beg, end;
     beg = this->sspairs_.begin();
     end = this->sspairs_.end();
     
     t_sspairs::iterator it_cvtEntry = find_if(beg, end, bind2nd(StrMapTable::TblCmp(),str));

     if(it_cvtEntry!=end)
          return it_cvtEntry->ssp_.second.c_str();
     else
          return "";
}
///////////////////////////////////////////////////////////////////////////////
//



bool 
KupTrOut::HtmlTreeOut_TD::operator() (const t_cell & cell, FILE *pf) const
{
     fprintf(pf, "<TD ALIGN=CENTER ");
     fprintf(pf, "COLSPAN=%d>", cell.second);
     fprintf(pf, "%s", cell.first.c_str());
     fprintf(pf, "</TD>");
     return true;
}


bool
KupTrOut::HtmlTreeOut::operator() (const t_cell_con & cell_con, FILE* pf) const
{
     fprintf(pf, "<TR>\n");
     for_each(cell_con.begin(),cell_con.end(), bind2nd(HtmlTreeOut_TD(),pf));
     fprintf(pf, "\n</TR>\n");
     return true;
}

void
KupTrOut::bracket(FILE* p_stream, Phrases & tree)
{
     write_.tree(p_stream, &(tree.words),&tree);
}

void 
KupTrOut::htmlTreeGraph(FILE* p_stream, Phrases & tree)
{
    // 원문 찍기 
    fprintf(p_stream, "<br><div align=\"center\">\n<font size=2 color=#826a5c><b>");
    for ( int i=0; tree.words.one[i].raw[0]!='\0'; i++ ) 
	{
		if ( (tree.words.one[i].pos[0]==' ') || (tree.words.one[i].pos[0]=='+') ) 
        { 
                fprintf( p_stream, "%s", tree.words.one[i].raw ); 
        }
		else { fprintf( p_stream, " %s", tree.words.one[i].raw ); }
	}
    fprintf(p_stream, "</b></font>\n</div>");
    
     //트리 찍기 
     //fprintf(p_stream,"<font size=9>\n");
     fprintf(p_stream,"<br><TABLE BORDER=1 ALIGN=CENTER>\n");
     t_tree_output tro;
     if(tree.root==NULL)
          tree_nonRecurse(tro, &tree, &(tree.words));
     else
          tree_recurse(tro, &tree, &(tree.words));

     for_each(tro.rbegin(), tro.rend(), bind2nd(HtmlTreeOut(), p_stream));

     fprintf(p_stream, "</TABLE>\n");
     //fprintf(p_stream,"</font>\n");
}

int
KupTrOut::tree_recurse_real(t_tree_output & tro, int idx_lvl, aPhrase * phr, Words *words)
{
     if(phr==NULL)
          return 0;
//     copy(tro.rbegin(), tro.rend(), ostream_iterator<string>(cerr, "\n"));
     //pre-order로 진행
     t_cell cel;
     cel.first=htmlPrintOneNode(phr, words);
     // 터미널 레벨이 제로 레벨이 아닐 경우를 처리
     if (phr->end_left==phr->end_right && idx_lvl>0)
     {
          t_cell cel_zeroLvl;cel_zeroLvl.first="";cel_zeroLvl.second=1;
          for(int idx_down=idx_lvl-1; idx_down>=0; --idx_down)
               tro[idx_down].push_back(cel_zeroLvl); // 연속적으로 집어 넣는다
     }
     int lspan =tree_recurse_real(tro, idx_lvl-1, phr->left, words);
     int rspan=tree_recurse_real(tro, idx_lvl-1, phr->right, words);

     int curspan=(rspan+lspan==0)?1:rspan+lspan;
     cel.second=curspan;
     tro[idx_lvl].push_back(cel);

     return curspan;
}

void 
KupTrOut::tree_recurse(t_tree_output & tro, Phrases * tree, Words *words)
{
     if(tree->root==NULL) 
          return;

     // 스트링 벡터 사이즈 조정 
     int tree_height = tree->root->end_right-tree->root->end_left;
     if(tro.empty()) 
          tro.resize(tree_height+1);

     tree_recurse_real(tro,tree_height, tree->root, words);      

}

void KupTrOut::tree_nonRecurse(t_tree_output & tro, Phrases * tree, Words *words)
{
    tro.resize(1);
    t_cell cel;
    cel.first="현재는 부분파스에 대한 트리출력은 안 됩니다.";
    cel.second=1;
    tro[0].push_back(cel);
}

string KupTrOut::htmlPrintOneNode(aPhrase * phrase, Words *words)
{
     // 노드의 구문태그 출력
     string node="<b><tt><font color=#616869>";
	if ( phrase->func.syn[0]!='_' ) { phrase->func.syn[0]='\0'; }
	if ( phrase->end_left == phrase->end_right ) 
     {
          node+=phrase->cont.syn; node+=phrase->func.syn;
          //fprintf( file, "(%s%s ", phrase->cont.syn, phrase->func.syn ); 
     }
	else if ( !strcmp(phrase->cont.syn,"VNP") ) 
     { 
          node+=phrase->cont.syn;/*manip*/node +=phrase->func.syn ;
          //fprintf( file, "(%s%-4s ", phrase->cont.syn, phrase->func.syn ); 
     }
	else 
     { 
          node += phrase->cont.syn;/*manip*/node+=phrase->func.syn;
          //fprintf( file, "(%s%-5s ", phrase->cont.syn, phrase->func.syn ); 
     }
     node+="</font></tt></b>";

     // 단말노드
	if ( phrase->end_left == phrase->end_right )
	{
          node +="<BR><font size =-2 color=#826a5c>";
          node +=words->one[phrase->end_left].pos;
          node +="</font>";
		// 복합구 점검
		//fprintf( file, "%s", words->one[phrase->end_left].pos );
		// 빈칸조정
	}
     return node;
}

/////////////////////////////////////////////////////////////////////////////////
//

bool
KupTrIn::bracketRead(const char* filename, Phrases & tree)
{
     FILE * pf = fopen(filename, "rt");
     if(pf!=NULL)
     {
          bool b_t_suc_f_fail=read_.tree(pf, &(tree.words), &tree);
          fclose(pf);     
          return b_t_suc_f_fail;
     }
     else
          return false;
     
}



