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
int pageNo; /* �� �������� ��ȣ */
int numOfRecords; /* �� �������� ��� �ִ� ���ڵ� �� */
int prevPageNo; /* ���� ������ ��ȣ */
int nextPageNo; /* ���� �����÷� ������ ��ȣ */
HashRecord *recArray; /* ���ڵ忡 ���� �迭 */
};
struct hashHeader {
int maxAddress;
};
void initHashHeader(int maxAddress);
/* �ؽ� ����� �ʱ�ȭ*/
void initHashPage(HashPage * page, int pageNo);
/* HashPage ����ü�� �ʱ�ȭ */
void loadHashHeaderPage();
/* ��� �������� ������ ��ũ���� �о� ���� */
void saveHashHeaderPage();
/* ��� �������� ������ ��ũ�� ���� */
BOOL readHashPage(int pageNo, HashPage * page);
/* (pageNo+1)��° �������� page�� �о� ����.
���� : �ؽ� �������� 1������ ���� (0���� ��� ������) */
BOOL writeHashPage(int pageNo, HashPage * page);
/* page�� ������ (pageNo+1)��° �������� ��.
���� : �ؽ� �������� 1������ ���� (0���� ��� ������) */
int newHashPage();
/* ��� �ִ� ���ο� �ؽ� �������� ��ȣ�� ��ȯ�� */
void freeHashPage(int victimHashPageNo);
/* �ؽ� �������� ��ȯ�� */
void bufferToHashPage(BYTE * buffer, HashPage * page);
/* buffer�� ������ �о� HashPage ����ü�� ä�� */
void hashPageToBuffer(HashPage * page, BYTE * buffer);
/* HashPage�� ������ buffer�� �� */
BOOL addRecord(HashPage * page, Key key, Value val);
/* HashPage ����ü�� (key, value)�� ���� ���ڵ� �߰� */
int addRecordToArray( HashRecord **arrayAddress,
int num, HashRecord newRecord);
/* arrayAddress�� ����Ű�� ���ڵ� �迭�� newRecord�� �߰��� */
BOOL removeRecord(HashPage * page, Key key);
/* HashPage ����ü���� key�� ���� ���ڵ� ���� */
int removeRecordFromArray(HashRecord * * arrayAddress, int num, int i);
/* arrayAddress�� ����Ű�� ���ڵ� �迭���� i��° ���ڵ带 ������ */
HashRecord * searchRecord(HashPage * page, Key key);
/* HashPage ����ü���� key�� ���� ���ڵ� �˻� */
int getRecordIndex(HashRecord * recordArray, int num, Key key);
/* ���ڵ� �迭���� key�� ���� ���ڵ��� �ε����� ��ȯ
* ��, ���ڵ尡 ������ -1�� ��ȯ */
BOOL isFull(HashPage * targetPage);
/* targetPage�� �� á���� �˻� */
void Show_Header();
/* ��������� ����*/
void Show_Page();
/*������ ���뺸��*/
void Show_Page_All();
/*��� ������ ���뺸��*/
# endif
