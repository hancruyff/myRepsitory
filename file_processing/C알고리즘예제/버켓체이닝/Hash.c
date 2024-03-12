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
/* maxAddress �� ��ŭ�� ������ �ʱ�ȭ */
for (index= 0; index < maxAddress; index++) {
initHashPage(page, index);
writeHashPage(index, page);
}
bufferManager -> maxPageNo= maxAddress;
} else {
initBufferManager(fileName, PAGE_SIZE);
loadHashHeaderPage();
}
free(page); /* �޸� ���� */
}
void closeHash() {
/*�ؽø� ����*/
saveHashHeaderPage();
fclose(bufferManager -> fp);
free(hashHeader);
free(bufferManager);
}
int hash(Key key) {
/* �ؽ� �Լ�(�����ܿ��ؽ� ���� :  maxAddress */
return key % hashHeader -> maxAddress;
}
BOOL Add(Key key, Value value) {
/* (key, value)�� ���� ���ڵ� �߰� */
BOOL ret;
int targetPageNo, newPageNo, currentPageNo;
HashPage * page;
HashRecord record;
/* ������ ������ �ʱ�ȭ */
page= (HashPage *)malloc(sizeof(HashPage));
initHashPage(page, -1);
targetPageNo= hash(key); /* �ؽ� �Լ��� ���� ���� �ּ� ���� */
/* �̹� ������ Ű�� ���ڵ尡 �����ϸ� ���� ��� */
if (Search(key, & record) == TRUE) {
return FALSE;
} else {
/* ���� ������ �ִ� �������� ã�� ������ Ž�� */
while (TRUE) {
readHashPage(targetPageNo, page);
/* ���� ������ �ִ� �������� ã����, ������ �������� */
if (isFull(page) == FALSE)
break;
/* page�� ������ ������ ���� ���������� Ž�� ��� */
else {
targetPageNo= page -> nextPageNo;
/* ���� �������� ������ (�� �ּ� ���� ���� ��� ��������
�� á����) �� �������� ���� ����, ������ �������� */
if (targetPageNo <= 0) {
/* ���ο� ������ ��ȣ newPageNo�� ���� */
currentPageNo= page -> pageNo;
newPageNo= newHashPage();
/*���� �������� ���� �������� newPageNo�� ������ ���� ���*/
page -> nextPageNo= newPageNo;
writeHashPage(page -> pageNo, page);
/* �� �������� �ʱ�ȭ */
if (page -> recArray != NULL) free(page -> recArray);
initHashPage(page, newPageNo);
page -> prevPageNo= currentPageNo;
break;
} else /* ���� �������� ������ Ž�� ��� */
continue;
}
}
}
/* ���ڵ尡 HashPage ����ü�� ����� �߰��Ǿ�����, ������ ��ũ�� ��� */
if (addRecord(page, key, value)) ret= writeHashPage(page -> pageNo, page);
if (page -> recArray != NULL) free(page -> recArray);
free(page);
return ret;
}
BOOL Remove(Key key) {
/* key�� ���� ���ڵ� ���� */
BOOL ret;
int targetPageNo;
HashRecord * target;
HashPage * page;
/* ������ ������ �ʱ�ȭ */
page= malloc(sizeof(HashPage));
initHashPage(page, -1);
/* �ؽ� �Լ��� ���� ���� �ּ� ���� */
targetPageNo= hash(key);
/* ������ ���ڵ尡 ���� ������ ������ Ž�� */
while (TRUE) {
readHashPage(targetPageNo, page);
target= searchRecord(page, key);
/* ���ڵ带 ã������ ������ �������� */
if (target != NULL) break;
/* ���� �������� ã�� ���ڵ尡 ������ ���� ���������� Ž�� ��� */
else {
targetPageNo= page -> nextPageNo;
/* ���� �������� ������, ���ڵ带 �� ã�� ä�� ���� */
if (targetPageNo == -1) return FALSE;
}
}
/* ���ڵ尡 HashPage ����ü���� ����� �����Ǿ�����, ������ ��ũ�� ��� */
if (removeRecord(page, key)) {
ret= writeHashPage(page -> pageNo, page);
/* �� �������� �����÷� �������̸鼭 ���ڵ尡 ���� ���,
���� ������ ����Ʈ�� ��ȯ */
if (page -> numOfRecords == 0
&& page -> pageNo >= hashHeader -> maxAddress)
freeHashPage(page -> pageNo);
}
if (page -> recArray != NULL) free(page -> recArray);
free(page);
return ret;
}
BOOL Search(Key key, HashRecord * record) {
/* key�� ���� ���ڵ� �˻� */
int targetPageNo; HashPage * page; HashRecord * target= NULL;
int i= 0; BOOL ret;
/* ������ ������ �ʱ�ȭ */
page= malloc(sizeof(HashPage));
initHashPage(page, -1);
/* �ؽ� �Լ��� ���� ���� �ּ� ���� */
targetPageNo= hash(key);
/* ���ڵ尡 �߰ߵ� ������ ������ Ž�� */
while (TRUE) {
readHashPage(targetPageNo, page);
target= searchRecord(page, key);
/* ���ڵ带 ã������ ��ȯ */
if (target != NULL) break;
/* ���� �������� ã�� ���ڵ尡 ������ ���� ���������� Ž�� ��� */
targetPageNo= page -> nextPageNo;
/* ���� �������� ������ Ž�� ���� */
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
	printf("��ȣ");
	printf("\t����");
	printf("\t����");
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
		printf("���������\n");
		printf("-----------------\n");
		Show_Header();
		page = malloc(sizeof(HashPage));
		printf("-----------------\n");
		initHashPage(page, -1);
			printf("��ȣ");
			printf("\t����");
			printf("\t����");
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
