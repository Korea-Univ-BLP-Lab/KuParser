#include <stdio.h> 
#include <string.h>

#include "DataType.h"
#include "WriteTree.h"


////////////////////////////////////////////////////////////////////
//
//  cWriteData Ŭ������ writeTree �Լ� 
//
//  ���� : �������������, �������迭, ���� 
//
//  ���� : �������迭�� �������� ����Ʈ���� ���Ͽ� ��� 
//

void cWriteTree::tree( FILE *file, Words *words, Phrases *phrases )
{	
	// �Է��� '\n'�̸� return
	if ( phrases->root == NULL ) { return; }

	// ����� �����м���� ���
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
//  cWriteData Ŭ������ writeNode �Լ� 
//
//  ���� : �������������, ����� ���, ����, ��ĭ��¿���
//   
//  ���� : ���̿� �°� ��ĭ�� ������� ��带 ���
//

void cWriteTree::writePhrase( FILE *file, Words *words, aPhrase *phrase, short depth, bool blank )
{
	// ��ĭ����
	if ( blank == true ) { for ( short i=0; i < depth; i++) { fprintf( file, "         " ); } }

	// ����� �����±� ���
	if ( phrase->func.syn[0]!='_' ) { phrase->func.syn[0]='\0'; }
	if ( phrase->end_left == phrase->end_right ) { fprintf( file, "(%s%s ", phrase->cont.syn, phrase->func.syn ); }
	else if ( !strcmp(phrase->cont.syn,"VNP") ) { fprintf( file, "(%s%-4s ", phrase->cont.syn, phrase->func.syn ); }
	else { fprintf( file, "(%s%-5s ", phrase->cont.syn, phrase->func.syn ); }

	// �ܸ����
	if ( phrase->end_left == phrase->end_right )
	{
		// ���ձ� ����
		fprintf( file, "%s", words->one[phrase->end_left].pos );
		// ��ĭ����
		depth = depth - 2;
	}
	// ��ܸ����
	else
	{
		if ( !strcmp(phrase->cont.syn,"S") ) { fprintf( file, " " ); }
		// ���� �ڽ� ��� ���
		writePhrase( file, words, phrase->left, depth+1, false );	
		fprintf( file, "\n" );
		// ������ �ڽ� ��� ���	
		writePhrase( file, words, phrase->right, depth+1, true );	
	}

	fprintf( file, ")" );
	return;
}


////////////////////////////////////////////////////////////////////
//
//  cWriteData Ŭ������ relation �Լ� 
//
//  ���� : �������������, �������迭
//
//  ���� : �������迭�� �������� ����Ʈ���� ���Ͽ� ��� 
//

#define		HEADWRD		0
#define		EOJEOL		1
#define		HEADEJL		2

void cWriteTree::dependency( FILE *file, Words *words, Phrases *phrases )
{	
	int	head[3][MAX_WRD_CNT]; memset(head,0,3*MAX_WRD_CNT*sizeof(int));
	findHead( head[HEADWRD], phrases->root->left,  phrases->root, phrases );
	findHead( head[HEADWRD], phrases->root->right, phrases->root, phrases );

	// ���¼� �м������ ���������� �ν�
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

	// ���������� �߽ɾ�� ����
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

	// ���������� �߽ɾ�� ��� 
	for ( wrd_idx=0; words->one[wrd_idx].raw[0]!='\0'; wrd_idx++ )
	{
		fprintf( file, "%3d  %3s %3d\t%s\t%s\n", head[EOJEOL][wrd_idx]+1, &(phrases->one[wrd_idx]->func.syn[1]), head[HEADEJL][wrd_idx]+1, words->one[wrd_idx].raw, words->one[wrd_idx].pos ); 
		for ( ; head[EOJEOL][wrd_idx]==head[EOJEOL][wrd_idx+1]; wrd_idx++ );
	}

	fprintf( file, "\n" ); 
}


////////////////////////////////////////////////////////////////////
//
//  cWriteData Ŭ������ relation �Լ� 
//
//  ���� : �������������, �������迭
//
//  ���� : �������迭�� �������� ����Ʈ���� ���Ͽ� ��� 
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
//  cWriteData Ŭ������ relation �Լ� 
//
//  ���� : �������������, �������迭
//
//  ���� : �������迭�� �������� ����Ʈ���� ���Ͽ� ��� 
//

void cWriteTree::POStaggedSnt( FILE *file, Words *words )
{	
	// ���¼� �м� ��� ���
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


