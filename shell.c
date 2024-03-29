// #define MAX_BYTE 256
#define SECTOR_SIZE 512
#define NOT_FOUND -1
#define DIRS_MAX_LENGTH 15
#define DIRS_ENTRY_LENGTH 16
#define MAX_DIRS 32
#define DIRS_SECTOR 0x101
#define TRUE 1
#define FALSE 0
#define INSUFFICINET_MEMORY -3
#define MAX_ENTRIES 32
#define MAX_FILES 32
int strcmp(char *a, char *b, int len);
char indexParentFile(char* path, char parentIndex);


int main() {
    char curDir, useDir; // curDir = current directory 0xFF
    char *argv[16];
    char argc;  // jumlah argumen
    char input[SECTOR_SIZE];
    char string[16][128];
    char dirs[SECTOR_SIZE];
    char runBackground; 
    int success=1, offset, newIdx;
    int i,j,temp[4]; // i =indeks string input
    int found,result;
    enableInterrupts();

    while(1){
    interrupt(0x21, 0x21, &curDir, 0, 0);
        runBackground  = 0;
        result = 0;
        argc = 0;
        i = 0;

        interrupt(0x21, 0x00, "$ ", 0, 0);
        interrupt(0x21, 0x01, input, 0, 0);
        interrupt(0x21, 0x00, "\r\n", 0, 0);


        while (input[i] != '\0') {
            if (input[i] == 0x20) { // 0x20 == space
                argv[argc++] = input + i + 1;
                input[i] = '\0';
            }
            ++i;
        }
        // jika ingin run program di background
        if (strcmp(argv[argc - 1], "&", 1)){
            argv[argc-1] = "\0";
            argc--;
            runBackground = 1;
        }

        // execute program
        if (strcmp(input, "cd", 2)) {
            useDir = curDir;
            offset = 2;
            newIdx = offset;
            if (argc == 0) {
                interrupt(0x21, 0x20, 0xFF, 0, 0); // jika hanya argumen cd, akan kembali ke root directory
                interrupt(0x21,0x00,"BACK TO ROOT\r\n",0,0);
            } 
            else {
                if(indexParentFile(argv[0], curDir) == 0xFE) {
                    interrupt(0x21, 0x00, "No such directory\r\n", 0, 0);
                } else {
                    curDir = indexParentFile(argv[0], curDir);
                    interrupt(0x21,0x20,curDir,0,0);
                    interrupt(0x21, 0x00, "berhasil cd\r\n",0,0);
                }
            }
        }
        else if (strcmp(input, "pause", 5)) {
            if (argc == 0) {
                interrupt(0x21, 0x00, "pause \r\n", 0, 0);
            } else {
                if (strcmp(argv[0], "0", 1)) {
                    interrupt(0x21, 0x00, "tidak dapat pause shell\r\n",0,0);
                } else {
                    interrupt(0x21, 0x32, ((*argv[0] - '0') + 2) << 12, &result, 0);
                    if (result == NOT_FOUND) {
                        interrupt(0x21, 0x00, "process tidak ada\r\n\r\n",0,0);
                    }
                }
            }
        }
        else if (strcmp(input,"resume",6)) {
            if (argc == 0) {
                interrupt(0x21, 0x00, "resume\r\n",0,0 );
            } else {
                interrupt(0x21, 0x33, ((*argv[0] - '0') + 2) << 12, &result, 0);
                if (result == NOT_FOUND) {
                    interrupt(0x21, 0x00, "process tidak ada\r\n",0,0);
                } else {
                    interrupt(0x21, 0x31,0,0,0); // pause the shell
                }
            }
        }
        else if (strcmp(input, "kill", 4)) {
            if (argc == 0) {
                interrupt(0x21, 0x00, "kill\r\n",0,0);
            } else {
                if (strcmp(argv[0],"0", 1)) {
                    interrupt(0x21, 0x00, "tidak dapat kill shell\r\n",0,0);
                } else {
                    interrupt(0x21, 0x34, ((*argv[0] - '0') + 2) << 12, &result, 0);
                    if (result == NOT_FOUND) {
                        interrupt(0x21, 0x00, "process tidak ada\r\n",0,0);
                    }
                }
            }
        }
        else {
            interrupt(0x21, 0x20, curDir, argc, argv);                // taruh argumen
            interrupt(0x21, (0xFF << 8) | 0x06, input, runBackground, &result);     // executeProgram
            if (result == NOT_FOUND){
                interrupt(0x21, 0x00, "Tidak ada program\r\n", 0, 0);
            } else if (result == INSUFFICINET_MEMORY){
                interrupt(0x21, 0x00, "Memory tidak cukup\r\n", 0, 0);
            }
        }

    }
    return 0;
}



char indexParentFile(char* path, char parentIndex) {
    char dirs[SECTOR_SIZE]; //untuk menampung sector yang ingin dicek
    char parent; //menampung parent dari path yang sedang dicari
	/** dirs sudah berisi sector **/
    int found = FALSE;
    int idxDirsname = 0;
    int idxCheckDirs = 0;
    int idxDirs = 0;
    parent = parentIndex;

    /* PROSES */
    //Membaca sector
    interrupt(0x21, 0x2, dirs, DIRS_SECTOR, 0); //memanggil readSector
    
    //Mencari direktori yang sesuai dengan path
    do { 
        found = FALSE; //Inisialisasi setiap pindah parent
        do { 
            if (dirs[idxDirsname * DIRS_ENTRY_LENGTH] == parent) { //jika ditemukan parent
                //Pencarian direktori
                found = TRUE;
                for (idxDirsname = 0; idxDirsname <= MAX_FILES && path[idxCheckDirs + idxDirsname] != '/' && path[idxCheckDirs + idxDirsname] != '\0'; ++idxDirsname) { 
                    if (dirs[(idxDirs * DIRS_ENTRY_LENGTH) + idxDirsname + 1] != path[idxCheckDirs + idxDirsname]) { 
                        //nama direktori pada direktori ke-idxDirs tidak sama dengan path yang dicari                               
                        found = FALSE;
                        ++idxDirs;
                        break;
                    } 
                }
            } 
            else ++idxDirs; //kalau dirs nya tidak sama dengan parent
        } while (!found && idxDirs < MAX_ENTRIES);
        
        //Pencarian berakhir 
        //tidak ditemukan direktori
        if (!found) {
            return 0xFE; 
        }
        //Ditemukannya direktori yang sesuai maka, lanjut ke anak nya direktori itu
        idxCheckDirs += idxDirsname + 1;
        parent = idxDirs; //parent di set direktori yang sudah ditemukan
    } while (path[idxCheckDirs - 1] != '\0');
    
    return parent;
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