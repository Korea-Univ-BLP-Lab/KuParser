#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <functional>
#include "BTree.h"

using namespace std;
////////////////////////////////////////////////////////////////////
//
//  cBTree Ŭ������ cBTree �Լ� 
//
//  ���� : �Է��ڷ�ũ��, ���Լ�������, ����Լ������� 
//   
//  ���� : ���������� �����ϰ� �ʱ�ȭ
//

cBTree::cBTree( size_t width, FCMP fcmp, FPNT fpnt )
{
	// ��� ���� �ʱ�ȭ
	m_root = (node*)malloc(sizeof(node));
	m_root->left  = NULL;
	m_root->right = NULL;

	// ����:0
	m_cnt = 0;
	
	// �Է��ڷ�ũ��:width
	m_width = width;

	// ���Լ��� ����Լ� ����
	m_fcmp = fcmp;
	m_fpnt = fpnt;

	// ��庯ȭ: 0
	m_change = 0;
}


////////////////////////////////////////////////////////////////////
//
//  cBTree Ŭ������ search �Լ� 
//
//  ���� : �񱳴��
//
//  ��ȯ : �񱳴��� ���ϰ��� ���� ���
//   
//  ���� : �񱳴���� ������������ Ž��
// 

void *cBTree::search( void *key )
{
	// ���������� Ž����ġ ����
	node *tmp = m_root->left;

	// tmp:Ž����ġ tmp+1:�����ڷ���ġ 
	while( tmp!=NULL && m_fcmp(key,tmp+1)!=0 ) 
	{
		// �񱳴���� ���ʿ� ��ġ
		if ( m_fcmp(key,tmp+1)<0 ) tmp=tmp->left;
		// �񱳴���� �����ʿ� ��ġ
		else tmp=tmp->right;
	}

	// Ž���� �����ϸ� NULL��ȯ
	if ( tmp==NULL ) return NULL;
	// Ž���� �����ϸ� �����ڷ���ġ ��ȯ
	else return tmp+1;
}


////////////////////////////////////////////////////////////////////
//
//  cBTree Ŭ������ insert �Լ� 
//
//  ���� : ���Դ��
//
//  ��ȯ : ������������ ���Գ���� ��ġ
//   
//  ���� : ���Գ�带 ���������� ���� 
// 
 
void *cBTree::insert( void *key )
{
	// ���� ȸ���� ������ ���� �������
	if ( (++m_change)>MAX_CNG_CNT ) { balance(); m_change=0; }

	// ���������� Ž����� ����
	node *parent=m_root, *son=m_root->left;

	// ���԰�����ġ Ž��
	while( son!=NULL )
	{
		parent=son;
		// �񱳴���� ���ʿ� ��ġ
		if ( m_fcmp(key,son+1)<0 ) son=son->left;
		// �񱳴���� �����ʿ� ��ġ
		else son=son->right;
	}

	// (��ġ���ũ��+�����ڷ�ũ��)�� ������ �Ҵ�
	if ( (son=(node*)malloc(sizeof(node)+m_width)) == NULL ) return NULL;

	// son �ʱ�ȭ 
	son->left =NULL;
	son->right=NULL;
	// son+1�� �����ڷḦ ����
	memcpy( son+1, key, m_width );
	
	// ���� ������ son�� parent�� ����
	if ( m_fcmp(key,parent+1)<0 || parent==m_root ) parent->left=son;
	else parent->right=son;
	
	// ���������� ���� ���� 
	m_cnt++;
	
	// ���� ������ ��� ��ȯ
	return son;
}


////////////////////////////////////////////////////////////////////
//
//  cBTree Ŭ������ deleteOne �Լ� 
//
//  ���� : �������
//
//  ��ȯ : ��������� �θ���
//   
//  ���� : ������带 ������������ �����ϰ� �������� ����
// 

void *cBTree::deleteOne( void *key )
{
	// ���� ȸ���� ������ ���� �������
	if ( (++m_change)>MAX_CNG_CNT ) { balance(); m_change=0; }

	// parent:�������θ� son:del�� ����� parent�� �ڽ� del:������� nexth:son����� �θ�
	node *parent, *son, *del, *nexth;
	parent = m_root;
	del = m_root->left;
	
	// ������带 Ž��
	while( m_fcmp(key,del+1)!=0 && del!=NULL )
	{
		parent = del;
		// ��������� ���ʿ� ��ġ
		if ( m_fcmp(key,del+1)<0 ) del=del->left;
		// ��������� �����ʿ� ��ġ
		else del = del->right;
	}

	// ��������� ������
	if ( del==NULL ) return NULL;
	// ��������� �������ڽ��� ������
	else if ( del->right==NULL ) son = del->left;
	// ��������� �������ڽ��� �����ڽ��� ������
	else if ( del->right->left==NULL )
	{
		son = del->right;
		son->left = del->left;
	}
	// ���̿��� ���
	else 
	{
		nexth = del->right;
		while( nexth->left->left!=NULL ) nexth = nexth->left;
		son = nexth->left;
		nexth->left = son->right;
		son->left = del->left;
		son->right = del->right;
	}

	// parent�� del��ſ� son�� ����
	if ( m_fcmp(key,parent+1)<0 || parent==m_root ) parent->left=son;
	else parent->right=son;

	// del�� ����
	free(del);
	// ���������� ��ü������ ����
	m_cnt--;
	
	// ��������� �θ��带 ��ȯ
	return parent;
}


////////////////////////////////////////////////////////////////////
//
//  cBTree Ŭ������ deleteNode �Լ� 
//
//  ���� : ������ ���
//
//  ���� : deleteAll()�� ���Լ��μ� ��带 ��� ����
// 

void cBTree::deleteNode( node *t )
{
	// ��� ��带 ���� 
	if ( t != NULL )
	{
		deleteNode( t->left );
		deleteNode( t->right);
		free(t);
	}
}


////////////////////////////////////////////////////////////////////
//
//  cBTree Ŭ������ deleteAll �Լ� 
//
//  ��ȯ : ���������� root
//   
//  ���� : ���������� ��� ��带 ����
// 

void *cBTree::deleteAll()
{
	// ������ ù���
	node *t = m_root->left;
	
	// ��� ��带 ����
	deleteNode(t);
	
	// BTree �ʱ�ȭ
	m_root->left  = NULL;
	m_root->right = NULL;
	m_cnt = 0;
	
	// �ʱ�ȭ�� ���������� ��ȯ
	return m_root;
}


////////////////////////////////////////////////////////////////////
//
//  cBTree Ŭ������ pntlist �Լ� 
//
//  ���� : ������� ������
//
//  ���� : ���������� ���Ļ��·� ������Ͽ� ���
// 

void cBTree::pntlist( FILE* out )
{
	sortlist( m_root->left, out );
}


////////////////////////////////////////////////////////////////////
//
//  cBTree Ŭ������ sortlist �Լ� 
//
//  ���� : ��³��, ������� ������
//
//  ���� : pntlist()�� ���Լ��μ� ���������� ����带 ���Ļ��·� ������Ͽ� ���
// 
		
void cBTree::sortlist( node *p, FILE* out )
{
	if ( p!=NULL )
	{
		sortlist( p->left, out );
		// ���� ������� ��� 
		m_fpnt(out,p+1);
		sortlist( p->right, out );
	}
}




////////////////////////////////////////////////////////////////////
//
//  cBTree Ŭ������ _balance �Լ� 
//
//  ���� : �������� ����, �迭������
//
//  ��ȯ : �������� ���� ��� 
//   
//  ���� : balance() �Լ��� ���Լ��μ� �迭������ �������������� ��ȯ
// 

node *cBTree::_balance( int n, void *a )
{
	int nl, nr;
	node *p;

	if ( n>0 ) 
	{
		// ���������� ������ ����
		nl = (n-1)/2;
		nr = n - nl - 1;
		// ���������� ��� �Ҵ�
		if ( (p=(node*)malloc(sizeof(node)+m_width)) == NULL ) return NULL;
		// ���� ���� ������� 
		p->left = _balance( nl, a );
		// �迭�� ������ ����� ����������ġ�� ���� 
		memcpy( p+1, (char*)a+(m_idx++)*m_width, m_width );
		// �����ʵ� �������
		p->right = _balance( nr, a );
		// �������� ��� ��ȯ
		return p;
	}
	// ũ�Ⱑ 0�̸� NULL�� ����
	else { return NULL; }
}

////////////////////////////////////////////////////////////////////
//
//  cBTree Ŭ������ sort �Լ� 
//
//  ���� : ���ĳ��, �迭
//
//  ���� : balance()�� �����Լ��μ� �迭�� ������ ��ġ�� ���븸 ����
// 

void cBTree::sort( node *p, void *a )
{
	if ( p!=NULL )
	{
		sort( p->left, a );
		// �迭�� ����� ���븸 ���� 
		memcpy( (char*)a+(m_idx++)*m_width, p+1, m_width );
		sort( p->right, a );
	}
}

////////////////////////////////////////////////////////////////////
//
//  cBTree Ŭ������ balance �Լ� 
//
//  ���� : ���������� ������ ��� �Լ�
// 

void *cBTree::balance()
{
	// ��ü����
	int  ntmp = m_cnt;
	// �迭 ���� 
	void *tmp;
	
	// �迭�ε��� �ʱ�ȭ�ϰ� �迭�� ���ĵ� ���·� ����
	if ( (tmp=malloc(m_width*ntmp)) == NULL ) { return NULL; }
	m_idx = 0;
	sort( m_root->left, tmp );
	
	// �������� ������ �迭�ε��� �ʱ�ȭ
	deleteAll();
	m_idx = 0;
	
	// �������� ���������� ���� ����
	m_root->left = _balance( ntmp, tmp );
	// ������ ��ü���� ���󺹱�
	m_cnt = ntmp;
	
	// �迭���� 
	free( tmp );

	return m_root;
}


////////////////////////////////////////////////////////////////////
//
//  cBTree Ŭ������ getSize �Լ� 
//
//  ���� : �������� ����� ������ ��ȯ
// 

int cBTree::getSize()
{
	return (int)m_cnt;
}

//////////////////////////////////////////////////////////////////////
// cBTree�� ���� ���� ����.

cBTree_v2::cBTree_v2( size_t width, FCMP fcmp, FPNT fpnt )
{
	// �Է��ڷ�ũ��:width
	m_width = width;

	// ���Լ��� ����Լ� ����
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
     elem_ptrs.push_back(p_new);// ���� �߰� 
     return p_new;
}

void 
cBTree_v2::sort()
{
     std::sort(elem_ptrs.begin(), elem_ptrs.end(), KeyCmp(m_fcmp));
}