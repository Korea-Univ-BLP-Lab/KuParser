#ifndef _DYNAMICRULE_H
#define _DYNAMICRULE_H

#include "DataType.h"
#include "BTree.h"

#define		THRESHOLD		0.00
#define		MAX_RUL_CNT		8				// �ִ밳��: ��Ģ�� ����

#define		LF0				0				// ����  ���
#define		LC0				1				// ����  ����
#define		LS0				2				// ����  ũ�� 
#define		RS0				3				// ������ũ��

#define		LF1				4				// ����  ���
#define		LC1				5				// ����  ����
#define		LS1				6				// ����  ũ��
#define		RS1				7				// ������ũ��


			
typedef	struct	_aRule						// ��Ģ����
{	
	char	rule[MAX_STR_LEN];				// ��Ģ
	int		freq_given;						// ���Ǻ��� �߻���(Ȯ��)
	int		freq_rule;						// ��  Ģ�� ������(Ȯ��)	

} aRule;



// ��Ģ�� ���õ� Ŭ����
class cDynamicRule
{
	public:	
		// ��Ģ Ŭ���� ������
		cDynamicRule( bool step, char *filename );
		// ��Ģ Ŭ���� �Ҹ���
		~cDynamicRule();

		// ��Ģ Ȯ�� ��� 
		double	getProb( aPhrase parent, aPhrase left, aPhrase right, aPhrase outer_left, aPhrase outer_right );	
		// ��Ģ �� �߰�
		void	tree( aPhrase *phrase, Phrases phrases );	

		// ��Ģ �н� ��� ���
		void	endToLearnRule();
		// ���� ���� ��� ���
		void	endToLearnLambda();
		// ��Ģ ũ�� ��ȯ
		int		getSize();

		// ��Ģ ��
		static int cmp( const void *a, const void *b );
		// ��Ģ ���
		static void	pnt( FILE *out, void *a );

	private:
		// ��Ģ �б�
		void	readRule( char *filename, cBTree_v2 *rule[MAX_RUL_CNT] );
		// ��Ģ ����
		void	writeRule(char *filename, cBTree_v2 *rule[MAX_RUL_CNT] );
		// ��Ģ ���� 
		void	findRule( aRule *rule[MAX_RUL_CNT], aPhrase parent, aPhrase left, aPhrase right, aPhrase outer_left, aPhrase outer_right, bool given );

	public:	
		double	m_lmd[MAX_LMD_CNT];			// lambda ��

	private:
		bool	m_learn;					// ó������
		char	*m_filename;				// ��Ģ�����̸� 
		//cBTree	*m_rule[MAX_RUL_CNT];		// ��Ģ 	
          cBTree_v2	*m_rule[MAX_RUL_CNT];		// ��Ģ 	
};

#endif

