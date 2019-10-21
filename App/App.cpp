
#include <stdio.h>
#include <string.h>

#include <pwd.h>

#include <sgx_urts.h>
#include "App.h"
#include "Enclave_u.h"
#include "ErrorSupport.h"
#include "Romulus_helper.h" //for romulus helper functions
#include <thread>

/* For romulus */

#define MAX_PATH FILENAME_MAX

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;
static int stack_val = 10;

/* Romulus objects */
extern PersistentHeader *per_out;
extern uint8_t *base_addr;
extern uint8_t *real_base_addr;

void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate
     * the input string to prevent buffer overflow.
     */
    printf("%s", str);
}

/* Do munmap and close file */
void my_ocall_close()
{
    close_file();
}

/* Thread function*/
void thread_func()
{
    size_t tid = std::hash<std::thread::id>()(std::this_thread::get_id());
    printf("Thread ID: %d\n", tid);
    stack_val++;// implement mutex/atomic..just for testing anyway
    ecall_nvram_worker(global_eid, stack_val, tid);
}
/* Initialize the enclave:
 *   Call sgx_create_enclave to initialize an enclave instance
 */
int initialize_enclave(void)
{
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;

    /* Call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
    ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, NULL, NULL, &global_eid, NULL);
    if (ret != SGX_SUCCESS)
    {
        print_error_message(ret);
        return -1;
    }

    return 0;
}

/* Application entry */
int SGX_CDECL main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    sgx_status_t ret;

    /* Initialize the enclave */
    if (initialize_enclave() < 0)
    {
        printf("Enter a character before exit ...\n");
        getchar();
        return -1;
    }

    //Create NUM_THRREADS threads
    std::thread trd[NUM_THREADS];

    //Initialize rom persistent mem
    rom_init();
    printf("Base addr is : %p\n", base_addr);

    //Initialize romLog object inside enclave with base addr...//Init persistent data structure
    ecall_init(global_eid, (void *)per_out, base_addr);

    //ecall_nvram_worker(global_eid,stack_val,0);
    
    //Threads will enter enclave and do push/pop on the persistent stack.
    for (int i = 0; i < NUM_THREADS; i++)
    {
        trd[i] = std::thread(thread_func);
    }
    for (int i = 0; i < NUM_THREADS; i++)
    {
        trd[i].join();
    }

    
    //Destroy enclave
    sgx_destroy_enclave(global_eid);
    return 0;
}
