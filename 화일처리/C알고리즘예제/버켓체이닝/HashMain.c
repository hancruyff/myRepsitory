/* 명령 유형
i : (키, 값)을 가지는 레코드 삽입
d : 키를 가지는 레코드 삭제
r : 키를 가지는 레코드 검색
p : 페이지 내용보기
a : 모든 페이지 보기
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
fp = fopen("hash.txt", "r");   //트리저장 화일 존재하는지 체크
if (fp == NULL) {
	file_not_Exist = TRUE;     //없으면 참
	fp = fopen("hash.txt", "w"); //화일 새로 생성
}
else
file_not_Exist = FALSE;
fclose(fp);
initHash(MAX_ADDRESS, "hash.txt", PAGE_SIZE, file_not_Exist);

while (1) {
	fscanf(stdin, "%c", &command);
	if (command == 'x' || command == 'X') break;
	switch (command) {
	case 'i':    //(키,값)을 가지는 레코드 삽입
		fscanf(stdin, "%i %s", &record.key, &record.value);
		if (Add(record.key, record.value)) {
			printf("insert (%d, %s) : success\n", record.key, record.value);
		}
		else {
			printf("Insert (%d, %s) : fail\n", record.key, record.value);
		}
		break;
	case 'd':     //키를 가지는 레코드 삭제
		fscanf(stdin, "%i", &record.key);
			if (Remove(record.key)) {
				printf("Delete (%d) : success\n", record.key);
			}
			else {
				printf("Delete (%d) : fail\n", record.key);
			}
		break;
	case 'r':    //키를 가지는 레코드 검색
		fscanf(stdin, "%i", &record.key);
			if (Search(record.key, &result)) {
				printf(
					"Retrive (%d, %s) : success\n",	record.key,	result.value);
			}
			else {
				printf("Retrive (%d) : fail\n", record.key);
			}
		break;
	case 'p':    //페이지 보기
		fscanf(stdin, "%i", &page_no);
		Show_Page(page_no);
		break;
	case 'a':    //모든페이지 보기
		Show_Page_All();
		break;
	case 'h':    //헤더페이지 보기
		Show_Header();
		break;
	}
}
	closeHash();
    return 0;
}