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
//  cLexicon 클래스의 cLexicon 함수 
//
//  인자 : 학습여부(true/false), 파일이름
//
//  역할 : 변수 및 사전 초기화 
//

cLexicon::cLexicon( bool learn, char *filename, int size )
{
	// 변수 초기화
	m_learn=learn; m_filename=filename; m_size=size;
	m_lexicon = new cBTree( sizeof(Lexeme), cmp, pnt ); 

	// 학습이 아니면 사전 읽기
	if( m_learn == false ) { readLexicon( m_filename, m_lexicon, m_size ); }
}


////////////////////////////////////////////////////////////////////
//
//  cLexicon 클래스의 ~cLexicon 함수 
//
//  역할 : 사전을 제거
//

cLexicon::~cLexicon()
{
	delete m_lexicon;
}


////////////////////////////////////////////////////////////////////
//
//  cLexicon 클래스의 ReadLexicon 함수 
//
//  인자 : 파일이름, 사전이름 
//
//  역할 : 파일에서 사전 읽기 
//

void cLexicon::readLexicon( char *filename, cBTree *lexicon, int size )
{
	// 파일(filename)에서 사전(lexicon)을 읽기 
	FILEOPEN( pFile, filename, "r", "" );
     if(pFile==NULL) throw runtime_error("[ERR] cLexicon::readLexicon: 어휘사전 파일열기 오류");
	char buf[MAX_BUF_LEN] = "";
	for ( int i=0; (i<size)&&(fgets(buf,MAX_BUF_LEN,pFile)!=NULL); i++)
	{
		// 단어 추출
		Lexeme key; memset(&key,'\0',sizeof(Lexeme));				
		key.freq = atoi( strtok(buf, " \t\n\0") );
		strcpy( key.word,strtok(NULL," \t\n\0") ); 
		
		// 단어 추가 
		Lexeme *temp=(Lexeme*)lexicon->search(&key);
		if ( temp==NULL ) { lexicon->insert(&key); }			
	}

	lexicon->balance();
	fclose( pFile );
}


////////////////////////////////////////////////////////////////////
//
//  cLexicon 클래스의 WriteLexicon 함수 
//
//  인자 : 파일이름, 사전이름 
//
//  역할 : 파일에 사전 쓰기 
//

void cLexicon::writeLexicon( char *filename, cBTree *lexicon )
{
	// 파일(filename)에 사전(lexicon)을 쓰기
	FILEOPEN( pFile, filename, "w", "cLexicon::~cLexicon()" );
	lexicon->pntlist( pFile );
	fclose( pFile );
}


////////////////////////////////////////////////////////////////////
//
//  cLexicon 클래스의 EndToLearn 함수 
//
//  역할 : 사전을 출력하고 학습 해제 
//

void cLexicon::endToLearn()
{
	if ( m_learn == true ) 
	{ 
		// 사전 출력 
		writeLexicon( m_filename, m_lexicon ); 
		// 학습 해제 
		m_learn = false; 
		// 모든 단어를 저장한 기존 사전은 지우고 
		delete m_lexicon; m_lexicon = new cBTree( sizeof(Lexeme), cmp, pnt ); 
		// 주어진 빈도 이상의 고빈도 단어만 새로 읽어서 초기화 
		readLexicon( m_filename, m_lexicon, m_size );
	}
}


////////////////////////////////////////////////////////////////////
//
//  cLexicon 클래스의 getSize 함수 
//
//  역할 : 사전 크기 반환
//

int	cLexicon::getSize()
{
	return m_lexicon->getSize();
}
	
		
////////////////////////////////////////////////////////////////////
//
//  cLexicon 클래스의 cmp 함수 
//
//  인자 : 후보노드, BTree의 노드  
//
//  반환 : 단어비교결과값 
//   
//  역할 : BTree에서 단어를 추가(insert)하거나 탐색(search)할 때 사용하는 함수 
// 

int cLexicon::cmp( const void *a, const void *b )
{
	Lexeme *aa=(Lexeme*)a, *bb=(Lexeme*)b;
	return strcmp(((Lexeme*)a)->word,((Lexeme*)b)->word);
}


////////////////////////////////////////////////////////////////////
//
//  cLexicon 클래스의 cmp 함수 
//
//  인자 : 사전 출력 파일포인터, 출력할 사전  
//
//  역할 : BTree에서 주어진 빈도 이상의 단어를 출력(pntlist)할 때 사용하는 함수 
// 

void cLexicon::pnt( FILE *pFile, void *one )
{
	fprintf( pFile, "%7d\t%s\n", ((Lexeme*)one)->freq, ((Lexeme*)one)->word ); 
}


////////////////////////////////////////////////////////////////////
//
//  cLexicon 클래스의 check 함수 
//
//  인자 : 단어 
//
//  반환 : 단어의 사전 존재 여부(true/false)
//   
//  역할 : 단어가 사전에 있는지 없는지 확인하고, 사전을 학습하는 경우 단어의 빈도를 증가  
//

bool cLexicon::check( char *word, char *pos, bool add )
{
	// 단어 검색 초기화
	Lexeme  key; sprintf( key.word, "%s/%s", word, pos ); key.freq=1;
	Lexeme  *temp = (Lexeme*)m_lexicon->search(&key);
	
	// 단어 검색 결과 
	     if ( (add==true )&&(temp!=NULL) ) { temp->freq++; return false; }
	else if ( (add==true )&&(temp==NULL) ) { m_lexicon->insert(&key); return false; }
	else if ( (add==false)&&(temp!=NULL) ) { return true; }
	else if ( (add==false)&&(temp==NULL) ) { return false; }
	else { return false; }// 이런경우가 있을까?
}


////////////////////////////////////////////////////////////////////
//
//  cLexicon 클래스의 AddToLexicon 함수 
//
//  인자 : 형태소 분석 결과
//
//  역할 : 형태소 분석 결과에서 형태소를 추출하여 사전에 등록 확인
//

void cLexicon::addToLexicon( Words *words )
{
	if ( m_learn == true ) 
	{
		for ( int idx=0; (idx<MAX_WRD_CNT)&&(words->one[idx].pos[0]!='\0'); idx++ )
		{
			// 형태소 분석 결과(words->one[idx].pos)에서 형태소(word,pos)를 추출하여 사전에 등록 확인
			char *word=NULL, *pos=NULL;
			while ( cStaticRule::getWordPOS(words->one[idx].pos,&word,&pos)==true ) { check( word, pos, true ); }
		}
	}
}


