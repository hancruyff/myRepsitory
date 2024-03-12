#include <stdio.h>

int main(void)
{
	int buffer[] = { 1000, 2000, 3000, 4000, 5000};
	FILE *fp = NULL;
	int i, size, count;

	fp = fopen("binary.txt", "wb");
	if( fp == NULL ) 
	{
		fprintf(stderr, "binary.txt 파일을 열 수 없습니다.");
		return 1;
	}

	size = sizeof(buffer[0]);
	count = sizeof(buffer) / sizeof(buffer[0]);

	i = fwrite(buffer, size, count, fp);
	fclose(fp);

//여기부터는 일반 택스트화일로 저장해보자

	fp = fopen("ascii.txt", "w");
	if( fp == NULL ) 
	{
		fprintf(stderr, "ascii.txt 파일을 열 수 없습니다.");
		return 1;
	}

	fprintf(fp, "%d%d%d%d%d", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
	fclose(fp);
	return 0;

// binary.txt화일은 입력 숫자에 관계없이 20바이트일 것이다. 왜? 정수4바이트*5=20바이트
// ascii.txt 화일의 경우는? 실험해보자
}
