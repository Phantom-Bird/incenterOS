#define UINTN unsigned long long int

UINTN kernel_main(UINTN, UINTN);

__attribute__ ((ms_abi))
UINTN kernel_entry(UINTN a, UINTN b) {
    return kernel_main(a, b);
}

UINTN kernel_main(UINTN a, UINTN b){
    return a + b;
}
