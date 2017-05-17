

#ifndef _DATATYPE_H
#define _DATATYPE_H

#include <memory.h>

#define		MAX_BUF_LEN		1024	// 최대길이: 버퍼
#define		MAX_STR_LEN		256		// 최대길이: 문자열
#define		MAX_WRD_LEN		128		// 최대길이: 단어 
#define		MAX_POS_LEN		4		// 최대길이: 품사     NNG=3
#define		MAX_SYN_LEN		5		// 최대길이: 구문범주 NP/VNP,_SBJ/_OBJ=4

#define		MAX_WRD_CNT		64		// 최대개수: 문장당 어절수 = 57
#define		MAX_PHS_CNT		MAX_WRD_CNT*MAX_WRD_CNT/2+MAX_WRD_CNT
									// 최대개수: 중간결과물(삼각테이블) = 단어수*단어수/2+단어수
#define		phrase_idx(left,right)	left + MAX_WRD_CNT*(right-left) - (right-left)*((right-left)-1)/2
									// 부모번호: 삼각테이블에서의 부모 색인 번호 
#define		FILEOPEN(FilePointer,FileName,Mode,FunctionName) FILE *FilePointer=fopen(FileName,Mode); /*if (FilePointer==NULL) { printf("\nERROR(%s): 파일열기오류(%s)\n",FunctionName,FileName); }*/ 
									// 파일열기: 오류점검후 파일열기 

#define		MAX_LMD_CNT		4		// 최대개수: lambda 수 

#define		LEARN_DIC		4		// 단계: 사전구성
#define		LEARN_RULE		0		// 단계: 학습
#define		LEARN_LAMBDA	1
#define		TEST_TRAINSET	2		// 단계: 실험(학습집합)
#define		TEST_TESTSET	3		// 단계: 실험(실험집합)


typedef	struct	_aWord				// 단어
{		
	char	raw[MAX_WRD_LEN];		// 원어절
	char	pos[MAX_STR_LEN];		// 품사 태깅 결과
	
} aWord;


typedef	struct	_Words				// 품사 태깅 결과
{	
	aWord	one[MAX_WRD_CNT];		// 구문범주

} Words;


typedef struct	_Lex
{
	char	syn[MAX_SYN_LEN];		// 구문 
	char	pos[MAX_POS_LEN];		// 품사
	char	raw[MAX_WRD_LEN];		// 어휘 

} Lex;


typedef	struct	_aPhrase			// 구문요소 
{	
	Lex		cont;					// 내용
	Lex		func;					// 기능 
	short	size;					// 길이 

	short	end_left;				// 왼쪽   지배 끝 
	short	end_right;				// 오른쪽 지배 끝

	double	prob;					// 확률

	struct _aPhrase* left;			// 왼쪽   하위 범주
	struct _aPhrase* right;			// 오른쪽 하위 범주

} aPhrase;


struct Phrases			// 구문분석 트리
{
     Phrases()
     {
         clear();
     }
     void clear()
     {
          memset(this,NULL,sizeof(Phrases));
     }
	aPhrase	*root;					// 루트 
	aPhrase	*one[MAX_PHS_CNT];		// 노드
     Words     words;                   // 형태소 분석 결과 

};

#endif
