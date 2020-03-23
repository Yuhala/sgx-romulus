/**
 * Author: Peterson Yuhala
 */

#ifndef _ENCLAVE_H_
#define _ENCLAVE_H_

#include <stdlib.h>
#include <assert.h>

//#include "Types.h" //types used for marshalling and enclave romulus object construction
#include "romulus/common/RomSGX.h"

#if defined(__cplusplus)
extern "C"
{
#endif
    void ecall_nvram_worker(int val, size_t tid);
    void ecall_init(void *per_out, uint8_t *base_addr_out);
    void empty_ecall();
    void ecall_sps(long nswaps, long *ops, double *timer);
    void do_work(int val, size_t tid);
    void do_close();

    //int printf(const char* fmt, ...);

#if defined(__cplusplus)
}
#endif

#endif /* !_ENCLAVE_H_ */
