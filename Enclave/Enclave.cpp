/**
 * Author: Peterson Yuhala
 * sgx-remus: sgx compatible nvram library based on romulus
 */

#include "Enclave.h"
#include "sgx_trts.h"
#include "sgx_thread.h" //for thread manipulation
#include "Enclave_t.h"  /* print_string */
#include <stdarg.h>
#include <stdio.h>
//#include <thread>
#include "romulus/datastructures/TMStack.hpp"

//romAttrib *romAttrib_out; //contains marshalled properties of outside romulus object
uint8_t *base_addr_in = NULL; //this will receive the value of the

/* 
 * sgx_printf: 
 * Invokes OCALL to display the enclave buffer to the terminal.
 */
int __cxa_thread_atexit(void (*dtor)(void *), void *obj, void *dso_symbol) {}

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

void do_close()
{
    my_ocall_close();
}

void empty_ecall()
{
    sgx_printf("Inside empty ecall\n");
}

void ecall_init(void *per_out, uint8_t *base_addr_out)
{
    //pointer safety/validity checks: TODO

    base_addr_in = base_addr_out;
    if (base_addr_in == NULL)
    {
        sgx_printf("Base addr null, exiting..\n");
        return;
    }
    romuluslog::gRomLog.per = (PersistentHeader *)per_out;
    sgx_printf("Pmem state: %d\n", romuluslog::gRomLog.per->id); //for testing

    romuluslog::gRomLog.ns_init(); //normally private..

    sgx_printf("Enclave init success..init base address for pmem\n");

    TM_WRITE_TRANSACTION([&]() {
        PArray *parray = RomulusLog::get_object<PArray>(0);
        if (parray == nullptr)
        {
            
            PArray *parray = (PArray *)TM_PMALLOC(sizeof(struct PArray));
            RomulusLog::put_object(0, parray);
            parray->allocate();
            sgx_printf("Created persistent array...\n");
        }
        else
        {
            sgx_printf("Persistent array exists...\n");
        }
    });

    // Create an empty stack in PM and save the root pointer (index 0) to use in a later tx
    /*  TM_WRITE_TRANSACTION([&]() {
        PStack *pstack = RomulusLog::get_object<PStack>(0);
        if (pstack == nullptr)
        {
            sgx_printf("Creating persistent stack...\n");
            PStack *pstack = (PStack *)TM_PMALLOC(sizeof(struct PStack));
            RomulusLog::put_object(0, pstack);
        }
        else
        {
            sgx_printf("Persistent stack exists...\n");
        }
    }); */
}
/* Run SPS benchmark on persistent array */
void ecall_sps(long nswaps, long *ops)
{
    printf("In ecall sps\n");
    PArray *parray = RomulusLog::get_object<PArray>(0);
    parray->do_sps(nswaps,ops);
}
/* Worker: core data structure manipulations initialize from here */
void ecall_nvram_worker(int val, size_t tid)
{
    //start worker: worker pushes and pops values from the start
    do_work(val, tid);
}

void do_work(int val, size_t tid)
{

    //Pops a value from the persistent stack
    TM_WRITE_TRANSACTION([&]() {
        PStack *pstack = RomulusLog::get_object<PStack>(0);
        // sgx_printf("Popped two items: %ld and %ld\n", pstack->pop(), pstack->pop());
        // This one should be "EMTPY" which is 999999999
        sgx_printf("Worker: %d Popped : %ld\n", tid, pstack->pop());
    });

    // Add items to the persistent stack
    TM_WRITE_TRANSACTION([&]() {
        PStack *pstack = RomulusLog::get_object<PStack>(0);
        pstack->push(val);
        //pstack->push(44);
    });

    // Pop two items from the persistent stack
    TM_WRITE_TRANSACTION([&]() {
        PStack *pstack = RomulusLog::get_object<PStack>(0);
        // sgx_printf("Popped two items: %ld and %ld\n", pstack->pop(), pstack->pop());
        // This one should be "EMTPY" which is 999999999
        sgx_printf("Worker: %d Pushed and Popped : %ld\n", tid, pstack->pop());
    });

    // Delete the persistent stack from persistent memory
    /*TM_WRITE_TRANSACTION([&]() {
        sgx_printf("Destroying persistent stack...\n");
        PStack *pstack = RomulusLog::get_object<PStack>(0);
        TM_PFREE(pstack);
        RomulusLog::put_object<PStack>(0, nullptr);
    });*/
}
