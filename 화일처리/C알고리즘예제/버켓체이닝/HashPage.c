# include "HashPage.h"
# include "BufferManager.h"
#include <string.h>
HashHeader *hashHeader;
extern BufferManager *bufferManager;
void initHashHeader(int maxAddress) {
/* �ؽ� ����� �ʱ�ȭ*/
hashHeader= malloc(sizeof(HashHeader));
hashHeader -> maxAddress= maxAddress;
}
void initHashPage(HashPage * page, int pageNo) {
	/* HashPage ����ü�� �ʱ�ȭ */
page -> pageNo= pageNo;
page -> numOfRecords= 0;
page -> prevPageNo= -1;
page -> nextPageNo= -1;
page-> recArray= NULL;
}
void loadHashHeaderPage() {
/* ��� �������� ������ ��ũ���� �о� ���� */
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
/* ��� �������� ������ ��ũ�� ���� */
BYTE * header= (BYTE *)malloc(bufferManager -> pageSize);
*((int*)header)= bufferManager -> pageSize;
*((int*)header + 1)= bufferManager -> maxPageNo;
*((int*)header + 2)= bufferManager -> lastFreePageNo;
*((int*)header + 3)= hashHeader -> maxAddress;
saveHeaderPage(header);
free(header);
}
BOOL readHashPage(int pageNo, HashPage * page) {
/* (pageNo+1)��° �������� page�� �о� ����.
���� : �ؽ� �������� 1������ ���� (0���� ��� ������) */
BYTE * buffer= (BYTE *)malloc(bufferManager -> pageSize);
readPage(pageNo + 1, buffer); /* (pageNo+1)��° �������� buffer�� ��� (+)�� ����������� �ǳʶٱ� ����)*/
bufferToHashPage(buffer, page); /* buffer�� ������ page�� �о� ���� */
free(buffer);
return TRUE;
}
/* page�� ������ (pageNo+1)��° �������� ��
���� : �ؽ� �������� 1������ ���� (0���� ��� ������) */

BOOL writeHashPage(int pageNo, HashPage * page) {
BOOL ret;
BYTE * buffer= (BYTE *)malloc(bufferManager -> pageSize);
hashPageToBuffer(page, buffer); /* page�� ������ buffer�� ��� */
ret= writePage(pageNo + 1, buffer);
/* buffer�� ������ (pageNo+1)��° �������� �� */
free(buffer);
return ret;
}
int newHashPage() {
/* ��� �ִ� ���ο� �ؽ� �������� ��ȣ�� ��ȯ */
/* BufferManager�� newPage() �Լ� ȣ�� */
return newPage() - 1;
}
void freeHashPage(int victimHashPageNo) {
/* �ؽ� �������� ��ȯ�� */
HashPage * currentPage, * prevPage, * nextPage;
currentPage= malloc(sizeof(HashPage));
readHashPage(victimHashPageNo, currentPage);
/* ���� �������� ������, �� �������� ���� ������ ������ nextPageNo�� ���� ��
*/
if (currentPage -> prevPageNo != -1) {
prevPage= malloc(sizeof(HashPage));
readHashPage(currentPage -> prevPageNo, prevPage);
prevPage -> nextPageNo= currentPage -> nextPageNo;
writeHashPage(currentPage -> prevPageNo, prevPage);
if (prevPage -> recArray != NULL) free(prevPage -> recArray);
free(prevPage);
}
/* ���� �������� ������, �� �������� ���� ������ ������
prevPageNo�� ���� �� */
if (currentPage -> nextPageNo != -1) {
nextPage= malloc(sizeof(HashPage));
readHashPage(currentPage -> nextPageNo, nextPage);
nextPage -> prevPageNo= currentPage -> prevPageNo;
writeHashPage(currentPage -> nextPageNo, nextPage);
if (nextPage -> recArray != NULL) free(nextPage -> recArray);
free(nextPage);
}
/* �޸� ���� */
if (currentPage -> recArray != NULL) free(currentPage -> recArray);
free(currentPage);
freePage(victimHashPageNo + 1); /* BufferManager�� freePage() �Լ� ȣ�� */
}
/* buffer�� ������ �о� HashPage ����ü�� ä�� */
void bufferToHashPage(BYTE * buffer, HashPage * page) {
int index;
/* buffer���� �� �������� pageNo �� ���� */
memcpy(& (page -> pageNo), buffer, sizeof(int));
buffer += sizeof(int);
/* buffer���� �� �������� numOfRecords �� ���� */
memcpy(& (page -> numOfRecords), buffer, sizeof(int));
buffer += sizeof(int);
/* buffer���� �� �������� prevPageNo ���� */
memcpy(& (page -> prevPageNo), buffer, sizeof(int));
buffer += sizeof(int);
/* buffer���� �� �������� nextPageNo ���� */
memcpy(& (page -> nextPageNo), buffer, sizeof(int));
buffer += sizeof(int);
/* ���ڵ� �迭�� ���� �޸� �Ҵ� */
if (page -> recArray != NULL) free(page -> recArray);
page -> recArray=(HashRecord *)malloc((page -> numOfRecords) * sizeof(HashRecord));
/* buffer���� �� �������� ���ڵ� �迭�� �о���� */
for (index= 0; index < page -> numOfRecords; index++) {
memcpy(& (page -> recArray[index]), buffer, sizeof(HashRecord));
buffer += sizeof(HashRecord);
}
}
void hashPageToBuffer(HashPage * page, BYTE * buffer) {
/* HashPage�� ������ buffer�� �� */
int index, offset;
/* �������� pageNo ���� buffer�� �� */
memcpy(buffer, & (page -> pageNo), sizeof(int));
buffer += sizeof(int);
/* �������� numOfRecords ���� buffer�� �� */
memcpy(buffer, & (page -> numOfRecords), sizeof(int));
buffer += sizeof(int);
/* �������� prevPageNo ���� buffer�� �� */
memcpy(buffer, & (page -> prevPageNo), sizeof(int));
buffer += sizeof(int);
/* �������� nextPageNo ���� buffer�� �� */
memcpy(buffer, & (page -> nextPageNo), sizeof(int));
buffer += sizeof(int);
/* �������� ���ڵ� �迭�� buffer�� ���ʷ� �� */
for (index= 0; index < page -> numOfRecords; index++) {
offset= index * sizeof(HashRecord);
memcpy(buffer + offset, & (page -> recArray[index]), sizeof(HashRecord));
}
}
BOOL addRecord(HashPage * page, Key key, Value val) {
/* HashPage ����ü�� (key, value)�� ���� ���ڵ� �߰� */
int ret;
HashRecord * freeAddress;
HashRecord newRecord;
/* ���� �߰��� newRecord�� ������ ä�� */
newRecord.key= key;
strcpy(newRecord.value, val);
/* ���߿� �����ؾ� �� �޸� ���� */
freeAddress= page -> recArray;
/* newRecord�� �迭�� ���� �߰� */
ret= addRecordToArray(&(page->recArray),page->numOfRecords, newRecord);
if (ret > 0) {
page -> numOfRecords= ret;
if (freeAddress != NULL) free(freeAddress);
return TRUE;
} else
return FALSE;
}
int addRecordToArray(HashRecord**arrayAddress, int num, HashRecord newRecord)
{ /* arrayAddress�� ����Ű�� ���ڵ� �迭�� newRecord�� �߰��� */
int index;
HashRecord * oldArray, * newArray;
oldArray= * arrayAddress;
/* ���ο� ���ڵ� �迭 newArray�� ���� �޸� �Ҵ� */
newArray= (HashRecord *)malloc((num + 1) * sizeof(HashRecord));
/* oldArray�� ���ڵ���� newArray�� ���� */
for (index= 0; index < num; index++) {
newArray[index].key= oldArray[index].key;
strcpy(newArray[index].value, oldArray[index].value);
}
/* ���ο� ���ڵ带 newArray�� ���� �߰� */
newArray[index].key= newRecord.key;
strcpy(newArray[index].value, newRecord.value);
/* ���ڵ� �迭�� �ּҸ� newArray�� �ٲ� */
* arrayAddress= newArray;
return ++index;
}
BOOL removeRecord(HashPage * page, Key key) {
/* HashPage ����ü���� key�� ���� ���ڵ� ���� */
int index, ret;
HashRecord * freeAddress;
/* ���߿� �����ؾ� �� �޸� ���� */
freeAddress= page -> recArray;
/* ������ ���ڵ��� �ε����� ���� */
index= getRecordIndex(page -> recArray, page -> numOfRecords, key);
/* key�� ���� ���ڵ带 �迭���� ���� */
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
/* arrayAddress�� ����Ű�� ���ڵ� �迭���� i��° ���ڵ带 ������ */
int index;
HashRecord * oldArray, * newArray;
oldArray= * arrayAddress;
/* ���ο� ���ڵ� �迭 newArray�� ���� �޸� �Ҵ� */
newArray= (HashRecord *)malloc(num * sizeof(HashRecord));
/* i�� ������ ���ڵ���� newArray�� ���� */
for (index= 0; index < i; index++) {
newArray[index].key= oldArray[index].key;
strcpy(newArray[index].value, oldArray[index].value);
}
/* i�� ������ ���ڵ���� newArray�� ���� */
for (index= i + 1; index < num; index++) {
newArray[index - 1].key= oldArray[index].key;
strcpy(newArray[index - 1].value, oldArray[index].value);
}
/* ������ ���ڵ��� �ڸ��� �ʱ�ȭ */
memset(& (newArray[num - 1]), '\0', sizeof(HashRecord));
/* ���ڵ� �迭�� �ּҸ� newArray�� �ٲ� */
*arrayAddress= newArray;
return --index;
}
/* HashPage ����ü���� key�� ���� ���ڵ� �˻� */
HashRecord * searchRecord(HashPage * page, Key key) {
int index= getRecordIndex(page -> recArray, page -> numOfRecords, key);
if (index >= 0) return & (page -> recArray[index]);
else return NULL;
}
int getRecordIndex(HashRecord * recordArray, int num, Key key) {
/* ���ڵ� �迭���� key�� ���� ���ڵ��� �ε����� ��ȯ.
��, ���ڵ尡 ������ -1�� ��ȯ */
int index;
HashRecord record;
for (index= 0; index < num; index++) {
record= recordArray[index];
if (key == record.key) return index;
}
return -1;
}
BOOL isFull(HashPage * targetPage) {
/* targetPage�� �� á���� �˻� */
int maxNum, freeSpace;
freeSpace= bufferManager -> pageSize - 4 * sizeof(int);
maxNum= freeSpace / sizeof(HashRecord);
if (maxNum > targetPage -> numOfRecords) return FALSE;
else return TRUE;
}
