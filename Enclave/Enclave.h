/**
 * Author: Peterson Yuhala
 */

#ifndef _ENCLAVE_H_
#define _ENCLAVE_H_

#include <stdlib.h>
#include <assert.h>

#include "romulus/common/Types.h" //types used for marshalling and enclave romulus object construction
#include "romulus/common/RomSGX.h"

#if defined(__cplusplus)
extern "C" {
#endif
void ecall_nvram_worker();
void ecall_init(uint8_t *base_addr_out);
void empty_ecall();
void do_work();
void do_close();

//int printf(const char* fmt, ...);

#if defined(__cplusplus)
}
#endif

#endif /* !_ENCLAVE_H_ */
