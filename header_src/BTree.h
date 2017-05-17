#ifndef _BTREE_H
#define	_BTREE_H

#include <vector>
#include <algorithm>

using namespace std;

#define		MAX_CNG_CNT		128*64						// 최대 변경 회수: 균형 잡기 실행 

typedef struct _node									// 링크만 있는 BTree 노드의 구조
{
	struct _node *left;		// 왼쪽   노드
	struct _node *right;	// 오른쪽 노드

}node;

typedef int  (*FCMP)(const void*, const void*);			// 멀리서 정의된 비교함수를 포인터로 가리킬 때
typedef void (*FPNT)(FILE*, void*);						// 멀리서 정의된 출력함수를 포인터로 가리킬 때

// BTree에 관련된 class
class cBTree
{
	public:
		cBTree( size_t width, FCMP fcmp, FPNT fpnt );	// 이진나무 초기화(크기,비교함수,출력함수) 
		~cBTree() { deleteAll(); }						// 이진나무 소멸 

		void	*search( void *key );					// 이진나무 탐색(노드키)
		void	*insert( void *key );					// 이진나무 노드 추가(노드키)
		void	*deleteOne( void *key );				// 이진나무 노드 삭제(노드키)
		void	*balance();								// 이진나무 균형 조정
		void	pntlist( FILE *out );					// 이진나무 정렬상태로 출력(출력파일포인터)
		int 	getSize();								// 이진나무 노드의 갯수를 반환
	
	private:
		void	*deleteAll();							// 이진나무 전체 삭제
		void	deleteNode( node *t );					// deleteAll에서 호출되는 부함수
		node	*_balance( int n, void *a );			// balance에서 호출되는 부함수
		void	sort( node *p, void *a );				// balance에서 호출되는 부함수 
		void	sortlist( node *p, FILE *out );			// pntlist에서 호출되는 부함수

	private:
		int		m_idx;									// 배열인덱스 
		node	*m_root;								// 이진트리 Root

		size_t	m_cnt;									// 노드수
		size_t	m_width;								// 입력자료크기
		
		FCMP	m_fcmp;									// 비교함수
		FPNT	m_fpnt;									// 출력함수

		short	m_change;								// 노드변경회수: 100개이상 변경시 균형잡기 
};

// 실제로는 aRule이란 것을 엘리먼트로 사용하는 컨테이너임.
class cBTree_v2
{
public:
typedef std::vector<void*> t_vptrvec;
typedef pair<t_vptrvec::iterator,t_vptrvec::iterator> t_vipair; 
     cBTree_v2( size_t width, FCMP fcmp, FPNT fpnt );	// 이진나무 초기화(크기,비교함수,출력함수) 
	~cBTree_v2();                                         // 이진나무 소멸      
     					
	void	* search( void *key );					// 이진나무 탐색(노드키)
	void	* insert( void *key );					// 이진나무 노드 추가(노드키)
     void sort();
     void	* deleteOne( void *key ){}	// 한 번도 안 불린다 // 이진나무 노드 삭제(노드키)
     void	* balance(){return NULL;}                                 // 이진나무 균형 조정
     void	pntlist( FILE *out ){}					// 이진나무 정렬상태로 출력(출력파일포인터)
     int 	getSize(){ return elem_ptrs.size();}		// 이진나무 노드의 갯수를 반환
     
private:    
     t_vptrvec  elem_ptrs;                    // void*의 벡터. 나중에 이것을 써서 탐색하게 된다.     

	size_t	m_width;								// 입력자료크기
		
	FCMP	m_fcmp;									// 비교함수
     FPNT	m_fpnt;									// 출력함수

};




#endif


