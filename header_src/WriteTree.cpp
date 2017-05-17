#include <stdio.h> 
#include <string.h>

#include "DataType.h"
#include "WriteTree.h"


////////////////////////////////////////////////////////////////////
//
//  cWriteData 클래스의 writeTree 함수 
//
//  인자 : 출력파일포인터, 구문노드배열, 문장 
//
//  역할 : 구문노드배열을 바탕으로 구문트리를 파일에 출력 
//

void cWriteTree::tree( FILE *file, Words *words, Phrases *phrases )
{	
	// 입력이 '\n'이면 return
	if ( phrases->root == NULL ) { return; }

	// 문장과 구문분석결과 출력
	fprintf( file, ";" );
	for ( int i=0; words->one[i].raw[0]!='\0'; i++ ) 
	{
		if ( (words->one[i].pos[0]==' ') || (words->one[i].pos[0]=='+') ) { fprintf( file, "%s", words->one[i].raw ); }
		else { fprintf( file, " %s", words->one[i].raw ); }
	}

	fprintf( file, "\n" );
	writePhrase( file, words, phrases->root, 0, false );
	fprintf( file, "\n\n" );
}


////////////////////////////////////////////////////////////////////
//
//  cWriteData 클래스의 writeNode 함수 
//
//  인자 : 출력파일포인터, 출력할 노드, 깊이, 빈칸출력여부
//   
//  역할 : 깊이에 맞게 빈칸을 출력한후 노드를 출력
//

void cWriteTree::writePhrase( FILE *file, Words *words, aPhrase *phrase, short depth, bool blank )
{
	// 빈칸조정
	if ( blank == true ) { for ( short i=0; i < depth; i++) { fprintf( file, "         " ); } }

	// 노드의 구문태그 출력
	if ( phrase->func.syn[0]!='_' ) { phrase->func.syn[0]='\0'; }
	if ( phrase->end_left == phrase->end_right ) { fprintf( file, "(%s%s ", phrase->cont.syn, phrase->func.syn ); }
	else if ( !strcmp(phrase->cont.syn,"VNP") ) { fprintf( file, "(%s%-4s ", phrase->cont.syn, phrase->func.syn ); }
	else { fprintf( file, "(%s%-5s ", phrase->cont.syn, phrase->func.syn ); }

	// 단말노드
	if ( phrase->end_left == phrase->end_right )
	{
		// 복합구 점검
		fprintf( file, "%s", words->one[phrase->end_left].pos );
		// 빈칸조정
		depth = depth - 2;
	}
	// 비단말노드
	else
	{
		if ( !strcmp(phrase->cont.syn,"S") ) { fprintf( file, " " ); }
		// 왼쪽 자식 노드 출력
		writePhrase( file, words, phrase->left, depth+1, false );	
		fprintf( file, "\n" );
		// 오른쪽 자식 노드 출력	
		writePhrase( file, words, phrase->right, depth+1, true );	
	}

	fprintf( file, ")" );
	return;
}


////////////////////////////////////////////////////////////////////
//
//  cWriteData 클래스의 relation 함수 
//
//  인자 : 출력파일포인터, 구문노드배열
//
//  역할 : 구문노드배열을 바탕으로 구문트리를 파일에 출력 
//

#define		HEADWRD		0
#define		EOJEOL		1
#define		HEADEJL		2

void cWriteTree::dependency( FILE *file, Words *words, Phrases *phrases )
{	
	int	head[3][MAX_WRD_CNT]; memset(head,0,3*MAX_WRD_CNT*sizeof(int));
	findHead( head[HEADWRD], phrases->root->left,  phrases->root, phrases );
	findHead( head[HEADWRD], phrases->root->right, phrases->root, phrases );

	// 형태소 분석결과를 어절단위로 인식
	int wrd_idx, ejl_idx;
	for ( wrd_idx=0, ejl_idx=0; words->one[wrd_idx].raw[0]!='\0'; wrd_idx++, ejl_idx++ ) 
	{
	    int i;
		for ( i=1; (words->one[wrd_idx+i].pos[0]==' ')||(words->one[wrd_idx+i].pos[0]=='+'); i++ ) 
		{
			strcat(words->one[wrd_idx].raw,words->one[wrd_idx+i].raw);
			strcat(words->one[wrd_idx].pos," ");
			strcat(words->one[wrd_idx].pos,words->one[wrd_idx+i].pos);
			head[EOJEOL][wrd_idx+i]=ejl_idx;
			strcpy(phrases->one[wrd_idx]->func.syn,phrases->one[wrd_idx+i]->func.syn);
		}

		head[EOJEOL][wrd_idx]=ejl_idx;
		wrd_idx=wrd_idx+i-1;
	}

	// 어절단위의 중심어로 조정
	for ( wrd_idx=wrd_idx-1, ejl_idx=0; wrd_idx>=0; wrd_idx-- )
	{
		if ( head[HEADWRD][wrd_idx] == -1 ) { ejl_idx = -1; }
		else { ejl_idx = head[EOJEOL][head[HEADWRD][wrd_idx]]; }
		int i;
        for ( i=0; (head[EOJEOL][wrd_idx]==head[EOJEOL][wrd_idx-i])&&(wrd_idx>=i); i++ )
		{ 
			head[HEADEJL][wrd_idx-i] = ejl_idx; 
			if ( ejl_idx == -1 ) { strcpy(phrases->one[wrd_idx-i]->func.syn,"_END"); }
		}

		wrd_idx=wrd_idx-i+1;
	}

	// 어절단위의 중심어로 출력 
	for ( wrd_idx=0; words->one[wrd_idx].raw[0]!='\0'; wrd_idx++ )
	{
		fprintf( file, "%3d  %3s %3d\t%s\t%s\n", head[EOJEOL][wrd_idx]+1, &(phrases->one[wrd_idx]->func.syn[1]), head[HEADEJL][wrd_idx]+1, words->one[wrd_idx].raw, words->one[wrd_idx].pos ); 
		for ( ; head[EOJEOL][wrd_idx]==head[EOJEOL][wrd_idx+1]; wrd_idx++ );
	}

	fprintf( file, "\n" ); 
}


////////////////////////////////////////////////////////////////////
//
//  cWriteData 클래스의 relation 함수 
//
//  인자 : 출력파일포인터, 구문노드배열
//
//  역할 : 구문노드배열을 바탕으로 구문트리를 파일에 출력 
//

void cWriteTree::findHead( int head[MAX_WRD_CNT], aPhrase *phrase, aPhrase *parent, Phrases *phrases )
{	
	if ( phrase==NULL ) { return; }
	findHead( head, phrase->left,  phrase, phrases );
	findHead( head, phrase->right, phrase, phrases );

	if ( phrase->end_right == phrases->root->end_right ) { head[phrase->end_right]=-1; }
	else { head[phrase->end_right] = parent->end_right; }
}


////////////////////////////////////////////////////////////////////
//
//  cWriteData 클래스의 relation 함수 
//
//  인자 : 출력파일포인터, 구문노드배열
//
//  역할 : 구문노드배열을 바탕으로 구문트리를 파일에 출력 
//

void cWriteTree::POStaggedSnt( FILE *file, Words *words )
{	
	// 형태소 분석 결과 출력
	int i;
	for ( i=0; words->one[i].raw[0]!='\0'; i++ ) 
	{
	    int j;
		for ( j=1; (words->one[i+j].pos[0]==' ')||(words->one[i+j].pos[0]=='+'); j++ ) 
		{
			strcat(words->one[i].raw,words->one[i+j].raw);
			strcat(words->one[i].pos," ");
			strcat(words->one[i].pos,words->one[i+j].pos);
		}

		fprintf( file, "%s\t%s\n", words->one[i].raw, words->one[i].pos ); 
		i=i+j-1;
	}
}


