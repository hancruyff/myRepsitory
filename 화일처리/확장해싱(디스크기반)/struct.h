#define PAGE_SIZE sizeof(bucket)            //페이지사이즈정의
#define PSEUODO_CODE_SIZE 4                //키 값 최대비트스트링
#define POW(x,y) (int) pow(x,y)    //pow함수이용정의
#define InitialBUCKET_COUNT 1   //초기 버켓수
#define BUCKET_SIZE 2   //버켓 크기(버켓 당 레코드 수
#define DIRECORY_COUNT 16 //디렉토리 멤버 수 2^PSEUDO_CODE_SIZE
#define DIRECORY_SIZE * sizeof(int)*3  //디렉토리 멤버는 정수 3자리

struct Record {        //레코드 구조체
	int key;
	char name[120];
};

struct Bucket {         //버킷 구조체
	int header;
	int recordCnt;
	struct Record record[BUCKET_SIZE];//bucket 내의record fields

};

struct Directory {     //디렉토리 구조체
	int header;
	int table[DIRECORY_COUNT][3];
	//directory table - index, bucket number, header value
};

struct BlockManager { // Block Manager 구조체
	int blockCnt; // Current Block 개수
	int blockTable[DIRECORY_COUNT]; // 전체block의 현재 할당 상황기록field 
};
//Global 변수
FILE * fp;
void *HashAddress;             // Hash 주소(포인터)
struct Directory *directory;     // Directory 
struct BlockManager blockManager;// Block manager 
struct Bucket bucket;

int initialize(int file_not_Exist);
int PseudoKeyFunc(int key, int digits);//Pseudo key 함수 (키 생성함수)
int Insert(int key, char name[120]);//삽입
int Delete(int key);//삭제
int Retrieve(int key);// 검색
void PrintHash();//출력
void PrintPseudo(int key);
int Hash(char *key);
int MakeAdress(char *key, int depth);
