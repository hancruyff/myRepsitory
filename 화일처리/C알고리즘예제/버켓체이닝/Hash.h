#ifndef __HASH_
#define __HASH_
#define PAGE_SIZE 64
#define MAX_ADDRESS 10

#include "BaseHeader.h"
#include "HashPage.h"
void initHash(int maxAddress, char* fileName, int pageSize, BOOL newStart);
/* �ؽø� ���� �ʱ�ȭ �Լ� */
void closeHash();
/*�ؽø� ����*/
int hash(Key key);
/* �ؽ� �Լ� */
BOOL Add(Key key, Value value);
/* (key, value)�� ���� ���ڵ� �߰� */
BOOL Remove(Key key);
/* key�� ���� ���ڵ� ���� */
BOOL Search(Key key, HashRecord* record);
/* key�� ���� ���ڵ� �˻� */
void Show_Header();
#endif
