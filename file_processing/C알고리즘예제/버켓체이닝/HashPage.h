#ifndef __HASH_PAGE_
#define __HASH_PAGE_
#include "BaseHeader.h"
typedef int Key;
typedef char Value[12];
typedef struct hashPage HashPage;
typedef struct hashRecord HashRecord;
typedef struct hashHeader HashHeader;
struct hashRecord {
Key key;
Value value;
};

struct hashPage {
int pageNo; /* 이 페이지의 번호 */
int numOfRecords; /* 이 페이지에 들어 있는 레코드 수 */
int prevPageNo; /* 이전 페이지 번호 */
int nextPageNo; /* 다음 오버플로 페이지 번호 */
HashRecord *recArray; /* 레코드에 대한 배열 */
};
struct hashHeader {
int maxAddress;
};
void initHashHeader(int maxAddress);
/* 해시 헤더를 초기화*/
void initHashPage(HashPage * page, int pageNo);
/* HashPage 구조체를 초기화 */
void loadHashHeaderPage();
/* 헤더 페이지의 내용을 디스크에서 읽어 들임 */
void saveHashHeaderPage();
/* 헤더 페이지의 내용을 디스크에 저장 */
BOOL readHashPage(int pageNo, HashPage * page);
/* (pageNo+1)번째 페이지를 page에 읽어 들임.
참고 : 해시 페이지는 1번부터 시작 (0번은 헤더 페이지) */
BOOL writeHashPage(int pageNo, HashPage * page);
/* page의 내용을 (pageNo+1)번째 페이지에 씀.
참고 : 해시 페이지는 1번부터 시작 (0번은 헤더 페이지) */
int newHashPage();
/* 비어 있는 새로운 해시 페이지의 번호를 반환함 */
void freeHashPage(int victimHashPageNo);
/* 해시 페이지를 반환함 */
void bufferToHashPage(BYTE * buffer, HashPage * page);
/* buffer의 내용을 읽어 HashPage 구조체에 채움 */
void hashPageToBuffer(HashPage * page, BYTE * buffer);
/* HashPage의 내용을 buffer에 씀 */
BOOL addRecord(HashPage * page, Key key, Value val);
/* HashPage 구조체에 (key, value)를 갖는 레코드 추가 */
int addRecordToArray( HashRecord **arrayAddress,
int num, HashRecord newRecord);
/* arrayAddress가 가리키는 레코드 배열에 newRecord를 추가함 */
BOOL removeRecord(HashPage * page, Key key);
/* HashPage 구조체에서 key를 갖는 레코드 삭제 */
int removeRecordFromArray(HashRecord * * arrayAddress, int num, int i);
/* arrayAddress가 가리키는 레코드 배열에서 i번째 레코드를 삭제함 */
HashRecord * searchRecord(HashPage * page, Key key);
/* HashPage 구조체에서 key를 갖는 레코드 검색 */
int getRecordIndex(HashRecord * recordArray, int num, Key key);
/* 레코드 배열에서 key를 갖는 레코드의 인덱스를 반환
* 단, 레코드가 없으면 -1을 반환 */
BOOL isFull(HashPage * targetPage);
/* targetPage가 꽉 찼는지 검사 */
void Show_Header();
/* 헤더페이지 보기*/
void Show_Page();
/*페이지 내용보기*/
void Show_Page_All();
/*모든 페이지 내용보기*/
# endif
