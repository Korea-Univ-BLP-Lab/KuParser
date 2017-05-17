#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

#include "EvalTree.h"


////////////////////////////////////////////////////////////////////
//
//  cRule 클래스의 cmp 함수 
//
//  인자 : 후보노드, BTree의 노드  
//
//  반환 : 규칙비교결과값 
//   
//  역할 : BTree의 search와 insert에서 규칙을 비교할 때 사용하는 함수 
// 

cEvalTree::cEvalTree()
{
	// 평가값 초기화: 문장수/문장일치/정생성/과생성/미생성/미생성i이하
	m_snt_cnt=0; m_exactCnt=0; m_overCnt=0; m_underCnt=0;
	for ( int i=0; i<MAX_LES_THN; i++ ) { m_underCnt_lessthan[i]=0; }

	// 평가파일 초기화: 미생성 자료 출력 파일, 평가결과 출력 파일 
	FILE *file;
	file=fopen("under.txt", "w");	fclose(file);
	file=fopen("result.txt","a");	
	fprintf(file,"         문장길이\t문장수	정확률\t재현율\t CBs" );
	for ( i=0; i<MAX_LES_THN; i++ ) { fprintf(file,"\t %d이하", i ); }
	fprintf(file,"\t  초시간\t사전\t자료\t" );
	for ( i=0; i<MAX_LMD_CNT; i++ ) { fprintf(file," lmd%2d", i ); }
	fprintf(file,"\n" );
	fclose(file);
}


////////////////////////////////////////////////////////////////////
//
//  cParseData 클래스의 tree 함수 
//
//  인자 : 구문노드배열
//
//  반환 : 구문분석결과의 root (NULL이면 구문분석 실패)
//
//  역할 : 구문노드배열를 입력받아 구문분석하고 구문분석결과의 root 반환
//   

void cEvalTree::init()
{
	// 평가값 초기화: 문장수/문장일치/정생성/과생성/미생성/미생성i이하
	m_snt_cnt=0; m_exactCnt=0; m_overCnt=0; m_underCnt=0;
	for ( int i=0; i<MAX_LES_THN; i++ ) { m_underCnt_lessthan[i]=0; }
}


////////////////////////////////////////////////////////////////////
//
//  cRule 클래스의 preproc 함수 
//
//  인자 : 처리단계, 규칙폴더, 기본규칙번호, 정답배열 
//
//  역할 : 제약규칙 적용을 위한 초기화 
//

#define	DividedByNonZero(a, b)	(((a+b) == 0) ? 1 : (a+b))		// (0+0)/0=1로 설정 

void cEvalTree::tree( Phrases *corrects, Phrases *candidates/**/, char *filename/**/ )
{
	// 정생성, 미생성, 과생성
	int exactCnt=0, underCnt=0, overCnt=0;

	// 맞은수/정생성 평가
	precision( candidates->root, corrects, &exactCnt, &overCnt );

	// 미생성 평가
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
	// 평가결과: 문장수/문장일치/정생성/과생성/미생성
	m_snt_cnt++;
	
	for ( int i=0; i<MAX_LES_THN; i++ ) { if ( (exactCnt>0)&&(underCnt<=i)&&(overCnt<=i) ) { m_underCnt_lessthan[i]++; } }

	m_exactCnt = m_exactCnt + exactCnt; 
	m_overCnt  = m_overCnt  + overCnt; 
	m_underCnt = m_underCnt + underCnt;
}


////////////////////////////////////////////////////////////////////
//
//  cRule 클래스의 preproc 함수 
//
//  인자 : 처리단계, 규칙폴더, 기본규칙번호, 정답배열 
//
//  역할 : 제약규칙 적용을 위한 초기화 
//

void cEvalTree::precision( aPhrase *candidate, Phrases *corrects, int *exactCnt, int *overCnt )
{
	if ( candidate==NULL ) { return; }
	precision( candidate->left, corrects, exactCnt, overCnt ); 
	precision( candidate->right,corrects, exactCnt, overCnt ); 

	// 정답비교
	int idx=phrase_idx(candidate->end_left,candidate->end_right);
	if ( compare(corrects->one[idx],candidate)==false ) { (*overCnt)++; }
	else { (*exactCnt)++; }

	// 재현율 계산시 참고: 정답후보노드/단순중간결과물인지 구분
	candidate->cont.syn[4]='_';
}


////////////////////////////////////////////////////////////////////
//
//  cRule 클래스의 preproc 함수 
//
//  인자 : 처리단계, 규칙폴더, 기본규칙번호, 정답배열 
//
//  역할 : 제약규칙 적용을 위한 초기화 
//

void cEvalTree::recall( aPhrase *correct, Phrases *candidates, int *underCnt )
{
	if ( correct==NULL ) { return; }
	recall( correct->left, candidates, underCnt ); 
	recall( correct->right,candidates, underCnt ); 

	// 정답비교
	int idx=phrase_idx(correct->end_left,correct->end_right);
	if ( (compare(correct,candidates->one[idx])==false)||(candidates->one[idx]->cont.syn[4]!='_') ) { (*underCnt)++; }
}


////////////////////////////////////////////////////////////////////
//
//  cRule 클래스의 check 함수 
//
//  인자 : 왼쪽노드, 오른쪽노드
//
//  반환 : 제약규칙 적용결과
//   
//  역할 : 제약규칙에 만족하면 true, 아니면 false
//

bool cEvalTree::compare( aPhrase *correct, aPhrase *candidate )
{
	if ( (correct==NULL)||(candidate==NULL)||(correct->cont.syn[0]=='\0')||(candidate->cont.syn[0]=='\0') ) { return false; }
	// 구문태그 불일치
	else if ( strcmp(correct->cont.syn,candidate->cont.syn) ) {	return false; }
	// 기능태그 불일치
	else if ( strcmp(correct->func.syn,candidate->func.syn) ) { return false; }
	// 정답과 일치
	else { return true; }
}


////////////////////////////////////////////////////////////////////
//
//  cRule 클래스의 preproc 함수 
//
//  인자 : 처리단계, 규칙폴더, 기본규칙번호, 정답배열 
//
//  역할 : 제약규칙 적용을 위한 초기화 
//

void cEvalTree::print( char *filename, int step, int snt_size, int snt_cnt_size, unsigned long time, int sizeDic, int sizeRule, double lmd[MAX_LMD_CNT], double *recall )
{
	// 출력파일 열기 
	FILE *file;
	if ( step == LEARN_RULE ) { return; }
	else if ( ((file=fopen(filename,"a"))==NULL) ) { printf("\nERROR(cEvalTree::print): 파일열기오류\n"); exit(1); }

	// 분모!=0
	if ( (m_exactCnt+m_overCnt )==0 ) { m_overCnt =1; } 
	if ( (m_exactCnt+m_underCnt)==0 ) { m_underCnt=1; }

	if ( recall!=NULL ) { *recall = (float)m_exactCnt/(float)(m_exactCnt+m_underCnt)*(float)100; }

	if ( step == LEARN_LAMBDA) /*/return; /*/	{ fprintf(file,"[보정변수] " ); }/**/
	else if ( step == TEST_TRAINSET ) { fprintf(file,"[학습집합] " ); }
	else if ( step == TEST_TESTSET  ) { fprintf(file,"[실험집합] " ); }

	// 실험결과 출력
	fprintf(file,"%6d\t%6d\t%6.2f\t%6.2f\t%5.2f", snt_size, snt_cnt_size, (float)m_exactCnt/(float)(m_exactCnt+m_overCnt)*(float)100, (float)m_exactCnt/(float)(m_exactCnt+m_underCnt)*(float)100, (float)m_underCnt/(float)m_snt_cnt );
	for ( int i=0; i<MAX_LES_THN; i++ ) { fprintf(file,"\t%6.2f", (float)m_underCnt_lessthan[i]/(float)m_snt_cnt*(float)100 ); }
	fprintf(file,"\t%8.2f\t%d\t%d\t", (float)time*0.01/(float)m_snt_cnt, sizeDic, sizeRule );
	for (     i=0; i<MAX_LMD_CNT; i++ ) { fprintf(file,"  %4.2f", lmd[i] ); }
	fprintf(file,"\n");
	fclose(file);
}


