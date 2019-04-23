int main(){
	
    int result;
	char argc,i;
	char argv[4][32];
    i = 0;
    enableInterrupts();
    interrupt(0x21, 0x22, &argc, 0, 0);
    if (argc>1){
            while(i < argc){
            interrupt(0x21, 0x23, i, argv, 0);
            interrupt(0x21, 0x00, argv, 0, 0);
            interrupt(0x21, 0x00, " ",0,0);
            i++;
	    }
    }
	else{
        interrupt(0x21,0x23,0,argv,0);
	    interrupt(0x21,0x00,argv,0,0);
    }
	interrupt(0x21, 0x00, "\r\n", 0, 0);
	interrupt(0x21, 0x07, &result, 0, 0);
}