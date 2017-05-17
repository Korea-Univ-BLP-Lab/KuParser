#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

#include "StaticRule.h"


////////////////////////////////////////////////////////////////////
//
//  ccConstraints Ŭ������ modifyNPcont �Լ� 
//
//  ���� : ���ձ� CONT
//
//  ���� : C4.5�� ���밡���ϵ��� �ܸ������ ��������
// 

aPhrase *cStaticRule::getParent( char *pos_tagged_word, short word_idx, Phrases *phrases )
{
	// ������ۿ��� ����ǥ �ʱ�ȭ
	if ( word_idx==0 ) { m_dquotation=0; m_squotation=0; m_brace=0; }

	// ǰ�� ���� ������ �ʱ� �������� ����
	aPhrase *parent; if ( (parent=getPhrase(word_idx,word_idx,phrases))==NULL ) { return NULL; }
	aPhrase temp[MAX_UPS_CNT]; memset(&temp,'\0',MAX_UPS_CNT*sizeof(aPhrase));

	char *pos=NULL, *raw=NULL;
	for ( int idx=1; getWordPOS(pos_tagged_word,&raw,&pos)==true; idx++ )
	{
          int i ;
		for (i=0; strcmp(m_pos2node[i].pos,pos)&&(m_pos2node[i].pos[0]!='\0'); i++ );  // ����/���(����)
		if( m_pos2node[i].pos[0]=='\0' ) 
        { 
            fprintf(stderr,"\nERROR(cStaticRule::initPhrase): �����ʱ�ȭ����(%s/%s)\n",raw,pos); return NULL; 
        }	
		else { strcpy(temp[idx].cont.syn,m_pos2node[i].cont_syn); strcpy(temp[idx].func.syn,m_pos2node[i].func_syn); } 

		strcpy(temp[idx].cont.raw,raw); strcpy(temp[idx].cont.pos,pos);						// ����(����/ǰ��)
		strcpy(temp[idx].func.raw,raw); strcpy(temp[idx].func.pos,pos);						// ���(����/ǰ��)

		aPhrase *a=&temp[idx]; checkSynTag(&a);												// cont.syn�� func.syn�� ���� 
		if ( (m_lexicon!=NULL)&&(m_lexicon->check(raw,pos,false)==false) ) { strcpy(temp[idx].func.raw,"UNK"); }
		if ( (m_lexicon!=NULL)&&(m_lexicon->check(raw,pos,false)==false) ) { strcpy(temp[idx].cont.raw,"UNK"); }
																							// ������ ���� �ܾ�� �̵�Ͼ�� ����
		temp[idx].end_left = word_idx; temp[idx].end_right = word_idx;						// ��ġ
		if ( *raw=='+' ) { temp[idx].size = 0; } else { temp[idx].size = 1; }				// ũ��(��������:1/������:0)
		
		// �Ļ� �� ���� ó��
		if ( (idx==1) || (temp[idx].cont.syn[0]=='L') || (temp[idx-1].cont.syn[0]=='L') ) { continue; }
		else 
		{	
			for ( ; (idx>1); idx-- )
			{	
				if ( (temp[idx-1].cont.syn[0]=='L')&&strcmp(temp[idx].func.syn,"_PRN") ) { break; }
				else if ( !strcmp(temp[idx-1].func.syn,"_PRN")&&strcmp(temp[idx].func.syn,"_PRN")) { strcpy(temp[idx-1].func.syn,""); }
				setParent(&temp[0],&temp[idx-1],&temp[idx]); memset(&temp[idx],'\0',sizeof(aPhrase));
				memcpy(&temp[idx-1],&temp[0],sizeof(aPhrase)); memset(&temp[0],'\0',sizeof(aPhrase));
			}
		}
	}
	
	memcpy(parent,&temp[1],sizeof(aPhrase));
	parent->size = 1;																	// ����
	parent->prob = 1.0;																	// Ȯ��
	parent->left = NULL; parent->right = NULL;											// �ڽ�
	
	// ����_��� ǥ�� ���� ����: (S- + ./SF)�̿��� ��� �±�(-)�� ����
	if ( strcmp(parent->cont.syn,"S-")&&((parent->cont.syn[1]=='-')||(parent->cont.syn[0]=='P')) ) { strcpy(parent->cont.syn,"X"); }
	// ��ȣ�� ����±״� _PRN
	else if ( !strcmp(parent->func.syn,"_PRN")&&(strstr(pos_tagged_word,"(")==NULL)&&(strstr(pos_tagged_word,")")==NULL) ) { strcpy(parent->func.syn,""); }

	// ���+��ǥ�� ����±״� _CNJ
	if ( !strcmp(parent->cont.syn,"NP")&&(parent->func.syn[0]=='\0')&&(strstr(pos_tagged_word,",")!=NULL) ) { strcpy(parent->func.syn,"_CNJ"); }
	// ����_��� ǥ�� ���� ����: 'ù����+!'�� ȣ�ݱ��
	else if ( !strcmp(parent->cont.syn,"NP")&&(parent->func.syn[0]=='\0')&&(strstr(pos_tagged_word,"!")!=NULL) ) { strcpy(parent->func.syn,"_INT"); }

	return parent;
}


////////////////////////////////////////////////////////////////////
//
//  cReadData Ŭ������ getNewSynData �Լ� 
//
//	��ȯ : ����������� �޸�
//   
//  ���� : ����带 ���� �޸� �Ҵ� 
//   

bool cStaticRule::getWordPOS( char *pos_tagged_word, char **word, char **pos )
{
	static char buf[MAX_STR_LEN];
	
	if ( (*word==NULL)&&(*pos==NULL) ) 
	{ 
		memset(buf,'\0',MAX_STR_LEN*sizeof(char)); 
		strcpy(buf,pos_tagged_word); 
		*word=strtok(buf,"/"); *pos=strtok(NULL," +");
	}
	else 
	{	
		*word=strtok(NULL,"/"); *pos=strtok(NULL," +");
	}

	if ( *word==NULL ) { return false; }
	else { checkWord( word, pos ); return true; }			// �Է� �ܾ��� ����
}


////////////////////////////////////////////////////////////////////
//
//  cReadData Ŭ������ getNewSynData �Լ� 
//
//	��ȯ : ����������� �޸�
//   
//  ���� : ����带 ���� �޸� �Ҵ� 
//   

aPhrase *cStaticRule::getPhrase( short left, short right, Phrases *phrases )
{
	short	idx = phrase_idx(left,right); 
	aPhrase *temp;

	if ( (idx>=MAX_PHS_CNT)||(idx<0)||(left<0)||(right<0) ) 
	{ 
		fprintf(stderr,"\nERROR(cBinaryRule::getPhrase): �޸� �Ҵ� ����(�����ʰ�)" ); 
		return NULL; 
	}
	else if ( phrases->one[idx]!=NULL ) 
	{
//		memset(phrases->one[idx],'\0',sizeof(aPhrase));
		return phrases->one[idx];
	}
	else if ( (temp=(aPhrase*)malloc(sizeof(aPhrase))) == NULL )
	{
		fprintf(stderr,"\nERROR(cBinaryRule::getPhrase): �޸� �Ҵ� ����(malloc)" ); 
		return NULL;
	}
	else
	{
		memset(temp,'\0',sizeof(aPhrase));
		phrases->one[idx]=temp;
		return phrases->one[idx];
	}
}


////////////////////////////////////////////////////////////////////
//
//  ccConstraints Ŭ������ modifyNPcont �Լ� 
//
//  ���� : ���ձ� CONT
//
//  ���� : C4.5�� ���밡���ϵ��� �ܸ������ ��������
// 

void cStaticRule::checkSynTag( aPhrase **phrase )
{
	// ����_��� ǥ�� ���� ����: ����ǥ ó��
	if ( (*phrase)->cont.syn[0]=='L' ) 
	{
		(*phrase)->cont.syn[0]=checkQuotation((*phrase)->cont.raw); 
		strcat((*phrase)->cont.pos,(*phrase)->cont.syn); 
		strcat((*phrase)->func.pos,(*phrase)->cont.syn); 
	}
}


////////////////////////////////////////////////////////////////////
//
//  ccConstraints Ŭ������ modifyNPcont �Լ� 
//
//  ���� : ���ձ� CONT
//
//  ���� : C4.5�� ���밡���ϵ��� �ܸ������ ��������
// 

#define	RAW				0
#define	POS				1

// q:qutation, v:value of q, l:left, r:right
#define COMPARE(loc,l,r)  ((!strncmp(loc,l,strlen(l))||!strncmp(loc,r,strlen(r)))) 
#define CHECKQUT(q,v,loc,l,r) ((q==v)&&COMPARE(&spacingUnit.loc,l,r))

void cStaticRule::segment( int idx[2][MAX_UPS_CNT], aWord spacingUnit )
{
	// �ʱ�ȭ
	int i=0, index_num=0; memset(idx,0,2*MAX_UPS_CNT*sizeof(int));
	
	// ����ǥ���� ���̻��� ������ ������ ��ġ���� ����
	bool prn, brk, brc, dqut, squt;
    for ( prn=false, brk=false, brc=false, dqut=false, squt=false; (i<=(int)strlen(spacingUnit.raw)); i++ )
	{
		if ( CHECKQUT(prn, true, raw[i],"(", ")" )||CHECKQUT(prn,false,raw[i],"(", ")" ) ) { prn=true; }
		else if ( CHECKQUT(brk, true,raw[i],"[", "]" )||CHECKQUT(brk, false,raw[i],"[", "]" ) ) { brk =true;  }
		else if ( CHECKQUT(brc, true,raw[i],"{", "}" )||CHECKQUT(brc, false,raw[i],"{", "}" ) ) { brc =true;  }
		else if ( CHECKQUT(dqut,true,raw[i],"\"","\"")||CHECKQUT(dqut,false,raw[i],"\"","\"") ) { dqut=true; }
		else if ( CHECKQUT(squt,true,raw[i],"`", "`" )||CHECKQUT(squt,false,raw[i],"`", "`" ) ) { squt=true; }
		else if ( CHECKQUT(squt,true,raw[i],"'", "'" )||CHECKQUT(squt,false,raw[i],"'", "'" ) ) { squt=true; }
/*		if ( CHECKQUT(prn, true, raw[i],"(", ")" )||CHECKQUT(prn,false,raw[i],"(", ")" ) ) { prn=!prn; }
		else if ( CHECKQUT(brk, true,raw[i],"[", "]" )||CHECKQUT(brk, false,raw[i],"[", "]" ) ) { brk =!brk;  }
		else if ( CHECKQUT(brc, true,raw[i],"{", "}" )||CHECKQUT(brc, false,raw[i],"{", "}" ) ) { brc =!brc;  }
		else if ( CHECKQUT(dqut,true,raw[i],"\"","\"")||CHECKQUT(dqut,false,raw[i],"\"","\"") ) { dqut=!dqut; }
		else if ( CHECKQUT(squt,true,raw[i],"`", "`" )||CHECKQUT(squt,false,raw[i],"`", "`" ) ) { squt=!squt; }
		else if ( CHECKQUT(squt,true,raw[i],"'", "'" )||CHECKQUT(squt,false,raw[i],"'", "'" ) ) { squt=!squt; }
*/	}

	// �ܾ�(raw)�� ������ġ: ����������ǥ(") 
	if ( spacingUnit.raw[0]=='\0' ) { return; }
	for ( i=0, index_num=0; (i<=(int)strlen(spacingUnit.raw)); i++ )
	{
		if ( CHECKQUT(prn, true,raw[i],"(",")" ) || CHECKQUT(brk, true,raw[i],"[", "]" ) ||
			 CHECKQUT(brc, true,raw[i],"{","}" ) || CHECKQUT(dqut,true,raw[i],"\"","\"") ||
			 CHECKQUT(squt,true,raw[i],"`","`" ) || CHECKQUT(squt,true,raw[i],"'", "'" ) )
		{
			if ( (i>0)&&(i>idx[RAW][index_num]) ) { idx[RAW][++index_num]=i; }
			idx[RAW][++index_num]=i+1;
		}
	}

	// �ܾ�(pos)�� ������ġ: ����������ǥ( + "/SS)
	if ( spacingUnit.pos[0]=='\0' ) { return; }
	for ( i=0, index_num=0; (i<=(int)strlen(spacingUnit.pos)); i++ )
	{
		if ( CHECKQUT(prn, true,pos[i]," + (/SS"," + )/SS" ) || CHECKQUT(brk, true,pos[i]," + [/SS", " + ]/SS" ) ||
			 CHECKQUT(brc, true,pos[i]," + {/SS"," + }/SS" ) || CHECKQUT(dqut,true,pos[i]," + \"/SS"," + \"/SS") ||
			 CHECKQUT(squt,true,pos[i]," + `/SS"," + `/SS" ) || CHECKQUT(squt,true,pos[i]," + '/SS", " + '/SS" ) )
		{
			if ( (i>0)&&(i>idx[POS][index_num]) ) { idx[POS][++index_num]=i; }
			idx[POS][++index_num]=i+strlen(" + (/SF");
		}
		else if ( CHECKQUT(prn,true,pos[i],"(/SS",")/SS" ) || CHECKQUT(brk, true,pos[i],"[/SS", "]/SS" ) ||
			 CHECKQUT( brc, true, pos[i], "{/SS", "}/SS" ) || CHECKQUT(dqut,true,pos[i],"\"/SS","\"/SS") ||
			 CHECKQUT( squt,true, pos[i], "`/SS", "`/SS" ) || CHECKQUT(squt,true,pos[i],"'/SS", "'/SS" ) )
		{
			if ( (i>0)&&(i>idx[POS][index_num]) ) { idx[POS][++index_num]=i; }
			idx[POS][++index_num]=i+strlen("(/SF");
		}
	}
}


////////////////////////////////////////////////////////////////////
//
//  ccConstraints Ŭ������ modifyNPcont �Լ� 
//
//  ���� : ���ձ� CONT
//
//  ���� : C4.5�� ���밡���ϵ��� �ܸ������ ��������
// 

void cStaticRule::getSynUnits( Words *words, Words *spacingUnits )
{
	memset( words, '\0', sizeof(Words) );
	int i, j, k; 
    int idx[2][MAX_UPS_CNT];
	for( i=0, j=0; spacingUnits->one[i].raw[0]!='\0'; i++, j++ )
	{ 
		// �⺻ ������ ����: ����, ��������ȣ, ��ħǥ
		segment( idx, spacingUnits->one[i] );
		for ( k=0; idx[RAW][k]<idx[RAW][k+1]; j++, k++ )
		{ 
			strncpy(words->one[j].raw,&(spacingUnits->one[i].raw[idx[RAW][k]]),idx[RAW][k+1]-idx[RAW][k]); 
			strncpy(words->one[j].pos,&(spacingUnits->one[i].pos[idx[POS][k]]),idx[POS][k+1]-idx[POS][k]); 
		}

		if ( spacingUnits->one[i].raw[idx[RAW][k]]=='\0') { j--; continue; }
		strcpy(words->one[j].raw,&(spacingUnits->one[i].raw[idx[RAW][k]])); 
		strcpy(words->one[j].pos,&(spacingUnits->one[i].pos[idx[POS][k]])); 
	}
}


////////////////////////////////////////////////////////////////////
//
//  ccConstraints Ŭ������ modifyNPcont �Լ� 
//
//  ���� : ���ձ� CONT
//
//  ���� : C4.5�� ���밡���ϵ��� �ܸ������ ��������
// 

void cStaticRule::checkWord( char **arg_raw, char **arg_pos )
{
	char	*raw=*arg_raw, *pos=*arg_pos;
	char	temp[MAX_WRD_LEN]; memset(temp,'\0',sizeof(char)*MAX_WRD_LEN);

	// ���� �� + ���� ����
	int i, j;
    for ( i=0, j=0; (raw[i]!='\0'); i++, j++ ) { if ( (raw[i]==' ') ){ j--; } else { raw[j]=raw[i]; } } 
	if ( (raw[j]!='\0') ) { raw[j]='\0'; }
	if ( (*raw=='+')&&((*pos=='N')||(*pos=='M')||(*pos=='I')||(*pos=='S')||!strcmp(pos,"VV")||!strcmp(pos,"VA")||!strcmp(pos,"VX"))) { strcpy(raw,raw+1); }
	
	// "//SP" = "/"(raw) + "SP"(pos)
	if ( !strcmp(raw,"SP") ) { *arg_pos=raw; strcpy(temp,"/"); *arg_raw=temp; return; }
	// "+//SP" = "+/"(raw) + "SP"(pos)
	else if ( !strcmp(pos,"/SP") ) { *arg_pos=pos+1; strcpy(temp,"+/"); *arg_raw=temp; return; }
	// ����
	else if ( !strcmp(pos,"SN") ) { strcpy(*arg_raw,"n"); *arg_pos=pos; return; }
	// �ܱ���
	else if ( !strcmp(pos,"SL") ) { strcpy(*arg_raw,"l"); *arg_pos=pos; return; }
	// ���� 
	else if ( !strcmp(pos,"SH") ) { strcpy(*arg_raw,"h"); *arg_pos=pos; return; }
	// �޷� 
	else if ( !strcmp(pos,"SW") ) { strcpy(*arg_raw,"w"); *arg_pos=pos; return; }
	// ��Ÿ
	else  { *arg_raw=raw; *arg_pos=pos; return; }
}


////////////////////////////////////////////////////////////////////
//
//  ccConstraints Ŭ������ modifyNPcont �Լ� 
//
//  ���� : ���ձ� CONT
//
//  ���� : C4.5�� ���밡���ϵ��� �ܸ������ ��������
// 

char cStaticRule::checkQuotation( char *raw )
{
	char symbol; 
    int i;
    for ( i=0; (raw[i]==' ')||(raw[i]=='+'); i++ ); 
	if ( (raw[i]==' ')||(raw[i]=='+') ) { symbol=raw[i+1]; } else { symbol=raw[i]; }

	// (�ձٰ�ȣ),[���ȣ],{�߰�ȣ},<�����ȣ>: �¿� ������ ��Ȯ
	if ( (symbol=='(')||(symbol=='[')||(symbol=='{')||(symbol=='<') ) { m_brace++; return 'L'; }
	else if ( (symbol==')')||(symbol==']')||(symbol=='}')||(symbol=='>') ) { m_brace--; return 'R'; }

	// "(ū����ǥ),`,'(��������ǥ): �¿� ������ ��ȣ 
	if ( (symbol=='"')&&(m_dquotation==0 ) ) { m_dquotation++; return 'L'; }
	else if ( (symbol=='"' )&&(m_dquotation >0 ) ) { m_dquotation--; return 'R'; }
	else if ( (symbol=='`' )&&(m_squotation==0 ) ) { m_squotation++; return 'L'; }
	else if ( (symbol=='`' )&&(m_squotation >0 ) ) { m_squotation--; return 'R'; }
	else if ( (symbol=='\'')&&(m_squotation==0 ) ) { m_squotation++; return 'L'; }
	else if ( (symbol=='\'')&&(m_squotation >0 ) ) { m_squotation--; return 'R'; }
	else { return 'X'; }	// ����
}

cnvrt cStaticRule::m_pos2node[MAX_POS_CNT] =	
{
	{ "NP",	"",		"NNG"	},		// �Ϲ� ��� 
	{ "NP",	"",		"NNP"	},		// ���� ���  
	{ "NP",	"",		"NNB"	},		// ���� ��� 
	{ "NP",	"",		"NP"	},		// ���� 
	{ "NP", "",		"NR"	},		// ���� 
	
	{ "VP", "",		"VV"	},		// ���� 
	{ "VP", "",		"VA"	},		// �����
	{ "V-", "",		"VX"	},		// ���� ���
	{ "VN-","",		"VCP"	},		// ���� ������ 
	{ "V-", "",		"VCN"	},		// ���� ������ 
	
	{ "DP", "",		"MM"	},		// ������ 
	{ "AP", "",		"MAG"	},		// �Ϲ� �λ� 
	{ "AP", "",		"MAJ"	},		// ���� �λ�[���� �Ǵ� ����] 
	
	{ "IP", "",		"IC"	},		// ��ź�� 	

	{ "P-",	"_SBJ",	"JKS"	},		// �ְ�   ����[�־�]
	{ "P-",	"_CMP",	"JKC"	},		// ����   ����[����]
	{ "D-",	"_MOD",	"JKG"	},		// ������ ����
	{ "P-",	"_OBJ",	"JKO"	},		// ������ ����[������]
	{ "P-",	"_AJT",	"JKB"	},		// �λ�� ����[�λ��]
	{ "I-",	"_INT",	"JKV"	},		// ȣ��   ����
	{ "P-",	"_CMP",	"JKQ"	},		// �ο�� ����
	{ "P-",	"",		"JX"	},		// ������     [������]
	{ "N-",	"_CNJ",	"JC"	},		// ���Ӱ� ����[���� �Ǵ� ����]
	
	{ "V-",	"",		"EP"	},		// ��� ���[����/��Ī/����/������]
	{ "V-",	"",		"EF"	},		// ����   ���[����]
	{ "V-",	"",		"EC"	},		// ����   ���[���� �Ǵ� ����]
	{ "N-",	"",		"ETN"	},		// ����� ���
	{ "D-",	"_MOD",	"ETM"	},		// ������ ���
	
	{ "NP",	"",		"XPN"	},		// ü��	  ���λ�[��-,��4��°]
	
	{ "N-",	"",		"XSN"	},		// ���   �Ļ� ���̻�[�ǹ��� ����/����]
	{ "V-",	"",		"XSV"	},		// ����   �Ļ� ���̻�
	{ "V-",	"",		"XSA"	},		// ����� �Ļ� ���̻�
	{ "A-",	"",		"XSB"	},		// �λ�   �Ļ� ���̻�

	{ "NP",	"",		"XR"	},		// ��� 

	{ "X",	"",		"SF"	},		// ��ħǥ, ����ǥ, ����ǥ 
	{ "X",	"",		"SP"	},		// ��ǥ, �����, �ݷ�, ����
	{ "L",	"_PRN",	"SS"	},		// ����ǥ, ��ȣǥ, ��ǥ
	{ "X",	"",		"SE"	},		// ����ǥ 
	{ "X",	"",		"SO"	},		// ����ǥ[����,����,����]
	
	{ "NP",	"",		"SN"	},		// ����
	{ "NP",	"",		"SL"	},		// �ܱ��� 
	{ "NP",	"",		"SH"	},		// ���� 
	{ "NP",	"",		"SW"	},		// ����, ȭ���ȣ

	{ "NP",	"",		"NF"	},		// ��� ���� ���� 
	{ "VP",	"",		"NV"	},		// ��� ���� ���� 
	{ "NP",	"",		"NA"	},		// �м� �Ҵ� ����  
	
	{ "--",	"",		""		},		// �ɺ� 
};


////////////////////////////////////////////////////////////////////
//
//  cReadData Ŭ������ setNewSynData �Լ� 
//
//  ���� : ���ʳ��, �����ʳ��, ������ ���
// 
//  ���� : ���ʳ��� �����ʳ�带 �������� ����� ���� 
//   

#define		EMPH(l,r)  !strcmp(r->cont.pos,"JX")&&((l->func.pos[0]=='J')||(l->func.pos[0]=='E')||(l->func.pos[0]=='M'))

#define		SYMB(l,r)  !strcmp(r->cont.pos,"SE")||!strcmp(r->cont.pos,"SO")
#define		PROD(l,r)  !strcmp(r->cont.pos,"SF")
#define		PAUS(l,r)  !strcmp(r->cont.pos,"SP") 

#define		MAJF(l,r)  !strcmp(r->cont.pos,"MAJ")

#define		PRNF(l,r)   strcmp(l->func.syn,"_PRN")&&!strcmp(r->func.syn,"_PRN")
#define		RQUT(l,r)  !strcmp(r->cont.syn,"R") 

#define		DERV(l,r)  !strncmp(r->cont.pos,"XS",strlen("XS"))
#define		INFL(l,r)  (r->cont.pos[0]=='J')||(r->cont.pos[0]=='E')||!strncmp(r->cont.pos,"VC",strlen("VC"))

#define		SUBC(l,r) ((l->cont.syn[0]=='P')||(l->cont.syn[0]=='N'))&&(r->cont.syn[0]=='V') 
#define		CONJ(l,r)  (l->cont.syn[0]==r->cont.syn[0])&&(!strcmp(l->func.pos,"EC")||!strcmp(l->func.pos,"SP")||!strcmp(l->func.pos,"JC"))
#define		MODF(l,r)  (l->cont.syn[0]!=r->cont.syn[0])

aPhrase* cStaticRule::setParent( aPhrase* parent, aPhrase* left, aPhrase* right )
{
	// ������ �ڽĳ�带 �������� �θ��� �ʱ�ȭ
	memcpy( parent, right, sizeof(aPhrase) );
	parent->size = left->size + right->size;
	
	// ����(������): �б�������, ������, �����Ե�
	if ( EMPH(left,right) ) { memcpy(parent,left,sizeof(aPhrase)); }
	// ����ǥ ó�� 
	else if ( SYMB(left,right) ) { memcpy(parent,left,sizeof(aPhrase)); }
	// ��ħǥ ó��
	else if ( PROD(left,right)&&strcmp(left->cont.raw,left->func.raw ) ) { memcpy(parent,left,sizeof(aPhrase)); }
	else if ( PROD(left,right) ) { memcpy(&(parent->cont),&(left->cont),sizeof(Lex)); }
	// ��ǥ   ó��
	else if ( PAUS(left,right)&&strcmp(left->cont.raw,left->func.raw ) ) { memcpy(parent,left,sizeof(aPhrase)); }
	else if ( PAUS(left,right) ) { memcpy(&(parent->cont),&(left->cont),sizeof(Lex)); }
	// ���Ӻλ��� ���ӱ��: ǰ�� �� 
	else if ( MAJF(left,right) ) { memcpy(&(parent->cont),&(left->cont),sizeof(Lex)); }
	// ������ ��ȣ/����ǥ ó��
	else if ( RQUT(left,right) ) { memcpy(&(parent->cont),&(left->cont),sizeof(Lex)); }
	// ���Ա���(NP(NP)=>NPNP_PRN)
	else if ( !RQUT(left,right)&&PRNF(left,right) ) { memcpy(parent,left,sizeof(aPhrase)); }
	// �Ļ�(���̻�)
	else if ( DERV(left,right) ) { memcpy(&(parent->cont),&(left->cont),sizeof(Lex)); }
	// ����(����/���)
	else if ( INFL(left,right) ) { memcpy(&(parent->cont),&(left->cont),sizeof(Lex)); }
	// ��������  
/*	else if ( SUBC(left,right) ) { 
		int i=0; }
	// ���
	else if ( CONJ(left,right) ) { 
		int i=0; }
	// ����
	else if ( MODF(left,right) ) { 
		int i=0; }
	// ��Ÿ ����
	else { 
		int i=0; }
*/
	// ����_��� ǥ�� ���� ����: ������ S�� 
	if ( (parent->cont.syn[0]=='S')||(parent->cont.syn[0]=='X')) { parent->cont.syn[1]='\0'; } 
	// ����_��� ǥ�� ���� ����: ����������
	else if ( !strncmp(parent->func.pos,"VC",strlen("VC")) ) { strcpy(parent->cont.syn,"VNP"); }
	// �������� P(phrase)����
	else { parent->cont.syn[1]='P'; }

     if(strcmp(parent->cont.syn, "VPP") ==0)
          strcpy(parent->cont.syn,"VNP");

	parent->end_left  = left->end_left;		
	parent->end_right = right->end_right;	

	parent->left  = left;	
	parent->right = right;
	
	return parent;
}


////////////////////////////////////////////////////////////////////
//
//  cReadData Ŭ������ setNewSynData �Լ� 
//
//  ���� : ���ʳ��, �����ʳ��, ������ ���
// 
//  ���� : ���ʳ��� �����ʳ�带 �������� ����� ���� 
//   

aPhrase *cStaticRule::getParent( aPhrase* left, aPhrase* right, Phrases* phrases )
{
	// �޸� ������ NULL ��ȯ
	aPhrase *parent; if ( (parent=getPhrase(left->end_left,right->end_right,phrases)) == NULL ) { return NULL; }

	// �θ� ����
	return setParent( parent, left, right );
}


////////////////////////////////////////////////////////////////////
//
//  cReadData Ŭ������ setNewSynData �Լ� 
//
//  ���� : ���ʳ��, �����ʳ��, ������ ���
// 
//  ���� : ���ʳ��� �����ʳ�带 �������� ����� ���� 
//   

void cStaticRule::retagging( aPhrase **parent, Words *words, Phrases *phrases )
{
	// �ܸ���� ����
/*	if ( (*parent)->end_left == (*parent)->end_right )
	{
		if ( (*parent=getParent(words->one[(*parent)->end_left].pos,(*parent)->end_left,phrases))==NULL ) { printf("ERROR(tagging): �����ʱ�ȭ ����(%s)\n",words->one[(*parent)->end_left].pos); return; }
	}
	// ��ܸ���� ����
	else
	{	
		retagging( &((*parent)->left), words, phrases );	
		retagging( &((*parent)->right),words, phrases );
		if ( (*parent=getParent((*parent)->left,(*parent)->right,phrases)) == NULL ) { printf("ERROR(tagging): �����ʱ�ȭ ����(%s)\n",words->one[(*parent)->end_left].pos); return; }
		if ( !strcmp((*parent)->left->cont.syn,"NP")&&(!strcmp((*parent)->right->cont.syn,"VP")||!strcmp((*parent)->right->cont.syn,"VNP")) )
		{
			for( aPhrase *temp=(*parent)->left;(temp!=NULL)&&!strcmp(temp->cont.syn,"NP")&&(temp->func.syn[0]=='\0'); temp=temp->right ) {strcpy(temp->func.syn,"_AJT");}
		}
	}
*/}

