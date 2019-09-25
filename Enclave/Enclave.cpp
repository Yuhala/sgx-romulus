/**
 * Author: Peterson Yuhala
 * sgx-remus: sgx compatible nvram library based on romulus
 */


#include "Enclave.h"
#include "sgx_trts.h"
#include "sgx_thread.h" //for thread manipulation
#include "Enclave_t.h" /* print_string */
#include <stdarg.h>
#include "pfences.h"

/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
int printf(const char* fmt, ...)
{
    char buf[BUFSIZ] = { '\0' };
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
    return (int)strnlen(buf, BUFSIZ - 1) + 1;
}


void ecall_repeat_ocalls(unsigned long nrepeats, int use_switchless) {
    sgx_status_t(*ocall_fn)(void) = use_switchless ? ocall_empty_switchless : ocall_empty;
    while (nrepeats--) {
        ocall_fn();
    }
}

void ecall_empty(void) {}
void ecall_empty_switchless(void) {}


/* Test cache line flush instructions inside enclave runtime */
void ecall_test_pwb(int *data){

    *data = 4;
    PWB(data);
    PFENCE();

    printf("Address flush and fence worked without error\n");
}

/* Worker: core data structure manipulations initialize from here */
void ecall_nvram_worker(/*pointer to persistent header*/){
    printf("Inside nvram worker\n");
}
