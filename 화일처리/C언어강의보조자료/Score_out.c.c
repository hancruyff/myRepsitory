#include <stdio.h>
#include <stdlib.h>
int main(void)
{
	FILE *fp;
	char fname[100];
	int number, count = 0;
	char name[20];
	float score, total = 0.0;
	// ���� ȭ���� ���� ���� ����.
	if( (fp = fopen("score.txt", "w")) == NULL )
	{
		fprintf(stderr,"���� ȭ�� %s�� �� �� �����ϴ�.\n", fname);
		exit(1);
	}
	// ����ڷκ��� �й�, �̸�, ������ �Է¹޾Ƽ� ȭ�Ͽ� �����Ѵ�.
	while( 1 )
	{
		printf("�й�, �̸�, ������ �Է��Ͻÿ�: (�����̸� ����)");
		scanf("%d", &number);
		if (number < 0) break;
		scanf("%s %f", name, &score);
		fprintf(fp, "%d %s %f\n", number, name, score);
	}
	fclose(fp);
	return 0;
} 
