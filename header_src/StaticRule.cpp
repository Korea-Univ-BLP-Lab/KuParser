#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

#include "StaticRule.h"


////////////////////////////////////////////////////////////////////
//
//  ccConstraints 클래스의 modifyNPcont 함수 
//
//  인자 : 복합구 CONT
//
//  역할 : C4.5에 적용가능하도록 단말노드의 어휘정리
// 

aPhrase *cStaticRule::getParent( char *pos_tagged_word, short word_idx, Phrases *phrases )
{
	// 문장시작에서 따옴표 초기화
	if ( word_idx==0 ) { m_dquotation=0; m_squotation=0; m_brace=0; }

	// 품사 부착 어절의 초기 자질구조 생성
	aPhrase *parent; if ( (parent=getPhrase(word_idx,word_idx,phrases))==NULL ) { return NULL; }
	aPhrase temp[MAX_UPS_CNT]; memset(&temp,'\0',MAX_UPS_CNT*sizeof(aPhrase));

	char *pos=NULL, *raw=NULL;
	for ( int idx=1; getWordPOS(pos_tagged_word,&raw,&pos)==true; idx++ )
	{
          int i ;
		for (i=0; strcmp(m_pos2node[i].pos,pos)&&(m_pos2node[i].pos[0]!='\0'); i++ );  // 내용/기능(구문)
		if( m_pos2node[i].pos[0]=='\0' ) 
        { 
            fprintf(stderr,"\nERROR(cStaticRule::initPhrase): 구문초기화오류(%s/%s)\n",raw,pos); return NULL; 
        }	
		else { strcpy(temp[idx].cont.syn,m_pos2node[i].cont_syn); strcpy(temp[idx].func.syn,m_pos2node[i].func_syn); } 

		strcpy(temp[idx].cont.raw,raw); strcpy(temp[idx].cont.pos,pos);						// 내용(어휘/품사)
		strcpy(temp[idx].func.raw,raw); strcpy(temp[idx].func.pos,pos);						// 기능(어휘/품사)

		aPhrase *a=&temp[idx]; checkSynTag(&a);												// cont.syn과 func.syn의 점검 
		if ( (m_lexicon!=NULL)&&(m_lexicon->check(raw,pos,false)==false) ) { strcpy(temp[idx].func.raw,"UNK"); }
		if ( (m_lexicon!=NULL)&&(m_lexicon->check(raw,pos,false)==false) ) { strcpy(temp[idx].cont.raw,"UNK"); }
																							// 사전에 없는 단어는 미등록어로 설정
		temp[idx].end_left = word_idx; temp[idx].end_right = word_idx;						// 위치
		if ( *raw=='+' ) { temp[idx].size = 0; } else { temp[idx].size = 1; }				// 크기(어절시작:1/나머지:0)
		
		// 파생 및 굴절 처리
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
	parent->size = 1;																	// 길이
	parent->prob = 1.0;																	// 확률
	parent->left = NULL; parent->right = NULL;											// 자식
	
	// 구문_기능 표지 부착 예외: (S- + ./SF)이외의 기능 태그(-)는 불허
	if ( strcmp(parent->cont.syn,"S-")&&((parent->cont.syn[1]=='-')||(parent->cont.syn[0]=='P')) ) { strcpy(parent->cont.syn,"X"); }
	// 괄호의 기능태그는 _PRN
	else if ( !strcmp(parent->func.syn,"_PRN")&&(strstr(pos_tagged_word,"(")==NULL)&&(strstr(pos_tagged_word,")")==NULL) ) { strcpy(parent->func.syn,""); }

	// 명사+쉼표의 기능태그는 _CNJ
	if ( !strcmp(parent->cont.syn,"NP")&&(parent->func.syn[0]=='\0')&&(strstr(pos_tagged_word,",")!=NULL) ) { strcpy(parent->func.syn,"_CNJ"); }
	// 구문_기능 표지 부착 예외: '첫어절+!'는 호격기능
	else if ( !strcmp(parent->cont.syn,"NP")&&(parent->func.syn[0]=='\0')&&(strstr(pos_tagged_word,"!")!=NULL) ) { strcpy(parent->func.syn,"_INT"); }

	return parent;
}


////////////////////////////////////////////////////////////////////
//
//  cReadData 클래스의 getNewSynData 함수 
//
//	반환 : 새구문노드의 메모리
//   
//  역할 : 새노드를 위한 메모리 할당 
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
	else { checkWord( word, pos ); return true; }			// 입력 단어의 점검
}


////////////////////////////////////////////////////////////////////
//
//  cReadData 클래스의 getNewSynData 함수 
//
//	반환 : 새구문노드의 메모리
//   
//  역할 : 새노드를 위한 메모리 할당 
//   

aPhrase *cStaticRule::getPhrase( short left, short right, Phrases *phrases )
{
	short	idx = phrase_idx(left,right); 
	aPhrase *temp;

	if ( (idx>=MAX_PHS_CNT)||(idx<0)||(left<0)||(right<0) ) 
	{ 
		fprintf(stderr,"\nERROR(cBinaryRule::getPhrase): 메모리 할당 오류(범위초과)" ); 
		return NULL; 
	}
	else if ( phrases->one[idx]!=NULL ) 
	{
//		memset(phrases->one[idx],'\0',sizeof(aPhrase));
		return phrases->one[idx];
	}
	else if ( (temp=(aPhrase*)malloc(sizeof(aPhrase))) == NULL )
	{
		fprintf(stderr,"\nERROR(cBinaryRule::getPhrase): 메모리 할당 오류(malloc)" ); 
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
//  ccConstraints 클래스의 modifyNPcont 함수 
//
//  인자 : 복합구 CONT
//
//  역할 : C4.5에 적용가능하도록 단말노드의 어휘정리
// 

void cStaticRule::checkSynTag( aPhrase **phrase )
{
	// 구문_기능 표지 부착 예외: 따옴표 처리
	if ( (*phrase)->cont.syn[0]=='L' ) 
	{
		(*phrase)->cont.syn[0]=checkQuotation((*phrase)->cont.raw); 
		strcat((*phrase)->cont.pos,(*phrase)->cont.syn); 
		strcat((*phrase)->func.pos,(*phrase)->cont.syn); 
	}
}


////////////////////////////////////////////////////////////////////
//
//  ccConstraints 클래스의 modifyNPcont 함수 
//
//  인자 : 복합구 CONT
//
//  역할 : C4.5에 적용가능하도록 단말노드의 어휘정리
// 

#define	RAW				0
#define	POS				1

// q:qutation, v:value of q, l:left, r:right
#define COMPARE(loc,l,r)  ((!strncmp(loc,l,strlen(l))||!strncmp(loc,r,strlen(r)))) 
#define CHECKQUT(q,v,loc,l,r) ((q==v)&&COMPARE(&spacingUnit.loc,l,r))

void cStaticRule::segment( int idx[2][MAX_UPS_CNT], aWord spacingUnit )
{
	// 초기화
	int i=0, index_num=0; memset(idx,0,2*MAX_UPS_CNT*sizeof(int));
	
	// 따옴표쌍이 둘이상의 어절에 영향을 끼치는지 점검
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

	// 단어(raw)의 분할위치: 어절간따옴표(") 
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

	// 단어(pos)의 분할위치: 어절간따옴표( + "/SS)
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
//  ccConstraints 클래스의 modifyNPcont 함수 
//
//  인자 : 복합구 CONT
//
//  역할 : C4.5에 적용가능하도록 단말노드의 어휘정리
// 

void cStaticRule::getSynUnits( Words *words, Words *spacingUnits )
{
	memset( words, '\0', sizeof(Words) );
	int i, j, k; 
    int idx[2][MAX_UPS_CNT];
	for( i=0, j=0; spacingUnits->one[i].raw[0]!='\0'; i++, j++ )
	{ 
		// 기본 단위로 분할: 띄어쓰기, 어절간괄호, 마침표
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
//  ccConstraints 클래스의 modifyNPcont 함수 
//
//  인자 : 복합구 CONT
//
//  역할 : C4.5에 적용가능하도록 단말노드의 어휘정리
// 

void cStaticRule::checkWord( char **arg_raw, char **arg_pos )
{
	char	*raw=*arg_raw, *pos=*arg_pos;
	char	temp[MAX_WRD_LEN]; memset(temp,'\0',sizeof(char)*MAX_WRD_LEN);

	// 공백 및 + 선별 제거
	int i, j;
    for ( i=0, j=0; (raw[i]!='\0'); i++, j++ ) { if ( (raw[i]==' ') ){ j--; } else { raw[j]=raw[i]; } } 
	if ( (raw[j]!='\0') ) { raw[j]='\0'; }
	if ( (*raw=='+')&&((*pos=='N')||(*pos=='M')||(*pos=='I')||(*pos=='S')||!strcmp(pos,"VV")||!strcmp(pos,"VA")||!strcmp(pos,"VX"))) { strcpy(raw,raw+1); }
	
	// "//SP" = "/"(raw) + "SP"(pos)
	if ( !strcmp(raw,"SP") ) { *arg_pos=raw; strcpy(temp,"/"); *arg_raw=temp; return; }
	// "+//SP" = "+/"(raw) + "SP"(pos)
	else if ( !strcmp(pos,"/SP") ) { *arg_pos=pos+1; strcpy(temp,"+/"); *arg_raw=temp; return; }
	// 숫자
	else if ( !strcmp(pos,"SN") ) { strcpy(*arg_raw,"n"); *arg_pos=pos; return; }
	// 외국어
	else if ( !strcmp(pos,"SL") ) { strcpy(*arg_raw,"l"); *arg_pos=pos; return; }
	// 한자 
	else if ( !strcmp(pos,"SH") ) { strcpy(*arg_raw,"h"); *arg_pos=pos; return; }
	// 달러 
	else if ( !strcmp(pos,"SW") ) { strcpy(*arg_raw,"w"); *arg_pos=pos; return; }
	// 기타
	else  { *arg_raw=raw; *arg_pos=pos; return; }
}


////////////////////////////////////////////////////////////////////
//
//  ccConstraints 클래스의 modifyNPcont 함수 
//
//  인자 : 복합구 CONT
//
//  역할 : C4.5에 적용가능하도록 단말노드의 어휘정리
// 

char cStaticRule::checkQuotation( char *raw )
{
	char symbol; 
    int i;
    for ( i=0; (raw[i]==' ')||(raw[i]=='+'); i++ ); 
	if ( (raw[i]==' ')||(raw[i]=='+') ) { symbol=raw[i+1]; } else { symbol=raw[i]; }

	// (둥근괄호),[대괄호],{중괄호},<꺽쇠괄호>: 좌우 구분이 명확
	if ( (symbol=='(')||(symbol=='[')||(symbol=='{')||(symbol=='<') ) { m_brace++; return 'L'; }
	else if ( (symbol==')')||(symbol==']')||(symbol=='}')||(symbol=='>') ) { m_brace--; return 'R'; }

	// "(큰따옴표),`,'(작은따옴표): 좌우 구분이 모호 
	if ( (symbol=='"')&&(m_dquotation==0 ) ) { m_dquotation++; return 'L'; }
	else if ( (symbol=='"' )&&(m_dquotation >0 ) ) { m_dquotation--; return 'R'; }
	else if ( (symbol=='`' )&&(m_squotation==0 ) ) { m_squotation++; return 'L'; }
	else if ( (symbol=='`' )&&(m_squotation >0 ) ) { m_squotation--; return 'R'; }
	else if ( (symbol=='\'')&&(m_squotation==0 ) ) { m_squotation++; return 'L'; }
	else if ( (symbol=='\'')&&(m_squotation >0 ) ) { m_squotation--; return 'R'; }
	else { return 'X'; }	// 오류
}

cnvrt cStaticRule::m_pos2node[MAX_POS_CNT] =	
{
	{ "NP",	"",		"NNG"	},		// 일반 명사 
	{ "NP",	"",		"NNP"	},		// 고유 명사  
	{ "NP",	"",		"NNB"	},		// 의존 명사 
	{ "NP",	"",		"NP"	},		// 대명사 
	{ "NP", "",		"NR"	},		// 수사 
	
	{ "VP", "",		"VV"	},		// 동사 
	{ "VP", "",		"VA"	},		// 형용사
	{ "V-", "",		"VX"	},		// 보조 용언
	{ "VN-","",		"VCP"	},		// 긍정 지정사 
	{ "V-", "",		"VCN"	},		// 부정 지정사 
	
	{ "DP", "",		"MM"	},		// 관형사 
	{ "AP", "",		"MAG"	},		// 일반 부사 
	{ "AP", "",		"MAJ"	},		// 접속 부사[접속 또는 연결] 
	
	{ "IP", "",		"IC"	},		// 감탄사 	

	{ "P-",	"_SBJ",	"JKS"	},		// 주격   조사[주어]
	{ "P-",	"_CMP",	"JKC"	},		// 보격   조사[보어]
	{ "D-",	"_MOD",	"JKG"	},		// 관형격 조사
	{ "P-",	"_OBJ",	"JKO"	},		// 목적격 조사[목적어]
	{ "P-",	"_AJT",	"JKB"	},		// 부사격 조사[부사어]
	{ "I-",	"_INT",	"JKV"	},		// 호격   조사
	{ "P-",	"_CMP",	"JKQ"	},		// 인용격 조사
	{ "P-",	"",		"JX"	},		// 보조사     [미지격]
	{ "N-",	"_CNJ",	"JC"	},		// 접속격 조사[접속 또는 연결]
	
	{ "V-",	"",		"EP"	},		// 선어말 어미[시제/존칭/강조/조동사]
	{ "V-",	"",		"EF"	},		// 종결   어미[종결]
	{ "V-",	"",		"EC"	},		// 연결   어미[접속 또는 연결]
	{ "N-",	"",		"ETN"	},		// 명사형 어미
	{ "D-",	"_MOD",	"ETM"	},		// 관형형 어미
	
	{ "NP",	"",		"XPN"	},		// 체언	  접두사[온-,제4번째]
	
	{ "N-",	"",		"XSN"	},		// 명사   파생 접미사[의미적 한정/복수]
	{ "V-",	"",		"XSV"	},		// 동사   파생 접미사
	{ "V-",	"",		"XSA"	},		// 형용사 파생 접미사
	{ "A-",	"",		"XSB"	},		// 부사   파생 접미사

	{ "NP",	"",		"XR"	},		// 어기 

	{ "X",	"",		"SF"	},		// 마침표, 물음표, 느낌표 
	{ "X",	"",		"SP"	},		// 쉼표, 가운데점, 콜론, 빗금
	{ "L",	"_PRN",	"SS"	},		// 따옴표, 괄호표, 줄표
	{ "X",	"",		"SE"	},		// 줄임표 
	{ "X",	"",		"SO"	},		// 붙임표[물결,숨김,빠짐]
	
	{ "NP",	"",		"SN"	},		// 숫자
	{ "NP",	"",		"SL"	},		// 외국어 
	{ "NP",	"",		"SH"	},		// 한자 
	{ "NP",	"",		"SW"	},		// 숫자, 화폐기호

	{ "NP",	"",		"NF"	},		// 명사 추정 범주 
	{ "VP",	"",		"NV"	},		// 용언 추정 범주 
	{ "NP",	"",		"NA"	},		// 분석 불능 범주  
	
	{ "--",	"",		""		},		// 심볼 
};


////////////////////////////////////////////////////////////////////
//
//  cReadData 클래스의 setNewSynData 함수 
//
//  인자 : 왼쪽노드, 오른쪽노드, 생성할 노드
// 
//  역할 : 왼쪽노드와 오른쪽노드를 바탕으로 새노드 설정 
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
	// 오른쪽 자식노드를 바탕으로 부모노드 초기화
	memcpy( parent, right, sizeof(aPhrase) );
	parent->size = left->size + right->size;
	
	// 강조(보조사): 학교에서는, 오르고도, 빠르게도
	if ( EMPH(left,right) ) { memcpy(parent,left,sizeof(aPhrase)); }
	// 붙임표 처리 
	else if ( SYMB(left,right) ) { memcpy(parent,left,sizeof(aPhrase)); }
	// 마침표 처리
	else if ( PROD(left,right)&&strcmp(left->cont.raw,left->func.raw ) ) { memcpy(parent,left,sizeof(aPhrase)); }
	else if ( PROD(left,right) ) { memcpy(&(parent->cont),&(left->cont),sizeof(Lex)); }
	// 쉼표   처리
	else if ( PAUS(left,right)&&strcmp(left->cont.raw,left->func.raw ) ) { memcpy(parent,left,sizeof(aPhrase)); }
	else if ( PAUS(left,right) ) { memcpy(&(parent->cont),&(left->cont),sizeof(Lex)); }
	// 접속부사의 접속기능: 품평 및 
	else if ( MAJF(left,right) ) { memcpy(&(parent->cont),&(left->cont),sizeof(Lex)); }
	// 오른쪽 괄호/따옴표 처리
	else if ( RQUT(left,right) ) { memcpy(&(parent->cont),&(left->cont),sizeof(Lex)); }
	// 삽입구문(NP(NP)=>NPNP_PRN)
	else if ( !RQUT(left,right)&&PRNF(left,right) ) { memcpy(parent,left,sizeof(aPhrase)); }
	// 파생(접미사)
	else if ( DERV(left,right) ) { memcpy(&(parent->cont),&(left->cont),sizeof(Lex)); }
	// 굴절(조사/어미)
	else if ( INFL(left,right) ) { memcpy(&(parent->cont),&(left->cont),sizeof(Lex)); }
	// 하위범주  
/*	else if ( SUBC(left,right) ) { 
		int i=0; }
	// 대등
	else if ( CONJ(left,right) ) { 
		int i=0; }
	// 수식
	else if ( MODF(left,right) ) { 
		int i=0; }
	// 기타 오류
	else { 
		int i=0; }
*/
	// 구문_기능 표지 부착 예외: 문장은 S로 
	if ( (parent->cont.syn[0]=='S')||(parent->cont.syn[0]=='X')) { parent->cont.syn[1]='\0'; } 
	// 구문_기능 표지 부착 예외: 서술격조사
	else if ( !strncmp(parent->func.pos,"VC",strlen("VC")) ) { strcpy(parent->cont.syn,"VNP"); }
	// 나머지는 P(phrase)부착
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
//  cReadData 클래스의 setNewSynData 함수 
//
//  인자 : 왼쪽노드, 오른쪽노드, 생성할 노드
// 
//  역할 : 왼쪽노드와 오른쪽노드를 바탕으로 새노드 설정 
//   

aPhrase *cStaticRule::getParent( aPhrase* left, aPhrase* right, Phrases* phrases )
{
	// 메모리 없으면 NULL 반환
	aPhrase *parent; if ( (parent=getPhrase(left->end_left,right->end_right,phrases)) == NULL ) { return NULL; }

	// 부모 설정
	return setParent( parent, left, right );
}


////////////////////////////////////////////////////////////////////
//
//  cReadData 클래스의 setNewSynData 함수 
//
//  인자 : 왼쪽노드, 오른쪽노드, 생성할 노드
// 
//  역할 : 왼쪽노드와 오른쪽노드를 바탕으로 새노드 설정 
//   

void cStaticRule::retagging( aPhrase **parent, Words *words, Phrases *phrases )
{
	// 단말노드 설정
/*	if ( (*parent)->end_left == (*parent)->end_right )
	{
		if ( (*parent=getParent(words->one[(*parent)->end_left].pos,(*parent)->end_left,phrases))==NULL ) { printf("ERROR(tagging): 구문초기화 오류(%s)\n",words->one[(*parent)->end_left].pos); return; }
	}
	// 비단말노드 설정
	else
	{	
		retagging( &((*parent)->left), words, phrases );	
		retagging( &((*parent)->right),words, phrases );
		if ( (*parent=getParent((*parent)->left,(*parent)->right,phrases)) == NULL ) { printf("ERROR(tagging): 구문초기화 오류(%s)\n",words->one[(*parent)->end_left].pos); return; }
		if ( !strcmp((*parent)->left->cont.syn,"NP")&&(!strcmp((*parent)->right->cont.syn,"VP")||!strcmp((*parent)->right->cont.syn,"VNP")) )
		{
			for( aPhrase *temp=(*parent)->left;(temp!=NULL)&&!strcmp(temp->cont.syn,"NP")&&(temp->func.syn[0]=='\0'); temp=temp->right ) {strcpy(temp->func.syn,"_AJT");}
		}
	}
*/}

