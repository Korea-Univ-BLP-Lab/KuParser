#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

#include "EvalTree.h"


////////////////////////////////////////////////////////////////////
//
//  cRule Ŭ������ cmp �Լ� 
//
//  ���� : �ĺ����, BTree�� ���  
//
//  ��ȯ : ��Ģ�񱳰���� 
//   
//  ���� : BTree�� search�� insert���� ��Ģ�� ���� �� ����ϴ� �Լ� 
// 

cEvalTree::cEvalTree()
{
	// �򰡰� �ʱ�ȭ: �����/������ġ/������/������/�̻���/�̻���i����
	m_snt_cnt=0; m_exactCnt=0; m_overCnt=0; m_underCnt=0;
	for ( int i=0; i<MAX_LES_THN; i++ ) { m_underCnt_lessthan[i]=0; }

	// ������ �ʱ�ȭ: �̻��� �ڷ� ��� ����, �򰡰�� ��� ���� 
	FILE *file;
	file=fopen("under.txt", "w");	fclose(file);
	file=fopen("result.txt","a");	
	fprintf(file,"         �������\t�����	��Ȯ��\t������\t CBs" );
	for ( i=0; i<MAX_LES_THN; i++ ) { fprintf(file,"\t %d����", i ); }
	fprintf(file,"\t  �ʽð�\t����\t�ڷ�\t" );
	for ( i=0; i<MAX_LMD_CNT; i++ ) { fprintf(file," lmd%2d", i ); }
	fprintf(file,"\n" );
	fclose(file);
}


////////////////////////////////////////////////////////////////////
//
//  cParseData Ŭ������ tree �Լ� 
//
//  ���� : �������迭
//
//  ��ȯ : �����м������ root (NULL�̸� �����м� ����)
//
//  ���� : �������迭�� �Է¹޾� �����м��ϰ� �����м������ root ��ȯ
//   

void cEvalTree::init()
{
	// �򰡰� �ʱ�ȭ: �����/������ġ/������/������/�̻���/�̻���i����
	m_snt_cnt=0; m_exactCnt=0; m_overCnt=0; m_underCnt=0;
	for ( int i=0; i<MAX_LES_THN; i++ ) { m_underCnt_lessthan[i]=0; }
}


////////////////////////////////////////////////////////////////////
//
//  cRule Ŭ������ preproc �Լ� 
//
//  ���� : ó���ܰ�, ��Ģ����, �⺻��Ģ��ȣ, ����迭 
//
//  ���� : �����Ģ ������ ���� �ʱ�ȭ 
//

#define	DividedByNonZero(a, b)	(((a+b) == 0) ? 1 : (a+b))		// (0+0)/0=1�� ���� 

void cEvalTree::tree( Phrases *corrects, Phrases *candidates/**/, char *filename/**/ )
{
	// ������, �̻���, ������
	int exactCnt=0, underCnt=0, overCnt=0;

	// ������/������ ��
	precision( candidates->root, corrects, &exactCnt, &overCnt );

	// �̻��� ��
	recall( corrects->root, candidates, &underCnt );

/**if ( (underCnt>0)||(overCnt>0) ) 
	{ 
		FILE *under=fopen("under.txt","a");
		fprintf(under, "\n%s\n", filename ); 
		for ( int i=0; i<MAX_PHS_CNT; i++ ) 
		{ 
			if ( (candidates->one[i]!=NULL)&&(candidates->one[i]->cont.syn[0]!='\0') ) { fprintf(under, "\n(%3d%3d):%s%s %s%s", candidates->one[i]->end_left, candidates->one[i]->end_right, candidates->one[i]->cont.syn, candidates->one[i]->func.syn, candidates->one[i]->cont.raw, candidates->one[i]->func.raw ); } 
		}
		fprintf(under,"\n"); 
		fclose(under);
	}
/**/
	// �򰡰��: �����/������ġ/������/������/�̻���
	m_snt_cnt++;
	
	for ( int i=0; i<MAX_LES_THN; i++ ) { if ( (exactCnt>0)&&(underCnt<=i)&&(overCnt<=i) ) { m_underCnt_lessthan[i]++; } }

	m_exactCnt = m_exactCnt + exactCnt; 
	m_overCnt  = m_overCnt  + overCnt; 
	m_underCnt = m_underCnt + underCnt;
}


////////////////////////////////////////////////////////////////////
//
//  cRule Ŭ������ preproc �Լ� 
//
//  ���� : ó���ܰ�, ��Ģ����, �⺻��Ģ��ȣ, ����迭 
//
//  ���� : �����Ģ ������ ���� �ʱ�ȭ 
//

void cEvalTree::precision( aPhrase *candidate, Phrases *corrects, int *exactCnt, int *overCnt )
{
	if ( candidate==NULL ) { return; }
	precision( candidate->left, corrects, exactCnt, overCnt ); 
	precision( candidate->right,corrects, exactCnt, overCnt ); 

	// �����
	int idx=phrase_idx(candidate->end_left,candidate->end_right);
	if ( compare(corrects->one[idx],candidate)==false ) { (*overCnt)++; }
	else { (*exactCnt)++; }

	// ������ ���� ����: �����ĺ����/�ܼ��߰���������� ����
	candidate->cont.syn[4]='_';
}


////////////////////////////////////////////////////////////////////
//
//  cRule Ŭ������ preproc �Լ� 
//
//  ���� : ó���ܰ�, ��Ģ����, �⺻��Ģ��ȣ, ����迭 
//
//  ���� : �����Ģ ������ ���� �ʱ�ȭ 
//

void cEvalTree::recall( aPhrase *correct, Phrases *candidates, int *underCnt )
{
	if ( correct==NULL ) { return; }
	recall( correct->left, candidates, underCnt ); 
	recall( correct->right,candidates, underCnt ); 

	// �����
	int idx=phrase_idx(correct->end_left,correct->end_right);
	if ( (compare(correct,candidates->one[idx])==false)||(candidates->one[idx]->cont.syn[4]!='_') ) { (*underCnt)++; }
}


////////////////////////////////////////////////////////////////////
//
//  cRule Ŭ������ check �Լ� 
//
//  ���� : ���ʳ��, �����ʳ��
//
//  ��ȯ : �����Ģ ������
//   
//  ���� : �����Ģ�� �����ϸ� true, �ƴϸ� false
//

bool cEvalTree::compare( aPhrase *correct, aPhrase *candidate )
{
	if ( (correct==NULL)||(candidate==NULL)||(correct->cont.syn[0]=='\0')||(candidate->cont.syn[0]=='\0') ) { return false; }
	// �����±� ����ġ
	else if ( strcmp(correct->cont.syn,candidate->cont.syn) ) {	return false; }
	// ����±� ����ġ
	else if ( strcmp(correct->func.syn,candidate->func.syn) ) { return false; }
	// ����� ��ġ
	else { return true; }
}


////////////////////////////////////////////////////////////////////
//
//  cRule Ŭ������ preproc �Լ� 
//
//  ���� : ó���ܰ�, ��Ģ����, �⺻��Ģ��ȣ, ����迭 
//
//  ���� : �����Ģ ������ ���� �ʱ�ȭ 
//

void cEvalTree::print( char *filename, int step, int snt_size, int snt_cnt_size, unsigned long time, int sizeDic, int sizeRule, double lmd[MAX_LMD_CNT], double *recall )
{
	// ������� ���� 
	FILE *file;
	if ( step == LEARN_RULE ) { return; }
	else if ( ((file=fopen(filename,"a"))==NULL) ) { printf("\nERROR(cEvalTree::print): ���Ͽ������\n"); exit(1); }

	// �и�!=0
	if ( (m_exactCnt+m_overCnt )==0 ) { m_overCnt =1; } 
	if ( (m_exactCnt+m_underCnt)==0 ) { m_underCnt=1; }

	if ( recall!=NULL ) { *recall = (float)m_exactCnt/(float)(m_exactCnt+m_underCnt)*(float)100; }

	if ( step == LEARN_LAMBDA) /*/return; /*/	{ fprintf(file,"[��������] " ); }/**/
	else if ( step == TEST_TRAINSET ) { fprintf(file,"[�н�����] " ); }
	else if ( step == TEST_TESTSET  ) { fprintf(file,"[��������] " ); }

	// ������ ���
	fprintf(file,"%6d\t%6d\t%6.2f\t%6.2f\t%5.2f", snt_size, snt_cnt_size, (float)m_exactCnt/(float)(m_exactCnt+m_overCnt)*(float)100, (float)m_exactCnt/(float)(m_exactCnt+m_underCnt)*(float)100, (float)m_underCnt/(float)m_snt_cnt );
	for ( int i=0; i<MAX_LES_THN; i++ ) { fprintf(file,"\t%6.2f", (float)m_underCnt_lessthan[i]/(float)m_snt_cnt*(float)100 ); }
	fprintf(file,"\t%8.2f\t%d\t%d\t", (float)time*0.01/(float)m_snt_cnt, sizeDic, sizeRule );
	for (     i=0; i<MAX_LMD_CNT; i++ ) { fprintf(file,"  %4.2f", lmd[i] ); }
	fprintf(file,"\n");
	fclose(file);
}


