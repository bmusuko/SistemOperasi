int main(){
	int result;
	char argc,i;
	char *argv[20];
	char curdir;
    i = 0;
    interrupt(0x21, 0x22, &argc, 0, 0);
    interrupt(0x21,0x21,&curdir,0,0);
    if(argc>0){ // hanya memakai argument pertama dan mengabaikan argument lain
    	while(i<argc){
        	interrupt(0x21, 0x23, i, argv[i], 0);
       		i++;    		
    	}
    	interrupt(0x21, curdir<<8 | 0x09,argv[0],&result,0);
    	if(result != 0){
    		interrupt(0x21, curdir<<8 | 0x0A,argv[0],&result,0);
    		if(result != 0){
    			interrupt(0x21,0x00,"Tidak dapat melakukan rm\r\n",0,0);
    		}
    	}
    }
    interrupt(0x21, (0x00 << 8) | 0x07, &result, 0, 0);
}