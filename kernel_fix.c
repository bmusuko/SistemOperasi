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




void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
int mod(int a, int b);
int div(int a, int b);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void clear(char *buffer, int length);
void getCurdir (char *curdir);
void getArgc (char *argc);
void getArgv (char index, char *argv);
void putArgs (char curdir, char argc, char **argv);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void deleteFileIndex(int idx);
void deleteDirectoryIndex(int idx);
void deleteFile(char *path, int *result, char parentIndex);
void executeProgram(char *path, int segment, int *result, char parentIndex);
void terminateProgram (int *result);
void makeDirectory(char *path, int *result, char parentIndex);
void deleteDirectory(char *path, int *success, char parentIndex);
char cmpArray(char * arr1, char * arr2, int length);
void findDir(char * parent, char * current, char * filename, int * idx, int * result);

int main() {
	int success;
	    char curdir = 0xFF; // root
    char argc = 0;
    char *argv[2];
	makeInterrupt21();
	// interrupt(0x21,0x00,"kernel.c",0,0);
	// printString("ete");
	interrupt(0x21, (0xFF << 8) | 0x00, "Memasuki kernel", 0, 0);
//     //handleInterrupt21(0x6,"keyproc",0x2000,s);
//     handleInterrupt21(0x4,xxx,"key.txt",s);
//     handleInterrupt21(0x0,xxx,0,0);
//     //Kode akses : CY7aJVLa
// //     while (1);
	interrupt(0x21, 0x20, curdir, argc, argv);
	interrupt(0x21, 0xFF << 8 | 0x6, "shell", 0x2000, &success);
	while(1);
}

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
      putArgs(BX, CX,DX);
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
	char directory[16];
	char dirs[SECTOR_SIZE];
	char files[SECTOR_SIZE];
	char sectors[SECTORS_SECTOR];
	char parent = parentIndex;

	readSector(dirs,DIRS_SECTOR);
	while(!isFile(path,pointerPath)){
		init(directory,16);
		i = 0;
		while(path[pointerPath] != '/'){
			directory[i] = path[pointerPath]; // directory nama dir paling kiri
			i++; 
			pointerPath++;
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

	while(!found && pointerNamaFile<MAX_FILENAME){
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
	pointerSector = 0;
	while(pointerSector < SECTORS_ENTRY_LENGTH && sectors[pointerNamaFile * SECTORS_ENTRY_LENGTH + pointerSector] != '\0') {
		readSector(buffer + (pointerSector * SECTOR_SIZE), sectors[(pointerNamaFile * SECTORS_ENTRY_LENGTH) + pointerSector]);
		pointerSector++;
	} 
	*result = SUCCESS;
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
	int jumlahMapKosong = 0;
	char directory[15];
	char dirs[SECTOR_SIZE];
	char files[SECTOR_SIZE];
	char map[SECTOR_SIZE];
	char sector[SECTOR_SIZE];
	char sectorBuffer[SECTOR_SIZE];
	char parent = parentIndex;
	int pointerMap = 0;
	readSector(dirs,DIRS_SECTOR);
	readSector(sector,SECTORS_SECTOR);
	readSector(map,MAP_SECTOR);

	while (pointerMap<MAX_BYTE){
		if(map[pointerMap] == '\0'){
			jumlahMapKosong++;
		}
		pointerMap++;
	}
	
	if(jumlahMapKosong < *sectors){
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
			pointerPath++;
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
	
	found = FALSE;
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
			pointerPath++;  
		}
	}
	for (i = 0, pointerSector = 0; i < MAX_BYTE && pointerSector < *sectors; i++) {
        if (map[i] == 0x00) {
            map[i] = 0xFF;
            sector[pointerFileKosong * SECTORS_ENTRY_LENGTH + pointerSector] = i;
            clear(sectorBuffer, SECTOR_SIZE);
            for (j = 0; j < SECTOR_SIZE; j++) {
                sectorBuffer[j] = buffer[pointerSector * SECTOR_SIZE + j];
            }
            writeSector(sectorBuffer, i);
            pointerSector++;
        }   	 
    }

	writeSector(files,FILES_SECTOR);
	writeSector(map,MAP_SECTOR);
	writeSector(sector,SECTORS_SECTOR);
}

//PUNYA KATING
char cmpArray(char * arr1, char * arr2, int length) {
    int i = 0;
    char equal = TRUE;
    while ((equal) && (i < length)) {
        equal = arr1[i] == arr2[i];
        if (equal) {
            if (arr1[i] == '\0') {
                i = length;
            }
        }
        i++;
    }
    return equal;
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
	char parent = parentIndex;
	int pointerMap = 0;

	readSector(dirs,DIRS_SECTOR);

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

void executeProgram(char *path, int segment, int *result, char parentIndex){
	char buffer[MAX_SECTORS * SECTOR_SIZE];
	int i;
	readFile(buffer, path,result,parentIndex);
	if (*result == SUCCESS){
		printString("\n\r");
		printString("Berhasil membaca");
		printString("\n\r");
		for (i = 0; i<MAX_SECTORS * SECTOR_SIZE ; i++){
			putInMemory(segment, i, buffer[i]);
		}
		launchProgram(segment);
	}
}

void terminateProgram (int *result) {
	char shell[6];
	shell[0] = 's';
	shell[1] = 'h';
	shell[2] = 'e';
	shell[3] = 'l';
	shell[4] = 'l';
	shell[5] = '\0';
	executeProgram(shell, 0x2000, result, 0xFF);
}

//PUNYA KATING
void findDir(char * parent, char * current, char * filename, int * idx, int * result) {
    // Read the segment of the name.
    char name[MAX_FILENAME + 1];
    char i = 0;
    char end;
    char dir[SECTOR_SIZE];
    char j;
    char found;
    if (filename[*idx] == '/') {
        // *parent = DIR_ROOT;
        *idx++;
    }
    while ((filename[*idx + i] != '\0') && (filename[*idx + i] != '/')) {
        name[i] = filename[*idx + i];
        i++;
    }
    // The name is of a file if it ends in a null terminator.
    end = filename[*idx + i] == '\0';
    name[i] = '\0';
    // Read from the respective sector.
    j = i;
    readSector(dir, DIRS_SECTOR);
    // Find the dir/file from the sector.
    found = FALSE;
    i = 0;
    while ((!found) && (i < MAX_DIRS)) {
        char buff[4];
        int k = 0;
        if ((dir[i * DIRS_ENTRY_LENGTH] == *parent) && (cmpArray(name, dir + (i * DIRS_ENTRY_LENGTH) + 1, MAX_FILENAME))) {
            found = TRUE;
        } else {
            i++;
        }
    }
    if (found) {
        *current = i;
        if (end) {
            // Dir found.
            *result = SUCCESS;
        } else {
            // Recursively search for the path.
            *parent = *current;
            *idx = *idx + j + 1;
            findDir(parent, current, filename, idx, result);
        }
    } else {
        *result = NOT_FOUND;
    }
}

void makeDirectory(char *path, int *result, char parentIndex) {
    char parent = parentIndex;
    int i = 0;
    char current;
    findDir(&parent, &current, path, &i, result);
    if (*result == SUCCESS) {
        *result = ALREADY_EXISTS;
    } else {
        char filename[MAX_FILENAME + 1];
        int j = 0;
        while ((path[i + j] != '\0') && (path[i + j] != '/')) {
            filename[j] = path[i + j];
            j++;
        }
        filename[j] = '\0';
        if (path[i + j] == '/') {
            *result = NOT_FOUND;
        } else {
            char dir[SECTOR_SIZE];
            current = 0;
            readSector(dir, DIRS_SECTOR);
            while ((dir[current * DIRS_ENTRY_LENGTH + 1] != '\0') && (current < MAX_DIRS)) {
                current++;
            }
            if (current < MAX_DIRS) {
                dir[current * DIRS_ENTRY_LENGTH] = parent;
                j = 0;
                while (filename[j] != '\0') {
                    dir[current * DIRS_ENTRY_LENGTH + 1 + j] = filename[j];
                    j++;
                }
                writeSector(dir, DIRS_SECTOR);
                *result = SUCCESS;
            } else {
                *result = INSUFFICIENT_ENTRIES;
            }
        }
    }
}

// void makeDirectory(char *path, int *result, char parentIndex){
// 	int pointerDirs = 0;
// 	int pointerDirectory = 0;
// 	int pointerPath = 0;
// 	int pointerFiles;
// 	int freee;
// 	int found;
// 	int sameName;
// 	int i;
// 	int freeDirSection;
// 	char dirs[SECTOR_SIZE];
// 	char files[SECTOR_SIZE];
// 	char directory[15];
// 	char parent = parentIndex;
	

// 	readSector(dirs,DIRS_SECTOR);
	
// 	freee = FALSE;
// 	while (pointerDirs<MAX_DIRS && !freee){
// 		if (dirs[(pointerDirs*DIRS_ENTRY_LENGTH)+1]=='\0'){
// 			freee = TRUE;
// 			freeDirSection = pointerDirs;
// 		}
// 		pointerDirs++;
// 	}
// 	if (freee){
// 		while(!isFile(path,pointerPath)){
// 			if(1){*result = -987;break;}
// 			pointerDirs = 0;
// 			found = FALSE;
// 			pointerDirectory = 0;
// 			init(directory,15);
// 			while(path[pointerPath]!='/'){
// 				directory[pointerDirectory] = path[pointerPath];
// 				pointerPath++;
// 				pointerDirectory++;
// 			}
// 			while(pointerDirs<MAX_DIRS && !found){
// 				if (parent==dirs[pointerDirs*DIRS_ENTRY_LENGTH]){
// 					sameName = TRUE;
// 					i = 1;
// 					pointerDirectory = 0;
// 					while (directory[pointerDirectory]!='\0' && pointerDirectory<DIRS_ENTRY_LENGTH-1 && sameName){
// 						if (directory[pointerDirectory]!=dirs[(pointerDirs*DIRS_ENTRY_LENGTH)+i]){
// 							sameName = FALSE;
// 						}
// 						i++;
// 						pointerDirectory++;
// 					}
// 				}
// 				if (sameName){
// 					found = TRUE;
// 				}else{
// 					pointerDirs++;
// 				}
// 			}
// 			if (found){
// 				parent = dirs[pointerDirs*DIRS_ENTRY_LENGTH];
// 			}else{
// 				*result = NOT_FOUND;
// 				return;
// 			}
// 		}
		
// 		readSector(files,FILES_SECTOR);
// 		found = FALSE;
// 		pointerFiles = 0;
// 		pointerDirectory = 0;
// 		init(directory,15);
// 		while (path[pointerPath]!='\0'){
// 			directory[pointerDirectory] = path[pointerPath];
// 			pointerPath++;
// 			pointerDirectory++;
// 		}
		
// 		while(pointerFiles<MAX_FILES && !found){
// 			if (files[pointerFiles*FILES_ENTRY_LENGTH] == parent){
// 				pointerDirectory = 0;
// 				sameName = TRUE;
// 				i = 1;
// 				while(directory[pointerDirectory]!='\0' && sameName){
// 					if (directory[pointerDirectory] != files[(pointerFiles*FILES_ENTRY_LENGTH)+i]){
// 						sameName = FALSE;
// 					}
// 					pointerDirectory++;
// 					i++;
// 				}
// 				if(sameName){
// 					*result = ALREADY_EXISTS;
// 					return;
// 				}
// 			}
// 			pointerFiles++;
// 		}
		
// 		dirs[freeDirSection*DIRS_ENTRY_LENGTH] = parent;
// 		i = 1;
// 		while (i<16){
// 			if (directory[i-1] == '\0'){
// 				break;
// 			}else
// 			{
// 				dirs[(freeDirSection*DIRS_ENTRY_LENGTH)+i] = directory[i-1];
// 			}
// 			i++;
// 		}
// 		*result = SUCCESS;
// 		writeSector(dirs,DIRS_SECTOR);
// 		writeSector(files,FILES_SECTOR);
// 	}else{
// 		*result = INSUFFICIENT_ENTRIES;
// 		return;
// 	}
// }

void deleteFileIndex(int idx){
	char files[SECTOR_SIZE];
	char map[SECTOR_SIZE];
	char sectors[SECTOR_SIZE];
	int pointerSector;

	readSector(map,MAP_SECTOR);
	readSector(sectors,SECTORS_SECTOR);
	readSector(files,FILES_SECTOR);

	pointerSector = 0;
	while(sectors[idx*SECTORS_ENTRY_LENGTH+pointerSector] != '\0'){
		map[sectors[pointerSector*SECTORS_ENTRY_LENGTH+pointerSector]] =  0x00;
		pointerSector++;
	}
	files[idx*FILES_ENTRY_LENGTH+1] = '\0'; 
	
	writeSector(map, MAP_SECTOR);
	writeSector(files, FILES_SECTOR);
	writeSector(sectors, SECTORS_SECTOR);
}

void deleteDirectory(char *path, int *success, char parentIndex){
	int pointerDirs = 0;
	int pointerDirectory = 0;
	int pointerPath = 0;
	int pointerFiles;
	int found;
	int sameName;
	int i;
	int x;
	int freeDirSection;
	char dirs[SECTOR_SIZE];
	char files[SECTOR_SIZE];
	char directory[15];
	char parent = parentIndex;
	
	readSector(dirs,DIRS_SECTOR);
	
	while(!isFile(path,pointerPath)){
		pointerDirs = 0;
		found = FALSE;
		pointerDirectory = 0;
		init(directory,15);
		while(path[pointerPath]!='/'){
			directory[pointerDirectory] = path[pointerPath];
			pointerPath++;
			pointerDirectory++;
		}
		while(pointerDirs<MAX_DIRS && !found){
			if (parent==dirs[pointerDirs*DIRS_ENTRY_LENGTH]){
				sameName = TRUE;
				i = 1;
				pointerDirectory = 0;
				while (directory[pointerDirectory]!='\0' && pointerDirectory<DIRS_ENTRY_LENGTH-1 && sameName){
					if (directory[pointerDirectory]!=dirs[(pointerDirs*DIRS_ENTRY_LENGTH)+i]){
						sameName = FALSE;
					}
					i++;
					pointerDirectory++;
				}
			}
			if (sameName){
				found = TRUE;
			}else{
				pointerDirs++;
			}
		}
		if (found){
			parent = dirs[pointerDirs*DIRS_ENTRY_LENGTH];
		}else{
			*success = NOT_FOUND;
			return;
		}
	}
	pointerDirs = 0;
	found = FALSE;
	pointerDirectory = 0;
	init(directory,15);
	while(path[pointerPath]!='\0'){
		directory[pointerDirectory] = path[pointerPath];
		pointerPath++;
		pointerDirectory++;
	}
	while(pointerDirs<MAX_DIRS && !found){
		if (parent==dirs[pointerDirs*DIRS_ENTRY_LENGTH]){
			sameName = TRUE;
			i = 1;
			pointerDirectory = 0;
			while (directory[pointerDirectory]!='\0' && pointerDirectory<DIRS_ENTRY_LENGTH-1 && sameName){
				if (directory[pointerDirectory]!=dirs[(pointerDirs*DIRS_ENTRY_LENGTH)+i]){
					sameName = FALSE;
				}
				i++;
				pointerDirectory++;
			}
		}
		if (sameName){
			found = TRUE;
		}else{
			pointerDirs++;
		}
	}
	
	if (found){
		parent = dirs[pointerDirs*DIRS_ENTRY_LENGTH];
		
	}else{
		*success = NOT_FOUND;
		return;
	}
	
	// if (1){*success = -987;return;}
	readSector(files,FILES_SECTOR);
	for (x = 0;x<MAX_FILES;x++){
		if (files[x*FILES_ENTRY_LENGTH] == pointerDirs){
			deleteFileIndex(x);
		}
	}
	for (i = 0;i<MAX_DIRS;i++){
		if (dirs[i*SECTORS_ENTRY_LENGTH] == pointerDirs){
			deleteDirectoryIndex(i);
		}
	}
	*success = SUCCESS;
	dirs[(parent*SECTORS_ENTRY_LENGTH)+1] = '\0';
	writeSector(dirs,DIRS_SECTOR);
	writeSector(files,FILES_SECTOR);
}

void deleteDirectoryIndex(int idx){
	char dirs[SECTOR_SIZE];
	char files[SECTOR_SIZE];
	int i;
	int x;
	readSector(dirs,DIRS_SECTOR);
	readSector(files,FILES_SECTOR);

	for (i = 0;i<MAX_DIRS;i++){
		if (dirs[i*SECTORS_ENTRY_LENGTH] == idx){
			deleteDirectoryIndex(i);
		}
	}
	for (x = 0;x<MAX_FILES;x++){
		if (files[x*FILES_ENTRY_LENGTH] == idx){
			deleteFileIndex(x);
		}
	}
	dirs[(idx*SECTORS_ENTRY_LENGTH)+1] = '\0';
	writeSector(dirs,DIRS_SECTOR);
	writeSector(files,FILES_SECTOR);
}

void putArgs (char curdir, char argc, char **argv) {
	char args[SECTOR_SIZE];
	int i, j, p;
	clear(args, SECTOR_SIZE);
	args[0] = curdir;
	args[1] = argc;
	i = 0;
	j = 0;
	for (p = 2; p < ARGS_SECTOR && i < argc; ++p) {
		args[p] = argv[i][j];
		if (argv[i][j] == '\0') {
			++i;
			j = 0;
		}
		else {
			++j;
		}
	}
	writeSector(args, ARGS_SECTOR);
}

void getCurdir (char *curdir) {
	char args[SECTOR_SIZE];
	readSector(args, ARGS_SECTOR);
	*curdir = args[0];
}

void getArgc (char *argc) {
	char args[SECTOR_SIZE];
	readSector(args, ARGS_SECTOR);
	*argc = args[1];
}

void getArgv (char index, char *argv) {
	char args[SECTOR_SIZE];
	int i, j, p;
	readSector(args, ARGS_SECTOR);
	i = 0;
	j = 0;
	for (p = 2; p < ARGS_SECTOR; ++p) {
		if (i == index) {
			argv[j] = args[p];
			++j;
		}
		if (args[p] == '\0') {
			if (i == index) {
				break;
			}
			else {
				++i;
			}
		}
	}
}