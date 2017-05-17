#include <stdio.h> 
#include <string.h>
#include <malloc.h>

#include "ReadTree.h"


////////////////////////////////////////////////////////////////////
//
//  cReadData Ŭ������ tree �Լ� 
//
//  ���� : �Է�����������, �������� Root, �������迭, �������¼ҿ�, ���� 
//
//  ��ȯ : �������� �б� ����/���� 
//
//  ���� : �������� ����ġ���� tree �б� 
//

cReadTree::cReadTree( cStaticRule *rule )
{
	// ���� �ʱ�ȭ 
	m_buf_idx = 0; memset(m_buf,'\0',sizeof(char)*MAX_BUF_LEN);
	m_rule = rule; m_words = NULL; m_phrases = NULL; 
}


////////////////////////////////////////////////////////////////////
//
//  cReadData Ŭ������ tree �Լ� 
//
//  ���� : �Է�����������, �������� Root, �������迭, �������¼ҿ�, ���� 
//
//  ��ȯ : �������� �б� ����/���� 
//
//  ���� : �������� ����ġ���� tree �б� 
//
// �ܾ� ����: ���¼� �м� m_words = words;
// ���� ����: ���� �м�   m_phrases = phrases;			

bool cReadTree::tree( FILE *file, Words *words, Phrases *phrases )
{
	// �ʱ�ȭ
	m_buf_idx=0;		memset( m_buf,  '\0', sizeof(char)*MAX_BUF_LEN );
	m_words  = words;	memset( m_words,'\0', sizeof(Words) );
	m_phrases= phrases;	for ( int idx=0; idx<MAX_PHS_CNT; idx++ ) { if ( m_phrases->one[idx]!=NULL ) { memset( m_phrases->one[idx], '\0', sizeof(aPhrase) ); } }

	// ���Ͽ��� �ѹ��徿 �б�
	while( fgets( m_buf, MAX_BUF_LEN, file ) != NULL )
	{
		// ����
		if ( m_buf[0] == ';' ) 
		{	 
			int idx=0; Words spacingUnits; memset(&spacingUnits,'\0',sizeof(Words));
			for( char *token=strtok(&m_buf[1]," \n"); token!=NULL; token=strtok(NULL," \n"), idx++ ) 
			{ 
				strcpy(spacingUnits.one[idx].raw,token); 
			}

			m_rule->getSynUnits( m_words, &spacingUnits );
		}
		// ���� ���
		else if ( m_buf[0] == '\n' ) { 
			return true; }
		// ��� �б� 
		else
		{
			// ��� �ϳ��� �б� 
			short eojeol_idx=0, word_idx=0; 
			if ( readPhrase(file,&(m_phrases->root),&eojeol_idx) == false ) { printf("\nERROR(tree in cReadTree): �������� �б� ����\n"); return false; }
		}

		memset( m_buf, '\0', sizeof(char)*MAX_BUF_LEN );
	}
	
	return false;
}


////////////////////////////////////////////////////////////////////
//
//  cReadData Ŭ������ Onephrase �Լ� 
//
//  ���� : �Է¸���ġ, ���, �������迭, ���¼ҹ�ȣ 
//
//  ��ȯ : ��带 ����� ������ true, �ƴϸ� false
//
//  ���� : �Է¸���ġ���� ��� �ϳ��� �б�
//

bool cReadTree::readPhrase( FILE *file, aPhrase **parent, short *word_idx )
{
	// ���۰� ������� ���� �о� ��ġ �̵�
	if ( (m_buf[m_buf_idx]=='\0') || (m_buf[m_buf_idx]=='\n') )
	{
		memset(m_buf,'\0',sizeof(char)*MAX_BUF_LEN);
		if ( fgets(m_buf,MAX_BUF_LEN,file)==NULL ) { printf("\nERROR(cReadTree::readPhrase): �����б����\n"); return false; }
		for ( m_buf_idx=0; (m_buf[m_buf_idx]==' ')||(m_buf[m_buf_idx]=='\t'); m_buf_idx++ ); 
	}

	// �������� �ӽ�����
	for ( ; (m_buf[m_buf_idx]==' ')||(m_buf[m_buf_idx]=='\t'); m_buf_idx++ ); m_buf_idx++;	// ++:'('�б�
	char org_cont_syn[MAX_STR_LEN]; memset(org_cont_syn,'\0',MAX_STR_LEN*sizeof(char));
	char org_func_syn[MAX_STR_LEN]; memset(org_func_syn,'\0',MAX_STR_LEN*sizeof(char));
	int i;
    for ( i=0;(m_buf[m_buf_idx+i]!='_')&&(m_buf[m_buf_idx+i]!=' ')&&(m_buf[m_buf_idx+i]!='\t'); i++ ) { org_cont_syn[i]=m_buf[m_buf_idx+i]; }	
	if ( m_buf[m_buf_idx+i]=='_') { for ( int j=0;(m_buf[m_buf_idx+i]!=' ')&&(m_buf[m_buf_idx+i]!='\t'); i++, j++ ) { org_func_syn[j]=m_buf[m_buf_idx+i]; } } 

	// ��ġ ���� 
	m_buf_idx = m_buf_idx + strlen(org_cont_syn) + strlen(org_func_syn) + 1;  // 1:' '�б�

	// �ܸ����
	if ( (m_buf[m_buf_idx]!='\t')&&(m_buf[m_buf_idx]!=' ')&&((m_buf[m_buf_idx]!='(')||((m_buf[m_buf_idx]=='(')&&(m_buf[m_buf_idx+1]=='/'))) )
	{
		// ǰ���±����� �ܾ������� �Է� 
		for ( int i=0; (m_buf[m_buf_idx]!=')')||!strncmp(&m_buf[m_buf_idx],")/SS",strlen(")/SS")); m_buf_idx++, i++ ) 
		{ m_words->one[*word_idx].pos[i]=m_buf[m_buf_idx]; } m_buf_idx++; 

		// �ܸ���� ����: �������ִ� ���� �������ַ� �缳��
		if ( (*parent=m_rule->getParent(m_words->one[*word_idx].pos,*word_idx,m_phrases))==NULL ) { return false; }
		else { (*word_idx)++; return true; }
	}
	// ��ܸ����
	else
	{
		// �ڽĳ�� ����
		aPhrase  *left, *right;
		readPhrase( file, &left,  word_idx );
		readPhrase( file, &right, word_idx );
	
		// ��ܸ���� ����: �������ִ� ���� �������ַ� �缳��
		if ( (*parent=m_rule->getParent(left,right,m_phrases)) == NULL ) { return false; }

		// pop�ϱ� 
		if ( m_buf[m_buf_idx]==')' ) { m_buf_idx++;	return true; }
		else { printf("\nERROR(readPhrase in cReadTree): �������� ��ȣ ����ġ\n"); return false; }
	}
	
	return true;
}


