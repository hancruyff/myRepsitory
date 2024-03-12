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
	
	;   //해시저장 화일 존재하는지 체크
	if (fopen("hash.txt", "r") == NULL) file_not_Exist = 1;
	else file_not_Exist = 0;
	//fclose(fp);

	if (initialize(file_not_Exist)==1) return 1;
	while (1) {
		fscanf(stdin, "%c", &command);
		if (command == 'x' || command == 'X') break;
		switch (command) {
		case 'I':
		case 'i': // 삽입 모드
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
		case 'd': // 삭제 모드
			scanf("%d", &key);
			if (Delete(key) == 0) {
			}
			else {
				printf("Error occurred. Input data corrupted.\n");
				return -1;
			}
			break;
		case 'R':
		case 'r': // 검색 모드
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
		case 'a': // 모든베켓 상태 보기
			PrintHash();
			break;
		default:
			break;
		}
	}
	printf("\n[Extendible Hashing] Ended\n\n");
	return 0;
}


// 디렉터리 초기화

int initialize(int file_not_Exist) {


		directory = (struct Directory*)malloc(sizeof(struct Directory));
		memset(directory, 0, sizeof(struct Directory));

		if (file_not_Exist) {  //hash.txt 가 없으면 디렉토리를 초기화하고 hash.txt를새로 만든다
		directory->header = 0;
		for (int i = 0; i < DIRECORY_COUNT; ++i) {
			directory->table[i][0] = -1;
			directory->table[i][1] = -1;
			directory->table[i][2] = 0;
		}
		// 디렉터리에 초기table 생성
		for (int i = 0; i < InitialBUCKET_COUNT; ++i) {
			directory->table[i][0] = i; //bit string 
			directory->table[i][1] = 0; //버킷번호는0 
			directory->table[i][2] = 0;
		}
		// Block manager 초기화
		blockManager.blockCnt = 0;
		for (int i = 0; i < DIRECORY_COUNT; ++i) {
			blockManager.blockTable[i] = 0;
		}

		// 초기 버킷 생성
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
	else {	//기존 화일에서 데이터 읽어오기
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
	if (len % 2 == 1) len++;  //len을 짝수로
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
//Pseudo key 함수 (키 생성함수)
int PseudoKeyFunc(int key, int digits) {
	int i;
	int pseudo_key = 0;
	int bit = 0;
	// key의 뒤8자리 bitstring에서 digits 만큼의 bitstring을 얻기
	for (i = PSEUODO_CODE_SIZE - 1; i >= PSEUODO_CODE_SIZE - digits; --i) {
		bit = (key & POW(2, i)) > 0 ? 1 : 0;
		pseudo_key += POW(2, i - (PSEUODO_CODE_SIZE - digits)) * bit; //해당bit를 누적
	}
	return pseudo_key;
}

//삽입
int Insert(int key, char name[120]) {
	int j, k, temp, More_Header = 0;
	int pseudo_key = 0;                 // 입력 key의pseudo key
	int bucketNum = -1;

	char String_Key[10]; // 문자열로 변환된 키값  
	struct Bucket bucket, newBucket; // disk의 버킷을 가져올 struct 
	struct Record record;
	struct Directory *newDirectory;

	// pseudo_key를 얻어 디렉토리를 참조
	pseudo_key = PseudoKeyFunc(key, directory->header);
		/*만약 키가 문자열이면 다음을 이용
		sprintf(String_Key,"%d",key );
		pseudo_key = MakeAdress(String_Key, directory->header);
		*/
	bucketNum = directory->table[pseudo_key][1];

	// 얻어온 버킷번호를 통해 접근
	if (bucketNum >= 0) {

		fp = fopen("hash.txt", "r");
		fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*bucketNum, SEEK_SET);
		fread(&bucket, sizeof(struct Bucket), 1, fp);
		fclose(fp);

		// 이미해당key가 존재하는지 확인
		if (bucket.recordCnt > 0) {
			for (int i = 0; i < bucket.recordCnt; ++i) {
				if (bucket.record[i].key == key) { // 해당key 존재
					printf("Insert failed. Key(%d) already exists.  ", key);
					return 0;
				}
			}
		}

		// 여유공간이 있는지 확인
		if (bucket.recordCnt < BUCKET_SIZE) { // 여유공간이 있으면 삽입
			record.key = key;
			memcpy(record.name, name, 120);
			bucket.record[bucket.recordCnt] = record;
			bucket.recordCnt++;

			// 바뀐 내용을 메모리에 기록
//			memcpy((char*)HashAddress + PAGE_SIZE * bucketNum, &bucket, sizeof(struct Bucket));
			// 블록관리자갱신

			blockManager.blockTable[bucketNum] = 1;
			fp = fopen("hash.txt", "r+");
			fwrite(&blockManager, sizeof(struct BlockManager), 1, fp);
			fwrite(directory, sizeof(struct Directory), 1, fp);
			fseek(fp, sizeof(struct BlockManager)+ sizeof(struct Directory)+ sizeof(struct Bucket)*bucketNum, SEEK_SET);
			fwrite(&bucket, sizeof(struct Bucket), 1, fp);
			fclose(fp);

			printf("(%d, %s) inserted.  ", key, name);


		}
		else { // 버킷에 여유공간이 없으면 분할
			while (1) {
				// 디렉터리를 확장해야 하는지 검사
				if (directory->header < (bucket.header + 1)) {
					// 디렉터리 확장
					if (directory->header >= PSEUODO_CODE_SIZE) { // 이미 만원인지 검사
						printf("Error. reached the maximum entry.\n");
						return 0;
					}

					// 새 디렉터리를 만들어 복사
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

				// 오버플로가 발생한 버킷을 분할
				// 새로운 버킷을 생성
				memset(&newBucket, 0, sizeof(struct Bucket));
				newBucket.header = bucket.header;
				newBucket.recordCnt = 0;

				// 레코드를 두 버킷에 분배
				bucket.recordCnt = 0;
				for (int i = 0; i < BUCKET_SIZE; ++i) {
					if (PseudoKeyFunc(bucket.record[i].key, bucket.header + 1) % 2 == 0) {
						// 첫 번째 버킷
						bucket.record[bucket.recordCnt] = bucket.record[i];
						bucket.recordCnt++;
					}
					else { // 두 번째 버킷
						newBucket.record[newBucket.recordCnt] = bucket.record[i];
						newBucket.recordCnt++;
					}
				}
				bucket.header++;
				newBucket.header++;

				// 디스크에 자유블록(중간에 비어 있는 블럭부터 검사) 이 있는지 확인
				temp = -1;
				for (int i = 0; i < blockManager.blockCnt; ++i) {
					if (blockManager.blockTable[i] == 0) { // 자유블록
						temp = i;
						break;
					}
				}
				if (temp != -1) { // 자유블록이 존재- 해당블록에 기록
				}
				else { // 새로운 블록할당 (중간에 비어있는 블럭 못찾으면 맨뒤의 새로운 블럭 할당)
					temp = blockManager.blockCnt;
					blockManager.blockCnt++;
				}
				// 위 과정을 거치면 temp에 비어 있는 블럭 주소가 할당 됨
				//이제 새로 할당된 블럭(temp)과 기존 블럭(bucketNum) 레코드가 나뉘어 저장되어 있다
				//이 내용을  HashAddress에 저장

//				memcpy((char*)HashAddress + PAGE_SIZE * temp, &newBucket, sizeof(struct Bucket));
				blockManager.blockTable[temp] = 1;
//				memcpy((char*)HashAddress + PAGE_SIZE * bucketNum, &bucket, sizeof(struct Bucket));

				// 디렉터리의 버킷 인덱스를 변경
				j = POW(2, directory->header - bucket.header + 1);
				k = PseudoKeyFunc(key, bucket.header - 1) * j;

				// 디렉터리의 첫 번째 바뀔부분 인덱스
				for (int i = 0; i < j / 2; i++, k++) {
					directory->table[k][1] = bucketNum;
					directory->table[k][2] = bucket.header;
				}
				for (int i = 0; i < j / 2; i++, k++) {
					directory->table[k][1] = temp;
					directory->table[k][2] = newBucket.header;
				}
				// 바뀐 내용을 디스크에 저장
				fp = fopen("hash.txt", "r+");
				fwrite(&blockManager, sizeof(struct BlockManager), 1, fp);
				fwrite(directory, sizeof(struct Directory), 1, fp);
				fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*bucketNum, SEEK_SET);
				fwrite(&bucket, sizeof(struct Bucket), 1, fp);
				fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*temp, SEEK_SET);
				fwrite(&newBucket, sizeof(struct Bucket), 1, fp);
				fclose(fp);

				//버켓 분할이 끝났으면 새로 입력받은레코드 삽입
				pseudo_key = PseudoKeyFunc(key, directory->header);
				bucketNum = directory->table[pseudo_key][1];
				// 해당버킷 위치를 찾아읽기
//				memcpy(&bucket, (char*)HashAddress + PAGE_SIZE * bucketNum, sizeof(struct Bucket));
				fp = fopen("hash.txt", "r");
				fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*bucketNum, SEEK_SET);
				fread(&bucket, sizeof(struct Bucket), 1, fp);
				fclose(fp);

				// 여유공간이 있는지 확인하고
				if (bucket.recordCnt < BUCKET_SIZE) { // 여유공간이 있으면 삽입
					record.key = key;
					memcpy(record.name, name, 120);
					// record.name = name
					bucket.record[bucket.recordCnt] = record;
					bucket.recordCnt++;

					// 바뀐 내용을 기록
//					memcpy((char*)HashAddress + PAGE_SIZE * bucketNum, &bucket, sizeof(struct Bucket));
					// block manager update 
					blockManager.blockTable[bucketNum] = 1;
			//바뀐 내용을 디스크에 저장
					fp = fopen("hash.txt", "r+");
					fwrite(&blockManager, sizeof(struct BlockManager), 1, fp);
					fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*bucketNum, SEEK_SET);
					fwrite(&bucket, sizeof(struct Bucket), 1, fp);
					fclose(fp);
					printf("(%d, %s) inserted.\n", key, name);
					return 0;
				}
				// 여유공간이 없으면 계속 while loop를 돌며 디렉터리 확장
			}


		}
	}
	else {
		printf("Error occurred while inserting.\n");
		return -1;
	}
	return 0;
}

// 삭제
int Delete(int key) {
	int i, j, k, temp;
	int b1, b2, cnt;
	int pseudo_key = 0; // 삭제될key의pseudo key
	int bucketNum = -1; // 버킷번호
	struct Bucket bucket, buddyBucket;
	// 디스크에서 버킷과 buddy bucket을 가져올struct 
	struct Directory *newDirectory;
	// pseudo key를 얻어 디렉터리를 참조
	pseudo_key = PseudoKeyFunc(key, directory->header);
	bucketNum = directory->table[pseudo_key][1];
	// 얻어온버킷번호를 통해 디스크접근
	if (bucketNum >= 0) {
		// 해당버킷의 위치를 찾아읽기

		fp = fopen("hash.txt", "r");
		fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*bucketNum, SEEK_SET);
		fread(&bucket, sizeof(struct Bucket), 1, fp);
		fclose(fp);

		// 해당key가 존재하는지 확인
		temp = -1;
		if (bucket.recordCnt > 0) {
			for (i = 0; i < bucket.recordCnt; ++i) {
				if (bucket.record[i].key == key) { // 해당key 존재
					temp = i;
					break;
				}
			}
			if (temp == -1) { // 해당key 없음
				printf("Delete failed. Key (%d) does not exist.\n", key);
				return 0;
			}
		}
		else { // 해당버킷에 레코드가 없음
			printf("Delete failed. Key (%d) does not exist.\n", key);
			return 0;
		}

		// 버킷에서key 삭제버킷정렬
		printf("(%d, %s) deleted.\n", bucket.record[temp].key, bucket.record[temp].name);
		for (i = temp; i < bucket.recordCnt - 1; ++i)  
			bucket.record[i] = bucket.record[i + 1];   //삭제 레코도 뒤에 있는 레코드를 한자리씩 앞으로
		bucket.recordCnt--;

		//버켓 정리가 되면 내용을 버켓이 기록
		fp = fopen("hash.txt", "r+");
		fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*bucketNum, SEEK_SET);
		fwrite(&bucket, sizeof(struct Bucket), 1, fp);
		fclose(fp);

		// 버킷이공백이거나합병가능하면buddy bucket을찾아서합병
		while (bucket.header >= 2) {
			// 버디버킷찾기
			b1 = PseudoKeyFunc(key, bucket.header - 1);
			b2 = PseudoKeyFunc(key, bucket.header);
			cnt = POW(2, directory->header - bucket.header);	// 연관된 리프 수
			k = (b2 + (b1 * 2 == b2 ? 1 : -1)) * cnt;			// 연관된 리프 중 첫 번째 리프 인덱스
			temp = directory->table[k][1];  // 버디버킷

			if (temp < 0) return 0;
			// 버디버킷 읽어오기
			memset(&buddyBucket, 0, sizeof(struct Bucket));
			fp = fopen("hash.txt", "r");
			fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*temp, SEEK_SET);
			fread(&buddyBucket, sizeof(struct Bucket), 1, fp);
			fclose(fp);
			// 합병해야 하는지 검사
			if ((bucket.header != buddyBucket.header) || (bucket.recordCnt + buddyBucket.recordCnt > BUCKET_SIZE)) {
				// 합병이필요없음
				break;
			}

			// 합병이 필요하면  buddy bucket에서 버킷으로레코드 복사
			for (i = 0; i < buddyBucket.recordCnt; ++i) {
				bucket.record[bucket.recordCnt] = buddyBucket.record[i];
				bucket.recordCnt++;
			}
			bucket.header--;

			//합병된 결과를 디록해줌
			fp = fopen("hash.txt", "r+");
			fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*bucketNum, SEEK_SET);
			fwrite(&bucket, sizeof(struct Bucket), 1, fp);
			fclose(fp);

			// 디렉터리에서 리프조정
			directory->table[pseudo_key][2] = bucket.header;
			for (i = 0; i < cnt; ++i) {        // 버디변경
				directory->table[k + i][1] = bucketNum;
				directory->table[k + i][2] = bucket.header;
			}
			// buddy bucket 제거
		
			blockManager.blockTable[temp] = 0; // 자유블록으로설정
			memset(&buddyBucket, 0, sizeof(struct Bucket));
			fp = fopen("hash.txt", "r+");
			fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*temp, SEEK_SET);
			fwrite(&buddyBucket, sizeof(struct Bucket), 1, fp);
			fclose(fp);

			// 디렉터리가 줄어들 수 있는지 확인
			j = 0;
			for (i = 0; i < POW(2, directory->header); ++i) {
				if (directory->table[i][2] >= directory->header) {
					j = 1;
					break;
				}
			}
			if (j == 0) {
				// 버킷의 모든헤더값이 디렉터리헤더값보다 작은 경우 디렉터리축소
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
	int pseudo_key = 0; // 삭제될 key의 pseudo key 
	int bucketNum = -1; // 버킷번호
	struct Bucket bucket; // 디스크에서 버킷을 가져 올 struct 
						  // pseudo_key를 얻어 디렉터리 참조
	pseudo_key = PseudoKeyFunc(key, directory->header);
	bucketNum = directory->table[pseudo_key][1];
	// 얻어온 버킷번호를 통해 디스크 접근
	if (bucketNum >= 0) {
		// 해당 버킷의 위치를 찾아읽기
		fp = fopen("hash.txt", "r");
		fseek(fp, sizeof(struct BlockManager) + sizeof(struct Directory) + sizeof(struct Bucket)*bucketNum, SEEK_SET);
		fread(&bucket, sizeof(struct Bucket), 1, fp);

		fclose(fp);		// 해당key 존재하는지 확인
		temp = -1;
		if (bucket.recordCnt > 0) {
			for (i = 0; i < bucket.recordCnt; ++i) {
				if (bucket.record[i].key == key) { // 해당key 존재
					temp = i;
					printf("(%d,%s) found\n", bucket.record[temp].key, bucket.record[temp].name);
					break;
				}
			}
			if (temp == -1) { // 해당key 없음
				printf("Search failed. Key (%d) was not found.\n", key);
				return 0;
			}
		}
		else { // 해당 버킷에 레코드가 없음
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

// 출력 Hash state Function 
void PrintHash() {
	int i, j;
	int prevBucket = -1;
	struct Bucket bucket;
	char bitStr[10];

	// hash 메모리할당
	HashAddress = malloc(sizeof(struct Bucket) * 1000);
	memset(HashAddress, 0, sizeof(struct Bucket)* 1000);
	fp = fopen("hash.txt", "r");
	fread(&blockManager, sizeof(struct BlockManager), 1, fp);
	fread(directory, sizeof(struct Directory), 1, fp);
	fread(HashAddress, 1000, 1, fp);
	fclose(fp);

	printf("===========================================================\n");
	printf("Drectory (D=%d)       \t해시상태\t버켓 크기 (%d)\n", directory->header, BUCKET_SIZE);
	printf("Pseudo\t[버켓]\t[버켓](d)\t레코드\n");
	printf("===========================================================\n");
	for (i = 0; i < POW(2, directory->header); ++i) {
		// bit string 생성
		for (j = 0; j < directory->header; ++j) {
			bitStr[j] = (directory->table[i][0] & POW(2, directory->header - j - 1)) > 0 ? '1' : '0';
		}
		if (directory->header == 0) {
			bitStr[0] = '0';
			bitStr[1] = '\0';
		}
		else bitStr[directory->header] = '\0';
		// 디렉터리 내용출력
		printf("[%s]\t[B:%d]", bitStr, directory->table[i][1]);
		if (directory->table[i][1] != prevBucket) {
			prevBucket = directory->table[i][1];
			// 해당버킷 내용출력     
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
	// 디렉터리 내용출력
	printf("%s \n", bitStr);
}
