#ifndef _READTREE_H
#define _READTREE_H

#include "DataType.h"
#include "StaticRule.h"

// �����м� ����ġ �б�
class cReadTree				
{
	public:	
		// ������
		cReadTree::cReadTree( cStaticRule *rule );
		// �������� �б�
		bool	tree( FILE *file, Words *words, Phrases *phrases ); 

	private:
		// ��� �ϳ� �б�
		bool	readPhrase( FILE *file, aPhrase **phrase, short *word_idx );

	private:
		cStaticRule	*m_rule;				// ��Ģ 
		
		char	m_buf[MAX_BUF_LEN];		// ����
		int		m_buf_idx;				// ���� �ε���

		Words	*m_words;				// ����
		Phrases	*m_phrases;				// ���
};

#endif

