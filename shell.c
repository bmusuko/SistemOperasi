#define MAX_BYTE 256
#define SECTOR_SIZE 512
#define MAX_FILES 16
#define MAX_FILENAME 12
#define MAX_DIRS 32
#define MAX_SECTORS 20
#define DIRS_ENTRY_LENGTH 16
#define FILES_ENTRY_LENGTH 16
#define SECTORS_ENTRY_LENGTH 16
#define INSUFFICIENT_ENTRIES -3
#define ALREADY_EXISTS -2
#define SUCCESS 0
#define MAP_SECTOR 0x100
#define DIRS_SECTOR 0x101
#define FILES_SECTOR 0x102
#define SECTORS_SECTOR 0x103
#define TRUE 1
#define FALSE 0
#define INSUFFICIENT_SECTORS 0
#define NOT_FOUND -1
#define INSUFFICIENT_DIR_ENTRIES -1
#define EMPTY 0x00
#define USED 0xFF

void init(char *s,int size){
	int i;
	for(i = 0;i<size;i++){
		s[i] = '\0';
	}
}

int stringCmp(char *a, char *b, int len) {
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

main() {
	int i;
    char curdir;
    char argc;
    char argv[4][16];
    char currentDir;
	char pathDir;
	char input[1000];
	char dirs[SECTOR_SIZE];
	int complete;
	int idx;
	int temp[4];
	interrupt(0x21, 0x21, &curdir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);
    for (i = 0; i < argc; ++i) {
        interrupt(0x21, 0x23, i, argv[i], 0);
    }
    while(1) {

        interrupt(0x21, 0x21, &currentDir, 0, 0);
        interrupt(0x21, 0x00, "$", 0, 0);
        interrupt(0x21, 0x01, input, 0, 0);
        interrupt(0x21, 0x00, "\r\n", 0, 0);

		if (stringCmp(input, "cd", 2)) {
			pathDir = currentDir;
			idx = 2;
			while (input[idx] != '\n') {
				if (input[idx + 1] == 0) {
					interrupt(0x21, 0x20, 0xFF, 0, 0);
				} else if (input[idx] == ' ') {
					interrupt(0x21, 0x30, input+idx, temp, &pathDir);
					if (temp[2] == NOT_FOUND) {
						interrupt(0x21, 0x00, "Direktori Tidak Ditemukan!\r\n", 0, 0);
					} else {
						interrupt(0x21, 0x02, dirs, DIRS_SECTOR, 0);
						interrupt(0x21, pathDir<<8 | 0x31, dirs, input+idx, temp);
						if (temp[2] == NOT_FOUND) {
							interrupt(0x21, 0x00, "Direktori Tidak Ditemukan!\r\n", 0, 0);
						} else {
							currentDir = (char) temp[2];
							interrupt(0x21, 0x20, currentDir, 0, 0);
						}
					}
				}
				idx++;		
			}
		}else if(stringCmp(input, "ls",2)){
            char dirs[SECTOR_SIZE];
            char files[SECTOR_SIZE];
            char nama[19];
            char namaFile[16];
            int i,j;

            interrupt(0x13, 0x201, dirs, div(DIRS_SECTOR, 36) * 0x100 + mod(DIRS_SECTOR, 18) + 1, mod(div(DIRS_SECTOR, 18), 2) * 0x100);
            interrupt(0x13, 0x201, files, div(FILES_SECTOR, 36) * 0x100 + mod(FILES_SECTOR, 18) + 1, mod(div(FILES_SECTOR, 18), 2) * 0x100);

            init(nama,18);
            init(namaFile,15);
            for (i = 0;i<MAX_DIRS;i++){
                if(dirs[i*SECTORS_ENTRY_LENGTH] == currentDir){
                    for (j = 1;j<MAX_DIRS;j++){
                        nama[j-1] = dirs[(i*SECTORS_ENTRY_LENGTH)+j];
                        nama[15] = 'd';
                        nama[16] = 'i';
                        nama[17] = 'r';
                        nama[18] = '\r\n';
                    }
                    interrupt(0x21, 0x00, nama, 0, 0);
                }
            }
            for (i = 0;i<MAX_DIRS;i++){
                if(files[i*SECTORS_ENTRY_LENGTH] == currentDir){
                    for (j = 1;j<MAX_DIRS;j++){
                        namaFile[j-1] = files[(i*SECTORS_ENTRY_LENGTH)+j];
                    }
                    namaFile[15] = '\r\n';
                    interrupt(0x21, 0x00, namaFile, 0, 0);
                }
            }
            interrupt(0x13, 0x301, dirs, div(DIRS_SECTOR, 36) * 0x100 + mod(DIRS_SECTOR, 18) + 1, mod(div(DIRS_SECTOR, 18), 2) * 0x100);
            interrupt(0x13, 0x301, files, div(FILES_SECTOR, 36) * 0x100 + mod(FILES_SECTOR, 18) + 1, mod(div(FILES_SECTOR, 18), 2) * 0x100);
        }else if(stringCmp(input,"mkdir",5)){
            char namaDir[100];
            char dirs[SECTOR_SIZE];
            char files[SECTOR_SIZE];
            char nama[18];
            char namaFile[15];
            int i,j,result;

            interrupt(0x13, 0x201, dirs, div(DIRS_SECTOR, 36) * 0x100 + mod(DIRS_SECTOR, 18) + 1, mod(div(DIRS_SECTOR, 18), 2) * 0x100);
            interrupt(0x13, 0x201, files, div(FILES_SECTOR, 36) * 0x100 + mod(FILES_SECTOR, 18) + 1, mod(div(FILES_SECTOR, 18), 2) * 0x100);

            i = 6;
            while(input[i]!='\0'){
                namaDir[i-6] = input[i];
                i++;
            }
            interrupt(0x21,0x08,namaDir,&result,0);
            // makeDirectory(namaDir,&result,currentDir);
            if(result == INSUFFICIENT_ENTRIES){
                // printString("Memory Tidak Cukup");
                interrupt(0x21, 0x00, "Memory Tidak Cukup", 0, 0);
            }else if(result == ALREADY_EXISTS){
                interrupt(0x21, 0x00, "Directory Sudah Ada", 0, 0);
                // printString("Directory Sudah Ada");
            }
            interrupt(0x13, 0x301, dirs, div(DIRS_SECTOR, 36) * 0x100 + mod(DIRS_SECTOR, 18) + 1, mod(div(DIRS_SECTOR, 18), 2) * 0x100);
            interrupt(0x13, 0x301, files, div(FILES_SECTOR, 36) * 0x100 + mod(FILES_SECTOR, 18) + 1, mod(div(FILES_SECTOR, 18), 2) * 0x100);
        } else if (stringCmp(input, "cat", 3)) {
            char currentDir;
            char argc;
            char argv[20];
            char temp[SECTOR_SIZE * MAP_SECTOR];
            char inputIsi[128];
            int  result;
            char type = 0;
            int length = SECTOR_SIZE * MAP_SECTOR;

            //get currentDir
            interrupt(0x21, 0x21, &currentDir, 0, 0);
            //get argc
            interrupt(0x21, 0x22, &argc, 0, 0);
            //get args
            for (int i = 0; i < argc; i++) {
                interrupt(0x21, 0x23, i, argv[i], 0);
            }

            if (argc > 0) {
                if (argc > 1) {
                    if (stringCmp("-w", argv[1], 2)) {
                        type = 1;
                    }
                }

                if (type == 1) {
                    interrupt(0x21, 0x00, "Masukan isi file: \r\n", 0, 0);
                    interrupt(0x21, 0x01, inputIsi, 0, 0);
                    interrupt(0x21, (currentDir << 8) | 0x05, inputIsi, argv[0], &result);
                    if (result == 0) {
                        interrupt(0x21, 0x00, "File sudah ditulis\r\n", 0, 0);
                    }
                } else if (type == 0) {
                    interrupt(0x21, (currentDir << 8) | 0x04, inputIsi, argv[0], &result);
                    if (result == 0) {
                        interrupt(0x21, 0x00, inputIsi, 0, 0);
                        interrupt(0x21, 0x00, "\n", 0, 0);
                    } else {
                        interrupt(0x21, 0x00, "File tidak ditemuka", 0, 0);
                    }
                }
            }
            interrupt(0x21, (0XFF << 8) | 0x06, "shell", 0x2000, result);
        }
	}
    return 0;
}