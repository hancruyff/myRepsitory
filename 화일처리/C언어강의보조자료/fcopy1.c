#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	FILE *fp1, *fp2;
	char buffer[100];

	// ù��° ������ �б� ���� ����.
	if( (fp1 = fopen("proverb.txt", "r")) == NULL )
	{
		fprintf(stderr,"���� ���� %s�� �� �� �����ϴ�.\n", "proverb.txt");
		return 1;
	}

	// �ι�° ������ ���� ���� ����.
	if( (fp2 = fopen("proverb2.txt", "w")) == NULL )
	{
		fprintf(stderr,"���� ���� %s�� �� �� �����ϴ�.\n", "copied.txt");
		return 2;
	}

	// ù��° ������ �ι�° ���Ϸ� �����Ѵ�.

	while( fgets(buffer, 100, fp1) != NULL )
    {
		fputs(buffer, fp2);	
	}

	fclose(fp1);
	fclose(fp2);
	system("pause");
	return 0;
} 
