#ifndef __HASH_
#define __HASH_
#define PAGE_SIZE 64
#define MAX_ADDRESS 10

#include "BaseHeader.h"
#include "HashPage.h"
void initHash(int maxAddress, char* fileName, int pageSize, BOOL newStart);
/* 해시를 위한 초기화 함수 */
void closeHash();
/*해시를 종료*/
int hash(Key key);
/* 해시 함수 */
BOOL Add(Key key, Value value);
/* (key, value)를 갖는 레코드 추가 */
BOOL Remove(Key key);
/* key를 갖는 레코드 삭제 */
BOOL Search(Key key, HashRecord* record);
/* key를 갖는 레코드 검색 */
void Show_Header();
#endif
