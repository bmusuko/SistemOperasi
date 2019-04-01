// #define MAX_BYTE 256
#define SECTOR_SIZE 512
#define NOT_FOUND -1
#define DIRS_SECTOR 0x101
int stringCmp(char *a, char *b, int len);


void main() {
    char currentDir, useDir;
    char *argv[16];
    char argc;
    char input[SECTOR_SIZE];
    char string[16][128];
    char dir[SECTOR_SIZE];
    int success=1, offset, newIdx;
    int se[4], i,j,temp;
    interrupt(0x21, 0x21, &currentDir, 0, 0);
        // read input
        interrupt(0x21, 0x00, "$ ", 0, 0);
        interrupt(0x21, 0x01, input, 0, 0);
        interrupt(0x21, 0x00, "\r\n", 0, 0);
        // if(0){
        //     interrupt(0x21,0x00,"cd\r\n",0,0);
        // }
        if (strcmp(input, "cd", 2)) {
            offset = 2;
            newIdx = offset;
            while (input[newIdx] == ' ') { //untuk melewati space antara command cd dan argumen selanjutnya
                newIdx++;
            }
            if (input[newIdx] == 0) {
                interrupt(0x21, 0x20, 0xFF, 0, 0); // jika hanya argumen cd, akan kembali ke root directory
            } else {
                if (se[2] == NOT_FOUND) {
                    interrupt(0x21, 0x00, "Tidak ada direktori tersebut.\r\n", 0, 0);
                } else {
                    interrupt(0x21, 0x02, dir, DIRS_SECTOR, 0);
                    if (se[2] == NOT_FOUND) {
                        interrupt(0x21, 0x00, "Tidak ada direktori.\r\n",0,0);
                    } else {
                        currentDir = (char) se[2];
                        interrupt(0x21, 0x20, currentDir, 0, 0);
                    }
                }
            }
        }
        else if(input[0]!=0){
            // Prepare the argument
            useDir = 0xFF;
            offset = 0;
            argc = 0;
            j = 0;
            temp = 0;
            if(strcmp(input, "./", 2)){
                useDir = currentDir;
                offset = 2;
                temp = offset;
            }
            while(input[offset] != '\0' && input[offset] != ' '){ // mencari ending kalimat
                offset++;
            }
            if(input[offset] != '\0'){
                while(1){
                    if(input[offset] =='\0' || input[offset] == ' '){
                        if(j != 0){
                            string[argc][j] = '\0';
                            argv[argc] = string[argc];
                            argc++;
                            j = 0;
                            if(input[offset]=='\0'){
                                break;
                            }
                        }
                    } else{
                        string[argc][j] = input[offset];
                        j++;
                    }
                    offset++;
                }
            }
            interrupt(0x21,0x20,currentDir,argc,argv);
            offset = temp;
            for(i=offset;input[i]!=' ' && input[i]!='\0';i++);
            input[i] = '\0';
            interrupt(0x21, 0xFF<<8|0x6, input, 0x2000, &success);
            if(!success){
                if(success == NOT_FOUND)
                    interrupt(0x21, 0x00, "Program Tidak ada\r\n", 0, 0);
                else
                    interrupt(0x21, 0x00, "Program Error\r\n", 0, 0);
            }
        }
        interrupt(0x21,0x07,&i,0,0);

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