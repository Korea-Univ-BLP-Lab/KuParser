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

#define MAXSENTENCELEN 10000	//ó���� �� �ִ� ������ �ִ� ���̸� �����Ѵ�.

char *Buffer;
void *HanTag;		//���¼� �м��� �� ǰ�� �°� ������Ʈ
FILE *infile;
char *NextToRead;		//������ �ؽ�Ʈ�� ó���ϱ� ���� ������
char *Wrd[MaxNumWrd];		//������ �������� �����Ѵ�.
char Sentence[MAXSENTENCELEN];	//������ �����Ѵ�.
char *Result[MaxNumWrd][MaxNumAmb];	//���¼� �м� ����� �����Ѵ�.

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
  
	//�Էµ� �ƱԸ�Ʈ�� ó���Ѵ�.
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

  //�ɼ��� �����Ѵ�.
	//s=SPACING_ENABLE;
	s=SPACING_DISABLE;
	p=PARENTHESES_READ;
	sb=RETURN_NOBREAK;

	SetHanTagOptions(HanTag,s,p,sb);

	//�Է� ������ ����.
	//���Ͽ� ����� �ؽ�Ʈ�� ���� ó���� ���
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
		fprintf(stderr,"�Էµ� ������ ���� ���߽��ϴ�.\n");
		exit(0);
	}	
	//���ۿ� ����� �ؽ�Ʈ�� ó���� ���
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

  //���ۿ� ����� �ؽ�Ʈ�� ó���� ���
//	while (ret = LoadSentenceFromString(&NextToRead, Sentence, SpareSentence, MAXSENTENCELEN, HanTag)) {
	//���Ͽ� ����� �ؽ�Ʈ�� ó���� ���
	while (LoadSentenceFromStream(hFile, Sentence, MAXSENTENCELEN, HanTag)) { //���Ͽ��� �� ������ �д´�.
		
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
