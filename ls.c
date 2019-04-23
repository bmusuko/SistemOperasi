#define SECTOR_SIZE 512
#define DIRS_SECTOR 0x101
#define FILES_SECTOR 0x102
#define MAX_FILES 16
#define DIRS_ENTRY_LENGTH 16

void initStr(char* string, int ln);

void main(){
    char dirs[SECTOR_SIZE];
    char files[SECTOR_SIZE];
    char nama[16];
    char currentDir;
    int i,j;

    enableInterrupts();
    interrupt(0x21, 0x21, &currentDir, 0, 0);
    interrupt(0x21, 0x02, dirs, DIRS_SECTOR, 0);
    interrupt(0x21, 0x02, files, FILES_SECTOR, 0);
    
    for(i=0;i<MAX_FILES;i++){
        if(dirs[i*DIRS_ENTRY_LENGTH] == currentDir && dirs[i*DIRS_ENTRY_LENGTH+1] != '\0'){
            initStr(nama,16);
            for (j = 1;j<16;j++){
                nama[j-1] = dirs[(i*DIRS_ENTRY_LENGTH)+j];
            }
            interrupt(0x21, 0x00, nama, 0, 0);
            interrupt(0x21, 0x00, " dir\r\n", 0, 0);
        }
    }

    for(i=0;i<MAX_FILES;i++){
        if(files[i*DIRS_ENTRY_LENGTH] == currentDir && files[i*DIRS_ENTRY_LENGTH+1] != '\0'){
            initStr(nama,16);
            for (j = 1;j<16;j++){
                nama[j-1] = dirs[(i*DIRS_ENTRY_LENGTH)+j];
            }
            interrupt(0x21, 0x00, files+(i*DIRS_ENTRY_LENGTH+1), 0, 0);
            interrupt(0x21, 0x00, "\r\n", 0, 0);
        }
    }
    
    interrupt(0x21, 0x00, "\r\n", 0, 0);
    interrupt(0x21, 0x07, &i, 0, 0);
}

void initStr(char* string, int ln){
    int i;
    for (i = 0;i<ln;i++){
        string[i] = '\0';
    }
}

