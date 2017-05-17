#ifndef _STATICRULE_H
#define _STATICRULE_H

#include "DataType.h"
#include "Lexicon.h"

#define		MAX_UPS_CNT		16		// �ִ밳��: �ܾ��� ����(UPS: Units Per Spacingunit)
#define		MAX_POS_CNT		66		// �ִ밳��: ǰ���� ����

typedef struct	_cnvrt
{
	char	cont_syn[MAX_SYN_LEN];	// ��������
	char	func_syn[MAX_SYN_LEN];	// ��ɹ��� 
	char	pos[MAX_POS_LEN];		// ǰ��

}cnvrt;

// ��Ģ�� ���õ� Ŭ����
class cStaticRule
{
	public:
		// ������Ģ Ŭ���� ������ 
		cStaticRule::cStaticRule( cLexicon *lexicon ) { m_lexicon = lexicon; };
		// �� ���� �Ҵ�  
		void	cStaticRule::newLexicon( cLexicon *lexicon ) { m_lexicon = lexicon; };
		// �ܾ� ���� ����
		void	getSynUnits( Words *syntacticUnits, Words *spacingUnits );
		// �θ� �Ҵ� �� ����(�ܸ����)
		aPhrase *getParent( char *pos_tagged_word, short word_idx, Phrases *phrases );
		// �θ� �Ҵ� �� ����(��ܸ����)
		aPhrase *getParent( aPhrase *left, aPhrase *right, Phrases *phrases );
		// �θ� ����
		aPhrase *setParent( aPhrase *parent, aPhrase *left, aPhrase *right );
		// ǥ�� �缳��
		void	retagging( aPhrase **parent, Words *words, Phrases *phrases );
		// ���¼� �м�������� �ܾ� ǰ�� ����
		static 	bool getWordPOS( char *pos_tagged_word, char **word, char **pos );

	private:
		// �ܾ� ���� ���� 
		void			segment( int index[2][MAX_UPS_CNT], aWord spacingUnit );
		// �޸� �Ҵ� 
		static aPhrase	*getPhrase( short left, short right, Phrases *phrases );
		// ����(��������±�)
		void			checkSynTag( aPhrase **phrase );
		// ����(����)
		static void		checkWord( char **raw, char **pos );
		// ��ȣ�� ���� 
		char			checkQuotation( char *raw );
		
	private:
		cLexicon		*m_lexicon;
		int				m_dquotation;					// ū   ����ǥ
		int				m_squotation;					// ���� ����ǥ
		int				m_brace;						// ��ȣ 

		static cnvrt	m_pos2node[MAX_POS_CNT];		// �����±�,ǰ��,����
};

#endif

