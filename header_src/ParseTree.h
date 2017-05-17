#ifndef _PARSETREE_H
#define _PARSETREE_H

#include "DataType.h"
#include "StaticRule.h"
#include "DynamicRule.h"

// 오른쪽 형태소의 다음 형태소 정보
#define		NEXT	m_phrases->one[right->end_right+1]

// 구문분석과 관련된 클래스
class cParseTree			
{
	public:	
		// 구문분석 클래스 생성자 
		cParseTree::cParseTree( cStaticRule *sRule, cDynamicRule *dRule ) { m_sRule = sRule; m_dRule = dRule; };
		// 새규칙 설정
		void cParseTree::newDynamicRule( cDynamicRule *dRule ) { m_dRule = dRule; };
		// 파스트리 생성  
		bool	tree( Words *words, Phrases *phrases ); 
					
	private:										
		// 한노드씩 구문분석												
		bool	parseNode( aPhrase* Left, aPhrase* Right );
		// 다음 후보 찾기
		aPhrase*	findNext( aPhrase *phrase );

	private:
		cStaticRule		*m_sRule;				// 결정적 규칙 클래스
		cDynamicRule	*m_dRule;				// 학습   규칙 클래스 
		Phrases			*m_phrases;				// 구구조 정보

};

#endif

