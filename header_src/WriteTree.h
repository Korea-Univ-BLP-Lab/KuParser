#ifndef _WRITETREE_H
#define _WRITETREE_H

#include "DataType.h"

// 구문분석 말뭉치 출력
class cWriteTree										
{
	public:											
		// 구문분석 출력
		void	tree( FILE *file, Words *words, Phrases *phrases );	
		// 형태소 분석 출력
		void	POStaggedSnt( FILE *file, Words *words );	
		// 의존관계 출력
		void	dependency( FILE *file, Words *words, Phrases *phrases );
													
	private:											
		// 중심어 번호 탐색 
		void	findHead( int head[MAX_WRD_CNT], aPhrase *phrase, aPhrase *parent, Phrases *phrases );
		// 구문분석된 노드 출력
		void	writePhrase( FILE *file, Words *words, aPhrase *phrase, short depth, bool blank );
};

#endif