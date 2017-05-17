

#ifndef _DATATYPE_H
#define _DATATYPE_H

#include <memory.h>

#define		MAX_BUF_LEN		1024	// �ִ����: ����
#define		MAX_STR_LEN		256		// �ִ����: ���ڿ�
#define		MAX_WRD_LEN		128		// �ִ����: �ܾ� 
#define		MAX_POS_LEN		4		// �ִ����: ǰ��     NNG=3
#define		MAX_SYN_LEN		5		// �ִ����: �������� NP/VNP,_SBJ/_OBJ=4

#define		MAX_WRD_CNT		64		// �ִ밳��: ����� ������ = 57
#define		MAX_PHS_CNT		MAX_WRD_CNT*MAX_WRD_CNT/2+MAX_WRD_CNT
									// �ִ밳��: �߰������(�ﰢ���̺�) = �ܾ��*�ܾ��/2+�ܾ��
#define		phrase_idx(left,right)	left + MAX_WRD_CNT*(right-left) - (right-left)*((right-left)-1)/2
									// �θ��ȣ: �ﰢ���̺����� �θ� ���� ��ȣ 
#define		FILEOPEN(FilePointer,FileName,Mode,FunctionName) FILE *FilePointer=fopen(FileName,Mode); /*if (FilePointer==NULL) { printf("\nERROR(%s): ���Ͽ������(%s)\n",FunctionName,FileName); }*/ 
									// ���Ͽ���: ���������� ���Ͽ��� 

#define		MAX_LMD_CNT		4		// �ִ밳��: lambda �� 

#define		LEARN_DIC		4		// �ܰ�: ��������
#define		LEARN_RULE		0		// �ܰ�: �н�
#define		LEARN_LAMBDA	1
#define		TEST_TRAINSET	2		// �ܰ�: ����(�н�����)
#define		TEST_TESTSET	3		// �ܰ�: ����(��������)


typedef	struct	_aWord				// �ܾ�
{		
	char	raw[MAX_WRD_LEN];		// ������
	char	pos[MAX_STR_LEN];		// ǰ�� �±� ���
	
} aWord;


typedef	struct	_Words				// ǰ�� �±� ���
{	
	aWord	one[MAX_WRD_CNT];		// ��������

} Words;


typedef struct	_Lex
{
	char	syn[MAX_SYN_LEN];		// ���� 
	char	pos[MAX_POS_LEN];		// ǰ��
	char	raw[MAX_WRD_LEN];		// ���� 

} Lex;


typedef	struct	_aPhrase			// ������� 
{	
	Lex		cont;					// ����
	Lex		func;					// ��� 
	short	size;					// ���� 

	short	end_left;				// ����   ���� �� 
	short	end_right;				// ������ ���� ��

	double	prob;					// Ȯ��

	struct _aPhrase* left;			// ����   ���� ����
	struct _aPhrase* right;			// ������ ���� ����

} aPhrase;


struct Phrases			// �����м� Ʈ��
{
     Phrases()
     {
         clear();
     }
     void clear()
     {
          memset(this,NULL,sizeof(Phrases));
     }
	aPhrase	*root;					// ��Ʈ 
	aPhrase	*one[MAX_PHS_CNT];		// ���
     Words     words;                   // ���¼� �м� ��� 

};

#endif
