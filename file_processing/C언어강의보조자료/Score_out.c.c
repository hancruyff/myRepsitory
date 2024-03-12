#include <stdio.h>
#include <stdlib.h>
int main(void)
{
	FILE *fp;
	char fname[100];
	int number, count = 0;
	char name[20];
	float score, total = 0.0;
	// 성적 화일을 쓰기 모드로 연다.
	if( (fp = fopen("score.txt", "w")) == NULL )
	{
		fprintf(stderr,"성적 화일 %s을 열 수 없습니다.\n", fname);
		exit(1);
	}
	// 사용자로부터 학번, 이름, 성적을 입력받아서 화일에 저장한다.
	while( 1 )
	{
		printf("학번, 이름, 성적을 입력하시요: (음수이면 종료)");
		scanf("%d", &number);
		if (number < 0) break;
		scanf("%s %f", name, &score);
		fprintf(fp, "%d %s %f\n", number, name, score);
	}
	fclose(fp);
	return 0;
} 
