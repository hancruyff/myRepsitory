// BTreeMain.c
/* 명령 유형
i : (키,값)을 가지는 레코드 삽입
d : 키를 가지는 레코드 삭제
r : 키를 가지는 레코드 검색
s : 순차탐색
? : 헤더정보보기
a : 모든 페이지 보기(1페이지부터)
*/
#include "BTree.h"
#define PAGE_SIZE 64
int main() {
	FILE *fp, *fp_backup;
	BTreeRecord record;
	char command;
	BOOL file_not_Exist;
	fp = fopen("data.txt", "r");   //트리저장 화일 존재하는지 체크
	if (fp == NULL) {
		file_not_Exist = TRUE;     //없으면 참
		fp = fopen("data.txt", "w"); //화일 새로 생성
	}
	else
		file_not_Exist =FALSE;
	fclose(fp);
	 	initBTree("data.txt", PAGE_SIZE, file_not_Exist); //새로 만들었으면 초기화. 헤더페이지와 루트페이지 생성

	while (1) {
		fscanf(stdin, "%c", &command);
		if (command == 'x' || command == 'X') break;
		switch (command) {
		case 'i':    //(키,값)을 가지는 레코드 삽입
			fscanf(stdin, "%i %s", &record.key, &record.value);
			if (insertRecord(&record)) {
				printf("insert (%d, %s) : success\n", record.key, record.value);
			}
			else {
				printf("Insert (%d, %s) : fail\n", record.key, record.value);
			}
			break;
		case 'd' :     //키를 가지는 레코드 삭제
			fscanf(stdin, "%i", &record.key);
			if (deleteRecord(record.key)) {
				printf("Delete (%d) : success\n", record.key, record.value);
			} else {
				printf("Delete (%d) : fail\n", record.key);
			}
			break;
		case 'r':    //키를 가지는 레코드 검색
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
		case 'a':    //모든페이지 검색
			retrieveAllPages();
			break;
		case 'h':    //헤더정보 보기
			Get_Header();
			break;
		case 's':    //순차검색
			break;

		case 'o':    //백업화일 가져오기

			break;
		case 'b':    //순차택스트화일(backup.txt)로 백업하기

			break;
		case 'n':    //데이터화일 초기화

			break;

		}
	}
	closeBTree();
	return 0;
}