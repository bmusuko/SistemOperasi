#define SUCCESS 0
#define SECTOR_SIZE 512
#define MAX_SECTORS 32

int strcmp(char *a, char *b, int len);
int bagi(int a, int b);
void initArr(char *s,int size);

int main() {
	char currentDir, argc;
	char argv[4][32];
	char inputIsiFile[SECTOR_SIZE*MAX_SECTORS];
    char inputLine[128];
	int result;
	int i,j;
	char type = 0;


    int sectors;

	// get curdir
	interrupt(0x21, 0x21, &currentDir, 0, 0);
	// get argc
	interrupt(0x21, 0x22, &argc, 0, 0);
	//get argv
	if (argc > 0) {
		interrupt(0x21, 0x23, 0, argv[0], 0);
		if (argc > 1) {
			interrupt(0x21, 0x23, 1, argv[1], 0);
			if (strcmp("-w", argv[1], 2)) {
				type = 1;
			} else {
				type = 0;
			}	
		}
	}

	if (type == 1) {
        initArr(inputIsiFile,SECTOR_SIZE*MAX_SECTORS);
        initArr(inputLine,128);
		//nerima input isi file
		interrupt(0x21, 0x00, "Input isi dari file:\r\n",0,0);
		interrupt(0x21, 0x01, inputLine, 0,0);
        i = 0;
        while (inputLine[0] != '\0') {
            j = 0;
            while (inputLine[j] != '\0') {
                inputIsiFile[i] = inputLine[j];
                j++;
                i++;
            }
            inputIsiFile[i] = '\n';
            i++;
            inputIsiFile[i] = '\r';
            i++;
            interrupt(0x21, 0x01, inputLine, 0, 0);
        }
        i--;
        inputIsiFile[i] = '\0';
        i--;
        inputIsiFile[i] = '\0';
        sectors = bagi(i,SECTOR_SIZE)+1;
		// melakukan nilai input ke file tujuan
		interrupt(0x21, (0xFF << 8) | 0x05, inputIsiFile, argv[0], &sectors);
        interrupt(0x21, 0x00, "File sudah berhasil ditulis.\r\n", 0,0);
	} else if (type == 0) {
		//readfile
        initArr(inputIsiFile,SECTOR_SIZE*MAX_SECTORS);
		interrupt(0x21, (currentDir << 8) | 0x04, inputIsiFile, argv[0], &result);
		//print
		if (result == SUCCESS) {
			interrupt(0x21, 0x00, inputIsiFile, 0, 0);
			interrupt(0x21, 0x00, "\r\n", 0, 0);
		}
	}
	interrupt(0x21, (currentDir << 8) | 0x07, &result, 0, 0 );
}

int strcmp(char *a, char *b, int len) {
	int i = 0;
    while (i < len) {
        if (a[i] != b[i]) {
            return 0;
        } else if ((a[i] == '\n') || (b[i] == '\n')) {
            return 0;
        }
        i++;
    }
    return 1;
}

int bagi(int a,int b){
    int m,n,x;
    m = a;
    n = b;
    x = 0;
    while (m>=n){
        m = m-n;
        x++;
    }
    return x;
}

void initArr(char *s,int size){
	int i;
	for(i = 0;i<size;i++){
		s[i] = '\0';
	}
}