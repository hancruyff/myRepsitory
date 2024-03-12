#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DIVIDER 10
#define REC_SIZE sizeof(Data)
typedef struct record Data;

struct record {
	int key;
	char name[20];

};

void initHash(char *file_name);
int Add(int key, char *name, char *file_name);
int Search(int key, char *file_name);
int Remove(int key, char *file_name);
void Show_Page_All(char *file_name);


void initHash(char *file_name) {
	FILE *fp;
	Data initialData;
	initialData.key = -1;
	initialData.name[0] = 0;
	fp = fopen(file_name, "w");   //트리저장 화일 존재하는지 체크
	for (int i = 0; i < DIVIDER; i++)	fwrite(&initialData, REC_SIZE, 1, fp);
	fclose(fp);
}
int main(void)
{
	FILE *fp;
	char *file_name = "hash.txt";
	char command;
	char *result = "";
	Data data;

	fp = fopen(file_name, "r");   //트리저장 화일 존재하는지 체크
	if (fp == NULL) {
		initHash(file_name);
	}
	while (1) {
		fscanf(stdin, "%c", &command);
		if (command == 'x' || command == 'X') break;
		switch (command) {
		case 'i':    //(키,값)을 가지는 레코드 삽입
			scanf("%i %s", &data.key, data.name);
			if (Add(data.key, data.name, file_name)) {
				printf("insert (%d, %s) : success\n", data.key, data.name);
			}
			else {
				printf("Insert (%d, %s) : fail\n", data.key, data.name);
			}
			break;
		case 'd':     //키를 가지는 레코드 삭제
			scanf("%i", &data.key);
			Remove(data.key, file_name);
 			break;
		case 'r':    //키를 가지는 레코드 검색
			fscanf(stdin, "%i", &data.key);
			Search(data.key, file_name);
			break;

		case 'a':    //모든페이지 보기
			Show_Page_All(file_name);
			break;

		}
	}
}
int Add(int key, char *name, char *file_name) {
	FILE *fp;
	int addr, home_addr;
	Data insert_data, probed_data;
	insert_data.key = key;
	strcpy(insert_data.name, name);
	home_addr = key % DIVIDER;
	fp = fopen(file_name, "r+");

	for (int i = 0; i < DIVIDER; i++) {
		addr = home_addr + i;
		fseek(fp, addr*REC_SIZE, SEEK_SET);
		fread(&probed_data, REC_SIZE, 1, fp);  //기존에 있는 데이터의 키값을 읽어봄
//		if (probed_data.key == -1 || probed_data.key == 0) {  //키값 0은 삭제표시
		if (probed_data.key == -1) {         //- 삭제 시 문제가 될 수 있음. 윗 문장으로!!!!!!!!!
			fseek(fp, addr*REC_SIZE, SEEK_SET);
			fwrite(&insert_data, REC_SIZE, 1, fp);
			fclose(fp);
			return 1; //성공(0)을 반환
		}
	}
	fclose(fp);
	return 0;  ////실패(1)을 반환
} 


int Search(int key, char *file_name) {
	FILE *fp;
	int addr, home_addr;
	Data probed_data;

	home_addr = key % DIVIDER;
	fp = fopen(file_name, "r");

	for (int i = 0; i < DIVIDER; i++) {
		addr = (home_addr + i) % DIVIDER;
		fseek(fp, addr*REC_SIZE, SEEK_SET);
		fread(&probed_data, REC_SIZE, 1, fp);  //기존에 있는 데이터의 키값을 읽어봄
		if (probed_data.key == -1) {
			fclose(fp);
			printf("Retrive (%d) : fail\n", key);
			return 1; //실패(1)을 반환
		}
		if (probed_data.key == key) {	
			printf(
				"Retrive (%d, %s) : success\n", probed_data.key, probed_data.name);
			fclose(fp);
			return 0; //성공(0)을 반환
		}

	}		
	fclose(fp);
	printf("Retrive (%d) : fail\n", key);
	return 1; //실패(1)을 반환
}

int Remove(int key, char *file_name) {
	FILE *fp;
	int addr, home_addr;
	Data probed_data;

	home_addr = key % DIVIDER;
	fp = fopen(file_name, "r+");

	for (int i = 0; i < DIVIDER; i++) {
		addr = (home_addr + i) % DIVIDER;
		fseek(fp, addr*REC_SIZE, SEEK_SET);
		fread(&probed_data, REC_SIZE, 1, fp);  //기존에 있는 데이터의 키값을 읽어봄
		if (probed_data.key == -1) {
			fclose(fp);
			printf("Remove (%d) : fail\n", key);
			return 1; //실패(1)을 반환
		}
		if (probed_data.key == key) {
			printf("Remove (%d, %s) : success\n", probed_data.key, probed_data.name);
//			probed_data.key = 0;
			probed_data.key = -1;   //- 삭제 시 문제가 될 수 있음. 윗 문장으로!!!!!!!!!
			probed_data.name[0] = '0';
			fseek(fp, addr*REC_SIZE, SEEK_SET);
			fwrite(&probed_data, REC_SIZE, 1, fp);
			fclose(fp);
			return 0; //성공(0)을 반환
		}

	}
	fclose(fp);
	printf("Remove (%d) : fail\n", key);
	return 1; //실패(1)을 반환

}

void Show_Page_All(char *file_name) {
	FILE *fp;
	int addr;
	Data probed_data;

	fp = fopen(file_name, "r");

	for (int i = 0; i < DIVIDER; i++) {
		fseek(fp, i*REC_SIZE, SEEK_SET);
		fread(&probed_data, REC_SIZE, 1, fp);  //기존에 있는 데이터의 키값을 읽어봄
		printf("%d번 버켓 (%d, %2s)\n", i, probed_data.key, probed_data.name);
		}
	fclose(fp);
	return 0; //성공(0)을 반환

}

