int main() {
    int* result;
	enableInterrupts();
    interrupt(0x21, 0x35, 0, 0, 0);
	interrupt(0x21, 0x07, &result, 0, 0);
}