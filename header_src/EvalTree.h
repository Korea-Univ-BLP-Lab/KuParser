#ifndef _EVALTREE_H
#define _EVALTREE_H

#include "DataType.h"

#define	 MAX_LES_THN		5

// ��ġ��Ģ�� ���õ� Ŭ����
class cEvalTree
{
	public:	
		cEvalTree::cEvalTree();													// ������
		void	init();															// �� �ʱ�ȭ
		void	tree( Phrases *correct, Phrases *candidate, char *filename );	// Ʈ�� ��
		void	print( char *filename, int step, int snt_size, int snt_cnt_size, unsigned long time, int sizeDic, int sizeRule, double lmd[MAX_LMD_CNT], double *recall );	
																				// ���� ���
	private:																	// ��Ȯ��
		void	precision( aPhrase *candidate, Phrases *corrects, int *exactCnt, int *overCnt );
		void	recall( aPhrase *correct, Phrases *Candidates, int *underCnt );	// ������
		bool	compare(aPhrase *correct, aPhrase *candidate );					// ��� �� 

	private:
		long	m_snt_cnt, m_exactCnt, m_overCnt, m_underCnt;					// ��: �����/������/������/�̻���
		long	m_underCnt_lessthan[MAX_LES_THN];								// ��: �̻���i���� 
};

#endif

