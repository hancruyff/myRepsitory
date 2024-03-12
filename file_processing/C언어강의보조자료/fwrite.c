#include <stdio.h>

int main(void)
{
	int buffer[] = { 1000, 2000, 3000, 4000, 5000};
	FILE *fp = NULL;
	int i, size, count;

	fp = fopen("binary.txt", "wb");
	if( fp == NULL ) 
	{
		fprintf(stderr, "binary.txt ������ �� �� �����ϴ�.");
		return 1;
	}

	size = sizeof(buffer[0]);
	count = sizeof(buffer) / sizeof(buffer[0]);

	i = fwrite(buffer, size, count, fp);
	fclose(fp);

//������ʹ� �Ϲ� �ý�Ʈȭ�Ϸ� �����غ���

	fp = fopen("ascii.txt", "w");
	if( fp == NULL ) 
	{
		fprintf(stderr, "ascii.txt ������ �� �� �����ϴ�.");
		return 1;
	}

	fprintf(fp, "%d%d%d%d%d", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
	fclose(fp);
	return 0;

// binary.txtȭ���� �Է� ���ڿ� ������� 20����Ʈ�� ���̴�. ��? ����4����Ʈ*5=20����Ʈ
// ascii.txt ȭ���� ����? �����غ���
}
