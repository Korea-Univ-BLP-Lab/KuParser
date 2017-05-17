#ifndef _PARSETREE_H
#define _PARSETREE_H

#include "DataType.h"
#include "StaticRule.h"
#include "DynamicRule.h"

// ������ ���¼��� ���� ���¼� ����
#define		NEXT	m_phrases->one[right->end_right+1]

// �����м��� ���õ� Ŭ����
class cParseTree			
{
	public:	
		// �����м� Ŭ���� ������ 
		cParseTree::cParseTree( cStaticRule *sRule, cDynamicRule *dRule ) { m_sRule = sRule; m_dRule = dRule; };
		// ����Ģ ����
		void cParseTree::newDynamicRule( cDynamicRule *dRule ) { m_dRule = dRule; };
		// �Ľ�Ʈ�� ����  
		bool	tree( Words *words, Phrases *phrases ); 
					
	private:										
		// �ѳ�徿 �����м�												
		bool	parseNode( aPhrase* Left, aPhrase* Right );
		// ���� �ĺ� ã��
		aPhrase*	findNext( aPhrase *phrase );

	private:
		cStaticRule		*m_sRule;				// ������ ��Ģ Ŭ����
		cDynamicRule	*m_dRule;				// �н�   ��Ģ Ŭ���� 
		Phrases			*m_phrases;				// ������ ����

};

#endif

