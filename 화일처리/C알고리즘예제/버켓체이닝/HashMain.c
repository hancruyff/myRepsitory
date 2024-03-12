/* ��� ����
i : (Ű, ��)�� ������ ���ڵ� ����
d : Ű�� ������ ���ڵ� ����
r : Ű�� ������ ���ڵ� �˻�
p : ������ ���뺸��
a : ��� ������ ����
*/
#include "BaseHeader.h"
#include "BufferManager.h"
#include "HashPage.h"
#include "Hash.h"

int main() {
FILE * fp;
HashRecord record;
HashRecord result;
char command;
int page_no;
BOOL file_not_Exist;
fp = fopen("hash.txt", "r");   //Ʈ������ ȭ�� �����ϴ��� üũ
if (fp == NULL) {
	file_not_Exist = TRUE;     //������ ��
	fp = fopen("hash.txt", "w"); //ȭ�� ���� ����
}
else
file_not_Exist = FALSE;
fclose(fp);
initHash(MAX_ADDRESS, "hash.txt", PAGE_SIZE, file_not_Exist);

while (1) {
	fscanf(stdin, "%c", &command);
	if (command == 'x' || command == 'X') break;
	switch (command) {
	case 'i':    //(Ű,��)�� ������ ���ڵ� ����
		fscanf(stdin, "%i %s", &record.key, &record.value);
		if (Add(record.key, record.value)) {
			printf("insert (%d, %s) : success\n", record.key, record.value);
		}
		else {
			printf("Insert (%d, %s) : fail\n", record.key, record.value);
		}
		break;
	case 'd':     //Ű�� ������ ���ڵ� ����
		fscanf(stdin, "%i", &record.key);
			if (Remove(record.key)) {
				printf("Delete (%d) : success\n", record.key);
			}
			else {
				printf("Delete (%d) : fail\n", record.key);
			}
		break;
	case 'r':    //Ű�� ������ ���ڵ� �˻�
		fscanf(stdin, "%i", &record.key);
			if (Search(record.key, &result)) {
				printf(
					"Retrive (%d, %s) : success\n",	record.key,	result.value);
			}
			else {
				printf("Retrive (%d) : fail\n", record.key);
			}
		break;
	case 'p':    //������ ����
		fscanf(stdin, "%i", &page_no);
		Show_Page(page_no);
		break;
	case 'a':    //��������� ����
		Show_Page_All();
		break;
	case 'h':    //��������� ����
		Show_Header();
		break;
	}
}
	closeHash();
    return 0;
}