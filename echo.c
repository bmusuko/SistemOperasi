int main(){
	int result;
	char argc;
	char *argv[10];
    interrupt(0x21, 0x22, &argc, 0, 0);

	while(argc > 1){
		interrupt(0x21, 0x23, 0, argv, 0);
		interrupt(0x21, 0x00, argv, 0, 0);
		interrupt(0x21, 0x00, " ",0,0);
		argc--;
	} 
	interrupt(0x21,0x23,0,argv,0);
	interrupt(0x21,0x00,argv,0,0);
	interrupt(0x21, 0x00, "\r\n", 0, 0);
	interrupt(0x21, 0x07, &result, 0, 0);
}