#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>

#include "ParseTree.h"


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

bool cParseTree::tree( Words *words, Phrases *phrases )
{	
	m_phrases=phrases;

	// �ʱ�ȭ 
	int idx;
	for ( idx=0; (idx<MAX_WRD_CNT)&&(words->one[idx].pos[0]!='\0'); idx++ )
	{
		if ( m_sRule->getParent(words->one[idx].pos,idx,m_phrases)==NULL ) { return false; }
	}

	// ������� �����м�
	for ( idx=1; (idx<MAX_WRD_CNT)&&(phrases->one[idx]!=NULL)&&(phrases->one[idx]->cont.syn[0]!='\0'); idx++ )
	{
		if ( parseNode(phrases->one[idx-1],phrases->one[idx]) == false ) 
               { return false; }
	}											 

	// �Ľ�Ʈ���� root
	m_phrases->root = m_phrases->one[phrase_idx(0,idx-1)];

	// �����м� �����ϸ� root=NULL
	if ( (m_phrases->root!=NULL)&&(m_phrases->root->cont.pos[0]=='\0') ) { m_phrases->root=NULL; }

	if ( m_phrases->root==NULL )
	{ int k=0; }
	return true;
}


////////////////////////////////////////////////////////////////////
//
//  cParseData Ŭ������ parseNode �Լ� 
//
//  ���� : ���ʳ��, �����ʳ��, �������迭
//
//  ��ȯ : �����м� �߰���� ����/����
//
//  ���� : ���ʳ��� �����ʳ�忡 ���� �����м�
//   

bool cParseTree::parseNode( aPhrase* left, aPhrase* right )
{
	for ( aPhrase *parent, temp; left!=NULL; left=findNext(left) ) 
	{	
		double prob_old=0, prob_new=0; 

		// Ȯ��(���� �θ�)
		parent = m_phrases->one[phrase_idx(left->end_left,right->end_right)];
		if ( (parent!=NULL) && (parent->cont.syn[0]!='\0') ) { prob_old = parent->prob; }

		// �ܺι��� ���� 
		aPhrase *outer_left, *outer_right, empty; memset(&empty,'\0',sizeof(aPhrase)); 
		if ( (left->end_left==0)||((outer_left=m_phrases->one[left->end_left-1])==NULL) ) { outer_left =&empty; }
		if ( (right->end_right==MAX_WRD_CNT)||((outer_right=m_phrases->one[right->end_right+1])==NULL)) { outer_right=&empty; }

		// Ȯ��(���θ�)
		parent = m_sRule->setParent( &temp, left, right );
		if ( m_dRule == NULL ) { prob_new = 1.0; }
		else { prob_new = m_dRule->getProb(*parent,*left,*right,*outer_left,*outer_right)*left->prob*right->prob; }

		// Ȯ����
		if ( (prob_old>prob_new) || (prob_new<=THRESHOLD) ) { continue; }

		// ���θ� ���� �� �����м�
		parent = m_sRule->getParent( left, right, m_phrases ); 
		parent->prob = prob_new;
		if ( left->end_left == 0 ) { continue; }
		if ( parseNode(m_phrases->one[left->end_left-1],parent) == false ) { return false; }
	}

	return true;
}


////////////////////////////////////////////////////////////////////
//
//  cParseData Ŭ������ findNext �Լ� 
//
//  ���� : ���, �������迭
//
//  ��ȯ : ���� �����ʹ����� �����ϰ� ���ʹ����� �ϳ��� �� ���� ���
//
//  ���� : ���հ����� ������� Ž�� 
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

