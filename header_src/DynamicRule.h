#ifndef _DYNAMICRULE_H
#define _DYNAMICRULE_H

#include "DataType.h"
#include "BTree.h"

#define		THRESHOLD		0.00
#define		MAX_RUL_CNT		8				// 최대개수: 규칙의 종류

#define		LF0				0				// 왼쪽  기능
#define		LC0				1				// 왼쪽  내용
#define		LS0				2				// 왼쪽  크기 
#define		RS0				3				// 오른쪽크기

#define		LF1				4				// 왼쪽  기능
#define		LC1				5				// 왼쪽  내용
#define		LS1				6				// 왼쪽  크기
#define		RS1				7				// 오른쪽크기


			
typedef	struct	_aRule						// 규칙형식
{	
	char	rule[MAX_STR_LEN];				// 규칙
	int		freq_given;						// 조건부의 발생빈도(확률)
	int		freq_rule;						// 규  칙의 빌생빈도(확률)	

} aRule;



// 규칙과 관련된 클래스
class cDynamicRule
{
	public:	
		// 규칙 클래스 생성자
		cDynamicRule( bool step, char *filename );
		// 규칙 클래스 소멸자
		~cDynamicRule();

		// 규칙 확률 계산 
		double	getProb( aPhrase parent, aPhrase left, aPhrase right, aPhrase outer_left, aPhrase outer_right );	
		// 규칙 빈도 추가
		void	tree( aPhrase *phrase, Phrases phrases );	

		// 규칙 학습 결과 출력
		void	endToLearnRule();
		// 보정 변수 결과 출력
		void	endToLearnLambda();
		// 규칙 크기 반환
		int		getSize();

		// 규칙 비교
		static int cmp( const void *a, const void *b );
		// 규칙 출력
		static void	pnt( FILE *out, void *a );

	private:
		// 규칙 읽기
		void	readRule( char *filename, cBTree_v2 *rule[MAX_RUL_CNT] );
		// 규칙 쓰기
		void	writeRule(char *filename, cBTree_v2 *rule[MAX_RUL_CNT] );
		// 규칙 설정 
		void	findRule( aRule *rule[MAX_RUL_CNT], aPhrase parent, aPhrase left, aPhrase right, aPhrase outer_left, aPhrase outer_right, bool given );

	public:	
		double	m_lmd[MAX_LMD_CNT];			// lambda 값

	private:
		bool	m_learn;					// 처리순서
		char	*m_filename;				// 규칙파일이름 
		//cBTree	*m_rule[MAX_RUL_CNT];		// 규칙 	
          cBTree_v2	*m_rule[MAX_RUL_CNT];		// 규칙 	
};

#endif

