#include <cassert>
#include <iostream>

#include "KomaWrap.h"
#include "RscPath.h"
#include<iostream>
#include <cstring>

#define MainFSTFilename "main.FST"
#define MainDataFilename "main.dat"
#define ConnectionTableFilename "connection.txt"
#define TagNameFileName "tag.nam"
#define TagOutNameFileName "tagout.nam"
#define ProbEntryFilename "prob.FST"
#define ProbDataFilename "prob.dat"

using std::cerr;
using std::endl;

KomaWrap::KomaWrap(const char * resourceRoot, bool b_verbose) 
throw (ku_parser::KupError)
:b_verbose_(b_verbose)
{
     RscPath mainFST(resourceRoot),
          mainData(resourceRoot),
          connTable(resourceRoot),
          tagName(resourceRoot),
          tagOut(resourceRoot),
          probEntry(resourceRoot),
          probData(resourceRoot);

try
{   
  if (!LoadKomaEngine(/*"./data/main.FST", */mainFST.filePath(MainFSTFilename),
       /*"./data/main.dat", */ mainData.filePath(MainDataFilename),
       /*"./data/connection.txt", */connTable.filePath(ConnectionTableFilename),
       /*"./data/tag.nam", */tagName.filePath(TagNameFileName),
       /*"./data/tagout.nam" */ tagOut.filePath(TagOutNameFileName)))
  {
	  //fprintf(stderr, "Cannot loading koma engine.\n");
	  //assert(false);
      throw KupError("[ERR] KomaWrap::KomaWrap(): Cannot loading KomaEngine");
  }

  if(b_verbose_){
     fprintf(stderr, "$ Loading KomaEngine..OK\n");
  }

  if (!LoadHanTagEngine( probEntry.filePath(ProbEntryFilename), 
       probData.filePath(ProbDataFilename))) 
  {
	  //fprintf(stderr, "Cannot loading hantag engine.\n");
	  //assert(false);
       throw KupError("[ERR] KomaWrap::KomaWrap(): Cannot loading HantagEngine."); 
  }

  if(b_verbose_){
     fprintf(stderr, "$ Loading HantagEngine..OK\n");
  }
  
  if (!(HanTag = CreateHanTag())) 
  {
	  //fprintf(stderr, "Cannot Create Hantag Instance.\n");
	  //assert(false);
      throw KupError("[ERR] KomaWrap::KomaWrap(): Cannot Create HantagInstance.");
  }

  if(b_verbose_){
     fprintf(stderr, "$ Loading HantagInstance..OK\n");
  }
}
catch(KupError & ke)
{
    char * pattern;
    int finalize_code=0;
    
    if((pattern = strstr(ke.what(), "HantagEngine"))!=NULL)
          ++finalize_code;
    if((pattern=strstr(ke.what(), "HantagInstance"))!=NULL)
        ++finalize_code;
    if(b_verbose_)
            fprintf(stderr,"- Partially Finalizing Object's component:");
    switch(finalize_code)
    {
    case 2:
        if(b_verbose_)
            fprintf(stderr, "HanTagEngine, ");
        EndHanTagEngine();    
    case 1:
        if(b_verbose_)
            fprintf(stderr, "KomaEngine...OK\n");
        EndKomaEngine();
    case 0:
         if(b_verbose_)
            fprintf(stderr, "End\n");   
    }
    
    throw;
}
}

KomaWrap::~KomaWrap()
{
    finalize();
}

int 
KomaWrap::tagPos(const char * sent, TaggingMethod tagMethod)
{    
     if(sent==NULL || strlen(sent)==0)
          return 0;
    strcpy(Sentence, sent);
    eojNum_=DoKomaAndHanTag(HanTag, tagMethod, Sentence, Wrd, Result);
     if(b_verbose_)
        cerr<<*this;
 
     return eojNum_;
}

void
KomaWrap::finalize()
{
    if(b_verbose_)
        fprintf(stderr, "- Finalizing KomaWrap Object...OK\n");
      FreeHanTag(HanTag);
      EndHanTagEngine();
      EndKomaEngine();
 }

ostream & 
ku_parser::operator<<(ostream & r_ost, const KomaWrap & r_kma)
{
     int i;
     for (i=0; r_kma.Result[i][0]; i++)
		{
			r_ost<<r_kma.Wrd[i]<<"\t";
			for (int j=0; r_kma.Result[i][j]; j++) 
            {
                if (j) r_ost<<"^";
                r_ost<<r_kma.Result[i][j];
			}
            r_ost<<endl;
		}
     return r_ost;
}

int __main()
{
     KomaWrap kma;
     kma.tagPos("그 남자는 착한 학생입니다.");
     cerr<<kma;
     return 0;
}