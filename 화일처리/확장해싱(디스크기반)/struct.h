#define PAGE_SIZE sizeof(bucket)            //����������������
#define PSEUODO_CODE_SIZE 4                //Ű �� �ִ��Ʈ��Ʈ��
#define POW(x,y) (int) pow(x,y)    //pow�Լ��̿�����
#define InitialBUCKET_COUNT 1   //�ʱ� ���ϼ�
#define BUCKET_SIZE 2   //���� ũ��(���� �� ���ڵ� ��
#define DIRECORY_COUNT 16 //���丮 ��� �� 2^PSEUDO_CODE_SIZE
#define DIRECORY_SIZE * sizeof(int)*3  //���丮 ����� ���� 3�ڸ�

struct Record {        //���ڵ� ����ü
	int key;
	char name[120];
};

struct Bucket {         //��Ŷ ����ü
	int header;
	int recordCnt;
	struct Record record[BUCKET_SIZE];//bucket ����record fields

};

struct Directory {     //���丮 ����ü
	int header;
	int table[DIRECORY_COUNT][3];
	//directory table - index, bucket number, header value
};

struct BlockManager { // Block Manager ����ü
	int blockCnt; // Current Block ����
	int blockTable[DIRECORY_COUNT]; // ��üblock�� ���� �Ҵ� ��Ȳ���field 
};
//Global ����
FILE * fp;
void *HashAddress;             // Hash �ּ�(������)
struct Directory *directory;     // Directory 
struct BlockManager blockManager;// Block manager 
struct Bucket bucket;

int initialize(int file_not_Exist);
int PseudoKeyFunc(int key, int digits);//Pseudo key �Լ� (Ű �����Լ�)
int Insert(int key, char name[120]);//����
int Delete(int key);//����
int Retrieve(int key);// �˻�
void PrintHash();//���
void PrintPseudo(int key);
int Hash(char *key);
int MakeAdress(char *key, int depth);
