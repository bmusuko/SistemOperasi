#include "proc.h"
#define MAX_BYTE 256
#define ARGS_SECTOR 512
#define SECTOR_SIZE 512
#define MAX_FILES 32
#define MAX_FILENAME 16
#define MAX_DIRS 32
#define MAX_SECTORS 32
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
#define INSUFFICIENT_SEGMENTS -2

void printInt(int i);
void show();

int main() {
    int* result;
	enableInterrupts();
    show();
    // interrupt(0x21, 0x35, 0, 0, 0);
	interrupt(0x21, 0x07, &result, 0, 0);
}

void show() {
    char files[SECTOR_SIZE];
	int i;
	char temp1;
	unsigned int temp2;
	char* temp3;
	readSector(files, FILES_SECTOR);
	setKernelDataSegment();
	for(i = 0; i < MAX_SEGMENTS; ++i) {
		if (memoryMap[i] == SEGMENT_USED) {
			temp1 = pcbPool[i].index;
			temp3 = "  |  ";
			restoreDataSegment();
			setKernelDataSegment();
			temp2 = (pcbPool[i].segment >> 12) - 2;
			printInt(temp2);
			restoreDataSegment();
			setKernelDataSegment();
			printString(temp3);
			restoreDataSegment();
			readSector(files, FILES_SECTOR);
			printString(files + temp1 * FILES_ENTRY_LENGTH + 1);
			setKernelDataSegment();
			printString("\r\n");
			restoreDataSegment();
			setKernelDataSegment();
		}
	}
	restoreDataSegment();
}

void printInt(int i) {
    char ir = '0' + (char) div(i, 100);
	char ip = '0' + (char) div(mod(i, 100), 10);
	char is = '0' + (char) mod(i, 10);
	interrupt(0x10, 0xE00 + ir, 0, 0, 0);
	interrupt(0x10, 0xE00 + ip, 0, 0, 0);
	interrupt(0x10, 0xE00 + is, 0, 0, 0);
}