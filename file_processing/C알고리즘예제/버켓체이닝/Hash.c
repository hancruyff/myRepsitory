# include "Hash.h"
# include "BufferManager.h"
# include "HashPage.h"
extern BufferManager *bufferManager;
extern HashHeader * hashHeader;
void initHash(int maxAddress, char * fileName, int pageSize, BOOL newStart) {
int index;
HashPage * page= malloc(sizeof(HashPage));
if (newStart) {
initBufferManager(fileName, pageSize);
initHashHeader(maxAddress);
/* maxAddress 수 만큼의 페이지 초기화 */
for (index= 0; index < maxAddress; index++) {
initHashPage(page, index);
writeHashPage(index, page);
}
bufferManager -> maxPageNo= maxAddress;
} else {
initBufferManager(fileName, PAGE_SIZE);
loadHashHeaderPage();
}
free(page); /* 메모리 해제 */
}
void closeHash() {
/*해시를 종료*/
saveHashHeaderPage();
fclose(bufferManager -> fp);
free(hashHeader);
free(bufferManager);
}
int hash(Key key) {
/* 해시 함수(제산잔여해시 제수 :  maxAddress */
return key % hashHeader -> maxAddress;
}
BOOL Add(Key key, Value value) {
/* (key, value)를 갖는 레코드 추가 */
BOOL ret;
int targetPageNo, newPageNo, currentPageNo;
HashPage * page;
HashRecord record;
/* 임의의 값으로 초기화 */
page= (HashPage *)malloc(sizeof(HashPage));
initHashPage(page, -1);
targetPageNo= hash(key); /* 해시 함수를 통해 시작 주소 얻음 */
/* 이미 동일한 키의 레코드가 존재하면 에러 출력 */
if (Search(key, & record) == TRUE) {
return FALSE;
} else {
/* 여유 공간이 있는 페이지를 찾을 때까지 탐색 */
while (TRUE) {
readHashPage(targetPageNo, page);
/* 여유 공간이 있는 페이지를 찾으면, 루프를 빠져나감 */
if (isFull(page) == FALSE)
break;
/* page에 공간이 없으면 다음 페이지에서 탐색 계속 */
else {
targetPageNo= page -> nextPageNo;
/* 다음 페이지가 없으면 (그 주소 값에 대한 모든 페이지가
꽉 찼으면) 새 페이지를 얻은 다음, 루프를 빠져나감 */
if (targetPageNo <= 0) {
/* 새로운 페이지 번호 newPageNo를 얻음 */
currentPageNo= page -> pageNo;
newPageNo= newHashPage();
/*현재 페이지의 다음 페이지를 newPageNo로 설정한 다음 기록*/
page -> nextPageNo= newPageNo;
writeHashPage(page -> pageNo, page);
/* 새 페이지를 초기화 */
if (page -> recArray != NULL) free(page -> recArray);
initHashPage(page, newPageNo);
page -> prevPageNo= currentPageNo;
break;
} else /* 다음 페이지가 있으면 탐색 계속 */
continue;
}
}
}
/* 레코드가 HashPage 구조체에 제대로 추가되었으면, 실제로 디스크에 기록 */
if (addRecord(page, key, value)) ret= writeHashPage(page -> pageNo, page);
if (page -> recArray != NULL) free(page -> recArray);
free(page);
return ret;
}
BOOL Remove(Key key) {
/* key를 갖는 레코드 삭제 */
BOOL ret;
int targetPageNo;
HashRecord * target;
HashPage * page;
/* 임의의 값으로 초기화 */
page= malloc(sizeof(HashPage));
initHashPage(page, -1);
/* 해시 함수를 통해 시작 주소 얻음 */
targetPageNo= hash(key);
/* 삭제할 레코드가 있을 때까지 페이지 탐색 */
while (TRUE) {
readHashPage(targetPageNo, page);
target= searchRecord(page, key);
/* 레코드를 찾았으면 루프를 빠져나감 */
if (target != NULL) break;
/* 현재 페이지에 찾는 레코드가 없으면 다음 페이지에서 탐색 계속 */
else {
targetPageNo= page -> nextPageNo;
/* 다음 페이지가 없으면, 레코드를 못 찾은 채로 끝남 */
if (targetPageNo == -1) return FALSE;
}
}
/* 레코드가 HashPage 구조체에서 제대로 삭제되었으면, 실제로 디스크에 기록 */
if (removeRecord(page, key)) {
ret= writeHashPage(page -> pageNo, page);
/* 이 페이지가 오버플로 페이지이면서 레코드가 없는 경우,
자유 페이지 리스트로 반환 */
if (page -> numOfRecords == 0
&& page -> pageNo >= hashHeader -> maxAddress)
freeHashPage(page -> pageNo);
}
if (page -> recArray != NULL) free(page -> recArray);
free(page);
return ret;
}
BOOL Search(Key key, HashRecord * record) {
/* key를 갖는 레코드 검색 */
int targetPageNo; HashPage * page; HashRecord * target= NULL;
int i= 0; BOOL ret;
/* 임의의 값으로 초기화 */
page= malloc(sizeof(HashPage));
initHashPage(page, -1);
/* 해시 함수를 통해 시작 주소 얻음 */
targetPageNo= hash(key);
/* 레코드가 발견될 때까지 페이지 탐색 */
while (TRUE) {
readHashPage(targetPageNo, page);
target= searchRecord(page, key);
/* 레코드를 찾았으면 반환 */
if (target != NULL) break;
/* 현재 페이지에 찾는 레코드가 없으면 다음 페이지에서 탐색 계속 */
targetPageNo= page -> nextPageNo;
/* 다음 페이지가 없으면 탐색 종료 */
if (targetPageNo == -1) break;
}
if (target != NULL) {
record -> key= target -> key;
for (i= 0; i < 12; i++)
record -> value[i]= target -> value[i];
ret= TRUE;
} else {
ret= FALSE;
}
if (page -> recArray != NULL) free(page -> recArray);
free(page);
return ret;
}
void Show_Header() {
	printf("pageSize %d\n", bufferManager->pageSize);
	printf("maxPage %d\n", bufferManager->maxPageNo);
	printf("lastFree %d\n", bufferManager->lastFreePageNo);
	printf("maxaddr %d\n", hashHeader->maxAddress);

}

void Show_Record(HashRecord * recordArray, int num) {
	HashRecord record;
	for (int i = 0; i < num; i++) {
		record = recordArray[i];
		printf("(%d, %s) ", record.key, record.value);
	}
	printf("\n", record.key, record.value);
}
void Show_Page(int page_no) {
	HashPage * page;
	HashRecord *recordArray;
	page = malloc(sizeof(HashPage));
	initHashPage(page, -1);
	readHashPage(page_no, page);
	printf("번호");
	printf("\t이전");
	printf("\t다음");
	printf("\t#rec\n");
	printf("-----------------------------------------\n");

	printf("%d  ", page->pageNo);
	printf("\t%d, ", page->prevPageNo);
	printf("\t%d, ", page->nextPageNo);
	printf("\t%d    ", page->numOfRecords);
	Show_Record(page->recArray, page->numOfRecords);
}

void Show_Page_All() {
		HashPage * page;
		HashRecord *recordArray;
		printf("헤더페이지\n");
		printf("-----------------\n");
		Show_Header();
		page = malloc(sizeof(HashPage));
		printf("-----------------\n");
		initHashPage(page, -1);
			printf("번호");
			printf("\t이전");
			printf("\t다음");
			printf("\t#rec\n");
			printf("-----------------------------------------\n");
			for (int i = 0; i < bufferManager->maxPageNo; i++) {
			readHashPage(i, page);
			printf("%d  ", page->pageNo);
			printf("\t%d, ", page->prevPageNo);
			printf("\t%d, ", page->nextPageNo);
			printf("\t%d   ", page->numOfRecords);
			Show_Record(page->recArray, page->numOfRecords);
		}

}
