#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include<stdexcept>
#include<string>
using std::string;
using std::runtime_error;

#include "DynamicRule.h"


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule 클래스의 cDynamicRule 함수 
//
//  인자 : 학습여부, 규칙파일이름 
//
//  역할 : 변수 및 규칙 초기화 
//

cDynamicRule::cDynamicRule( bool learn, char *filename )
{
     /*
	// 변수 초기화
	m_learn=learn; m_filename=filename; memset(m_lmd,0,MAX_LMD_CNT*sizeof(double));
	for ( int idx=0; idx<MAX_RUL_CNT; idx++ ) { m_rule[idx] = new cBTree_v2(sizeof(aRule),cmp,pnt); }

	// 학습이 아니면 규칙 읽기
	if( m_learn == false ) { readRule( m_filename, m_rule ); }
     */
     
     // 변수 초기화
	m_learn=learn; m_filename=filename; memset(m_lmd,0,MAX_LMD_CNT*sizeof(double));
	for ( int idx=0; idx<MAX_RUL_CNT; idx++ ) { m_rule[idx] = new cBTree_v2(sizeof(aRule),cmp,pnt); }

	// 학습이 아니면 규칙 읽기
	if( m_learn == false ) { readRule( m_filename, m_rule ); }
     
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule 클래스의 ~cDynamicRule 함수 
//
//  역할 : 규칙 제거 
//

cDynamicRule::~cDynamicRule()
{
	for ( int idx=0; idx<MAX_RUL_CNT; idx++ ) { delete m_rule[idx]; }
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule 클래스의 readRule 함수 
//
//  인자 : 파일이름, 규칙이름 
//
//  역할 : 파일에서 규칙 읽기
//

void cDynamicRule::readRule( char *filename, cBTree_v2 *rule[MAX_RUL_CNT] )
{
	// 보정변수 초기화
	for ( int i=0; i<MAX_LMD_CNT; i++ ) { m_lmd[i]=(double)1.0; }

	// 파일(filename)에서 규칙(rule)을 읽기
	int idx=-1;	FILEOPEN(pFile,filename,"r","getRule in cRule");
     if(pFile==NULL) 
          throw runtime_error(string("[ERR] cDynamicRule: Rule File open Error"));
	for ( char buf[MAX_BUF_LEN] = ""; fgets(buf,MAX_BUF_LEN,pFile)!=NULL; )
	{
		if ( !strcmp(buf,"규칙빈도 조건빈도\t규칙\n" ) ) { continue; }
		else if ( !strncmp(buf,"//rule",strlen("//rule")) ) { idx++; }
		// 보정변수 읽기 
		else if ( !strncmp(buf,"//variable",strlen("//variable")) ) 
		{ 
			strtok(buf,"\t\n"); 
			for ( int i=0; i<MAX_LMD_CNT; i++ ) { m_lmd[i] = atof(strtok(NULL,"\t\n")); }
		}
		// 규칙 읽기 
		else 
		{
			// 규칙 설정
			aRule key; memset(&key,'\0',sizeof(aRule));
			key.freq_rule = atoi( strtok(buf, "\t\n") );
			key.freq_given= atoi( strtok(NULL,"\t\n") );
			strcpy( key.rule, strtok(NULL,"\n") ); 

			// 규칙 추가 
               // aRule *temp; if ( (temp=(aRule*)m_rule[idx]->search(&key))==NULL ) { m_rule[idx]->insert(&key); }  // 이전버전
               m_rule[idx]->insert(&key); // 바뀐 버전
		}
	}

     //for ( idx=0; idx<MAX_RUL_CNT; idx++ ) { m_rule[idx]->balance(); } // 이전버전
     for ( idx=0; idx<MAX_RUL_CNT; idx++ ) { m_rule[idx]->sort(); } // 바뀐 버전 

     /* aRule key1; strcpy(key1.rule,",EC,,,,1,0,,,,,0,0,,,,,0,0|JX,UNK");
     for ( idx=0; idx<MAX_RUL_CNT; idx++ )
     {
          aRule* p_find = (aRule*) m_rule[idx]->search(&key1);
     }
     */
	fclose(pFile);
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule 클래스의 writeRule 함수 
//
//  인자 : 파일이름, 규칙이름 
//
//  역할 : 파일에 규칙 쓰기
//

void cDynamicRule::writeRule( char *filename, cBTree_v2 *rule[MAX_RUL_CNT] )
{
	// 초기규칙의 파일(filename)에 조건부빈도수(freq_given)를 추가하여 파일(OutFileName)에 재출력
	if ( m_learn == true )
	{
		// 규칙 학습결과 출력(규칙빈도 \t 0 \t규칙\n)
		FILE *pFile=fopen(filename,"w"); if (pFile==NULL) { printf("\nERROR(getRule in cRule): 파일열기오류(%s)\n",filename); }
		fprintf(pFile,"규칙빈도 조건빈도\t규칙\n" );
		for ( int idx=0; idx<MAX_RUL_CNT; idx++ ) { fprintf(pFile,"//rule[%d]\n",idx); m_rule[idx]->pntlist(pFile); }
		fclose(pFile);

		// 규칙 학습결과 재출력(규칙빈도 \t 조건빈도 \t조건부|규칙\n)
		char  OutFileName[MAX_STR_LEN]; strcpy(OutFileName,filename); strcat(OutFileName,".bak");
		char  given[MAX_STR_LEN], freq_given[MAX_STR_LEN], buf[MAX_BUF_LEN], *temp;
		FILE * InFile, *OutFile; 
        for ( InFile=fopen(filename,"r"),OutFile=fopen(OutFileName,"w"); fgets(buf,MAX_BUF_LEN,InFile)!= NULL; )
		{
			if ( !strcmp(buf,"규칙빈도 조건빈도\t규칙\n" ) ) { fprintf(OutFile,"%s",buf ); continue; }
			else if ( !strncmp(buf,"//rule",strlen("//rule")) ) { fprintf(OutFile,"%s",buf ); continue; }

			// 조건부인 경우(규칙빈도 \t 조건빈도 \t조건부|\n)
			if ( buf[strlen(buf)-2]=='|' )
			{
				// 조건부와 조건빈도(freq_given) 저장
				strcpy(freq_given,strtok(buf," \t")); strtok(NULL," \t");
				strcpy(given,strtok(NULL," \t\n")); 
			}
			// 규칙인 경우(규칙빈도 \t 조건빈도 \t조건부|\n)
			else if ( !strncmp(given,&buf[17],strlen(given)) )
			{
				fprintf(OutFile,"%s\t",strtok(buf, "\t\n"));
				fprintf(OutFile,"%6s\t",freq_given ); temp=strtok(NULL, "\t\n");
				fprintf(OutFile,"%s\n",strtok(NULL, "\t\n"));
			}
		}
		
		fclose( InFile );
		fclose( OutFile);

		sprintf( buf, "del %s", filename ); system( buf );
		sprintf( buf,"rename %s %s", OutFileName, filename ); system( buf );
	}
	// 학습된 규칙 및 보정변수 출력
	else
	{
		// 학습된 규칙 출력
		FILE *pFile=fopen(filename,"w"); if (pFile==NULL) { printf("\nERROR(getRule in cRule): 파일열기오류(%s)\n",filename); }
		fprintf(pFile,"규칙빈도 조건빈도\t규칙\n" );
		for ( int idx=0; idx<MAX_RUL_CNT; idx++ ) { fprintf(pFile,"//rule[%d]\n",idx); m_rule[idx]->pntlist(pFile); }
		fclose(pFile);
	}
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule 클래스의 endToLearn 함수 
//
//  역할 : 규칙을 출력하고 학습 해제 
//

void cDynamicRule::endToLearnRule()
{
	// 규칙 출력
	writeRule( m_filename, m_rule );
	// 보정변수 수정 단계가 아니라 규칙 학습 단계이면 
	if ( m_learn == true ) 
	{ 
		// 학습 해제 
		m_learn = false; 
		// 조건부 빈도정보가 빠진 기존 규칙은 지우고 
		for ( int idx=0; idx<MAX_RUL_CNT; idx++ ) { delete m_rule[idx]; m_rule[idx] = new cBTree_v2(sizeof(aRule),cmp,pnt); } 
		// 조건부 빈도정보를 포함하는 규칙을 새로 읽어서 초기화 
		readRule( m_filename, m_rule );
	}
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule 클래스의 endToLearn 함수 
//
//  역할 : 규칙을 출력하고 학습 해제 
//

void cDynamicRule::endToLearnLambda()
{
	// 보정변수 출력 
	FILE *pFile=fopen(m_filename,"a"); if (pFile==NULL) { printf("\nERROR(getRule in cRule): 파일열기오류(%s)\n",m_filename); return; }
	fprintf(pFile,"//variable\t" );
	for ( int i=0; i<MAX_LMD_CNT; i++ ) { fprintf(pFile,"\t%4.2f", (float)m_lmd[i] ); }
	fprintf(pFile,"\n" );
	fclose(pFile);
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule 클래스의 getSize 함수 
//
//  역할 : 규칙 크기 반환
//

int	cDynamicRule::getSize()
{
    int size,idx;
	for ( size=0, idx=0; idx<MAX_RUL_CNT; idx++ ) { size = size + m_rule[idx]->getSize(); }
	return size;
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule 클래스의 cmp 함수 
//
//  인자 : 후보노드, BTree의 노드  
//
//  반환 : 규칙비교결과값 
//   
//  역할 : BTree에서 규칙을 추가(insert)하거나 탐색(search)할 때 사용하는 함수 
// 

int cDynamicRule::cmp( const void *a, const void *b )
{
	aRule *aa=(aRule*)a, *bb=(aRule*)b;
	return strcmp(((aRule*)a)->rule,((aRule*)b)->rule);
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule 클래스의 cmp 함수 
//
//  인자 : 규칙 출력 파일포인터, 출력할 규칙  
//
//  역할 : BTree에서 규칙을 출력(pntlist)할 때 사용하는 함수 
// 

void cDynamicRule::pnt( FILE *pFile, void *one )
{
	aRule *aa=(aRule*)one;
	fprintf( pFile, "%7d\t%8d\t%s\n", ((aRule*)one)->freq_rule, ((aRule*)one)->freq_given, ((aRule*)one)->rule );
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule 클래스의 tree 함수 
//
//  인자 : 현재노드, 구문트리
//
//  역할 : 현재노드를 생성하는 규칙을 학습
//

void cDynamicRule::tree( aPhrase *phrase, Phrases phrases )
{
	if ( m_learn == false ) { return; }
	else if ( (phrase==NULL)||(phrase->left==NULL)||(phrase->right==NULL) ) { return; }
	
	tree( phrase->left, phrases ); tree( phrase->right, phrases ); // 하위자식 학습 

	// 규칙, 외부문맥
	aRule *rule[MAX_RUL_CNT]; aPhrase *outer_left, *outer_right, temp; memset(&temp,'\0',sizeof(aPhrase)); 
	if ( (phrase->end_left==0)||((outer_left=phrases.one[phrase->end_left-1])==NULL) ) { outer_left =&temp; }
	if ( (phrase->end_right==MAX_WRD_CNT)||((outer_right=phrases.one[phrase->end_right+1])==NULL) ) { outer_right=&temp; }

	// 규칙 빈도 계산(규칙,부모,왼쪽,오른쪽,조건부/규칙) 
	findRule( rule, *phrase, *(phrase->left), *(phrase->right), *outer_left, *outer_right, false );
	int idx;
    for ( idx=0; idx<MAX_RUL_CNT; idx++ ) { rule[idx]->freq_rule++; }

	// 조건부 빈도 계산(규칙,부모,왼쪽,오른쪽,조건부/규칙) 
	findRule( rule, *phrase, *(phrase->left), *(phrase->right), *outer_left, *outer_right, true );
	//int idx;
    for ( idx=0; idx<MAX_RUL_CNT; idx++ ) { rule[idx]->freq_rule++; }
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule 클래스의 getProb 함수 
//
//  인자 : 부모, 왼쪽자식, 오른쪽자식, 왼쪽외부문맥, 오른쪽외부문맥
//
//  반환 : 부모노드의 생성확률
//   
//  역할 : 주어진 노드정보를 바탕으로 부모노드의 생성확률을 계산 
//

/*// 이공주 모델 & Collins 모델 
double cDynamicRule::getProb( aPhrase parent, aPhrase left, aPhrase right, aPhrase outer_left, aPhrase outer_right )
{
	aRule *rule[MAX_RUL_CNT]; findRule( rule, parent, *(parent.left), *(parent.right), outer_left, outer_right, false );
	
	double probRule = (double)0.0;
	double coOcurre = (double)0.0; 

	if ( rule[0]!=NULL ) { probRule = probRule + (double)rule[0]->freq_rule / (double)rule[0]->freq_given * m_lmd[0]; }
	if ( rule[1]!=NULL ) { probRule = probRule + (double)rule[1]->freq_rule / (double)rule[1]->freq_given * ((double)1.0-m_lmd[0]) * m_lmd[1]; }
	if ( rule[2]!=NULL ) { probRule = probRule + (double)rule[2]->freq_rule / (double)rule[2]->freq_given * ((double)1.0-m_lmd[0]) * ((double)1.0-m_lmd[1]) * m_lmd[2]; }
	if ( rule[3]!=NULL ) { probRule = probRule + (double)rule[3]->freq_rule / (double)rule[3]->freq_given * ((double)1.0-m_lmd[0]) * ((double)1.0-m_lmd[1]) * ((double)1.0-m_lmd[2]); }

	if ( rule[4]!=NULL ) { coOcurre = coOcurre + (double)rule[4]->freq_rule / (double)rule[4]->freq_given * m_lmd[3]; }
	if ( rule[5]!=NULL ) { coOcurre = coOcurre + (double)rule[5]->freq_rule / (double)rule[5]->freq_given * ((double)1.0-m_lmd[3]) * m_lmd[4]; }
	if ( rule[6]!=NULL ) { coOcurre = coOcurre + (double)rule[6]->freq_rule / (double)rule[6]->freq_given * ((double)1.0-m_lmd[3]) * ((double)1.0-m_lmd[4]) * m_lmd[5]; }
	if ( rule[7]!=NULL ) { coOcurre = coOcurre + (double)rule[7]->freq_rule / (double)rule[7]->freq_given * ((double)1.0-m_lmd[3]) * ((double)1.0-m_lmd[4]) * ((double)1.0-m_lmd[5]); }

	double prob = probRule * coOcurre;
	return prob;
}

/*/// 박소영 모델
double cDynamicRule::getProb( aPhrase parent, aPhrase left, aPhrase right, aPhrase outer_left, aPhrase outer_right )
{
	aRule *rule[MAX_RUL_CNT]; findRule( rule, parent, *(parent.left), *(parent.right), outer_left, outer_right, false );
	
	double lf = (double)0.0; 
	double lc = (double)0.0; 
	double ls = (double)0.0; 
	double rs = (double)0.0; 

	if ( rule[LF1]!=NULL ) { lf = lf + (double)rule[LF1]->freq_rule / (double)rule[LF1]->freq_given * m_lmd[0]; }
	if ( rule[LC1]!=NULL ) { lc = lc + (double)rule[LC1]->freq_rule / (double)rule[LC1]->freq_given * m_lmd[1]; }
	if ( rule[LS1]!=NULL ) { ls = ls + (double)rule[LS1]->freq_rule / (double)rule[LS1]->freq_given * m_lmd[2]; }
	if ( rule[RS1]!=NULL ) { rs = rs + (double)rule[RS1]->freq_rule / (double)rule[RS1]->freq_given * m_lmd[3]; }

	if ( rule[LF0]!=NULL ) { lf = lf + (double)rule[LF0]->freq_rule / (double)rule[LF0]->freq_given * ((double)1.0-m_lmd[0]); }
	if ( rule[LC0]!=NULL ) { lc = lc + (double)rule[LC0]->freq_rule / (double)rule[LC0]->freq_given * ((double)1.0-m_lmd[1]); }
	if ( rule[LS0]!=NULL ) { ls = ls + (double)rule[LS0]->freq_rule / (double)rule[LS0]->freq_given * ((double)1.0-m_lmd[2]); }
	if ( rule[RS0]!=NULL ) { rs = rs + (double)rule[RS0]->freq_rule / (double)rule[RS0]->freq_given * ((double)1.0-m_lmd[3]); }

	double prob = lf * lc * ls * rs;
	return prob;
}
/**/

////////////////////////////////////////////////////////////////////
//
//  cDynamicRule 클래스의 findRule 함수 
//
//  인자 : 규칙, 부모, 왼쪽자식, 오른쪽자식, 왼쪽외부문맥, 오른쪽외부문맥, 조건부/규칙
//
//  역할 : 주어진 노드정보를 바탕으로 부모노드의 생성규칙을 탐색 
//

// 작은 구의 최대 길이:4, 중간 구의 최대 길이:7 

#define	SMT(size,right)	((right.cont.pos[0]=='\0')?(4):((size<4)?(1):(((size<7)?(2):(3))))) 

#define	OLFP	outer_left.func.pos

#define PFP		parent.func.pos
#define PCP		parent.cont.pos
#define PSP		SMT(parent.size,outer_right)

#define LFP		left.func.pos
#define LCP		left.cont.pos
#define LSP		SMT(left.size,right)

#define RFP		right.func.pos
#define RCP		right.cont.pos
#define RSP		SMT(right.size,outer_right)

#define PFW		parent.func.raw
#define PCW		parent.cont.raw
#define PSW		parent.size

#define LFW		left.func.raw
#define LCW		left.cont.raw
#define LSW		((left.size<13)?(left.size):(13))

#define RFW		right.func.raw
#define RCW		right.cont.raw
#define RSW		right.size

// "datatype.h"		조정: #define  MAX_LMD_CNT 24  
// "DynamicRule.cpp"조정: DynamicRule::getProb()의 확률 계산 식

// Collins 모델
/*#define	DIST(size)	((size<2)?(1):(2))
void cDynamicRule::findRule( aRule *rule[MAX_RUL_CNT], aPhrase parent, aPhrase left, aPhrase right, aPhrase outer_left, aPhrase outer_right, bool given )
{
	aRule	*temp, key; key.freq_given=0; key.freq_rule=0; 

	for ( int idx=0; idx<MAX_RUL_CNT; idx++)
	{
		switch (idx)
		{
		case 0:	   { if(given==true) { sprintf( key.rule, "%s,%s|%s",PFP,PFW,"" ); break; } 
			         else {            sprintf( key.rule, "%s,%s|%s",PFP,PFW,RFP); break; }
				   }
		case 1:    { if(given==true) { sprintf( key.rule, "%s,%s|%s",PFP,"" ,"" ); break; }  
				     else {            sprintf( key.rule, "%s,%s|%s",PFP,"" ,RFP); break; }
				   }   
		case 2:    { if(given==true) { sprintf( key.rule, "%s,%s|%s","" ,"" ,"" ); break; }  
				     else {            sprintf( key.rule, "%s,%s|%s","" ,"" ,RFP); break; }
				   }   
		case 3:    { if(given==true) { sprintf( key.rule, "%s,%s|%s","" ,"" ,"" ); break; }  
				     else {            sprintf( key.rule, "%s,%s|%s","" ,"" ,"" ); break; }
				   }   

		case 4:    { if(given==true) { sprintf( key.rule, "%s,%s,%d|%s%s%s",PFP,PFW,RSP,"" ,"" ,"" ); break; } 
				     else {            sprintf( key.rule, "%s,%s,%d|%s%s%s",PFP,PFW,RSP,LFP,",",LFW); break; }
				   }   
		case 5:    { if(given==true) { sprintf( key.rule, "%s,%s,%d|%s%s%s",PFP,"" ,RSP,"" ,"" ,"" ); break; } 
				     else {            sprintf( key.rule, "%s,%s,%d|%s%s%s",PFP,"" ,RSP,LFP,",",LFW); break; }
				   }   
		case 6:    { if(given==true) { sprintf( key.rule, "%s,%s,%d|%s%s%s","" ,"" ,RSP,"" ,"" ,"" ); break; } 
				     else {            sprintf( key.rule, "%s,%s,%d|%s%s%s","" ,"" ,RSP,LFP,",",LFW); break; }
				   }   
		case 7:    { if(given==true) { sprintf( key.rule, "%s,%s,%d|%s%s%s","" ,"" ,0  ,"" ,"" ,"" ); break; } 
				     else {            sprintf( key.rule, "%s,%s,%d|%s%s%s","" ,"" ,0  ,"" ,",","" ); break; }
				   }   
		}

		if ( m_learn == true ) { while ( (temp=(aRule*)m_rule[idx]->search(&key))==NULL ) { m_rule[idx]->insert(&key); } }
		else { temp=(aRule*)m_rule[idx]->search(&key); }

		rule[idx]=temp;
	}
}
/*
//이공주 모델
void cDynamicRule::findRule( aRule *rule[MAX_RUL_CNT], aPhrase parent, aPhrase left, aPhrase right, aPhrase outer_left, aPhrase outer_right, bool given )
{
	aRule	*temp, key; key.freq_given=0; key.freq_rule=0; 

	for ( int idx=0; idx<MAX_RUL_CNT; idx++)
	{
		switch (idx)
		{
		case 0:	   { if(given==true) { sprintf( key.rule, "%s,%s,%s|%s%s%s",outer_left.func.pos,outer_right.func.pos,parent.func.pos,""           ,"" ,""            ); break; } 
			         else {            sprintf( key.rule, "%s,%s,%s|%s%s%s",outer_left.func.pos,outer_right.func.pos,parent.func.pos,left.func.pos,",",right.func.pos); break; }
				   }
		case 1:    { if(given==true) { sprintf( key.rule, "%s,%s,%s|%s%s%s",""                 ,outer_right.func.pos,parent.func.pos,""           ,"" ,""            ); break; } 
				     else {            sprintf( key.rule, "%s,%s,%s|%s%s%s",""                 ,outer_right.func.pos,parent.func.pos,left.func.pos,",",right.func.pos); break; }
				   }   
		case 2:    { if(given==true) { sprintf( key.rule, "%s,%s,%s|%s%s%s",outer_left.func.pos,""                  ,parent.func.pos,""           ,"" ,""            ); break; } 
				     else {            sprintf( key.rule, "%s,%s,%s|%s%s%s",outer_left.func.pos,""                  ,parent.func.pos,left.func.pos,",",right.func.pos); break; }
				   }   
		case 3:    { if(given==true) { sprintf( key.rule, "%s,%s,%s|%s%s%s",""                 ,""                  ,parent.func.pos,""           ,"" ,""            ); break; } 
				     else {            sprintf( key.rule, "%s,%s,%s|%s%s%s",""                 ,""                  ,parent.func.pos,left.func.pos,",",right.func.pos); break; }
				   }   

		case 4:    { if(given==true) { sprintf( key.rule, "%s,%s,%d,%s|%s%s%s",right.cont.pos,right.cont.raw,0,left.func.pos,""           ,"" ,""           ); break; } 
				     else {            sprintf( key.rule, "%s,%s,%d,%s|%s%s%s",right.cont.pos,right.cont.raw,0,left.func.pos,left.cont.pos,",",left.cont.raw); break; }
				   }   
		case 5:    { if(given==true) { sprintf( key.rule, "%s,%s,%d,%s|%s%s%s",right.cont.pos,""            ,0,left.func.pos,""           ,"" ,""           ); break; } 
				     else {            sprintf( key.rule, "%s,%s,%d,%s|%s%s%s",right.cont.pos,""            ,0,left.func.pos,left.cont.pos,",",left.cont.raw); break; }
				   }   
		case 6:    { if(given==true) { sprintf( key.rule, "%s,%s,%d,%s|%s%s%s",right.cont.pos,right.cont.raw,0,left.func.pos,""           ,"" ,""           ); break; } 
				     else {            sprintf( key.rule, "%s,%s,%d,%s|%s%s%s",right.cont.pos,right.cont.raw,0,left.func.pos,left.cont.pos,",",""           ); break; }
				   }   
		case 7:    { if(given==true) { sprintf( key.rule, "%s,%s,%d,%s|%s%s%s",right.cont.pos,""            ,0,left.func.pos,""           ,"" ,""           ); break; } 
				     else {            sprintf( key.rule, "%s,%s,%d,%s|%s%s%s",right.cont.pos,""            ,0,left.func.pos,left.cont.pos,",",""           ); break; }
				   }   
		}

		if ( m_learn == true ) { while ( (temp=(aRule*)m_rule[idx]->search(&key))==NULL ) { m_rule[idx]->insert(&key); } }
		else { temp=(aRule*)m_rule[idx]->search(&key); }

		rule[idx]=temp;
	}
}
/**/

#define PRN(a,OLFP,PFP,PFW,PCP,PCW,PSP,PSW,LFP,LFW,LCP,LCW,LSP,LSW,RFP,RFW,RCP,RCW,RSP,RSW,POS,AND,WRD) ((a==0)?(sprintf( key.rule, "%s,%s,%s,%s,%s,%d,%d,%s,%s,%s,%s,%d,%d,%s,%s,%s,%s,%d,%d|%s%s%s", OLFP,PFP,PFW,PCP,PCW,PSP,PSW,LFP,LFW,LCP,LCW,LSP,LSW,RFP,RFW,RCP,RCW,RSP,RSW,POS,AND,WRD)):((a==1)?(sprintf( key.rule, "%s,%s,%s,%s,%s,%d,%d,%s,%s,%s,%s,%d,%d,%s,%s,%s,%s,%d,%d|",       OLFP,PFP,PFW,PCP,PCW,PSP,PSW,LFP,LFW,LCP,LCW,LSP,LSW,RFP,RFW,RCP,RCW,RSP,RSW)):(sprintf( key.rule, "%s,%s,%s,%s,%s,%d,%d,%s,%s,%s,%s,%d,%d,%s,%s,%s,%s,%d,%d|%d%s%d", OLFP,PFP,PFW,PCP,PCW,PSP,PSW,LFP,LFW,LCP,LCW,LSP,LSW,RFP,RFW,RCP,RCW,RSP,RSW,POS,AND,WRD))))

// "datatype.h"		조정: #define  MAX_LMD_CNT 24  
// "DynamicRule.cpp"조정: DynamicRule::getProb()의 확률 계산 식
// "DynamicRule.h"  조정: #define  MAX_RUL_CNT 8
//                        #define		MAX_RUL_CNT		2				// 최대개수: 규칙의 종류
//                        #define		LF0				0				// 왼쪽  기능
//                        #define		LC0				1				// 왼쪽  내용
//                        #define		LS0				2				// 왼쪽  크기 
//                        #define		RS0				3				// 오른쪽크기
//                        #define		LF1				1				// 왼쪽  기능
//                        #define		LC1				5				// 왼쪽  내용
//                        #define		LS1				6				// 왼쪽  크기
//                        #define		RS1				7				// 오른쪽크기
// "main.cpp"       조정: if ((i==2)&&(i==3)){ dRule->m_lmd[i]=(double)1.0; continue; }

/**/
//박소영 모델

// 내용/기능/크기
/**
void cDynamicRule::findRule( aRule *rule[MAX_RUL_CNT], aPhrase parent, aPhrase left, aPhrase right, aPhrase outer_left, aPhrase outer_right, bool given )
{
	aRule	*temp, key; key.freq_given=0; key.freq_rule=0; 

	for ( int idx=0; idx<MAX_RUL_CNT; idx++)
	{
		switch (idx)
		{
		case LF0: { if(given==true) { PRN(1,""  ,"" ,"" ,"" ,"" ,PSP,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
			        else {            PRN(2,""  ,"" ,"" ,"" ,"" ,PSP,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,LSP,",",LSW); break; }
				  }
		case LC0: { if(given==true) { PRN(1,""  ,"" ,"" ,"" ,"" ,PSP,0  ,"" ,"" ,"" ,"" ,LSP,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(2,""  ,"" ,"" ,"" ,"" ,PSP,0  ,"" ,"" ,"" ,"" ,LSP,0  ,"" ,"" ,"" ,"" ,0  ,0  ,RSP,",",RSW); break; }
				  }   
		case LS0: { if(given==true) { PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"A",",","B"); break; }
				  }   
		case RS0: { if(given==true) { PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"A",",","B"); break; }
				  }   
		case LF1: { if(given==true) { PRN(1,""  ,"" ,"" ,"" ,"" ,PSP,PSW,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
			         else {           PRN(2,""  ,"" ,"" ,"" ,"" ,PSP,PSW,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,LSP,",",LSW); break; }
				  }
		case LC1: { if(given==true) { PRN(1,""  ,"" ,"" ,"" ,"" ,PSP,PSW,"" ,"" ,"" ,"" ,LSP,LSW,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(2,""  ,"" ,"" ,"" ,"" ,PSP,PSW,"" ,"" ,"" ,"" ,LSP,LSW,"" ,"" ,"" ,"" ,0  ,0  ,RSP,",",RSW); break; }
				  }   
		case LS1: { if(given==true) { PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"A",",","B"); break; }
				  }   
		case RS1: { if(given==true) { PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"A",",","B"); break; }
				  }   
		}

		if ( m_learn == true ) { while ( (temp=(aRule*)m_rule[idx]->search(&key))==NULL ) { m_rule[idx]->insert(&key); } }
		else { temp=(aRule*)m_rule[idx]->search(&key); }

		rule[idx]=temp;
	}
}/**/

//내용+기능
/**
void cDynamicRule::findRule( aRule *rule[MAX_RUL_CNT], aPhrase parent, aPhrase left, aPhrase right, aPhrase outer_left, aPhrase outer_right, bool given )
{
	aRule	*temp, key; key.freq_given=0; key.freq_rule=0; 

	for ( int idx=0; idx<MAX_RUL_CNT; idx++)
	{
		switch (idx)
		{
		case LF0: { if(given==true) { PRN(0,""  ,PFP,"" ,PCP,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
			        else {            PRN(0,""  ,PFP,"" ,PCP,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,LFP,",",LFW); break; }
				  }
		case LC0: { if(given==true) { PRN(0,""  ,"" ,"" ,PCP,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,"" ,"" ,PCP,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,LCP,",",LCW); break; }
				  }   
		case LS0: { if(given==true) { PRN(0,""  ,PFP,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,PFP,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,RFP,",",RFW); break; }
				  }   
		case RS0: { if(given==true) { PRN(0,""  ,"" ,"" ,PCP,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,"" ,"" ,PCP,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,RCP,",",RCW); break; }
				  }   
		case LF1: { if(given==true) { PRN(0,""  ,PFP,"" ,PCP,PCW,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
			         else {           PRN(0,""  ,PFP,"" ,PCP,PCW,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,LFP,",",LFW); break; }
				  }
		case LC1: { if(given==true) { PRN(0,""  ,PFP,"" ,PCP,PCW,0  ,0  ,LFP,LFW,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,PFP,"" ,PCP,PCW,0  ,0  ,LFP,LFW,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,LCP,",",LCW); break; }
				  }   
		case LS1: { if(given==true) { PRN(0,""  ,PFP,"" ,PCP,"" ,0  ,0  ,LFP,"" ,LCP,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,PFP,"" ,PCP,"" ,0  ,0  ,LFP,"" ,LCP,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,RFP,",",RFW); break; }
				  }   
		case RS1: { if(given==true) { PRN(0,""  ,PFP,"" ,PCP,"" ,0  ,0  ,LFP,"" ,LCP,"" ,0  ,0  ,RFP,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,PFP,"" ,PCP,"" ,0  ,0  ,LFP,"" ,LCP,"" ,0  ,0  ,RFP,"" ,"" ,"" ,0  ,0  ,RCP,",",RCW); break; }
				  }   
		}

		if ( m_learn == true ) { while ( (temp=(aRule*)m_rule[idx]->search(&key))==NULL ) { m_rule[idx]->insert(&key); } }
		else { temp=(aRule*)m_rule[idx]->search(&key); }

		rule[idx]=temp;
	}
}


/**
// 기능, 내용, 크기
/*/void cDynamicRule::findRule( aRule *rule[MAX_RUL_CNT], aPhrase parent, aPhrase left, aPhrase right, aPhrase outer_left, aPhrase outer_right, bool given )
{
	aRule	*temp, key; key.freq_given=0; key.freq_rule=0; 

	for ( int idx=0; idx<MAX_RUL_CNT; idx++)
	{
		switch (idx)
		{
		case LF0: { if(given==true) { PRN(0,""  ,PFP,"" ,"" ,"" ,PSP,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
			        else {            PRN(0,""  ,PFP,"" ,"" ,"" ,PSP,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,LFP,",",LFW); break; }
				  }
		case LC0: { if(given==true) { PRN(0,""  ,"" ,"" ,PCP,"" ,PSP,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,"" ,"" ,PCP,"" ,PSP,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,LCP,",",LCW); break; }
				  }   
		case LS0: { if(given==true) { PRN(1,""  ,"" ,"" ,PCP,"" ,PSP,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,0  ,"" ,0  ); break; } 
				     else {           PRN(2,""  ,"" ,"" ,PCP,"" ,PSP,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,LSP,",",LSW); break; }
				  }   
		case RS0: { if(given==true) { PRN(1,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,0  ,"" ,0  ); break; } 
				     else {           PRN(2,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,0  ,"" ,0  ); break; }
				  }   
		case LF1: { if(given==true) { PRN(0,OLFP,PFP,"" ,PCP,PCW,PSP,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
			         else {           PRN(0,OLFP,PFP,"" ,PCP,PCW,PSP,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,LFP,",",LFW); break; }
				  }
		case LC1: { if(given==true) { PRN(0,""  ,PFP,"" ,PCP,PCW,PSP,0  ,LFP,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,PFP,"" ,PCP,PCW,PSP,0  ,LFP,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,LCP,",",LCW); break; }
				  }   
		case LS1: { if(given==true) { PRN(1,""  ,PFP,"" ,PCP,"" ,PSP,0  ,LFP,"" ,LCP,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,0  ,"" ,0  ); break; } 
				     else {           PRN(2,""  ,PFP,"" ,PCP,"" ,PSP,0  ,LFP,"" ,LCP,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,LSP,",",LSW); break; }
				  }   
		case RS1: { if(given==true) { PRN(1,""  ,PFP,"" ,PCP,"" ,PSP,0  ,LFP,"" ,LCP,"" ,LSP,0  ,RFP,"" ,RCP,"" ,0  ,0  ,0  ,"" ,0  ); break; } 
				     else {           PRN(2,""  ,PFP,"" ,PCP,"" ,PSP,0  ,LFP,"" ,LCP,"" ,LSP,0  ,RFP,"" ,RCP,"" ,0  ,0  ,RSP,",",RSW); break; }
				  }   
		}

		if ( m_learn == true ) { while ( (temp=(aRule*)m_rule[idx]->search(&key))==NULL ) { m_rule[idx]->insert(&key); } }
		else { temp=(aRule*)m_rule[idx]->search(&key); }

		rule[idx]=temp;
	}
}
/**/





//=================
/* 규칙 추출을 위한 부분
void cDynamicRule::findRule( aRule *rule[MAX_RUL_CNT], aPhrase parent, aPhrase left, aPhrase right, aPhrase outer_left, aPhrase outer_right, bool given )
{
	aRule	*temp, key; key.freq_given=0; key.freq_rule=0; 

	for ( int idx=0; idx<MAX_RUL_CNT; idx++)
	{
		switch (idx)
		{
		case LF0: { if(given==true) { PRN(0,OLFP,PFP,PFW,PCP,PCW,PSP,PSW,LFP,LFW,LCP,LCW,LSP,LSW,RFP,RFW,RCP,RCW,RSP,RSW,"" ,"" ,"" ); break; } 
			        else {            PRN(0,OLFP,PFP,PFW,PCP,PCW,PSP,PSW,LFP,LFW,LCP,LCW,LSP,LSW,RFP,RFW,RCP,RCW,RSP,RSW,"1",",","1"); break; }
				  }
		case LC0: { if(given==true) { PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,",","" ); break; }
				  }   
		case LS0: { if(given==true) { PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; }
				  }   
		case RS0: { if(given==true) { PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; }
				  }   
		case LF1: { if(given==true) { PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
			         else {           PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; }
				  }
		case LC1: { if(given==true) { PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; }
				  }   
		case LS1: { if(given==true) { PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; }
				  }   
		case RS1: { if(given==true) { PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; } 
				     else {           PRN(0,""  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ,"" ,0  ,0  ,"" ,"" ,"" ); break; }
				  }   
		}

		if ( m_learn == true ) { while ( (temp=(aRule*)m_rule[idx]->search(&key))==NULL ) { m_rule[idx]->insert(&key); } }
		else { temp=(aRule*)m_rule[idx]->search(&key); }

		rule[idx]=temp;
	}
}
/**/
