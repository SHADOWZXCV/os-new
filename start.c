extern void main();

__attribute__((section(".start")))
void start() {
	main();
}
