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
     sRule	= new cStaticRule( NULL );				// ������Ģ(���ֻ���)
	parse	= new cParseTree(sRule, NULL );			// �����м�(������Ģ,�н���Ģ) 
     
     try
     {
          size=112;
          RscPath rpth(resourceRot);
          lexicon= new cLexicon( false, 
               (char*) rpth.filePath(b_parseWithLx==false?LexiconFileNLX:LexiconFileLX), 
               size); // ���ֻ���(�н�=true/false,�����̸�=���/�Է�,���غ�)
          sRule->newLexicon( lexicon ); 	// ���ֻ��� ���μ��� 


          if(b_verbose_){
          fprintf(stderr, "$ Loading KupLexicon..OK\n");
          }

	     dRule  = new cDynamicRule( false,
               (char*) rpth.filePath(b_parseWithLx==false?DynaRuleFileNLX:DynaRuleFileLX)
               );        // �н���Ģ(�н�=true/false,�����̸�=���/�Է�)  
          parse->newDynamicRule(dRule); 	// �н���Ģ ���� ���� 

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

// �����ϸ� true, �����ϸ� false
bool 
Kup::doKup(const char * sent, Phrases & tree, TaggingMethod tagMeth)
{
      // ������ ���� ��ó��(�� ���ڿ��̸� ����)
     string snt(sent);
     if(snt.empty()) return false;
     
     tree.clear();
     
      // ǰ�� �±�
     int num_eoj = this->kmw_.tagPos(snt.c_str(), tagMeth);
     if(MAX_WRD_CNT < num_eoj || num_eoj<=0) 
          return false;
     
     // �ܸ���� ���� 
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

     // ���� �м� (�����ϸ� true, �ƴϸ� false)																// �򰡴ܰ�2: ���� ���� �� 			
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
    //      //�⺻: ������ ����
        if(*pp=='/') // ǰ�� ���� 
        {
             // ƨ���� ���� ����: [/]/, SS[/]
             if(!(*(pp+1)=='/' || *(pp-1)=='S'))
             {
               //+���� ã��, ���� ���������� ó��.
               const char * pplus = pp; while(*pplus!='+' && *pplus!='\0') ++pplus; if(*(pplus+1)=='+') ++pplus;
               KuPos.assign(pp+1,pplus); // POS ó��
               const char * mappedPos = p_posCvtTable->getMappedStr(KuPos.c_str());

               // ��ȯǰ�� ���ڿ� ����
               converted+='/';  
               converted+=mappedPos; // Ku POS -> Sejong POS�� ��ȯ
               converted+=" "; // �ҿ��� �ڵ带 ���� ������ �ϳ� �� �־��ش�  
               
               pp+=KuPos.length(); // �ϴ� ǰ�� ���̸�ŭ �̵��Ѵ�. ���� ������ �ѹ� �� �����Ǵϱ� 
               continue;  
             }
        }
        
        converted+=*pp;
        if(*pp=='+')
             converted +=" "; // ���ϱ� ��ȣ���ٸ�, �ҿ����ڵ带 ���� �ϳ� �� �־��ش�.
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
     // ������ �о ���̺� ���� 
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
    // ���� ��� 
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
    
     //Ʈ�� ��� 
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
     //pre-order�� ����
     t_cell cel;
     cel.first=htmlPrintOneNode(phr, words);
     // �͹̳� ������ ���� ������ �ƴ� ��츦 ó��
     if (phr->end_left==phr->end_right && idx_lvl>0)
     {
          t_cell cel_zeroLvl;cel_zeroLvl.first="";cel_zeroLvl.second=1;
          for(int idx_down=idx_lvl-1; idx_down>=0; --idx_down)
               tro[idx_down].push_back(cel_zeroLvl); // ���������� ���� �ִ´�
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

     // ��Ʈ�� ���� ������ ���� 
     int tree_height = tree->root->end_right-tree->root->end_left;
     if(tro.empty()) 
          tro.resize(tree_height+1);

     tree_recurse_real(tro,tree_height, tree->root, words);      

}

void KupTrOut::tree_nonRecurse(t_tree_output & tro, Phrases * tree, Words *words)
{
    tro.resize(1);
    t_cell cel;
    cel.first="����� �κ��Ľ��� ���� Ʈ������� �� �˴ϴ�.";
    cel.second=1;
    tro[0].push_back(cel);
}

string KupTrOut::htmlPrintOneNode(aPhrase * phrase, Words *words)
{
     // ����� �����±� ���
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

     // �ܸ����
	if ( phrase->end_left == phrase->end_right )
	{
          node +="<BR><font size =-2 color=#826a5c>";
          node +=words->one[phrase->end_left].pos;
          node +="</font>";
		// ���ձ� ����
		//fprintf( file, "%s", words->one[phrase->end_left].pos );
		// ��ĭ����
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



