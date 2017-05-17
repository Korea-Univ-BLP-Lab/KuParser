#include <stdio.h> 
#include <string.h>
#include <malloc.h>

#include "ReadTree.h"


////////////////////////////////////////////////////////////////////
//
//  cReadData 클래스의 tree 함수 
//
//  인자 : 입력파일포인터, 구문구조 Root, 구문노드배열, 원시형태소열, 문장 
//
//  반환 : 구문구조 읽기 성공/실패 
//
//  역할 : 구문구조 말뭉치에서 tree 읽기 
//

cReadTree::cReadTree( cStaticRule *rule )
{
	// 변수 초기화 
	m_buf_idx = 0; memset(m_buf,'\0',sizeof(char)*MAX_BUF_LEN);
	m_rule = rule; m_words = NULL; m_phrases = NULL; 
}


////////////////////////////////////////////////////////////////////
//
//  cReadData 클래스의 tree 함수 
//
//  인자 : 입력파일포인터, 구문구조 Root, 구문노드배열, 원시형태소열, 문장 
//
//  반환 : 구문구조 읽기 성공/실패 
//
//  역할 : 구문구조 말뭉치에서 tree 읽기 
//
// 단어 정보: 형태소 분석 m_words = words;
// 구조 정보: 구문 분석   m_phrases = phrases;			

bool cReadTree::tree( FILE *file, Words *words, Phrases *phrases )
{
	// 초기화
	m_buf_idx=0;		memset( m_buf,  '\0', sizeof(char)*MAX_BUF_LEN );
	m_words  = words;	memset( m_words,'\0', sizeof(Words) );
	m_phrases= phrases;	for ( int idx=0; idx<MAX_PHS_CNT; idx++ ) { if ( m_phrases->one[idx]!=NULL ) { memset( m_phrases->one[idx], '\0', sizeof(aPhrase) ); } }

	// 파일에서 한문장씩 읽기
	while( fgets( m_buf, MAX_BUF_LEN, file ) != NULL )
	{
		// 원문
		if ( m_buf[0] == ';' ) 
		{	 
			int idx=0; Words spacingUnits; memset(&spacingUnits,'\0',sizeof(Words));
			for( char *token=strtok(&m_buf[1]," \n"); token!=NULL; token=strtok(NULL," \n"), idx++ ) 
			{ 
				strcpy(spacingUnits.one[idx].raw,token); 
			}

			m_rule->getSynUnits( m_words, &spacingUnits );
		}
		// 문장 경계
		else if ( m_buf[0] == '\n' ) { 
			return true; }
		// 노드 읽기 
		else
		{
			// 노드 하나씩 읽기 
			short eojeol_idx=0, word_idx=0; 
			if ( readPhrase(file,&(m_phrases->root),&eojeol_idx) == false ) { printf("\nERROR(tree in cReadTree): 구문구조 읽기 오류\n"); return false; }
		}

		memset( m_buf, '\0', sizeof(char)*MAX_BUF_LEN );
	}
	
	return false;
}


////////////////////////////////////////////////////////////////////
//
//  cReadData 클래스의 Onephrase 함수 
//
//  인자 : 입력말뭉치, 노드, 구문노드배열, 형태소번호 
//
//  반환 : 노드를 제대로 읽으면 true, 아니면 false
//
//  역할 : 입력말뭉치에서 노드 하나씩 읽기
//

bool cReadTree::readPhrase( FILE *file, aPhrase **parent, short *word_idx )
{
	// 버퍼가 비었으면 새로 읽어 위치 이동
	if ( (m_buf[m_buf_idx]=='\0') || (m_buf[m_buf_idx]=='\n') )
	{
		memset(m_buf,'\0',sizeof(char)*MAX_BUF_LEN);
		if ( fgets(m_buf,MAX_BUF_LEN,file)==NULL ) { printf("\nERROR(cReadTree::readPhrase): 파일읽기오류\n"); return false; }
		for ( m_buf_idx=0; (m_buf[m_buf_idx]==' ')||(m_buf[m_buf_idx]=='\t'); m_buf_idx++ ); 
	}

	// 구문범주 임시저장
	for ( ; (m_buf[m_buf_idx]==' ')||(m_buf[m_buf_idx]=='\t'); m_buf_idx++ ); m_buf_idx++;	// ++:'('읽기
	char org_cont_syn[MAX_STR_LEN]; memset(org_cont_syn,'\0',MAX_STR_LEN*sizeof(char));
	char org_func_syn[MAX_STR_LEN]; memset(org_func_syn,'\0',MAX_STR_LEN*sizeof(char));
	int i;
    for ( i=0;(m_buf[m_buf_idx+i]!='_')&&(m_buf[m_buf_idx+i]!=' ')&&(m_buf[m_buf_idx+i]!='\t'); i++ ) { org_cont_syn[i]=m_buf[m_buf_idx+i]; }	
	if ( m_buf[m_buf_idx+i]=='_') { for ( int j=0;(m_buf[m_buf_idx+i]!=' ')&&(m_buf[m_buf_idx+i]!='\t'); i++, j++ ) { org_func_syn[j]=m_buf[m_buf_idx+i]; } } 

	// 위치 조정 
	m_buf_idx = m_buf_idx + strlen(org_cont_syn) + strlen(org_func_syn) + 1;  // 1:' '읽기

	// 단말노드
	if ( (m_buf[m_buf_idx]!='\t')&&(m_buf[m_buf_idx]!=' ')&&((m_buf[m_buf_idx]!='(')||((m_buf[m_buf_idx]=='(')&&(m_buf[m_buf_idx+1]=='/'))) )
	{
		// 품사태깅결과를 단어정보로 입력 
		for ( int i=0; (m_buf[m_buf_idx]!=')')||!strncmp(&m_buf[m_buf_idx],")/SS",strlen(")/SS")); m_buf_idx++, i++ ) 
		{ m_words->one[*word_idx].pos[i]=m_buf[m_buf_idx]; } m_buf_idx++; 

		// 단말노드 설정: 구문범주는 정답 구문범주로 재설정
		if ( (*parent=m_rule->getParent(m_words->one[*word_idx].pos,*word_idx,m_phrases))==NULL ) { return false; }
		else { (*word_idx)++; return true; }
	}
	// 비단말노드
	else
	{
		// 자식노드 설정
		aPhrase  *left, *right;
		readPhrase( file, &left,  word_idx );
		readPhrase( file, &right, word_idx );
	
		// 비단말노드 설정: 구문범주는 정답 구문범주로 재설정
		if ( (*parent=m_rule->getParent(left,right,m_phrases)) == NULL ) { return false; }

		// pop하기 
		if ( m_buf[m_buf_idx]==')' ) { m_buf_idx++;	return true; }
		else { printf("\nERROR(readPhrase in cReadTree): 구문구조 괄호 불일치\n"); return false; }
	}
	
	return true;
}


