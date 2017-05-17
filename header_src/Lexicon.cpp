#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

#include "Lexicon.h"
#include "StaticRule.h"
#include<stdexcept>
#include<string>
using std::runtime_error;

////////////////////////////////////////////////////////////////////
//
//  cLexicon Ŭ������ cLexicon �Լ� 
//
//  ���� : �н�����(true/false), �����̸�
//
//  ���� : ���� �� ���� �ʱ�ȭ 
//

cLexicon::cLexicon( bool learn, char *filename, int size )
{
	// ���� �ʱ�ȭ
	m_learn=learn; m_filename=filename; m_size=size;
	m_lexicon = new cBTree( sizeof(Lexeme), cmp, pnt ); 

	// �н��� �ƴϸ� ���� �б�
	if( m_learn == false ) { readLexicon( m_filename, m_lexicon, m_size ); }
}


////////////////////////////////////////////////////////////////////
//
//  cLexicon Ŭ������ ~cLexicon �Լ� 
//
//  ���� : ������ ����
//

cLexicon::~cLexicon()
{
	delete m_lexicon;
}


////////////////////////////////////////////////////////////////////
//
//  cLexicon Ŭ������ ReadLexicon �Լ� 
//
//  ���� : �����̸�, �����̸� 
//
//  ���� : ���Ͽ��� ���� �б� 
//

void cLexicon::readLexicon( char *filename, cBTree *lexicon, int size )
{
	// ����(filename)���� ����(lexicon)�� �б� 
	FILEOPEN( pFile, filename, "r", "" );
     if(pFile==NULL) throw runtime_error("[ERR] cLexicon::readLexicon: ���ֻ��� ���Ͽ��� ����");
	char buf[MAX_BUF_LEN] = "";
	for ( int i=0; (i<size)&&(fgets(buf,MAX_BUF_LEN,pFile)!=NULL); i++)
	{
		// �ܾ� ����
		Lexeme key; memset(&key,'\0',sizeof(Lexeme));				
		key.freq = atoi( strtok(buf, " \t\n\0") );
		strcpy( key.word,strtok(NULL," \t\n\0") ); 
		
		// �ܾ� �߰� 
		Lexeme *temp=(Lexeme*)lexicon->search(&key);
		if ( temp==NULL ) { lexicon->insert(&key); }			
	}

	lexicon->balance();
	fclose( pFile );
}


////////////////////////////////////////////////////////////////////
//
//  cLexicon Ŭ������ WriteLexicon �Լ� 
//
//  ���� : �����̸�, �����̸� 
//
//  ���� : ���Ͽ� ���� ���� 
//

void cLexicon::writeLexicon( char *filename, cBTree *lexicon )
{
	// ����(filename)�� ����(lexicon)�� ����
	FILEOPEN( pFile, filename, "w", "cLexicon::~cLexicon()" );
	lexicon->pntlist( pFile );
	fclose( pFile );
}


////////////////////////////////////////////////////////////////////
//
//  cLexicon Ŭ������ EndToLearn �Լ� 
//
//  ���� : ������ ����ϰ� �н� ���� 
//

void cLexicon::endToLearn()
{
	if ( m_learn == true ) 
	{ 
		// ���� ��� 
		writeLexicon( m_filename, m_lexicon ); 
		// �н� ���� 
		m_learn = false; 
		// ��� �ܾ ������ ���� ������ ����� 
		delete m_lexicon; m_lexicon = new cBTree( sizeof(Lexeme), cmp, pnt ); 
		// �־��� �� �̻��� ��� �ܾ ���� �о �ʱ�ȭ 
		readLexicon( m_filename, m_lexicon, m_size );
	}
}


////////////////////////////////////////////////////////////////////
//
//  cLexicon Ŭ������ getSize �Լ� 
//
//  ���� : ���� ũ�� ��ȯ
//

int	cLexicon::getSize()
{
	return m_lexicon->getSize();
}
	
		
////////////////////////////////////////////////////////////////////
//
//  cLexicon Ŭ������ cmp �Լ� 
//
//  ���� : �ĺ����, BTree�� ���  
//
//  ��ȯ : �ܾ�񱳰���� 
//   
//  ���� : BTree���� �ܾ �߰�(insert)�ϰų� Ž��(search)�� �� ����ϴ� �Լ� 
// 

int cLexicon::cmp( const void *a, const void *b )
{
	Lexeme *aa=(Lexeme*)a, *bb=(Lexeme*)b;
	return strcmp(((Lexeme*)a)->word,((Lexeme*)b)->word);
}


////////////////////////////////////////////////////////////////////
//
//  cLexicon Ŭ������ cmp �Լ� 
//
//  ���� : ���� ��� ����������, ����� ����  
//
//  ���� : BTree���� �־��� �� �̻��� �ܾ ���(pntlist)�� �� ����ϴ� �Լ� 
// 

void cLexicon::pnt( FILE *pFile, void *one )
{
	fprintf( pFile, "%7d\t%s\n", ((Lexeme*)one)->freq, ((Lexeme*)one)->word ); 
}


////////////////////////////////////////////////////////////////////
//
//  cLexicon Ŭ������ check �Լ� 
//
//  ���� : �ܾ� 
//
//  ��ȯ : �ܾ��� ���� ���� ����(true/false)
//   
//  ���� : �ܾ ������ �ִ��� ������ Ȯ���ϰ�, ������ �н��ϴ� ��� �ܾ��� �󵵸� ����  
//

bool cLexicon::check( char *word, char *pos, bool add )
{
	// �ܾ� �˻� �ʱ�ȭ
	Lexeme  key; sprintf( key.word, "%s/%s", word, pos ); key.freq=1;
	Lexeme  *temp = (Lexeme*)m_lexicon->search(&key);
	
	// �ܾ� �˻� ��� 
	     if ( (add==true )&&(temp!=NULL) ) { temp->freq++; return false; }
	else if ( (add==true )&&(temp==NULL) ) { m_lexicon->insert(&key); return false; }
	else if ( (add==false)&&(temp!=NULL) ) { return true; }
	else if ( (add==false)&&(temp==NULL) ) { return false; }
	else { return false; }// �̷���찡 ������?
}


////////////////////////////////////////////////////////////////////
//
//  cLexicon Ŭ������ AddToLexicon �Լ� 
//
//  ���� : ���¼� �м� ���
//
//  ���� : ���¼� �м� ������� ���¼Ҹ� �����Ͽ� ������ ��� Ȯ��
//

void cLexicon::addToLexicon( Words *words )
{
	if ( m_learn == true ) 
	{
		for ( int idx=0; (idx<MAX_WRD_CNT)&&(words->one[idx].pos[0]!='\0'); idx++ )
		{
			// ���¼� �м� ���(words->one[idx].pos)���� ���¼�(word,pos)�� �����Ͽ� ������ ��� Ȯ��
			char *word=NULL, *pos=NULL;
			while ( cStaticRule::getWordPOS(words->one[idx].pos,&word,&pos)==true ) { check( word, pos, true ); }
		}
	}
}


