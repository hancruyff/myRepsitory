#include <stdio.h>
#define SIZE 5

int main(void)
{
	int buffer[SIZE], i, read_data;
	FILE *fp1, *fp2 = NULL;
	int size;

	fp1 = fopen("binary.txt", "rb");

	if( fp1 == NULL ) 
	{
		fprintf(stderr, "binary.txt ������ �� �� �����ϴ�.");
		return 1;
	}
	fp2 = fopen("ascii.txt", "r");
		if( fp1 == NULL ) 
	{
		fprintf(stderr, "ascii.txt ������ �� �� �����ϴ�.");
		return 1;
	}

	size = fread(buffer, sizeof(int), SIZE, fp1);

	if( size != SIZE )
	{
		fprintf(stderr, "�б� ���� �� ������ �߻��߽��ϴ�.\n");
		fclose(fp1);
		return 1;
	}
//binary.txt ���
	for(i=0;i<SIZE;i++)
	{
	printf("%d ", buffer[i]); 
	}
	printf("\n"); 

//ascii.txt ���

    for(i=0;i<SIZE;i++)
	{
	fscanf(fp2,"%4d", &read_data); 
	printf("%d ",read_data);
	}
	printf("\n"); 

	return 0;
}
