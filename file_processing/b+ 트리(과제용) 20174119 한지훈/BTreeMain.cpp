// BTreeMain.c
/* ��� ����
i : (Ű,��)�� ������ ���ڵ� ����
d : Ű�� ������ ���ڵ� ����
r : Ű�� ������ ���ڵ� �˻�
s : ����Ž��
? : �����������
a : ��� ������ ����(1����������)
*/
#include "BTree.h"
#define PAGE_SIZE 64
int main() {
	FILE *fp, *fp_backup;
	BTreeRecord record;
	char command;
	BOOL file_not_Exist;
	fp = fopen("data.txt", "r");   //Ʈ������ ȭ�� �����ϴ��� üũ
	if (fp == NULL) {
		file_not_Exist = TRUE;     //������ ��
		fp = fopen("data.txt", "w"); //ȭ�� ���� ����
	}
	else
		file_not_Exist =FALSE;
	fclose(fp);
	 	initBTree("data.txt", PAGE_SIZE, file_not_Exist); //���� ��������� �ʱ�ȭ. ����������� ��Ʈ������ ����
	//fp_backup = fopen("backup.txt", "w");
	while (1) {
		fscanf(stdin, "%c", &command);
		if (command == 'x' || command == 'X') break;
		switch (command) {
		case 'i':    //(Ű,��)�� ������ ���ڵ� ����
			fscanf(stdin, "%i %s", &record.key, &record.value);
			if (insertRecord(&record)) {
				printf("insert (%d, %s) : success\n", record.key, record.value);
			}
			else {
				printf("Insert (%d, %s) : fail\n", record.key, record.value);
			}
			break;
		case 'd' :     //Ű�� ������ ���ڵ� ����
			fscanf(stdin, "%i", &record.key);
			if (deleteRecord(record.key)) {
				printf("Delete (%d) : success\n", record.key, record.value);
			} else {
				printf("Delete (%d) : fail\n", record.key);
			}
			break;
		case 'r':    //Ű�� ������ ���ڵ� �˻�
			fscanf(stdin, "%i", &record.key);
			if (retrieveRecord(record.key, & record)) {
				printf(
					"Retrive (%d, %s) : success\n",
					record.key,
					record.value);
			} else {
				printf("Retrive (%d) : fail\n", record.key);
			}
			break;
		case 'a':    //��������� �˻�
			retrieveAllPages();
			break;
		case 'h':    //������� ����
			Get_Header();
			break;
		case 's':    //�����˻�
			
			Sequential_Search();
			break;
		case 'o':    //���ȭ�� ��������
			closeBTree();
			fp_backup=fopen("backup.txt", "r");
			if (fp_backup == NULL) {
				file_not_Exist = TRUE;
				printf("��������� �����ϴ�\n");
				fp = fopen("data.txt", "r");   //Ʈ������ ȭ�� �����ϴ��� üũ
				fclose(fp);
				initBTree("data.txt", PAGE_SIZE, FALSE);
				break;
			}
			else

			fp = fopen("data.txt", "w");
			initBTree("data.txt", PAGE_SIZE, TRUE);
			printf("������� �������� ����\n");
			fclose(fp);
			int ret;
			while (1) {

				ret = fscanf(fp_backup, "%d %s", &record.key, &record.value);
				if (ret == EOF) //fscanf �Լ��� ���ϳ��� �����ϰų�,������ �߻��� EOF�� ��ȯ�Ѵ�                  // 15��
					break;
				insertRecord(&record);
			}
			fclose(fp_backup);
			break;
		case 'b':    //�����ý�Ʈȭ��(backup.txt)�� ����ϱ�
			Sequential_Backup();
			break;
		case 'n':    //������ȭ�� �ʱ�ȭ
			closeBTree();
			char strPath[] = { "data.txt" };
			int nResult=remove(strPath);
			if (nResult == 0)
			{
				printf("������ ���� �ʱ�ȭ ����\n");
			}
			else if (nResult == -1)
			{
				perror("������ ���� �ʱ�ȭ ����\n");
			}
			fclose(fp);
			fp = fopen("data.txt", "r");   //Ʈ������ ȭ�� �����ϴ��� üũ
			
			file_not_Exist = TRUE;     //������ ��
			fp = fopen("data.txt", "w"); //ȭ�� ���� ����
				
			
			
			
			fclose(fp);
			initBTree("data.txt", PAGE_SIZE, file_not_Exist);
			fp = fopen("data.txt", "r");
			break;

		}
	}
	closeBTree();
	return 0;
}