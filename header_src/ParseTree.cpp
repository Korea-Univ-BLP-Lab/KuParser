#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>

#include "ParseTree.h"


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

bool cParseTree::tree( Words *words, Phrases *phrases )
{	
	m_phrases=phrases;

	// 초기화 
	int idx;
	for ( idx=0; (idx<MAX_WRD_CNT)&&(words->one[idx].pos[0]!='\0'); idx++ )
	{
		if ( m_sRule->getParent(words->one[idx].pos,idx,m_phrases)==NULL ) { return false; }
	}

	// 순서대로 구문분석
	for ( idx=1; (idx<MAX_WRD_CNT)&&(phrases->one[idx]!=NULL)&&(phrases->one[idx]->cont.syn[0]!='\0'); idx++ )
	{
		if ( parseNode(phrases->one[idx-1],phrases->one[idx]) == false ) 
               { return false; }
	}											 

	// 파스트리의 root
	m_phrases->root = m_phrases->one[phrase_idx(0,idx-1)];

	// 구문분석 실패하면 root=NULL
	if ( (m_phrases->root!=NULL)&&(m_phrases->root->cont.pos[0]=='\0') ) { m_phrases->root=NULL; }

	if ( m_phrases->root==NULL )
	{ int k=0; }
	return true;
}


////////////////////////////////////////////////////////////////////
//
//  cParseData 클래스의 parseNode 함수 
//
//  인자 : 왼쪽노드, 오른쪽노드, 구문노드배열
//
//  반환 : 구문분석 중간결과 성공/실패
//
//  역할 : 왼쪽노드와 오른쪽노드에 대해 구문분석
//   

bool cParseTree::parseNode( aPhrase* left, aPhrase* right )
{
	for ( aPhrase *parent, temp; left!=NULL; left=findNext(left) ) 
	{	
		double prob_old=0, prob_new=0; 

		// 확률(기존 부모)
		parent = m_phrases->one[phrase_idx(left->end_left,right->end_right)];
		if ( (parent!=NULL) && (parent->cont.syn[0]!='\0') ) { prob_old = parent->prob; }

		// 외부문맥 설정 
		aPhrase *outer_left, *outer_right, empty; memset(&empty,'\0',sizeof(aPhrase)); 
		if ( (left->end_left==0)||((outer_left=m_phrases->one[left->end_left-1])==NULL) ) { outer_left =&empty; }
		if ( (right->end_right==MAX_WRD_CNT)||((outer_right=m_phrases->one[right->end_right+1])==NULL)) { outer_right=&empty; }

		// 확률(새부모)
		parent = m_sRule->setParent( &temp, left, right );
		if ( m_dRule == NULL ) { prob_new = 1.0; }
		else { prob_new = m_dRule->getProb(*parent,*left,*right,*outer_left,*outer_right)*left->prob*right->prob; }

		// 확률비교
		if ( (prob_old>prob_new) || (prob_new<=THRESHOLD) ) { continue; }

		// 새부모 설정 및 구문분석
		parent = m_sRule->getParent( left, right, m_phrases ); 
		parent->prob = prob_new;
		if ( left->end_left == 0 ) { continue; }
		if ( parseNode(m_phrases->one[left->end_left-1],parent) == false ) { return false; }
	}

	return true;
}


////////////////////////////////////////////////////////////////////
//
//  cParseData 클래스의 findNext 함수 
//
//  인자 : 노드, 구문노드배열
//
//  반환 : 노드와 오른쪽범위는 동일하고 왼쪽범위는 하나더 긴 다음 노드
//
//  역할 : 결합가능한 다음노드 탐색 
//   

aPhrase* cParseTree::findNext( aPhrase *phrase )
{
	for ( int left=phrase->end_left-1; left>=0; left-- )
	{
		int idx = phrase_idx(left,phrase->end_right);
		if ( (m_phrases->one[idx]!=NULL)&&(m_phrases->one[idx]->cont.syn[0]!='\0') ) { return m_phrases->one[idx]; }
	}

	return NULL;
}

