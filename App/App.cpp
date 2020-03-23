
#include <stdio.h>
#include <string.h>

#include <pwd.h>

#include <sgx_urts.h>
#include "App.h"
#include "Enclave_u.h"
#include "ErrorSupport.h"
#include "Romulus_helper.h" //for romulus helper functions
#include <thread>

/* For benchmarking */
#include <time.h>

struct timespec start, stop;
double diff;
/* For romulus */

#define MAX_PATH FILENAME_MAX

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;
static int stack_val = 10;

/* Romulus objects */
extern PersistentHeader *per_out;
extern uint8_t *base_addr;
extern uint8_t *real_base_addr;

/* Calculates time diff between two timestamps in m */

double time_diff(timespec *start, timespec *stop, granularity gran)
{

    double diff = 0.0;

    switch (gran)
    {
    case MILLI:
        diff = (double)(stop->tv_sec - start->tv_sec) * 1.0e3 + (double)((stop->tv_nsec - start->tv_nsec) / 1.0e6);
        break;

    case MICRO:
        diff = (double)(stop->tv_sec - start->tv_sec) * 1.0e6 + (double)((stop->tv_nsec - start->tv_nsec) / 1.0e3);
        break;

    case NANO:
        diff = (double)(stop->tv_sec - start->tv_sec) * 1.0e39 + (double)((stop->tv_nsec - start->tv_nsec));
        break;

    default: //seconds
        diff = (double)(stop->tv_sec - start->tv_sec) + (double)((stop->tv_nsec - start->tv_nsec) / 1.0e9);
        break;
    }
}

void run_sps()
{
    diff = 0;
    long nswaps = 2;
    long *ops = (long *)malloc(sizeof(long));
    int signal = 0;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    while (diff <= 20)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &stop);
        diff = time_diff(&start, &stop, MILLI);
        diff /= 1000; //convert time to seconds
        signal = diff <= 20 ? 1 : 0;
        ecall_sps(global_eid, nswaps, ops, &signal);
    }
    printf("Number of ops: %l\n", *ops);
}
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
    stack_val++; // implement mutex/atomic..just for testing anyway
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
    /* for (int i = 0; i < NUM_THREADS; i++)
    {
        trd[i] = std::thread(thread_func);
    }
    for (int i = 0; i < NUM_THREADS; i++)
    {
        trd[i].join();
    } */

    run_sps();
    //Destroy enclave
    sgx_destroy_enclave(global_eid);
    return 0;
}
