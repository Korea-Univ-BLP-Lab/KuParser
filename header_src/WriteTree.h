#ifndef _WRITETREE_H
#define _WRITETREE_H

#include "DataType.h"

// �����м� ����ġ ���
class cWriteTree										
{
	public:											
		// �����м� ���
		void	tree( FILE *file, Words *words, Phrases *phrases );	
		// ���¼� �м� ���
		void	POStaggedSnt( FILE *file, Words *words );	
		// �������� ���
		void	dependency( FILE *file, Words *words, Phrases *phrases );
													
	private:											
		// �߽ɾ� ��ȣ Ž�� 
		void	findHead( int head[MAX_WRD_CNT], aPhrase *phrase, aPhrase *parent, Phrases *phrases );
		// �����м��� ��� ���
		void	writePhrase( FILE *file, Words *words, aPhrase *phrase, short depth, bool blank );
};

#endif