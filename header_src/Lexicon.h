#ifndef _LEXICON_H
#define _LEXICON_H

#include "DataType.h"
#include "BTree.h"


typedef	struct	_Lexeme												// ���� ����
{	
	char	word[MAX_STR_LEN];										// �ܾ�
	int		freq;													// ������

} Lexeme;


class cLexicon														// ���ָ�ϰ� ���õ� Ŭ����
{
	public:	
		cLexicon( bool learn, char *filename, int size );			// ������(�н�����,�����̸�)
		~cLexicon();												// �Ҹ���

		void		addToLexicon( Words *words );					// ���� �߰�(���¼Һм����)
		bool		check( char *word, char *pos, bool add );		// ���� ����(����,ǰ��)
		void		endToLearn();									// �н� �Ϸ�	
		int			getSize();										// ���� ũ�� ��ȯ 

		static int  cmp( const void *a, const void *b );			// ���� ��(�ĺ�,�񱳴��)
		static void	pnt( FILE *out, void *a );						// ���� ���(�������,��´��)

	private:
		void		readLexicon( char *filename, cBTree *lexicon, int size );	// ���� �б�	
		void		writeLexicon(char *filename, cBTree *lexicon );	// ���� ����

	private:
		bool		 m_learn;										// �н� ����
		char		*m_filename;									// ���� �����̸� 
		cBTree		*m_lexicon;										// ���� 	
		int			 m_size;										// ������ ũ��
};


#endif

