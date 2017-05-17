#ifndef _BTREE_H
#define	_BTREE_H

#include <vector>
#include <algorithm>

using namespace std;

#define		MAX_CNG_CNT		128*64						// �ִ� ���� ȸ��: ���� ��� ���� 

typedef struct _node									// ��ũ�� �ִ� BTree ����� ����
{
	struct _node *left;		// ����   ���
	struct _node *right;	// ������ ���

}node;

typedef int  (*FCMP)(const void*, const void*);			// �ָ��� ���ǵ� ���Լ��� �����ͷ� ����ų ��
typedef void (*FPNT)(FILE*, void*);						// �ָ��� ���ǵ� ����Լ��� �����ͷ� ����ų ��

// BTree�� ���õ� class
class cBTree
{
	public:
		cBTree( size_t width, FCMP fcmp, FPNT fpnt );	// �������� �ʱ�ȭ(ũ��,���Լ�,����Լ�) 
		~cBTree() { deleteAll(); }						// �������� �Ҹ� 

		void	*search( void *key );					// �������� Ž��(���Ű)
		void	*insert( void *key );					// �������� ��� �߰�(���Ű)
		void	*deleteOne( void *key );				// �������� ��� ����(���Ű)
		void	*balance();								// �������� ���� ����
		void	pntlist( FILE *out );					// �������� ���Ļ��·� ���(�������������)
		int 	getSize();								// �������� ����� ������ ��ȯ
	
	private:
		void	*deleteAll();							// �������� ��ü ����
		void	deleteNode( node *t );					// deleteAll���� ȣ��Ǵ� ���Լ�
		node	*_balance( int n, void *a );			// balance���� ȣ��Ǵ� ���Լ�
		void	sort( node *p, void *a );				// balance���� ȣ��Ǵ� ���Լ� 
		void	sortlist( node *p, FILE *out );			// pntlist���� ȣ��Ǵ� ���Լ�

	private:
		int		m_idx;									// �迭�ε��� 
		node	*m_root;								// ����Ʈ�� Root

		size_t	m_cnt;									// ����
		size_t	m_width;								// �Է��ڷ�ũ��
		
		FCMP	m_fcmp;									// ���Լ�
		FPNT	m_fpnt;									// ����Լ�

		short	m_change;								// ��庯��ȸ��: 100���̻� ����� ������� 
};

// �����δ� aRule�̶� ���� ������Ʈ�� ����ϴ� �����̳���.
class cBTree_v2
{
public:
typedef std::vector<void*> t_vptrvec;
typedef pair<t_vptrvec::iterator,t_vptrvec::iterator> t_vipair; 
     cBTree_v2( size_t width, FCMP fcmp, FPNT fpnt );	// �������� �ʱ�ȭ(ũ��,���Լ�,����Լ�) 
	~cBTree_v2();                                         // �������� �Ҹ�      
     					
	void	* search( void *key );					// �������� Ž��(���Ű)
	void	* insert( void *key );					// �������� ��� �߰�(���Ű)
     void sort();
     void	* deleteOne( void *key ){}	// �� ���� �� �Ҹ��� // �������� ��� ����(���Ű)
     void	* balance(){return NULL;}                                 // �������� ���� ����
     void	pntlist( FILE *out ){}					// �������� ���Ļ��·� ���(�������������)
     int 	getSize(){ return elem_ptrs.size();}		// �������� ����� ������ ��ȯ
     
private:    
     t_vptrvec  elem_ptrs;                    // void*�� ����. ���߿� �̰��� �Ἥ Ž���ϰ� �ȴ�.     

	size_t	m_width;								// �Է��ڷ�ũ��
		
	FCMP	m_fcmp;									// ���Լ�
     FPNT	m_fpnt;									// ����Լ�

};




#endif


