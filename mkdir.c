#define INSUFFICIENT_ENTRIES -3
#define ALREADY_EXISTS -2
#define NOT_FOUND -1

void main() {
    char i,currentDir,argc,j;
    char argv[4][32];
    int success;

    enableInterrupts();
    interrupt(0x21, 0x21, &currentDir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);

    for (i = 0; i < argc; i++) {
        interrupt(0x21, 0x23, i, argv[i], 0);
    }
    if (argc > 0) {
        j = 0;
        while (j<argc){
              interrupt(0x21, (currentDir << 8) | 0x08, argv[j], &success, 0);
            if (success == ALREADY_EXISTS) {
                interrupt(0x21, 0x00, "Directory Sudah Ada\r\n", 0, 0);
            } else if (success == NOT_FOUND){
                interrupt(0x21, 0x00, "Directory Tidak Ditemukan\r\n", 0, 0); 
            } else if (success == INSUFFICIENT_ENTRIES){
                interrupt(0x21, 0x00, "Memory Tidak Cukup\r\n", 0, 0);
            }else{
                interrupt(0x21, 0x00, "Direktori berhasil dibuat\r\n", 0, 0);
            }
            j++;
        }
    }
    interrupt(0x21, (0x00 << 8) | 0x07, &success, 0, 0);
}