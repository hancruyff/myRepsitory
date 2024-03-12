# include "HashPage.h"
# include "BufferManager.h"
#include <string.h>
HashHeader *hashHeader;
extern BufferManager *bufferManager;
void initHashHeader(int maxAddress) {
/* 해시 헤더를 초기화*/
hashHeader= malloc(sizeof(HashHeader));
hashHeader -> maxAddress= maxAddress;
}
void initHashPage(HashPage * page, int pageNo) {
	/* HashPage 구조체를 초기화 */
page -> pageNo= pageNo;
page -> numOfRecords= 0;
page -> prevPageNo= -1;
page -> nextPageNo= -1;
page-> recArray= NULL;
}
void loadHashHeaderPage() {
/* 헤더 페이지의 내용을 디스크에서 읽어 들임 */
BYTE * header= (BYTE *)malloc(bufferManager -> pageSize);
loadHeaderPage(header);
bufferManager -> pageSize= * ((int *)header);
bufferManager -> maxPageNo= * ((int *)header + 1);
bufferManager -> lastFreePageNo= * ((int *)header + 2);
initHashHeader(* ((int *)header + 3));
hashHeader -> maxAddress= * ((int *)header + 3);

free(header);
}
void saveHashHeaderPage() {
/* 헤더 페이지의 내용을 디스크에 저장 */
BYTE * header= (BYTE *)malloc(bufferManager -> pageSize);
*((int*)header)= bufferManager -> pageSize;
*((int*)header + 1)= bufferManager -> maxPageNo;
*((int*)header + 2)= bufferManager -> lastFreePageNo;
*((int*)header + 3)= hashHeader -> maxAddress;
saveHeaderPage(header);
free(header);
}
BOOL readHashPage(int pageNo, HashPage * page) {
/* (pageNo+1)번째 페이지를 page에 읽어 들임.
참고 : 해시 페이지는 1번부터 시작 (0번은 헤더 페이지) */
BYTE * buffer= (BYTE *)malloc(bufferManager -> pageSize);
readPage(pageNo + 1, buffer); /* (pageNo+1)번째 페이지를 buffer에 기록 (+)은 헤더페이지를 건너뛰기 위함)*/
bufferToHashPage(buffer, page); /* buffer의 내용을 page에 읽어 들임 */
free(buffer);
return TRUE;
}
/* page의 내용을 (pageNo+1)번째 페이지에 씀
참고 : 해시 페이지는 1번부터 시작 (0번은 헤더 페이지) */

BOOL writeHashPage(int pageNo, HashPage * page) {
BOOL ret;
BYTE * buffer= (BYTE *)malloc(bufferManager -> pageSize);
hashPageToBuffer(page, buffer); /* page의 내용을 buffer에 기록 */
ret= writePage(pageNo + 1, buffer);
/* buffer의 내용을 (pageNo+1)번째 페이지에 씀 */
free(buffer);
return ret;
}
int newHashPage() {
/* 비어 있는 새로운 해시 페이지의 번호를 반환 */
/* BufferManager의 newPage() 함수 호출 */
return newPage() - 1;
}
void freeHashPage(int victimHashPageNo) {
/* 해시 페이지를 반환함 */
HashPage * currentPage, * prevPage, * nextPage;
currentPage= malloc(sizeof(HashPage));
readHashPage(victimHashPageNo, currentPage);
/* 이전 페이지가 있으면, 그 페이지의 다음 페이지 값으로 nextPageNo를 갖게 함
*/
if (currentPage -> prevPageNo != -1) {
prevPage= malloc(sizeof(HashPage));
readHashPage(currentPage -> prevPageNo, prevPage);
prevPage -> nextPageNo= currentPage -> nextPageNo;
writeHashPage(currentPage -> prevPageNo, prevPage);
if (prevPage -> recArray != NULL) free(prevPage -> recArray);
free(prevPage);
}
/* 다음 페이지가 있으면, 그 페이지의 이전 페이지 값으로
prevPageNo를 갖게 함 */
if (currentPage -> nextPageNo != -1) {
nextPage= malloc(sizeof(HashPage));
readHashPage(currentPage -> nextPageNo, nextPage);
nextPage -> prevPageNo= currentPage -> prevPageNo;
writeHashPage(currentPage -> nextPageNo, nextPage);
if (nextPage -> recArray != NULL) free(nextPage -> recArray);
free(nextPage);
}
/* 메모리 해제 */
if (currentPage -> recArray != NULL) free(currentPage -> recArray);
free(currentPage);
freePage(victimHashPageNo + 1); /* BufferManager의 freePage() 함수 호출 */
}
/* buffer의 내용을 읽어 HashPage 구조체에 채움 */
void bufferToHashPage(BYTE * buffer, HashPage * page) {
int index;
/* buffer에서 이 페이지의 pageNo 값 읽음 */
memcpy(& (page -> pageNo), buffer, sizeof(int));
buffer += sizeof(int);
/* buffer에서 이 페이지의 numOfRecords 값 읽음 */
memcpy(& (page -> numOfRecords), buffer, sizeof(int));
buffer += sizeof(int);
/* buffer에서 이 페이지의 prevPageNo 읽음 */
memcpy(& (page -> prevPageNo), buffer, sizeof(int));
buffer += sizeof(int);
/* buffer에서 이 페이지의 nextPageNo 읽음 */
memcpy(& (page -> nextPageNo), buffer, sizeof(int));
buffer += sizeof(int);
/* 레코드 배열을 위한 메모리 할당 */
if (page -> recArray != NULL) free(page -> recArray);
page -> recArray=(HashRecord *)malloc((page -> numOfRecords) * sizeof(HashRecord));
/* buffer에서 이 페이지의 레코드 배열을 읽어들임 */
for (index= 0; index < page -> numOfRecords; index++) {
memcpy(& (page -> recArray[index]), buffer, sizeof(HashRecord));
buffer += sizeof(HashRecord);
}
}
void hashPageToBuffer(HashPage * page, BYTE * buffer) {
/* HashPage의 내용을 buffer에 씀 */
int index, offset;
/* 페이지의 pageNo 값을 buffer에 씀 */
memcpy(buffer, & (page -> pageNo), sizeof(int));
buffer += sizeof(int);
/* 페이지의 numOfRecords 값을 buffer에 씀 */
memcpy(buffer, & (page -> numOfRecords), sizeof(int));
buffer += sizeof(int);
/* 페이지의 prevPageNo 값을 buffer에 씀 */
memcpy(buffer, & (page -> prevPageNo), sizeof(int));
buffer += sizeof(int);
/* 페이지의 nextPageNo 값을 buffer에 씀 */
memcpy(buffer, & (page -> nextPageNo), sizeof(int));
buffer += sizeof(int);
/* 페이지의 레코드 배열을 buffer에 차례로 씀 */
for (index= 0; index < page -> numOfRecords; index++) {
offset= index * sizeof(HashRecord);
memcpy(buffer + offset, & (page -> recArray[index]), sizeof(HashRecord));
}
}
BOOL addRecord(HashPage * page, Key key, Value val) {
/* HashPage 구조체에 (key, value)를 갖는 레코드 추가 */
int ret;
HashRecord * freeAddress;
HashRecord newRecord;
/* 새로 추가할 newRecord의 내용을 채움 */
newRecord.key= key;
strcpy(newRecord.value, val);
/* 나중에 해제해야 할 메모리 지정 */
freeAddress= page -> recArray;
/* newRecord를 배열의 끝에 추가 */
ret= addRecordToArray(&(page->recArray),page->numOfRecords, newRecord);
if (ret > 0) {
page -> numOfRecords= ret;
if (freeAddress != NULL) free(freeAddress);
return TRUE;
} else
return FALSE;
}
int addRecordToArray(HashRecord**arrayAddress, int num, HashRecord newRecord)
{ /* arrayAddress가 가리키는 레코드 배열에 newRecord를 추가함 */
int index;
HashRecord * oldArray, * newArray;
oldArray= * arrayAddress;
/* 새로운 레코드 배열 newArray를 위한 메모리 할당 */
newArray= (HashRecord *)malloc((num + 1) * sizeof(HashRecord));
/* oldArray의 레코드들을 newArray에 복사 */
for (index= 0; index < num; index++) {
newArray[index].key= oldArray[index].key;
strcpy(newArray[index].value, oldArray[index].value);
}
/* 새로운 레코드를 newArray의 끝에 추가 */
newArray[index].key= newRecord.key;
strcpy(newArray[index].value, newRecord.value);
/* 레코드 배열의 주소를 newArray로 바꿈 */
* arrayAddress= newArray;
return ++index;
}
BOOL removeRecord(HashPage * page, Key key) {
/* HashPage 구조체에서 key를 갖는 레코드 삭제 */
int index, ret;
HashRecord * freeAddress;
/* 나중에 해제해야 할 메모리 지정 */
freeAddress= page -> recArray;
/* 삭제할 레코드의 인덱스를 얻음 */
index= getRecordIndex(page -> recArray, page -> numOfRecords, key);
/* key를 갖는 레코드를 배열에서 삭제 */
if (index >= 0) {
ret=removeRecordFromArray(
&(page->recArray),page -> numOfRecords,index);
if (ret >= 0) {
page -> numOfRecords= ret;
if (freeAddress != NULL) free(freeAddress);
return TRUE;
} else
return FALSE;
} else
return FALSE;
}
int removeRecordFromArray(HashRecord * * arrayAddress, int num, int i) {
/* arrayAddress가 가리키는 레코드 배열에서 i번째 레코드를 삭제함 */
int index;
HashRecord * oldArray, * newArray;
oldArray= * arrayAddress;
/* 새로운 레코드 배열 newArray를 위한 메모리 할당 */
newArray= (HashRecord *)malloc(num * sizeof(HashRecord));
/* i번 이전의 레코드들을 newArray에 복사 */
for (index= 0; index < i; index++) {
newArray[index].key= oldArray[index].key;
strcpy(newArray[index].value, oldArray[index].value);
}
/* i번 이후의 레코드들을 newArray에 복사 */
for (index= i + 1; index < num; index++) {
newArray[index - 1].key= oldArray[index].key;
strcpy(newArray[index - 1].value, oldArray[index].value);
}
/* 마지막 레코드의 자리는 초기화 */
memset(& (newArray[num - 1]), '\0', sizeof(HashRecord));
/* 레코드 배열의 주소를 newArray로 바꿈 */
*arrayAddress= newArray;
return --index;
}
/* HashPage 구조체에서 key를 갖는 레코드 검색 */
HashRecord * searchRecord(HashPage * page, Key key) {
int index= getRecordIndex(page -> recArray, page -> numOfRecords, key);
if (index >= 0) return & (page -> recArray[index]);
else return NULL;
}
int getRecordIndex(HashRecord * recordArray, int num, Key key) {
/* 레코드 배열에서 key를 갖는 레코드의 인덱스를 반환.
단, 레코드가 없으면 -1을 반환 */
int index;
HashRecord record;
for (index= 0; index < num; index++) {
record= recordArray[index];
if (key == record.key) return index;
}
return -1;
}
BOOL isFull(HashPage * targetPage) {
/* targetPage가 꽉 찼는지 검사 */
int maxNum, freeSpace;
freeSpace= bufferManager -> pageSize - 4 * sizeof(int);
maxNum= freeSpace / sizeof(HashRecord);
if (maxNum > targetPage -> numOfRecords) return FALSE;
else return TRUE;
}
