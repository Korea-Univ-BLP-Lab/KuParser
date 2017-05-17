#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <functional>
#include "BTree.h"

using namespace std;
////////////////////////////////////////////////////////////////////
//
//  cBTree 클래스의 cBTree 함수 
//
//  인자 : 입력자료크기, 비교함수포인터, 출력함수포인터 
//   
//  역할 : 이진나무를 생성하고 초기화
//

cBTree::cBTree( size_t width, FCMP fcmp, FPNT fpnt )
{
	// 노드 생성 초기화
	m_root = (node*)malloc(sizeof(node));
	m_root->left  = NULL;
	m_root->right = NULL;

	// 노드수:0
	m_cnt = 0;
	
	// 입력자료크기:width
	m_width = width;

	// 비교함수와 출력함수 설정
	m_fcmp = fcmp;
	m_fpnt = fpnt;

	// 노드변화: 0
	m_change = 0;
}


////////////////////////////////////////////////////////////////////
//
//  cBTree 클래스의 search 함수 
//
//  인자 : 비교대상
//
//  반환 : 비교대상과 동일값을 갖는 노드
//   
//  역할 : 비교대상을 이진나무에서 탐색
// 

void *cBTree::search( void *key )
{
	// 이진나무의 탐색위치 지정
	node *tmp = m_root->left;

	// tmp:탐색위치 tmp+1:실제자료위치 
	while( tmp!=NULL && m_fcmp(key,tmp+1)!=0 ) 
	{
		// 비교대상이 왼쪽에 위치
		if ( m_fcmp(key,tmp+1)<0 ) tmp=tmp->left;
		// 비교대상이 오른쪽에 위치
		else tmp=tmp->right;
	}

	// 탐색에 실패하면 NULL반환
	if ( tmp==NULL ) return NULL;
	// 탐색에 성공하면 실제자료위치 반환
	else return tmp+1;
}


////////////////////////////////////////////////////////////////////
//
//  cBTree 클래스의 insert 함수 
//
//  인자 : 삽입대상
//
//  반환 : 이진나무에서 삽입노드의 위치
//   
//  역할 : 삽입노드를 이진나무에 삽입 
// 
 
void *cBTree::insert( void *key )
{
	// 변경 회수가 많으면 새로 균형잡기
	if ( (++m_change)>MAX_CNG_CNT ) { balance(); m_change=0; }

	// 이진나무의 탐색노드 설정
	node *parent=m_root, *son=m_root->left;

	// 삽입가능위치 탐색
	while( son!=NULL )
	{
		parent=son;
		// 비교대상이 왼쪽에 위치
		if ( m_fcmp(key,son+1)<0 ) son=son->left;
		// 비교대상이 오른쪽에 위치
		else son=son->right;
	}

	// (위치노드크기+실제자료크기)의 공간을 할당
	if ( (son=(node*)malloc(sizeof(node)+m_width)) == NULL ) return NULL;

	// son 초기화 
	son->left =NULL;
	son->right=NULL;
	// son+1에 실제자료를 복사
	memcpy( son+1, key, m_width );
	
	// 새로 설정된 son을 parent에 연결
	if ( m_fcmp(key,parent+1)<0 || parent==m_root ) parent->left=son;
	else parent->right=son;
	
	// 이진나무의 노드수 증가 
	m_cnt++;
	
	// 새로 설정된 노드 반환
	return son;
}


////////////////////////////////////////////////////////////////////
//
//  cBTree 클래스의 deleteOne 함수 
//
//  인자 : 삭제대상
//
//  반환 : 삭제노드의 부모노드
//   
//  역할 : 삭제노드를 이진나무에서 삭제하고 이진나무 조정
// 

void *cBTree::deleteOne( void *key )
{
	// 변경 회수가 많으면 새로 균형잡기
	if ( (++m_change)>MAX_CNG_CNT ) { balance(); m_change=0; }

	// parent:삭제노드부모 son:del을 대신할 parent의 자식 del:삭제노드 nexth:son노드의 부모
	node *parent, *son, *del, *nexth;
	parent = m_root;
	del = m_root->left;
	
	// 삭제노드를 탐색
	while( m_fcmp(key,del+1)!=0 && del!=NULL )
	{
		parent = del;
		// 삭제대상이 왼쪽에 위치
		if ( m_fcmp(key,del+1)<0 ) del=del->left;
		// 삭제대상이 오른쪽에 위치
		else del = del->right;
	}

	// 삭제대상이 없으면
	if ( del==NULL ) return NULL;
	// 삭제노드의 오른쪽자식이 없으면
	else if ( del->right==NULL ) son = del->left;
	// 삭제노드의 오른쪽자식의 왼쪽자식이 없으면
	else if ( del->right->left==NULL )
	{
		son = del->right;
		son->left = del->left;
	}
	// 그이외의 경우
	else 
	{
		nexth = del->right;
		while( nexth->left->left!=NULL ) nexth = nexth->left;
		son = nexth->left;
		nexth->left = son->right;
		son->left = del->left;
		son->right = del->right;
	}

	// parent에 del대신에 son을 연결
	if ( m_fcmp(key,parent+1)<0 || parent==m_root ) parent->left=son;
	else parent->right=son;

	// del을 없앰
	free(del);
	// 이진나무의 전체노드수를 줄임
	m_cnt--;
	
	// 삭제노드의 부모노드를 반환
	return parent;
}


////////////////////////////////////////////////////////////////////
//
//  cBTree 클래스의 deleteNode 함수 
//
//  인자 : 삭제할 노드
//
//  역할 : deleteAll()의 부함수로서 노드를 모두 삭제
// 

void cBTree::deleteNode( node *t )
{
	// 모든 노드를 삭제 
	if ( t != NULL )
	{
		deleteNode( t->left );
		deleteNode( t->right);
		free(t);
	}
}


////////////////////////////////////////////////////////////////////
//
//  cBTree 클래스의 deleteAll 함수 
//
//  반환 : 이진나무의 root
//   
//  역할 : 이진나무의 모든 노드를 삭제
// 

void *cBTree::deleteAll()
{
	// 삭제할 첫노드
	node *t = m_root->left;
	
	// 모든 노드를 삭제
	deleteNode(t);
	
	// BTree 초기화
	m_root->left  = NULL;
	m_root->right = NULL;
	m_cnt = 0;
	
	// 초기화된 이진나무의 반환
	return m_root;
}


////////////////////////////////////////////////////////////////////
//
//  cBTree 클래스의 pntlist 함수 
//
//  인자 : 출력파일 포인터
//
//  역할 : 이진나무를 정렬상태로 출력파일에 출력
// 

void cBTree::pntlist( FILE* out )
{
	sortlist( m_root->left, out );
}


////////////////////////////////////////////////////////////////////
//
//  cBTree 클래스의 sortlist 함수 
//
//  인자 : 출력노드, 출력파일 포인터
//
//  역할 : pntlist()의 부함수로서 이진나무의 각노드를 정렬상태로 출력파일에 출력
// 
		
void cBTree::sortlist( node *p, FILE* out )
{
	if ( p!=NULL )
	{
		sortlist( p->left, out );
		// 정렬 순서대로 출력 
		m_fpnt(out,p+1);
		sortlist( p->right, out );
	}
}




////////////////////////////////////////////////////////////////////
//
//  cBTree 클래스의 _balance 함수 
//
//  인자 : 균형잡을 노드수, 배열포인터
//
//  반환 : 현내용을 담은 노드 
//   
//  역할 : balance() 함수의 부함수로서 배열구조를 이진나무구조로 변환
// 

node *cBTree::_balance( int n, void *a )
{
	int nl, nr;
	node *p;

	if ( n>0 ) 
	{
		// 이진나무의 포인터 조정
		nl = (n-1)/2;
		nr = n - nl - 1;
		// 이진나무의 노드 할당
		if ( (p=(node*)malloc(sizeof(node)+m_width)) == NULL ) return NULL;
		// 왼쪽 먼저 균형잡기 
		p->left = _balance( nl, a );
		// 배열의 내용을 노드의 실제내용위치에 복사 
		memcpy( p+1, (char*)a+(m_idx++)*m_width, m_width );
		// 오른쪽도 균형잡기
		p->right = _balance( nr, a );
		// 균형잡힌 노드 반환
		return p;
	}
	// 크기가 0이면 NULL을 리턴
	else { return NULL; }
}

////////////////////////////////////////////////////////////////////
//
//  cBTree 클래스의 sort 함수 
//
//  인자 : 정렬노드, 배열
//
//  역할 : balance()의 정렬함수로서 배열의 적절한 위치에 내용만 복사
// 

void cBTree::sort( node *p, void *a )
{
	if ( p!=NULL )
	{
		sort( p->left, a );
		// 배열에 노드의 내용만 복사 
		memcpy( (char*)a+(m_idx++)*m_width, p+1, m_width );
		sort( p->right, a );
	}
}

////////////////////////////////////////////////////////////////////
//
//  cBTree 클래스의 balance 함수 
//
//  역할 : 이진나무의 균형을 잡는 함수
// 

void *cBTree::balance()
{
	// 전체노드수
	int  ntmp = m_cnt;
	// 배열 생성 
	void *tmp;
	
	// 배열인덱스 초기화하고 배열에 정렬된 상태로 저장
	if ( (tmp=malloc(m_width*ntmp)) == NULL ) { return NULL; }
	m_idx = 0;
	sort( m_root->left, tmp );
	
	// 아진나무 삭제후 배열인덱스 초기화
	deleteAll();
	m_idx = 0;
	
	// 균형잡힌 이진나무를 새로 생성
	m_root->left = _balance( ntmp, tmp );
	// 생성후 전체노드수 원상복귀
	m_cnt = ntmp;
	
	// 배열삭제 
	free( tmp );

	return m_root;
}


////////////////////////////////////////////////////////////////////
//
//  cBTree 클래스의 getSize 함수 
//
//  역할 : 이진나무 노드의 갯수를 반환
// 

int cBTree::getSize()
{
	return (int)m_cnt;
}

//////////////////////////////////////////////////////////////////////
// cBTree의 성능 개선 버전.

cBTree_v2::cBTree_v2( size_t width, FCMP fcmp, FPNT fpnt )
{
	// 입력자료크기:width
	m_width = width;

	// 비교함수와 출력함수 설정
	m_fcmp = fcmp;
	m_fpnt = fpnt;
     
}

struct DeleteElem:unary_function<void*, void>
{
     void operator()(void * p_void) const
     {
          free(p_void);
     }
};

cBTree_v2::~cBTree_v2()
{
     for_each(this->elem_ptrs.begin(), elem_ptrs.end(), DeleteElem());
}

struct KeyCmp
{
     KeyCmp(FCMP fcm): fcmp_(fcm) {}
     bool operator()(const void* p_lhs, const void* p_rhs) const
     {
          return (fcmp_(p_lhs, p_rhs) < 0);
     }

     FCMP fcmp_;
};
     					
void	* 
cBTree_v2::search( void *key )
{
     t_vipair vi_search = equal_range(elem_ptrs.begin(), elem_ptrs.end(), key, KeyCmp(m_fcmp));
     
     if(vi_search.first!=vi_search.second)// found
     {
          return *(vi_search.first);
     }
     else return NULL;
}

void	* 
cBTree_v2::insert( void *key )
{
     void * p_new = malloc(m_width);
     if(p_new==NULL) return NULL;

     memcpy(p_new, key, m_width );
     elem_ptrs.push_back(p_new);// 냅다 추가 
     return p_new;
}

void 
cBTree_v2::sort()
{
     std::sort(elem_ptrs.begin(), elem_ptrs.end(), KeyCmp(m_fcmp));
}