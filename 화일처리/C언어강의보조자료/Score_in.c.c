#include <stdio.h>
#include <stdlib.h>
int main(void)
{
	FILE *fp;
	int number, count = 0;
	char name[20];
	float score, total = 0.0;
	// 성적 화일을 읽기 모드로 연다.
	if( (fp = fopen("score.txt", "r")) == NULL )
	{
		fprintf(stderr,"화일을 열 수 없습니다.\n");
		exit(1);
	}
	// 화일에서 성적을 읽어서 평균을 구한다.
	while( !feof( fp ) )
	{
		fscanf(fp, "%d %s %f", &number, name, &score);
		total += score;
		count++;
	}
	printf("평균 = %f\n", total/count);
	fclose(fp);
	system("pause");
	return 0;
} 