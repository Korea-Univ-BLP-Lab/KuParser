#ifndef _READTREE_H
#define _READTREE_H

#include "DataType.h"
#include "StaticRule.h"

// 구문분석 말뭉치 읽기
class cReadTree				
{
	public:	
		// 생성자
		cReadTree::cReadTree( cStaticRule *rule );
		// 구문구조 읽기
		bool	tree( FILE *file, Words *words, Phrases *phrases ); 

	private:
		// 노드 하나 읽기
		bool	readPhrase( FILE *file, aPhrase **phrase, short *word_idx );

	private:
		cStaticRule	*m_rule;				// 규칙 
		
		char	m_buf[MAX_BUF_LEN];		// 버퍼
		int		m_buf_idx;				// 버퍼 인덱스

		Words	*m_words;				// 문장
		Phrases	*m_phrases;				// 노드
};

#endif

