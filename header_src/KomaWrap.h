#if !defined (KOMA_WRAP_H)
#define KOMA_WRAP_H

#include<cstdio>
#include "../KOMA/KomaAPI.h"
#include "../KOMA/HanTagAPI.h"
#include "../KOMA/nlp.def"
#include "KupError.h"
#include<ostream>

using std::ostream; 

namespace ku_parser
{

class KomaWrap
{
public:
     KomaWrap(const char * resourceRoot=NULL, bool b_verbose=false) throw (KupError);
     ~KomaWrap();
     int tagPos(const char * sent, 
          TaggingMethod tagMethod=PATH_BASED_TAGGING);
     friend ostream & operator<<(ostream & r_ost, const KomaWrap & r_kma);

public:
     void *HanTag;		//형태소 분석기 및 품사 태거 오브젝트
     char *Wrd[MaxNumWrd];		//문장의 어절들을 저장한다.
     char Sentence[10000];	//문장을 저장한다.
     char *Result[MaxNumWrd][MaxNumAmb];	//형태소 분석 결과를 저장한다
     int   eojNum_;
     bool b_verbose_;
private:
    void finalize();
  
 };

} // namespace ku_parser

using ku_parser::KomaWrap;

#endif