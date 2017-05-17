#ifndef _LEXICON_H
#define _LEXICON_H

#include "DataType.h"
#include "BTree.h"


typedef	struct	_Lexeme												// 사전 형식
{	
	char	word[MAX_STR_LEN];										// 단어
	int		freq;													// 빌생빈도

} Lexeme;


class cLexicon														// 어휘목록과 관련된 클래스
{
	public:	
		cLexicon( bool learn, char *filename, int size );			// 생성자(학습여부,파일이름)
		~cLexicon();												// 소멸자

		void		addToLexicon( Words *words );					// 어휘 추가(형태소분석결과)
		bool		check( char *word, char *pos, bool add );		// 어휘 점검(어휘,품사)
		void		endToLearn();									// 학습 완료	
		int			getSize();										// 사전 크기 반환 

		static int  cmp( const void *a, const void *b );			// 어휘 비교(후보,비교대상)
		static void	pnt( FILE *out, void *a );						// 어휘 출력(출력파일,출력대상)

	private:
		void		readLexicon( char *filename, cBTree *lexicon, int size );	// 사전 읽기	
		void		writeLexicon(char *filename, cBTree *lexicon );	// 사전 쓰기

	private:
		bool		 m_learn;										// 학습 여부
		char		*m_filename;									// 사전 파일이름 
		cBTree		*m_lexicon;										// 사전 	
		int			 m_size;										// 사전의 크기
};


#endif

