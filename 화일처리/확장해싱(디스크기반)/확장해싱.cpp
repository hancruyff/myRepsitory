#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include "struct.h"

// main
int main(int argc, char *argv[]) {
	int init, key, file_not_Exist;
	char command;
	char name[120];
	
	;   //�ؽ����� ȭ�� �����ϴ��� üũ
	if (fopen("hash.txt", "r") == NULL) file_not_Exist = 1;
	else file_not_Exist = 0;
	//fclose(fp);

	if (initialize(file_not_Exist)==1) return 1;
	while (1) {
		fscanf(stdin, "%c", &command);
		if (command == 'x' || command == 'X') break;
		switch (command) {
		case 'I':
		case 'i': // ���� ���
			scanf("%d %s", &key, name);
			if (Insert(key, name) == 0) {
				PrintPseudo(key);
//				PrintHash();
			}
			else {
				printf("Insert (%d, %s) : fail\n", &key, name);
			}
			break;
		case 'D':
		case 'd': // ���� ���
			scanf("%d", &key);
			if (Delete(key) == 0) {
			}
			else {
				printf("Error occurred. Input data corrupted.\n");
				return -1;
			}
			break;
		case 'R':
		case 'r': // �˻� ���
			scanf("%d", &key);
			if (Retrieve(key) == 0) {
//				PrintHash();
			}
			else {
				printf("Error occurred. Input data corrupted.\n");
				return -1;
			}
			
			break;
		case 'A':
		case 'a': // ��纣�� ���� ����
			PrintHash();
			break;
		default:
			break;
		}
	}
	printf("\n[Extendible Hashing] Ended\n\n");
	return 0;
}


// ���͸� �ʱ�ȭ

int initialize(int file_not_Exist) {


		directory = (struct Directory*)malloc(sizeof(struct Directory));
		memset(directory, 0, sizeof(struct Directory));

		if (file_not_Exist) {  //hash.txt �� ������ ���丮�� �ʱ�ȭ�ϰ� hash.txt������ �����
		directory->header = 0;
		for (int i = 0; i < DIRECORY_COUNT; ++i) {
			directory->table[i][0] = -1;
			directory->table[i][1] = -1;
			directory->table[i][2] = 0;
		}
		// ���͸��� �ʱ�table ����
		for (int i = 0; i < InitialBUCKET_COUNT; ++i) {
			directory->table[i][0] = i; //bit string 
			directory->table[i][1] = 0; //��Ŷ��ȣ��0 
			directory->table[i][2] = 0;
		}
		// Block manager �ʱ�ȭ
		blockManager.blockCnt = 0;
		for (int i = 0; i < DIRECORY_COUNT; ++i) {
			blockManager.blockTable[i] = 0;
		}

		// �ʱ� ��Ŷ ����
		bucket.header = 0;
		bucket.recordCnt = 0;
		blockManager.blockCnt = InitialBUCKET_COUNT;
		memset(&bucket, 0, sizeof(struct Bucket));
		fp = fopen("hash.txt", "w");
		fwrite(&blockManager, sizeof(struct BlockManager), 1, fp);
		fwrite(directory, sizeof(struct Directory), 1, fp);
		fwrite(&bucket, sizeof(struct Bucket), 1, fp);
		fclose(fp);
		printf("[Extendible Hashing] Initialized\n");



	}
	else {	//���� ȭ�Ͽ��� ������ �о����
		fp = fopen("hash.txt", "r");
		fread(&blockManager, sizeof(struct BlockManager), 1, fp);
		fseek(fp, sizeof(struct BlockManager), 0);
		fread(directory, sizeof(struct Directory), 1, fp);
//		fread(HashAddress, 10 * sizeof(struct Bucket), 1, fp);
		fclose(fp);

		printf("[Extendible Hashing] Started\n\n");	
	}
//		PrintHash();
		return 0;
}
int Hash(char *key)
{
	int sum = 0;
	int len = strlen(key);
	if (len % 2 == 1) len++;  //len�� ¦����
		for (int j = 0; j < len; j += 2)
			sum = (sum + 100 * key[j] + key[j + 1]) % 19937;
	return sum;
}
int MakeAdress(char *key, int depth)
{
	int retval = 0;
	int hashval = Hash(key);
	for (int j = 0; j < depth; j++)
	{
		retval = retval << 1;
		int  lowbit = hashval & 1;
		retval = retval | lowbit;
		hashval = hashval >> 1;
	}
	return hashval;
}
//Pseudo key �Լ� (Ű �����Լ�)
int PseudoKeyFunc(int key, int digits) {
	int i;
	int pseudo_key = 0;
	int bit = 0;
	// key�� ��8�ڸ� bitstring���� digits ��ŭ�� bitstring�� ���
	for (i = PSEUODO_CODE_SIZE - 1; i >= PSEUODO_CODE_SIZE - digits; --i) {
		bit = (key & POW(2, i)) > 0 ? 1 : 0;
		pseudo_key += POW(2, i - (PSEUODO_CODE_SIZE - digits)) * bit; //�ش�bit�� ����
	}
	return pseudo_key;
}

//����
int Insert(int key, char name[120]) {
	int j, k, temp, More_Header = 0;
	int pseudo_key = 0;                 // �Է� key��pseudo key
	int bucketNum = -1;

	char String_Key[10]; // ���ڿ��� ��ȯ�� Ű��  
	struct Bucket bucket, newBucket; // disk�� ��Ŷ�� ������ struct 
	struct Record record;
	struct Directory *newDirectory;

	// pseudo_key�� ��� ���丮�� ����
	pseudo_key = PseudoKeyFunc(key, directory->header);
		/*���� Ű�� ���ڿ��̸� ������ �̿�
		sprintf(String_Key,"%d",key );
		pseudo_key = MakeAdress(String_Key, directory->header);
		*/
	bucketNum = directory->table[pseudo_key][1];

	// ���� ��Ŷ��ȣ�� ���� ����
	if (bucketNum >= 0) {

		fp = fopen("hash.txt", "r");
		fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*bucketNum, SEEK_SET);
		fread(&bucket, sizeof(struct Bucket), 1, fp);
		fclose(fp);

		// �̹��ش�key�� �����ϴ��� Ȯ��
		if (bucket.recordCnt > 0) {
			for (int i = 0; i < bucket.recordCnt; ++i) {
				if (bucket.record[i].key == key) { // �ش�key ����
					printf("Insert failed. Key(%d) already exists.  ", key);
					return 0;
				}
			}
		}

		// ���������� �ִ��� Ȯ��
		if (bucket.recordCnt < BUCKET_SIZE) { // ���������� ������ ����
			record.key = key;
			memcpy(record.name, name, 120);
			bucket.record[bucket.recordCnt] = record;
			bucket.recordCnt++;

			// �ٲ� ������ �޸𸮿� ���
//			memcpy((char*)HashAddress + PAGE_SIZE * bucketNum, &bucket, sizeof(struct Bucket));
			// ��ϰ����ڰ���

			blockManager.blockTable[bucketNum] = 1;
			fp = fopen("hash.txt", "r+");
			fwrite(&blockManager, sizeof(struct BlockManager), 1, fp);
			fwrite(directory, sizeof(struct Directory), 1, fp);
			fseek(fp, sizeof(struct BlockManager)+ sizeof(struct Directory)+ sizeof(struct Bucket)*bucketNum, SEEK_SET);
			fwrite(&bucket, sizeof(struct Bucket), 1, fp);
			fclose(fp);

			printf("(%d, %s) inserted.  ", key, name);


		}
		else { // ��Ŷ�� ���������� ������ ����
			while (1) {
				// ���͸��� Ȯ���ؾ� �ϴ��� �˻�
				if (directory->header < (bucket.header + 1)) {
					// ���͸� Ȯ��
					if (directory->header >= PSEUODO_CODE_SIZE) { // �̹� �������� �˻�
						printf("Error. reached the maximum entry.\n");
						return 0;
					}

					// �� ���͸��� ����� ����
					newDirectory = (struct Directory*)malloc(sizeof(struct Directory));
					memset(newDirectory, 0, sizeof(struct Directory));
					newDirectory->header = directory->header + 1;
					for (int i = 0; i < DIRECORY_COUNT; ++i) {
						newDirectory->table[i][0] = -1;
						newDirectory->table[i][1] = -1;
						newDirectory->table[i][2] = 0;
					}
					for (int i = 0; i < POW(2, newDirectory->header); ++i) {
						newDirectory->table[i][0] = i;
						newDirectory->table[i][1] = directory->table[i / 2][1];
						newDirectory->table[i][2] = directory->table[i / 2][2];
					}
					free(directory);
					directory = newDirectory;
				}

				// �����÷ΰ� �߻��� ��Ŷ�� ����
				// ���ο� ��Ŷ�� ����
				memset(&newBucket, 0, sizeof(struct Bucket));
				newBucket.header = bucket.header;
				newBucket.recordCnt = 0;

				// ���ڵ带 �� ��Ŷ�� �й�
				bucket.recordCnt = 0;
				for (int i = 0; i < BUCKET_SIZE; ++i) {
					if (PseudoKeyFunc(bucket.record[i].key, bucket.header + 1) % 2 == 0) {
						// ù ��° ��Ŷ
						bucket.record[bucket.recordCnt] = bucket.record[i];
						bucket.recordCnt++;
					}
					else { // �� ��° ��Ŷ
						newBucket.record[newBucket.recordCnt] = bucket.record[i];
						newBucket.recordCnt++;
					}
				}
				bucket.header++;
				newBucket.header++;

				// ��ũ�� �������(�߰��� ��� �ִ� ������ �˻�) �� �ִ��� Ȯ��
				temp = -1;
				for (int i = 0; i < blockManager.blockCnt; ++i) {
					if (blockManager.blockTable[i] == 0) { // �������
						temp = i;
						break;
					}
				}
				if (temp != -1) { // ��������� ����- �ش��Ͽ� ���
				}
				else { // ���ο� ����Ҵ� (�߰��� ����ִ� �� ��ã���� �ǵ��� ���ο� �� �Ҵ�)
					temp = blockManager.blockCnt;
					blockManager.blockCnt++;
				}
				// �� ������ ��ġ�� temp�� ��� �ִ� �� �ּҰ� �Ҵ� ��
				//���� ���� �Ҵ�� ��(temp)�� ���� ��(bucketNum) ���ڵ尡 ������ ����Ǿ� �ִ�
				//�� ������  HashAddress�� ����

//				memcpy((char*)HashAddress + PAGE_SIZE * temp, &newBucket, sizeof(struct Bucket));
				blockManager.blockTable[temp] = 1;
//				memcpy((char*)HashAddress + PAGE_SIZE * bucketNum, &bucket, sizeof(struct Bucket));

				// ���͸��� ��Ŷ �ε����� ����
				j = POW(2, directory->header - bucket.header + 1);
				k = PseudoKeyFunc(key, bucket.header - 1) * j;

				// ���͸��� ù ��° �ٲ�κ� �ε���
				for (int i = 0; i < j / 2; i++, k++) {
					directory->table[k][1] = bucketNum;
					directory->table[k][2] = bucket.header;
				}
				for (int i = 0; i < j / 2; i++, k++) {
					directory->table[k][1] = temp;
					directory->table[k][2] = newBucket.header;
				}
				// �ٲ� ������ ��ũ�� ����
				fp = fopen("hash.txt", "r+");
				fwrite(&blockManager, sizeof(struct BlockManager), 1, fp);
				fwrite(directory, sizeof(struct Directory), 1, fp);
				fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*bucketNum, SEEK_SET);
				fwrite(&bucket, sizeof(struct Bucket), 1, fp);
				fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*temp, SEEK_SET);
				fwrite(&newBucket, sizeof(struct Bucket), 1, fp);
				fclose(fp);

				//���� ������ �������� ���� �Է¹������ڵ� ����
				pseudo_key = PseudoKeyFunc(key, directory->header);
				bucketNum = directory->table[pseudo_key][1];
				// �ش��Ŷ ��ġ�� ã���б�
//				memcpy(&bucket, (char*)HashAddress + PAGE_SIZE * bucketNum, sizeof(struct Bucket));
				fp = fopen("hash.txt", "r");
				fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*bucketNum, SEEK_SET);
				fread(&bucket, sizeof(struct Bucket), 1, fp);
				fclose(fp);

				// ���������� �ִ��� Ȯ���ϰ�
				if (bucket.recordCnt < BUCKET_SIZE) { // ���������� ������ ����
					record.key = key;
					memcpy(record.name, name, 120);
					// record.name = name
					bucket.record[bucket.recordCnt] = record;
					bucket.recordCnt++;

					// �ٲ� ������ ���
//					memcpy((char*)HashAddress + PAGE_SIZE * bucketNum, &bucket, sizeof(struct Bucket));
					// block manager update 
					blockManager.blockTable[bucketNum] = 1;
			//�ٲ� ������ ��ũ�� ����
					fp = fopen("hash.txt", "r+");
					fwrite(&blockManager, sizeof(struct BlockManager), 1, fp);
					fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*bucketNum, SEEK_SET);
					fwrite(&bucket, sizeof(struct Bucket), 1, fp);
					fclose(fp);
					printf("(%d, %s) inserted.\n", key, name);
					return 0;
				}
				// ���������� ������ ��� while loop�� ���� ���͸� Ȯ��
			}


		}
	}
	else {
		printf("Error occurred while inserting.\n");
		return -1;
	}
	return 0;
}

// ����
int Delete(int key) {
	int i, j, k, temp;
	int b1, b2, cnt;
	int pseudo_key = 0; // ������key��pseudo key
	int bucketNum = -1; // ��Ŷ��ȣ
	struct Bucket bucket, buddyBucket;
	// ��ũ���� ��Ŷ�� buddy bucket�� ������struct 
	struct Directory *newDirectory;
	// pseudo key�� ��� ���͸��� ����
	pseudo_key = PseudoKeyFunc(key, directory->header);
	bucketNum = directory->table[pseudo_key][1];
	// ���¹�Ŷ��ȣ�� ���� ��ũ����
	if (bucketNum >= 0) {
		// �ش��Ŷ�� ��ġ�� ã���б�

		fp = fopen("hash.txt", "r");
		fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*bucketNum, SEEK_SET);
		fread(&bucket, sizeof(struct Bucket), 1, fp);
		fclose(fp);

		// �ش�key�� �����ϴ��� Ȯ��
		temp = -1;
		if (bucket.recordCnt > 0) {
			for (i = 0; i < bucket.recordCnt; ++i) {
				if (bucket.record[i].key == key) { // �ش�key ����
					temp = i;
					break;
				}
			}
			if (temp == -1) { // �ش�key ����
				printf("Delete failed. Key (%d) does not exist.\n", key);
				return 0;
			}
		}
		else { // �ش��Ŷ�� ���ڵ尡 ����
			printf("Delete failed. Key (%d) does not exist.\n", key);
			return 0;
		}

		// ��Ŷ����key ������Ŷ����
		printf("(%d, %s) deleted.\n", bucket.record[temp].key, bucket.record[temp].name);
		for (i = temp; i < bucket.recordCnt - 1; ++i)  
			bucket.record[i] = bucket.record[i + 1];   //���� ���ڵ� �ڿ� �ִ� ���ڵ带 ���ڸ��� ������
		bucket.recordCnt--;

		//���� ������ �Ǹ� ������ ������ ���
		fp = fopen("hash.txt", "r+");
		fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*bucketNum, SEEK_SET);
		fwrite(&bucket, sizeof(struct Bucket), 1, fp);
		fclose(fp);

		// ��Ŷ�̰����̰ų��պ������ϸ�buddy bucket��ã�Ƽ��պ�
		while (bucket.header >= 2) {
			// �����Ŷã��
			b1 = PseudoKeyFunc(key, bucket.header - 1);
			b2 = PseudoKeyFunc(key, bucket.header);
			cnt = POW(2, directory->header - bucket.header);	// ������ ���� ��
			k = (b2 + (b1 * 2 == b2 ? 1 : -1)) * cnt;			// ������ ���� �� ù ��° ���� �ε���
			temp = directory->table[k][1];  // �����Ŷ

			if (temp < 0) return 0;
			// �����Ŷ �о����
			memset(&buddyBucket, 0, sizeof(struct Bucket));
			fp = fopen("hash.txt", "r");
			fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*temp, SEEK_SET);
			fread(&buddyBucket, sizeof(struct Bucket), 1, fp);
			fclose(fp);
			// �պ��ؾ� �ϴ��� �˻�
			if ((bucket.header != buddyBucket.header) || (bucket.recordCnt + buddyBucket.recordCnt > BUCKET_SIZE)) {
				// �պ����ʿ����
				break;
			}

			// �պ��� �ʿ��ϸ�  buddy bucket���� ��Ŷ���η��ڵ� ����
			for (i = 0; i < buddyBucket.recordCnt; ++i) {
				bucket.record[bucket.recordCnt] = buddyBucket.record[i];
				bucket.recordCnt++;
			}
			bucket.header--;

			//�պ��� ����� �������
			fp = fopen("hash.txt", "r+");
			fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*bucketNum, SEEK_SET);
			fwrite(&bucket, sizeof(struct Bucket), 1, fp);
			fclose(fp);

			// ���͸����� ��������
			directory->table[pseudo_key][2] = bucket.header;
			for (i = 0; i < cnt; ++i) {        // ���𺯰�
				directory->table[k + i][1] = bucketNum;
				directory->table[k + i][2] = bucket.header;
			}
			// buddy bucket ����
		
			blockManager.blockTable[temp] = 0; // ����������μ���
			memset(&buddyBucket, 0, sizeof(struct Bucket));
			fp = fopen("hash.txt", "r+");
			fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*temp, SEEK_SET);
			fwrite(&buddyBucket, sizeof(struct Bucket), 1, fp);
			fclose(fp);

			// ���͸��� �پ�� �� �ִ��� Ȯ��
			j = 0;
			for (i = 0; i < POW(2, directory->header); ++i) {
				if (directory->table[i][2] >= directory->header) {
					j = 1;
					break;
				}
			}
			if (j == 0) {
				// ��Ŷ�� ���������� ���͸���������� ���� ��� ���͸����
				newDirectory = (struct Directory*)malloc(sizeof(struct Directory));
				memset(newDirectory, 0, sizeof(struct Directory));
				newDirectory->header = directory->header - 1;
				for (i = 0; i < DIRECORY_COUNT; ++i) {
					newDirectory->table[i][0] = -1;
					newDirectory->table[i][1] = -1;
					newDirectory->table[i][2] = 0;
				}
				for (i = 0; i < POW(2, newDirectory->header); ++i) {
					newDirectory->table[i][0] = i;
					newDirectory->table[i][1] = directory->table[i * 2][1];
					newDirectory->table[i][2] = directory->table[i * 2][2];
				}
				free(directory);
				directory = newDirectory;
			}
			fp = fopen("hash.txt", "r+");
			fwrite(&blockManager, sizeof(struct BlockManager), 1, fp);
			fwrite(directory, sizeof(struct Directory), 1, fp);
			fclose(fp);

		}
	}
	else {
		printf("Error occurred while deleting.\n");
		return -1;
	}
	return 0;
}


int Retrieve(int key) {
	int i, j, temp;
	int pseudo_key = 0; // ������ key�� pseudo key 
	int bucketNum = -1; // ��Ŷ��ȣ
	struct Bucket bucket; // ��ũ���� ��Ŷ�� ���� �� struct 
						  // pseudo_key�� ��� ���͸� ����
	pseudo_key = PseudoKeyFunc(key, directory->header);
	bucketNum = directory->table[pseudo_key][1];
	// ���� ��Ŷ��ȣ�� ���� ��ũ ����
	if (bucketNum >= 0) {
		// �ش� ��Ŷ�� ��ġ�� ã���б�
		fp = fopen("hash.txt", "r");
		fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*bucketNum, SEEK_SET);
		fread(&bucket, sizeof(struct Bucket), 1, fp);

		fclose(fp);		// �ش�key �����ϴ��� Ȯ��
		temp = -1;
		if (bucket.recordCnt > 0) {
			for (i = 0; i < bucket.recordCnt; ++i) {
				if (bucket.record[i].key == key) { // �ش�key ����
					temp = i;
					printf("(%d,%s) found\n", bucket.record[temp].key, bucket.record[temp].name);
					break;
				}
			}
			if (temp == -1) { // �ش�key ����
				printf("Search failed. Key (%d) was not found.\n", key);
				return 0;
			}
		}
		else { // �ش� ��Ŷ�� ���ڵ尡 ����
			printf("Search failed. Key (%d) was not found.\n", key);
			return 0;
		}
	}
	else {
		printf("Error occurred while searching.\n");
		return -1;
	}
	return 0;
}

// ��� Hash state Function 
void PrintHash() {
	int i, j;
	int prevBucket = -1;
	struct Bucket bucket;
	char bitStr[10];

	// hash �޸��Ҵ�
	HashAddress = malloc(sizeof(struct Bucket) * 1000);
	memset(HashAddress, 0, sizeof(struct Bucket)* 1000);
	fp = fopen("hash.txt", "r");
	fread(&blockManager, sizeof(struct BlockManager), 1, fp);
	fread(directory, sizeof(struct Directory), 1, fp);
	fread(HashAddress, 1000, 1, fp);
	fclose(fp);

	printf("===========================================================\n");
	printf("Drectory (D=%d)       \t�ؽû���\t���� ũ�� (%d)\n", directory->header, BUCKET_SIZE);
	printf("Pseudo\t[����]\t[����](d)\t���ڵ�\n");
	printf("===========================================================\n");
	for (i = 0; i < POW(2, directory->header); ++i) {
		// bit string ����
		for (j = 0; j < directory->header; ++j) {
			bitStr[j] = (directory->table[i][0] & POW(2, directory->header - j - 1)) > 0 ? '1' : '0';
		}
		if (directory->header == 0) {
			bitStr[0] = '0';
			bitStr[1] = '\0';
		}
		else bitStr[directory->header] = '\0';
		// ���͸� �������
		printf("[%s]\t[B:%d]", bitStr, directory->table[i][1]);
		if (directory->table[i][1] != prevBucket) {
			prevBucket = directory->table[i][1];
			// �ش��Ŷ �������     
			memcpy(&bucket, (char*)HashAddress + PAGE_SIZE * prevBucket, sizeof(struct Bucket));
			printf("\t\t[B:%d](%d)", prevBucket, bucket.header);
			printf("\t", bucket.record[j].key, bucket.record[j].name);
			for (j = 0; j < bucket.recordCnt; ++j) {
				printf("(%d,%s) ", bucket.record[j].key, bucket.record[j].name);

			}
		}
		printf("\n");
	}
	printf("\n");
}
void PrintPseudo(int key) {
	int i, j;
	char bitStr[10];
	printf("Pseudo(%d) : ", key);
	for (j = 0; j < PSEUODO_CODE_SIZE; ++j) {
		bitStr[j] = (key & POW(2, PSEUODO_CODE_SIZE - j - 1)) > 0 ? '1' : '0';
	}
	bitStr[PSEUODO_CODE_SIZE] = '\0';
	// ���͸� �������
	printf("%s \n", bitStr);
}
