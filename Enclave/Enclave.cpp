/**
 * Author: Peterson Yuhala
 * sgx-remus: sgx compatible nvram library based on romulus
 */

#include "Enclave.h"
#include "sgx_trts.h"
#include "sgx_thread.h" //for thread manipulation
#include "Enclave_t.h"  /* print_string */
#include <stdarg.h>
#include "romulus/datastructures/TMStack.hpp"

//romAttrib *romAttrib_out; //contains marshalled properties of outside romulus object
uint8_t *base_addr_in = NULL;    //this will receive the value of the

/* 
 * sgx_printf: 
 * Invokes OCALL to display the enclave buffer to the terminal.
 */

int sgx_printf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
    return (int)strnlen(buf, BUFSIZ - 1) + 1;
}
void abort_h()
{
    sgx_printf("called abort from rom sgx\n");
}

int __cxa_thread_atexit(void (*dtor)(void *), void *obj,
                        void *dso_symbol) throw()
{
    //sgx_printf("called thread atexit");
    return 0;
}

//called by romulus constructor if mmapped file is corrupted
void do_create_file()
{
    sgx_printf("Romulus mmapped file not good\n");
    ocall_createFile();
}

void do_close()
{
    my_ocall_close();
}

void empty_ecall(){
    sgx_printf("Inside empty ecall\n");
}

void ecall_init(uint8_t *base_addr_out)
{
    //validity checks: TODO
    sgx_printf("Inside ecall init\n");
    base_addr_in = base_addr_out;

    //start worker: create romulus persistent objects..
    //do_work();
}


/* Worker: core data structure manipulations initialize from here */
void ecall_nvram_worker(/*pointer to persistent header*/)
{
    sgx_printf("Inside nvram worker\n");
}

void do_work()
{
    sgx_printf("in do work\n");
    return;
    // Create an empty stack in PM and save the root pointer (index 0) to use in a later tx
    /*RomulusLog::updateTx([&]() {
        sgx_printf("Creating persistent stack...\n");
        PStack *pstack = RomulusLog::tmNew<PStack>();
        RomulusLog::put_object(0, pstack);
    });

    // Add two items to the persistent stack
    RomulusLog::updateTx([&]() {
        PStack *pstack = RomulusLog::get_object<PStack>(0);
        pstack->push(33);
        pstack->push(44);
    });

    // Pop two items from the persistent stack
    RomulusLog::updateTx([&]() {
        PStack *pstack = RomulusLog::get_object<PStack>(0);
        sgx_printf("Popped two items: %ld and %ld\n", pstack->pop(), pstack->pop());
        // This one should be "EMTPY" which is 999999999
        sgx_printf("Popped one more: %ld\n", pstack->pop());
    });

    // Delete the persistent stack from persistent memory
    RomulusLog::updateTx([&]() {
        sgx_printf("Destroying persistent stack...\n");
        PStack *pstack = RomulusLog::get_object<PStack>(0);
        RomulusLog::tmDelete(pstack);
        RomulusLog::put_object<PStack>(0, nullptr);
    });*/
}
