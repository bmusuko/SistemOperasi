// #include "proc.h"
// #define MAX_BYTE 256
// #define ARGS_SECTOR 512
// #define SECTOR_SIZE 512
// #define MAX_FILES 32
// #define MAX_FILENAME 16
// #define MAX_DIRS 32
// #define MAX_SECTORS 32
// #define DIRS_ENTRY_LENGTH 16
// #define FILES_ENTRY_LENGTH 16
// #define SECTORS_ENTRY_LENGTH 16
// #define INSUFFICIENT_ENTRIES -3
// #define ALREADY_EXISTS -2
// #define SUCCESS 0
// #define MAP_SECTOR 0x100
// #define DIRS_SECTOR 0x101
// #define FILES_SECTOR 0x102
// #define SECTORS_SECTOR 0x103
// #define TRUE 1
// #define FALSE 0
// #define INSUFFICIENT_SECTORS 0
// #define NOT_FOUND -1
// #define INSUFFICIENT_DIR_ENTRIES -1
// #define EMPTY 0x00
// #define USED 0xFF
// #define INSUFFICIENT_SEGMENTS -2

// void printInt(int i);
// void show();

// int main() {
//     int* result;
// 	enableInterrupts();
//     show();
//     interrupt(0x21, 0x35, 0, 0, 0);
// 	interrupt(0x21, 0x07, &result, 0, 0);
// }
