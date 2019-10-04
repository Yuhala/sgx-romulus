/**
 * Author: Peterson Yuhala
 * Based of RomulusLog code by: Andreia Correia, Pascal Felber, Pedro Ramalhete
 */

#ifndef ROMSGX_H
#define ROMSGX_H

#include <cstdint>  //for uint64_t
#include <stdlib.h> //for size_t
#include <atomic>
#include "Types.h"

//---------------------------------------------------------------

/**
 * The following macros are used to replace unsupported routines
 * and types in the malloc.c file
 */

#define SGX_MALLOC_FAILURE_ACTION abort_h()//TODO
#define SGX_MMAP abort_h() //TODO
#define SGX_MUNMAP(a, s) (0) //TODO i.e. do munmap via an ocall and return 
#define SGX_ABORT abort_h()
#define SGX_MAP_DEFAULT(s) (0)

#define SGX_PTHREAD_T
#define SGX_MAP_PROT //TODO
#define SGX_MAP_FLAGS

#define THREAD_LIB //TODO
#define SCHED_LIB //TODO
#define PTHREAD_LIB //TODO  
#define SYS_PARAM //TODO
//---------------------------------------------------------------


//--------------------------------------------------------------

//extern romAttrib *romAttrib_out; //contains the values of the untrusted romulus object
extern uint8_t *base_addr_in;

//------------------------------------------------------------------------------------

/* Fxn prototypes */
//void destruct_out();
//LogChunk* get_chunk_out();

#if defined(__cplusplus)
extern "C" {
#endif

void do_create_file();
void do_mmap();
void do_close();
void abort_h();
int sgx_printf(const char* fmt, ...);
int __cxa_thread_atexit() throw();

#if defined(__cplusplus)
}
#endif

#endif /*!ROMSGX_H*/

/**
 * ///////////////////////
#-----------------------------------------------------------------------------------------------------
$(Rom_Folder)/romuluslog/RomulusLogSGX.o: $(Rom_Folder)/romuluslog/RomulusLogSGX.cpp $(Rom_Folder)/romuluslog/RomulusLogSGX.hpp
	$(ROM_CXX) $(Romulus_Cpp_Flags) -c $< -o $@
	@echo "Compiling RomulusLogSGX class"

$(Rom_Folder)/romuluslog/malloc.o: $(Rom_Folder)/romuluslog/malloc.cpp
	$(ROM_CXX) $(Romulus_Cpp_Flags) -c $< -o $@
	@echo "Compiling romulus malloc file"

$(Rom_Folder)/common/ThreadRegistry.o: $(Rom_Folder)/common/ThreadRegistry.cpp $(Rom_Folder)/common/ThreadRegistry.hpp
	$(ROM_CXX) $(Romulus_Cpp_Flags) -c $< -o $@
	@echo "Compiling romulus ThreadRegistry.cpp"
#---------------------------------------------------------------------------------------------------------
*/