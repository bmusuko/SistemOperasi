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
int strcmp(char *a, char *b, int len);


int main() {
    char curDir, useDir; // curDir = current directory 0xFF
    char *argv[16];
    char argc;  // jumlah argumen
    char input[SECTOR_SIZE];
    char string[16][128];
    char dirs[SECTOR_SIZE];
    char runBackground; 
    int success=1, offset, newIdx;
    int i,j,temp,result; // i =indeks string input
    int found;
    enableInterrupts();
    interrupt(0x21, 0x21, &curDir, 0, 0);

    while(1){
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
            argc--;
            runBackground = 1;
        }

        // execute program
        if (strcmp(input, "cd", 2)) {
            offset = 2;
            newIdx = offset;
            interrupt(0x21, 0x02, dirs, DIRS_SECTOR, 0);
            while (input[newIdx] == ' ' && input[newIdx] != '\0') { //untuk melewati space antara command cd dan argumen selanjutnya
                newIdx++;
            }
            if (input[newIdx] == 0) {
                interrupt(0x21, 0x20, 0xFF, 0, 0); // jika hanya argumen cd, akan kembali ke root directory
                interrupt(0x21,0x00,"BACK TO ROOT\r\n",0,0);
            } else {
                i = 0;
                found = FALSE;
                while(i<MAX_DIRS && !found){
                    j = 0;
                    while(dirs[i*DIRS_ENTRY_LENGTH+j+1] == input[newIdx+j] && j<DIRS_MAX_LENGTH && dirs[i*DIRS_ENTRY_LENGTH+j+1] != '\0' && input[newIdx+j] != '\0'){
                        j++;
                    }
                    if(j==DIRS_MAX_LENGTH){
                        found = TRUE;
                    } else{
                        if(dirs[i*DIRS_ENTRY_LENGTH+j+1] == '\0' && input[newIdx+j] =='\0'){
                            found = TRUE;
                        }
                    }
                    if(found){
                        curDir = dirs[i*DIRS_ENTRY_LENGTH];
                        interrupt(0x21,0x20,&curDir,0,0);
                        interrupt(0x21,0x00,"berhasil cd\r\n",0,0);
                        break;
                    } else{
                        i++;
                    }
                }
                if(!found){
                    interrupt(0x21,0x00,"NOT FOUND\r\n",0,0);
                }
            
            }
        }
        else if (strcmp(input, "pause", 5)) {
            if (argc == 0) {
                interrupt(0x21, 0x00, "pause \r\n", 0, 0);
            } else {
                if (strcmp(argv[0], "0", 1)) {
                    interrupt(0x21, 0x00, "Can't Pause Shell\r\n",0,0);
                } else {
                    interrupt(0x21, 0x32, ((*argv[0] - '0') + 2) * 4096, &result, 0);
                    if (result == NOT_FOUND) {
                        interrupt(0x21, 0x00, "no running process\r\n",0,0);
                    }
                }
            }
        }
        else if (strcmp(input,"resume",6)) {
            if (argc == 0) {
                interrupt(0x21, 0x00, "resume\r\n",0,0 );
            } else {
                interrupt(0x21, 0x33, ((*argv[0] - '0') + 2) * 4096, &result, 0);
                if (result == NOT_FOUND) {
                    interrupt(0x21, 0x00, "no pause process",0,0);
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
                    interrupt(0x21, 0x00, "can't kill shell",0,0);
                } else {
                    interrupt(0x21, 0x34, ((*argv[0] - '0') + 2) * 4096, &result, 0);
                    if (result == NOT_FOUND) {
                        interrupt(0x21, 0x00, "no running process\r\n",0,0);
                    }
                }
            }
        }
        else {
            interrupt(0x21, 0x20, curDir, argc, argv);                // taruh argumen
            interrupt(0x21, (curDir << 8) | 0x06, input, runBackground, &result);     // executeProgram
            if (result == NOT_FOUND) interrupt(0x21, 0x00, "No such program\r\n", 0, 0);
            else if (result == INSUFFICINET_MEMORY)interrupt(0x21, 0x00, "Insufficient memory\r\n", 0, 0);
            
        }

    }
    return 0;
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