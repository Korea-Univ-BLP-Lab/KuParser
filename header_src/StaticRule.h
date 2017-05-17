#ifndef _STATICRULE_H
#define _STATICRULE_H

#include "DataType.h"
#include "Lexicon.h"

#define		MAX_UPS_CNT		16		// 최대개수: 단어의 분할(UPS: Units Per Spacingunit)
#define		MAX_POS_CNT		66		// 최대개수: 품사의 종류

typedef struct	_cnvrt
{
	char	cont_syn[MAX_SYN_LEN];	// 구문범주
	char	func_syn[MAX_SYN_LEN];	// 기능범주 
	char	pos[MAX_POS_LEN];		// 품사

}cnvrt;

// 규칙과 관련된 클래스
class cStaticRule
{
	public:
		// 결정규칙 클래스 생성자 
		cStaticRule::cStaticRule( cLexicon *lexicon ) { m_lexicon = lexicon; };
		// 새 사전 할당  
		void	cStaticRule::newLexicon( cLexicon *lexicon ) { m_lexicon = lexicon; };
		// 단어 단위 설정
		void	getSynUnits( Words *syntacticUnits, Words *spacingUnits );
		// 부모 할당 및 설정(단말노드)
		aPhrase *getParent( char *pos_tagged_word, short word_idx, Phrases *phrases );
		// 부모 할당 및 설정(비단말노드)
		aPhrase *getParent( aPhrase *left, aPhrase *right, Phrases *phrases );
		// 부모 설정
		aPhrase *setParent( aPhrase *parent, aPhrase *left, aPhrase *right );
		// 표지 재설정
		void	retagging( aPhrase **parent, Words *words, Phrases *phrases );
		// 형태소 분석결과에서 단어 품사 추출
		static 	bool getWordPOS( char *pos_tagged_word, char **word, char **pos );

	private:
		// 단어 단위 분할 
		void			segment( int index[2][MAX_UPS_CNT], aWord spacingUnit );
		// 메모리 할당 
		static aPhrase	*getPhrase( short left, short right, Phrases *phrases );
		// 점검(구문기능태그)
		void			checkSynTag( aPhrase **phrase );
		// 점검(어휘)
		static void		checkWord( char **raw, char **pos );
		// 괄호쌍 점검 
		char			checkQuotation( char *raw );
		
	private:
		cLexicon		*m_lexicon;
		int				m_dquotation;					// 큰   따옴표
		int				m_squotation;					// 작은 따옴표
		int				m_brace;						// 괄호 

		static cnvrt	m_pos2node[MAX_POS_CNT];		// 구문태그,품사,형태
};

#endif

