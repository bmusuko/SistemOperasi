// #define MAX_BYTE 256
#define SECTOR_SIZE 512
#define NOT_FOUND -1
int stringCmp(char *a, char *b, int len);


void main() {
    char curDir, useDir;
    char *argv[16];
    char argc;
    char input[512];
    char string[16][128];
    char dir[SECTOR_SIZE];
    int success=1, offset;
    int se[4], i,j,temp;
    interrupt(0x21, 0x21, &curDir, 0, 0);
        // read input
        interrupt(0x21, 0x00, "$ ", 0, 0);
        interrupt(0x21, 0x01, input, 0, 0);
        interrupt(0x21, 0x00, "\r\n", 0, 0);
        if(0){
            interrupt(0x21,0x00,"cd\r\n",0,0);
        }
        else if(input[0]!=0){
            // Prepare the argument
            useDir = 0xFF;
            offset = 0;
            argc = 0;
            j = 0;
            temp = 0;
            if(strcmp(input, "./", 2)){
                useDir = curDir;
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
            interrupt(0x21,0x20,curDir,argc,argv);
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