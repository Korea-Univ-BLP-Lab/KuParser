#ifndef _EVALTREE_H
#define _EVALTREE_H

#include "DataType.h"

#define	 MAX_LES_THN		5

// 일치규칙과 관련된 클래스
class cEvalTree
{
	public:	
		cEvalTree::cEvalTree();													// 생성자
		void	init();															// 평가 초기화
		void	tree( Phrases *correct, Phrases *candidate, char *filename );	// 트리 평가
		void	print( char *filename, int step, int snt_size, int snt_cnt_size, unsigned long time, int sizeDic, int sizeRule, double lmd[MAX_LMD_CNT], double *recall );	
																				// 파일 출력
	private:																	// 정확률
		void	precision( aPhrase *candidate, Phrases *corrects, int *exactCnt, int *overCnt );
		void	recall( aPhrase *correct, Phrases *Candidates, int *underCnt );	// 재현율
		bool	compare(aPhrase *correct, aPhrase *candidate );					// 노드 비교 

	private:
		long	m_snt_cnt, m_exactCnt, m_overCnt, m_underCnt;					// 평가: 문장수/정생성/과생성/미생성
		long	m_underCnt_lessthan[MAX_LES_THN];								// 평가: 미생성i이하 
};

#endif

