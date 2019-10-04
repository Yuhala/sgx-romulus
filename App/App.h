/**
 * Author: Peterson Yuhala
 */

#ifndef _APP_H_
#define _APP_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "sgx_error.h"   /* sgx_status_t */
#include "sgx_eid.h"     /* sgx_enclave_id_t */

#ifndef TRUE
# define TRUE 1
#endif

#ifndef FALSE
# define FALSE 0
#endif

# define TOKEN_FILENAME   "enclave.token"
# define ENCLAVE_FILENAME "enclave.signed.so"
# define MMAP_FILENAME "/dev/shm/romuluslog_shared"
#define MAX_SIZE 10*1024*1024 

extern sgx_enclave_id_t global_eid;    /* global enclave id */

#if defined(__cplusplus)

extern "C" {
#endif
void ocall_createFile();
void create_file();
void ocall_print_string(const char *str);
void my_ocall_close();

#if defined(__cplusplus)
}
#endif

#endif /* !_APP_H_ */
