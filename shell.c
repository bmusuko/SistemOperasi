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

void main() {
	char currentDir;
	char pathDir
	char input[1000];
	char dirs[SECTOR_SIZE];
	int complete;
	int idx;
	int temp[4];
	interrupt(0x21, 0x21, &currentDir, 0, 0);
	interrupt(0x21, 0x00, "$", 0, 0);
	interrupt(0x21, 0x01, input, 0, 0);
	interrupt(0x21, 0x00, "\r\n", 0, 0);

	while(1) {
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
		}

		//



	}
}



