// TaggerConsole.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <string.h>
#include <stdio.h>
#include "KomaAPI.h"
#include "HanTagAPI.h"
#include "nlp.def"
//#include "hcc98.h"

#define MainFSTFilename "./data/main.FST"
#define MainDataFilename "./data/main.dat"
#define ConnectionTableFilename "./data/BConnection.bin"
#define ProbEntryFilename "./data/prob.FST"
#define ProbDataFilename "./data/prob.dat"
#define SpaceEntryFilename "./data/space.FST"
#define SpaceDataFilename "./data/space.bin"
#define DicFileName "./data/user_nnp.dic"

#define MAXSENTENCELEN 10000	//처리할 수 있는 문장의 최대 길이를 지정한다.

char *Buffer;
void *HanTag;		//형태소 분석기 및 품사 태거 오브젝트
FILE *infile;
char *NextToRead;		//버퍼의 텍스트를 처리하기 위한 포인터
char *Wrd[MaxNumWrd];		//문장의 어절들을 저장한다.
char Sentence[MAXSENTENCELEN];	//문장을 저장한다.
char *Result[MaxNumWrd][MaxNumAmb];	//형태소 분석 결과를 저장한다.

void help (void) 
{
  fprintf (stdout, "KUTagger -t inputfilename : for Path_Based_Tagging,\n");
  fprintf (stdout, "KUTagger -m inputfilename : for Morphological Analysis,\n");
	exit(0);
}

int __main(int argc, char *argv[])
{ 
  int i;
  int FileSize;
  
	TaggingMethod m;
	SpacingMethod s;
	ParenthesesMethod p;
	SentenceBreakMethod sb;
  
	//입력된 아규먼트를 처리한다.
	if (argc<=1) help();
  else if (!strcmp ("-t", argv[1]) || !strcmp ("-T", argv[1]))
  	m = PATH_BASED_TAGGING;
  else if (!strcmp ("-m", argv[1]) || !strcmp ("-M", argv[1]))
		m = NO_TAGGING;
  else
  {
  	fprintf(stderr,"Invalide option\n");
  	return 1;
  }

  if (LoadKomaEngine(MainFSTFilename, MainDataFilename, ConnectionTableFilename, DicFileName)==0) {
		fprintf(stderr, "ERROR :: cannot load KOMA engine\n");
		return 1;
  }

  fprintf(stderr,"LoadKomaEngine OK\n");

  if (LoadHanTagEngine(ProbEntryFilename, ProbDataFilename,SpaceEntryFilename,SpaceDataFilename)==0) {
		fprintf(stderr, "ERROR :: cannot load HanTag engine\n");
		return 1;
  }

  fprintf(stderr,"LoadHanTagEngine OK\n");

  if ((HanTag=CreateHanTag())==NULL) {
		fprintf(stderr, "ERROR :: cannot make HanTag instance\n");
		return 1;
  }
  fprintf(stderr,"CreateHanTag OK\n");

  //옵션을 설정한다.
	//s=SPACING_ENABLE;
	s=SPACING_DISABLE;
	p=PARENTHESES_READ;
	sb=RETURN_NOBREAK;

	SetHanTagOptions(HanTag,s,p,sb);

	//입력 파일을 연다.
	//파일에 저장된 텍스트를 직접 처리할 경우
	HANDLE hFile;
	DWORD dwRead;

	hFile = CreateFile(
		argv[2],
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		fprintf(stderr,"입력된 파일을 열지 못했습니다.\n");
		exit(0);
	}	
	//버퍼에 저장된 텍스트를 처리할 경우
  /*if ((infile = fopen(argv[2],"rb"))==NULL) {
		fprintf(stderr,"file cann't be opened!\n");
		exit(0);
  }

  fseek(infile,0,SEEK_END);
  FileSize = ftell(infile);
  Buffer = (char *)malloc(FileSize);
  fseek(infile,0,SEEK_SET);
  fread(Buffer,FileSize,1,infile);
  Buffer[FileSize]='\0';
  NextToRead = Buffer;  */

  //버퍼에 저장된 텍스트를 처리할 경우
//	while (ret = LoadSentenceFromString(&NextToRead, Sentence, SpareSentence, MAXSENTENCELEN, HanTag)) {
	//파일에 저장된 텍스트를 처리할 경우
	while (LoadSentenceFromStream(hFile, Sentence, MAXSENTENCELEN, HanTag)) { //파일에서 한 문장을 읽는다.
		
		DoKomaAndHanTag(HanTag, m, Sentence, Wrd, Result);
		
		for (i=0; Result[i][0]; i++)
		{
			fprintf(stdout,"%s\t", Wrd[i]);
			for (int j=0; Result[i][j]; j++) {
			if (j) fprintf(stdout, "^");
				fprintf(stdout, "%s", Result[i][j]);
			}
			fprintf(stdout,"\r\n");
		}
		fprintf(stdout,"\n");
  }
  fprintf(stderr,"Tagging End\n");

  FreeHanTag(HanTag);
  EndHanTagEngine();
  EndKomaEngine();
	CloseHandle(hFile);
  return 0;
}
