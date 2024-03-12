#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	FILE *fp1, *fp2;
	char buffer[100];

	// 첫번째 파일을 읽기 모드로 연다.
	if( (fp1 = fopen("proverb.txt", "r")) == NULL )
	{
		fprintf(stderr,"원본 파일 %s을 열 수 없습니다.\n", "proverb.txt");
		return 1;
	}

	// 두번째 파일을 쓰기 모드로 연다.
	if( (fp2 = fopen("proverb2.txt", "w")) == NULL )
	{
		fprintf(stderr,"복사 파일 %s을 열 수 없습니다.\n", "copied.txt");
		return 2;
	}

	// 첫번째 파일을 두번째 파일로 복사한다.

	while( fgets(buffer, 100, fp1) != NULL )
    {
		fputs(buffer, fp2);	
	}

	fclose(fp1);
	fclose(fp2);
	system("pause");
	return 0;
} 
