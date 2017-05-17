#ifndef __MorphAPI__
#define __MorphAPI__

#include "nlp.def"

// Koma 실행을 위한 사전 및 정보 읽기
// 반환값 : 1 - 성공, 0 - 실패
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
int LoadKomaEngine(const char *MainEntryFilename, const char *MainDataFilename,
                    const char *ConnectionTableFilename,
                    const char *TagFilename, const char *TagOutFilename);

// 사전 및 정보를 위한 메모리 해제
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
void EndKomaEngine(void);

/* CodeType   -   1: KS5601 code, 0: TRIGEM code */

// Koma class의 instance를 할당 받기
// 반환값 : NULL - 실패, 기타 - 성공
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
void *CreateKoma(int CodeType);

// Koma class의 instance를 해제
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
void FreeKoma(void *Koma);

// 이미 할당 받은 Koma의 instance를 이용하여 한 어절을 형태소 분석 수행
// 반환값 : 1 - 성공, 0 - 실패
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
int DoKomaOneWord(void *Koma, char *Eojeol, char Result[MaxNumAmb][BuffSize]);

// 이미 할당 받은 Koma의 instance를 이용하여 한 문장을 형태소 분석 수행
// 반환값 : 문장내 어절 개수
// 주의 : 이 함수는 태거에서 사용하기 위한 것임
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
int DoKomaOneSentence(void *Koma, char *Sentence, void *Result);

// 형태소 분석 결과 접근용 함수

// 형태소 분석 결과 개수
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
short GetNumResult(void *Koma);

// wai번째 분석 결과의 wcol번째 형태소의 인덱스 가져오기
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
short GetPair(void *Koma, short wai, short wcol);

// pi번째 인덱스에 해당하는 형태소의 문자열
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
const char *GetMorph(void *Koma, short pi);

// pi번째 인덱스에 해당하는 형태소의 품사
#ifdef WIN32
extern "C" __declspec(dllexport) 
#endif
const char *GetTag(void *Koma, short pi);

#endif
