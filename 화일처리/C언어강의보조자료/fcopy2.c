#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	FILE *fp1, *fp2;
	int ch;

	// ù��° ������ �б� ���� ����.
	if( (fp1 = fopen("sample.txt", "r")) == NULL )
	{
		fprintf(stderr,"���� ȭ���� �� �� �����ϴ�.\n");
		system("pause");
		return 1;

	}

	// �ι�° ������ ���� ���� ����.
	if( (fp2 = fopen("copied.txt", "w")) == NULL )
	{
		fprintf(stderr,"���� ���� %s�� �� �� �����ϴ�.\n");
		return 2;
	}

	// ù��° ������ �ι�° ���Ϸ� �����Ѵ�.

	while( (ch=fgetc(fp1)) != EOF )
    {
		fputc(ch, fp2);	
	}

	fclose(fp1);
	fclose(fp2);
	system("pause");
	return 0;
} 
