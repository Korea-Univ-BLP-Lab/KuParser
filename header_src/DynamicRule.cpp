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
//  cDynamicRule Ŭ������ cDynamicRule �Լ� 
//
//  ���� : �н�����, ��Ģ�����̸� 
//
//  ���� : ���� �� ��Ģ �ʱ�ȭ 
//

cDynamicRule::cDynamicRule( bool learn, char *filename )
{
     /*
	// ���� �ʱ�ȭ
	m_learn=learn; m_filename=filename; memset(m_lmd,0,MAX_LMD_CNT*sizeof(double));
	for ( int idx=0; idx<MAX_RUL_CNT; idx++ ) { m_rule[idx] = new cBTree_v2(sizeof(aRule),cmp,pnt); }

	// �н��� �ƴϸ� ��Ģ �б�
	if( m_learn == false ) { readRule( m_filename, m_rule ); }
     */
     
     // ���� �ʱ�ȭ
	m_learn=learn; m_filename=filename; memset(m_lmd,0,MAX_LMD_CNT*sizeof(double));
	for ( int idx=0; idx<MAX_RUL_CNT; idx++ ) { m_rule[idx] = new cBTree_v2(sizeof(aRule),cmp,pnt); }

	// �н��� �ƴϸ� ��Ģ �б�
	if( m_learn == false ) { readRule( m_filename, m_rule ); }
     
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule Ŭ������ ~cDynamicRule �Լ� 
//
//  ���� : ��Ģ ���� 
//

cDynamicRule::~cDynamicRule()
{
	for ( int idx=0; idx<MAX_RUL_CNT; idx++ ) { delete m_rule[idx]; }
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule Ŭ������ readRule �Լ� 
//
//  ���� : �����̸�, ��Ģ�̸� 
//
//  ���� : ���Ͽ��� ��Ģ �б�
//

void cDynamicRule::readRule( char *filename, cBTree_v2 *rule[MAX_RUL_CNT] )
{
	// �������� �ʱ�ȭ
	for ( int i=0; i<MAX_LMD_CNT; i++ ) { m_lmd[i]=(double)1.0; }

	// ����(filename)���� ��Ģ(rule)�� �б�
	int idx=-1;	FILEOPEN(pFile,filename,"r","getRule in cRule");
     if(pFile==NULL) 
          throw runtime_error(string("[ERR] cDynamicRule: Rule File open Error"));
	for ( char buf[MAX_BUF_LEN] = ""; fgets(buf,MAX_BUF_LEN,pFile)!=NULL; )
	{
		if ( !strcmp(buf,"��Ģ�� ���Ǻ�\t��Ģ\n" ) ) { continue; }
		else if ( !strncmp(buf,"//rule",strlen("//rule")) ) { idx++; }
		// �������� �б� 
		else if ( !strncmp(buf,"//variable",strlen("//variable")) ) 
		{ 
			strtok(buf,"\t\n"); 
			for ( int i=0; i<MAX_LMD_CNT; i++ ) { m_lmd[i] = atof(strtok(NULL,"\t\n")); }
		}
		// ��Ģ �б� 
		else 
		{
			// ��Ģ ����
			aRule key; memset(&key,'\0',sizeof(aRule));
			key.freq_rule = atoi( strtok(buf, "\t\n") );
			key.freq_given= atoi( strtok(NULL,"\t\n") );
			strcpy( key.rule, strtok(NULL,"\n") ); 

			// ��Ģ �߰� 
               // aRule *temp; if ( (temp=(aRule*)m_rule[idx]->search(&key))==NULL ) { m_rule[idx]->insert(&key); }  // ��������
               m_rule[idx]->insert(&key); // �ٲ� ����
		}
	}

     //for ( idx=0; idx<MAX_RUL_CNT; idx++ ) { m_rule[idx]->balance(); } // ��������
     for ( idx=0; idx<MAX_RUL_CNT; idx++ ) { m_rule[idx]->sort(); } // �ٲ� ���� 

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
//  cDynamicRule Ŭ������ writeRule �Լ� 
//
//  ���� : �����̸�, ��Ģ�̸� 
//
//  ���� : ���Ͽ� ��Ģ ����
//

void cDynamicRule::writeRule( char *filename, cBTree_v2 *rule[MAX_RUL_CNT] )
{
	// �ʱ��Ģ�� ����(filename)�� ���Ǻκ󵵼�(freq_given)�� �߰��Ͽ� ����(OutFileName)�� �����
	if ( m_learn == true )
	{
		// ��Ģ �н���� ���(��Ģ�� \t 0 \t��Ģ\n)
		FILE *pFile=fopen(filename,"w"); if (pFile==NULL) { printf("\nERROR(getRule in cRule): ���Ͽ������(%s)\n",filename); }
		fprintf(pFile,"��Ģ�� ���Ǻ�\t��Ģ\n" );
		for ( int idx=0; idx<MAX_RUL_CNT; idx++ ) { fprintf(pFile,"//rule[%d]\n",idx); m_rule[idx]->pntlist(pFile); }
		fclose(pFile);

		// ��Ģ �н���� �����(��Ģ�� \t ���Ǻ� \t���Ǻ�|��Ģ\n)
		char  OutFileName[MAX_STR_LEN]; strcpy(OutFileName,filename); strcat(OutFileName,".bak");
		char  given[MAX_STR_LEN], freq_given[MAX_STR_LEN], buf[MAX_BUF_LEN], *temp;
		FILE * InFile, *OutFile; 
        for ( InFile=fopen(filename,"r"),OutFile=fopen(OutFileName,"w"); fgets(buf,MAX_BUF_LEN,InFile)!= NULL; )
		{
			if ( !strcmp(buf,"��Ģ�� ���Ǻ�\t��Ģ\n" ) ) { fprintf(OutFile,"%s",buf ); continue; }
			else if ( !strncmp(buf,"//rule",strlen("//rule")) ) { fprintf(OutFile,"%s",buf ); continue; }

			// ���Ǻ��� ���(��Ģ�� \t ���Ǻ� \t���Ǻ�|\n)
			if ( buf[strlen(buf)-2]=='|' )
			{
				// ���Ǻο� ���Ǻ�(freq_given) ����
				strcpy(freq_given,strtok(buf," \t")); strtok(NULL," \t");
				strcpy(given,strtok(NULL," \t\n")); 
			}
			// ��Ģ�� ���(��Ģ�� \t ���Ǻ� \t���Ǻ�|\n)
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
	// �н��� ��Ģ �� �������� ���
	else
	{
		// �н��� ��Ģ ���
		FILE *pFile=fopen(filename,"w"); if (pFile==NULL) { printf("\nERROR(getRule in cRule): ���Ͽ������(%s)\n",filename); }
		fprintf(pFile,"��Ģ�� ���Ǻ�\t��Ģ\n" );
		for ( int idx=0; idx<MAX_RUL_CNT; idx++ ) { fprintf(pFile,"//rule[%d]\n",idx); m_rule[idx]->pntlist(pFile); }
		fclose(pFile);
	}
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule Ŭ������ endToLearn �Լ� 
//
//  ���� : ��Ģ�� ����ϰ� �н� ���� 
//

void cDynamicRule::endToLearnRule()
{
	// ��Ģ ���
	writeRule( m_filename, m_rule );
	// �������� ���� �ܰ谡 �ƴ϶� ��Ģ �н� �ܰ��̸� 
	if ( m_learn == true ) 
	{ 
		// �н� ���� 
		m_learn = false; 
		// ���Ǻ� �������� ���� ���� ��Ģ�� ����� 
		for ( int idx=0; idx<MAX_RUL_CNT; idx++ ) { delete m_rule[idx]; m_rule[idx] = new cBTree_v2(sizeof(aRule),cmp,pnt); } 
		// ���Ǻ� �������� �����ϴ� ��Ģ�� ���� �о �ʱ�ȭ 
		readRule( m_filename, m_rule );
	}
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule Ŭ������ endToLearn �Լ� 
//
//  ���� : ��Ģ�� ����ϰ� �н� ���� 
//

void cDynamicRule::endToLearnLambda()
{
	// �������� ��� 
	FILE *pFile=fopen(m_filename,"a"); if (pFile==NULL) { printf("\nERROR(getRule in cRule): ���Ͽ������(%s)\n",m_filename); return; }
	fprintf(pFile,"//variable\t" );
	for ( int i=0; i<MAX_LMD_CNT; i++ ) { fprintf(pFile,"\t%4.2f", (float)m_lmd[i] ); }
	fprintf(pFile,"\n" );
	fclose(pFile);
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule Ŭ������ getSize �Լ� 
//
//  ���� : ��Ģ ũ�� ��ȯ
//

int	cDynamicRule::getSize()
{
    int size,idx;
	for ( size=0, idx=0; idx<MAX_RUL_CNT; idx++ ) { size = size + m_rule[idx]->getSize(); }
	return size;
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule Ŭ������ cmp �Լ� 
//
//  ���� : �ĺ����, BTree�� ���  
//
//  ��ȯ : ��Ģ�񱳰���� 
//   
//  ���� : BTree���� ��Ģ�� �߰�(insert)�ϰų� Ž��(search)�� �� ����ϴ� �Լ� 
// 

int cDynamicRule::cmp( const void *a, const void *b )
{
	aRule *aa=(aRule*)a, *bb=(aRule*)b;
	return strcmp(((aRule*)a)->rule,((aRule*)b)->rule);
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule Ŭ������ cmp �Լ� 
//
//  ���� : ��Ģ ��� ����������, ����� ��Ģ  
//
//  ���� : BTree���� ��Ģ�� ���(pntlist)�� �� ����ϴ� �Լ� 
// 

void cDynamicRule::pnt( FILE *pFile, void *one )
{
	aRule *aa=(aRule*)one;
	fprintf( pFile, "%7d\t%8d\t%s\n", ((aRule*)one)->freq_rule, ((aRule*)one)->freq_given, ((aRule*)one)->rule );
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule Ŭ������ tree �Լ� 
//
//  ���� : ������, ����Ʈ��
//
//  ���� : �����带 �����ϴ� ��Ģ�� �н�
//

void cDynamicRule::tree( aPhrase *phrase, Phrases phrases )
{
	if ( m_learn == false ) { return; }
	else if ( (phrase==NULL)||(phrase->left==NULL)||(phrase->right==NULL) ) { return; }
	
	tree( phrase->left, phrases ); tree( phrase->right, phrases ); // �����ڽ� �н� 

	// ��Ģ, �ܺι���
	aRule *rule[MAX_RUL_CNT]; aPhrase *outer_left, *outer_right, temp; memset(&temp,'\0',sizeof(aPhrase)); 
	if ( (phrase->end_left==0)||((outer_left=phrases.one[phrase->end_left-1])==NULL) ) { outer_left =&temp; }
	if ( (phrase->end_right==MAX_WRD_CNT)||((outer_right=phrases.one[phrase->end_right+1])==NULL) ) { outer_right=&temp; }

	// ��Ģ �� ���(��Ģ,�θ�,����,������,���Ǻ�/��Ģ) 
	findRule( rule, *phrase, *(phrase->left), *(phrase->right), *outer_left, *outer_right, false );
	int idx;
    for ( idx=0; idx<MAX_RUL_CNT; idx++ ) { rule[idx]->freq_rule++; }

	// ���Ǻ� �� ���(��Ģ,�θ�,����,������,���Ǻ�/��Ģ) 
	findRule( rule, *phrase, *(phrase->left), *(phrase->right), *outer_left, *outer_right, true );
	//int idx;
    for ( idx=0; idx<MAX_RUL_CNT; idx++ ) { rule[idx]->freq_rule++; }
}


////////////////////////////////////////////////////////////////////
//
//  cDynamicRule Ŭ������ getProb �Լ� 
//
//  ���� : �θ�, �����ڽ�, �������ڽ�, ���ʿܺι���, �����ʿܺι���
//
//  ��ȯ : �θ����� ����Ȯ��
//   
//  ���� : �־��� ��������� �������� �θ����� ����Ȯ���� ��� 
//

/*// �̰��� �� & Collins �� 
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

/*/// �ڼҿ� ��
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
//  cDynamicRule Ŭ������ findRule �Լ� 
//
//  ���� : ��Ģ, �θ�, �����ڽ�, �������ڽ�, ���ʿܺι���, �����ʿܺι���, ���Ǻ�/��Ģ
//
//  ���� : �־��� ��������� �������� �θ����� ������Ģ�� Ž�� 
//

// ���� ���� �ִ� ����:4, �߰� ���� �ִ� ����:7 

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

// "datatype.h"		����: #define  MAX_LMD_CNT 24  
// "DynamicRule.cpp"����: DynamicRule::getProb()�� Ȯ�� ��� ��

// Collins ��
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
//�̰��� ��
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

// "datatype.h"		����: #define  MAX_LMD_CNT 24  
// "DynamicRule.cpp"����: DynamicRule::getProb()�� Ȯ�� ��� ��
// "DynamicRule.h"  ����: #define  MAX_RUL_CNT 8
//                        #define		MAX_RUL_CNT		2				// �ִ밳��: ��Ģ�� ����
//                        #define		LF0				0				// ����  ���
//                        #define		LC0				1				// ����  ����
//                        #define		LS0				2				// ����  ũ�� 
//                        #define		RS0				3				// ������ũ��
//                        #define		LF1				1				// ����  ���
//                        #define		LC1				5				// ����  ����
//                        #define		LS1				6				// ����  ũ��
//                        #define		RS1				7				// ������ũ��
// "main.cpp"       ����: if ((i==2)&&(i==3)){ dRule->m_lmd[i]=(double)1.0; continue; }

/**/
//�ڼҿ� ��

// ����/���/ũ��
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

//����+���
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
// ���, ����, ũ��
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
/* ��Ģ ������ ���� �κ�
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
