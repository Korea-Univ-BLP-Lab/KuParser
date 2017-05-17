#include "Kup.h"
#include<iostream>
#include<stdio.h>
#include<string>
#if defined (WIN32)
    #include "./getopt.h"
#else
    #include <getopt.h>
#endif    

using std::cin;
using std::cerr;

void real_end()
{
    cerr<<"Uncaught exception(s) is(are) raised!! "<<endl;
}


int main(int argc, char** argv)
{
	 bool b_t_verb_f_nvb=false;
	 bool b_t_lex_f_nlex=true;
     bool b_t_html_f_none=false;
	 string resource_dir="./data";
	 int opt;
     while(true)
	 {
	 	opt = getopt(argc, argv, "r:vnh");
		if(opt==EOF)
			break;
		switch(opt)
		{
		case 'r':
			resource_dir = optarg; break;
		case 'v':
			b_t_verb_f_nvb=true; break;
		case 'n':
			b_t_lex_f_nlex=false;break;
		case 't':
			b_t_html_f_none=true;break;
		case 'h':
			{
				cerr<<"Ku_parser: A PSG Korean Syntactic Analyzer"<<endl;
				cerr<<"\t\t(c) NLPLab.,Korea University, 2004"<<endl;
				cerr<<"* Usage:kup [options]"<<endl;
				cerr<<"\t-r <path> : resource directory (default:./data)"<<endl;
				cerr<<"\t-v : verbose mode (default:false)"<<endl;
				cerr<<"\t-n : without lexical information (default:false)"<<endl;
				cerr<<"\t-t : html tree-graph output (default:false)"<<endl;
				return 0;
			}
		default:
			break;
		}
	 }
     Kup * kup;
     try
     {
          const char * ON="ON", * OFF="OFF";
          if(b_t_verb_f_nvb)
		{
			cerr<<"-[resource dir]:"<<resource_dir<<endl;
			cerr<<"-[verbose mode]:"<<(b_t_verb_f_nvb?ON:OFF)<<endl;
			cerr<<"-[lexicon mode]:"<<(b_t_lex_f_nlex?ON:OFF)<<endl;
			cerr<<"-[htmlout mode]:"<<(b_t_html_f_none?ON:OFF)<<endl;
  		}

		/*Kup **/ kup = new Kup(resource_dir.c_str(),b_t_verb_f_nvb,b_t_lex_f_nlex);

		if (b_t_verb_f_nvb)
			printf("true1\n");
		if (b_t_lex_f_nlex)
			printf("true2\n");




      Phrases tree;
		string input_string;
		while(getline(cin, input_string))
		{
	     	bool b_t_succ_f_fail=kup->doKup(input_string.c_str(), tree); 
         	     KupTrOut kto;
          	if(b_t_succ_f_fail)
          	{
               	if(b_t_html_f_none)
					kto.htmlTreeGraph(stderr, tree);
				else
					kto.bracket(stderr, tree);
          	}
			input_string.clear();
		}
          delete kup;
     }
     catch(KupError & ke)
     {
         cerr<<"- (<rcved by>"<<__FILE__<<"): "<<ke.what()<<endl;
         cerr<<typeid(ke).name()<<endl;
     }
  
     return 0;
}
