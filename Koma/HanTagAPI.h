#ifndef __HANTAGAPI__
#define __HANTAGAPI__

#include "nlp.def"
#include <stdio.h>

#ifndef __TaggingMethod__
#define __TaggingMethod__
typedef enum {
  NO_TAGGING,           // 형태소 분석만
  PATH_BASED_TAGGING,   // 경로 기반 태깅
  STATE_BASED_TAGGING   // 상태 기반 태깅
} TaggingMethod;
#endif

// 파일에서 한 문장을 읽어들이기
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
int LoadSentenceFromStream(FILE *f, char *Sentence, int MaxLength);

// 스트링(*pString)에서 한 문장을 읽어들이기
// 문장을 읽은 후 *pString의 값은 다음 문장의 시작 위치로 옮겨짐
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
int LoadSentenceFromString(char **pString, char *Sentence, int MaxLength);

// HanTag 실행을 위한 사전 및 정보 읽기
// 반환값 : 1 - 성공, 0 - 실패
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
int LoadHanTagEngine(const char *ProbEntryFilename, const char *ProbDataFilename);

// 사전 및 정보를 위한 메모리 해제
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
void EndHanTagEngine(void);

// HanTag class의 instance를 할당 받기
// 반환값 : NULL - 실패, 기타 - 성공
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
void *CreateHanTag(void);

// HanTag class의 instance를 해제
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
void FreeHanTag(void *HanTag);

// 이미 할당 받은 HanTag의 instance를 이용하여 한 문장을 형태소 분석 및 태깅
// 반환값 : 문장내 어절의 개수
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
int DoKomaAndHanTag(void *HanTag, TaggingMethod Method, char *Sentence, char *Wrd[MaxNumWrd], char *Result[MaxNumWrd][MaxNumAmb]);

// 이미 할당 받은 HanTag의 instance를 이용하여 한 문장을 형태소 분석 및 태깅
// 반환값 : 문장내 어절의 개수
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
int DoHanTag(void *HanTag, TaggingMethod Method, char *Cnd[MaxNumWrd][MaxNumAmb], char *Result[MaxNumWrd][MaxNumAmb]);

#endif
