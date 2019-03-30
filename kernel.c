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



void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
int mod(int a, int b);
int div(int a, int b);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void clear(char *buffer, int length);
void deleteFile(char *path, int *result, char parentIndex);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void executeProgram(char *filename, int segment, int *success);

// int main() {
//     char xxx[1000];
//     int s;
//     makeInterrupt21();
//     //handleInterrupt21(0x6,"keyproc",0x2000,s);
//     handleInterrupt21(0x4,xxx,"key.txt",s);
//     handleInterrupt21(0x0,xxx,0,0);
//     //Kode akses : CY7aJVLa
//     while (1);
// }

void handleInterrupt21 (int AX, int BX, int CX, int DX) {
  char AL, AH;
  AL = (char) (AX);
  AH = (char) (AX >> 8);
  switch (AL) {
    case 0x00:
      printString(BX);
      break;
    case 0x01:
      readString(BX);
      break;
    case 0x02:
      readSector(BX, CX);
      break;
    case 0x03:
      writeSector(BX, CX);
      break;
    case 0x04:
      readFile(BX, CX, DX, AH);
      break;
    case 0x05:
      writeFile(BX, CX, DX, AH);
      break;
    case 0x06:
      executeProgram(BX, CX, DX, AH);
      break;
    case 0x07:
      terminateProgram(BX);
      break;
    case 0x08:
      makeDirectory(BX, CX, AH);
      break;
    case 0x09:
      deleteFile(BX, CX, AH);
      break;
    case 0x0A:
      deleteDirectory(BX, CX, AH);
      break;
    case 0x20:
      putArgs(BX, CX);
      break;
    case 0x21:
      getCurdir(BX);
      break;
    case 0x22:
      getArgc(BX);
      break;
    case 0X23:
      getArgv(BX, CX);
      break;
    default:
      printString("Invalid interrupt");
  }
}

void printString(char *string) {
    int i = 0;
    char c;
    c = string[i];
    while (c!='\0'){
        interrupt(0x10, 0xE00 + c, 0, 0, 0);
        i++;
        c = string[i];
    }
}

void readString(char *string){
    int i = 0;
    char c = interrupt(0x16, 0, 0, 0, 0);
    string[i] = c;
    while (c!='\r'){
        if ((c=='\b')&&(i!=0)){
            interrupt(0x10, 0xE00 + '\b', 0, 0, 0);
            interrupt(0x10, 0xE00 + '\0', 0, 0, 0);
            interrupt(0x10, 0xE00 + '\b', 0, 0, 0); 
            string[i]='\0'; 
            i--;
            string[i]='\0';     
            i--;      
        }else{
            interrupt(0x10, 0xE00 + c, 0, 0, 0);
        }
        c = interrupt(0x16, 0, 0, 0, 0);
        i++;
        string[i] = c;
    }
    string[i] = '\0';
    interrupt(0x10, 0xE00 + '\r', 0, 0, 0);
    interrupt(0x10, 0xE00 + '\n', 0, 0, 0);
}   


int mod(int a, int b) {
   while(a >= b) {
      a = a - b;
   }
   return a;
}

int div(int a, int b) {
   int q = 0;
   while(q*b <= a) {
      q = q+1;
   }
   return q-1;
}

void readSector(char *buffer, int sector){
    interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char *buffer, int sector){
    interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}
int isFile(char *path,int pointer){
	while(path[pointer]){
		if(path[pointer] == '/'){
			return FALSE;
		}
		pointer++;
	}
	return TRUE;
}


void init(char *s,int size){
	int i;
	for(i = 0;i<size;i++){
		s[i] = '\0';
	}
}
void readFile(char *buffer, char *path, int *result, char parentIndex){
	int pointerDirs;
	int pointerPath = 0;
	int i,j,k,l,found;
	int pointerNamaFile = 0;
	int pointerSector=0; 
	char directory[15];
	char dirs[SECTOR_SIZE];
	char files[SECTOR_SIZE];
	char sectors[SECTORS_SECTOR];
	readSector(dirs,DIRS_SECTOR);
	char parent = parentIndex;
	while(!isFile(path,pointerPath)){
		init(directory,15);
		i = 0;
		while(path[pointerPath] != '/'){
			directory[i] = path[pointerPath]; // directory nama dir paling kiri
			i++; 
			pointerDirs++;
		}
		found = FALSE;
		pointerDirs = 0;
		while(pointerDirs<MAX_DIRS && !found){
			if(dirs[pointerDirs * DIRS_ENTRY_LENGTH] == parent){
				j = 0;
				found = TRUE;
				while(j<i){
					if(dirs[(pointerDirs*DIRS_ENTRY_LENGTH)+j+1] != directory[j]){
						pointerDirs++;
						found = FALSE;
						break;
					}
					j++;
				}
				if(found){
					if(i<15){
						if(dirs[(pointerDirs*DIRS_ENTRY_LENGTH)+j+1] != '\0'){
							found = FALSE;
							pointerDirs++;
						}
					}
				}
			} else{
				pointerDirs++;
			}
		}
		pointerPath++;
		if(!found){
			*result = NOT_FOUND;
			return;
		} else{
			parent = pointerDirs;
		}
	}
	readSector(files,FILES_SECTOR);
	found = FALSE;
	j = 0;

	while(!found && pointerNamaFile<MAX_FILES){
		if(files[pointerNamaFile*FILES_ENTRY_LENGTH] == parent){
			k = 0;
			l = pointerPath;
			found = TRUE;
			while(path[l] != '\0'){
				if(path[l] != files[(pointerNamaFile*FILES_ENTRY_LENGTH)+1+k]){
					pointerNamaFile++;
					found = FALSE;
					break;
				} else{
					l++;
					k++;
				}
			}
			if(found){
				if(k<15){
					if(files[(pointerNamaFile*FILES_ENTRY_LENGTH)+1+k] != '\0'){
						found = FALSE;
						pointerNamaFile++;
					}
				}
			}
		} else{
			pointerNamaFile++;
		}
	}
	if(!found){
		*result = NOT_FOUND;
		return;
	}
	readSector(sectors,SECTORS_SECTOR);
	while(pointerSector < SECTORS_ENTRY_LENGTH && sectors[pointerNamaFile * SECTORS_ENTRY_LENGTH + pointerSector] != '\0') {
		readSector(buffer + pointerSector * SECTOR_SIZE, sectors[pointerNamaFile * SECTORS_ENTRY_LENGTH + pointerSector]);
		pointerSector++;
	} 
	*result = pointerNamaFile;

}


void clear(char *buffer, int length) {
   int i;
   for(i = 0; i < length; ++i) {
      buffer[i] = EMPTY;
   }
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex){
	int pointerDirs;
	int pointerPath = 0;
	int i,j,k,l,found;
	int pointerNamaFile = 0;
	int pointerSector=0; 
	int pointerFileKosong = 0; 
	char directory[15];
	char dirs[SECTOR_SIZE];
	char files[SECTOR_SIZE];
	char map[SECTOR_SIZE];
	readSector(dirs,DIRS_SECTOR);

	char parent = parentIndex;
	int pointerMap = 0;
	readSector(map,MAP_SECTOR);
	while(pointerMap<SECTOR_SIZE){
		if(map[pointerMap] == '\0'){
			break;
		} else{
			pointerMap++;
		}	
	}
	if(pointerMap == SECTOR_SIZE){
		*sectors = INSUFFICIENT_SECTORS;
		return;
	} else{ // cek files
		readSector(files,FILES_SECTOR);
		while(pointerFileKosong<MAX_FILES){
			if(files[(pointerFileKosong*FILES_ENTRY_LENGTH)+1] == '\0'){
				break;
			} else{
				pointerFileKosong++;
			}
		}
		if(pointerFileKosong == MAX_FILES){
			*sectors = INSUFFICIENT_ENTRIES;
			return; 
		}
	}

	while(!isFile(path,pointerPath)){
		init(directory,15);
		i = 0;
		while(path[pointerPath] != '/'){
			directory[i] = path[pointerPath]; // directory nama dir paling kiri
			i++; 
			pointerDirs++;
		}
		found = FALSE;
		pointerDirs = 0;
		while(pointerDirs<MAX_DIRS && !found){
			if(dirs[pointerDirs * DIRS_ENTRY_LENGTH] == parent){
				j = 0;
				found = TRUE;
				while(j<i && found){
					if(dirs[(pointerDirs*DIRS_ENTRY_LENGTH)+j+1] != directory[j]){
						pointerDirs++;
						found = FALSE;
					}
					j++;
				}
				if(found){
					if(i<15){
						if(dirs[(pointerDirs*DIRS_ENTRY_LENGTH)+j+1] != '\0'){
							found = FALSE;
							pointerDirs++;
						}
					}
				}
			} else{
				pointerDirs++;
			}
		}
		pointerPath++;
		if(!found){
			*sectors = NOT_FOUND;
			return;
		} else{
			parent = pointerDirs;
		}
	}
	while(!found && pointerNamaFile<MAX_FILES){
		if(files[pointerNamaFile*FILES_ENTRY_LENGTH] == parent){
			k = 0;
			l = pointerPath;
			found = TRUE;
			while(path[l] != '\0'){
				if(path[l] != files[(pointerNamaFile*FILES_ENTRY_LENGTH)+1+k]){
					pointerNamaFile++;
					found = FALSE;
					break;
				} else{
					l++;
					k++;
				}
			}
			if(found){
				if(k<15){
					if(files[(pointerNamaFile*FILES_ENTRY_LENGTH)+1+k] != '\0'){
						found = FALSE;
						pointerNamaFile++;
					}
				}
			}
		} else{
			pointerNamaFile++;
		}
	}
	if(found){
		*sectors = ALREADY_EXISTS;
		return;
	} else{
		j = 1;
		files[pointerFileKosong*FILES_ENTRY_LENGTH] = parent;
		while(path[pointerPath] != '\0'){
			files[(pointerFileKosong*FILES_ENTRY_LENGTH)+j] = path[pointerPath];
			j++;
			pointerPath;  
		}
	}
	map[pointerMap] = 0xFF;
	while(sectors[pointerSector*SECTOR_SIZE] != '\0'){
		pointerMap = 0;
		while(pointerMap<SECTOR_SIZE){
			if(map[pointerMap] == '\0'){
				break;
			} else{
				pointerMap++;
			}	
		}
		writeSector(buffer+pointerSector * SECTOR_SIZE,pointerMap);
		sectors[pointerFileKosong * SECTORS_ENTRY_LENGTH+pointerSector] = pointerMap;
		map[pointerMap] = 0xFF;
		pointerSector++;
	}
	writeSector(map,MAP_SECTOR);
	writeSector(files,FILES_SECTOR);
	writeSector(sectors,SECTORS_SECTOR);
}

void deleteFile(char *path, int *result, char parentIndex){
	int pointerDirs;
	int pointerPath = 0;
	int i,j,k,l,found;
	int pointerNamaFile = 0;
	int pointerSector=0; 
	int pointerFileKosong = 0; 
	char directory[15];
	char dirs[SECTOR_SIZE];
	char files[SECTOR_SIZE];
	char map[SECTOR_SIZE];
	char sectors[SECTOR_SIZE];
	readSector(dirs,DIRS_SECTOR);

	char parent = parentIndex;
	int pointerMap = 0;

	while(!isFile(path,pointerPath)){
		init(directory,15);
		i = 0;
		while(path[pointerPath] != '/'){
			directory[i] = path[pointerPath]; // directory nama dir paling kiri
			i++; 
			pointerDirs++;
		}
		found = FALSE;
		pointerDirs = 0;
		while(pointerDirs<MAX_DIRS && !found){
			if(dirs[pointerDirs * DIRS_ENTRY_LENGTH] == parent){
				j = 0;
				found = TRUE;
				while(j<i && found){
					if(dirs[(pointerDirs*DIRS_ENTRY_LENGTH)+j+1] != directory[j]){
						pointerDirs++;
						found = FALSE;
					}
					j++;
				}
				if(found){
					if(i<15){
						if(dirs[(pointerDirs*DIRS_ENTRY_LENGTH)+j+1] != '\0'){
							found = FALSE;
							pointerDirs++;
						}
					}
				}
			} else{
				pointerDirs++;
			}
		}
		pointerPath++;
		if(!found){
			*result = NOT_FOUND;
			return;
		} else{
			parent = pointerDirs;
		}
	}
	while(!found && pointerNamaFile<MAX_FILES){
		if(files[pointerNamaFile*FILES_ENTRY_LENGTH] == parent){
			k = 0;
			l = pointerPath;
			found = TRUE;
			while(path[l] != '\0'){
				if(path[l] != files[(pointerNamaFile*FILES_ENTRY_LENGTH)+1+k]){
					pointerNamaFile++;
					found = FALSE;
					break;
				} else{
					l++;
					k++;
				}
			}
			if(found){
				if(k<15){
					if(files[(pointerNamaFile*FILES_ENTRY_LENGTH)+1+k] != '\0'){
						found = FALSE;
						pointerNamaFile++;
					}
				}
			}
		} else{
			pointerNamaFile++;
		}
	}
	if(!found){
		*result = ALREADY_EXISTS;
		return;
	}
	readSector(map,MAP_SECTOR);
	readSector(sectors,SECTORS_SECTOR);
	readSector(files,FILES_SECTOR);
	pointerSector = 0;
	while(sectors[pointerNamaFile*SECTORS_ENTRY_LENGTH+pointerSector] != '\0'){
		map[sectors[pointerSector*SECTORS_ENTRY_LENGTH+pointerSector]] =  0x00;
		pointerSector++;
	}
	files[pointerNamaFile*FILES_ENTRY_LENGTH+1] = '\0'; 
	writeSector(map, MAP_SECTOR);
	writeSector(files, FILES_SECTOR);
	writeSector(sectors, SECTORS_SECTOR);
	*result = SUCCESS;

}

void executeProgram(char *filename, int segment, int *success) {
	char buffer[MAX_SECTORS * SECTOR_SIZE];
	int i;
	readFile(buffer, filename,success);
	if (*success == TRUE){
		for (i = 0; i<MAX_SECTORS * SECTOR_SIZE ; i++){
			putInMemory(segment, i, buffer[i]);
		}
		launchProgram(segment);
	}
}
