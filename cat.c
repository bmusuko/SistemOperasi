#define SUCCESS 0
#define SECTOR_SIZE 512
#define MAX_SECTORS 32

int strcmp(char *a, char *b, int len);

int main() {
	char currentDir, argc;
	char argv[32][32];
	char inputIsiFile[10000];
	int *result;
	int i;
	char type = 0;

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
		//nerima input isi file
		interrupt(0x21, 0x00, "Input isi dari file:\r\n",0,0);
		interrupt(0x21, 0x01, inputIsiFile, 0,0);
		// melakukan nilai input ke file tujuan
		interrupt(0x21, (0xFF << 8) | 0x05, inputIsiFile, argv, result);
		if (result == SUCCESS) {
			interrupt(0x21, 0x00, "File sudah berhasil ditulis.\r\n", 0,0);
		} else {
			interrupt(0x21, 0x00, "File gagal ditulis.\r\n", 0, 0);
		}
	} else if (type == 0) {
		//readfile
		interrupt(0x21, (currentDir << 8) | 0x04, inputIsiFile, argv, result);
		//print
		if (result == SUCCESS) {
			interrupt(0x21, 0x00, inputIsiFile, 0, 0);
			interrupt(0x21, 0x00, "\r\n", 0, 0);
		}

	}

	interrupt(0x21, (currentDir << 8) | 0x07, &result, 0, 0 );
	//return 0;
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