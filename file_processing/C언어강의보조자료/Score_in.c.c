#include <stdio.h>
#include <stdlib.h>
int main(void)
{
	FILE *fp;
	int number, count = 0;
	char name[20];
	float score, total = 0.0;
	// ���� ȭ���� �б� ���� ����.
	if( (fp = fopen("score.txt", "r")) == NULL )
	{
		fprintf(stderr,"ȭ���� �� �� �����ϴ�.\n");
		exit(1);
	}
	// ȭ�Ͽ��� ������ �о ����� ���Ѵ�.
	while( !feof( fp ) )
	{
		fscanf(fp, "%d %s %f", &number, name, &score);
		total += score;
		count++;
	}
	printf("��� = %f\n", total/count);
	fclose(fp);
	system("pause");
	return 0;
} 