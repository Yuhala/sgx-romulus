/**
 * Author: Peterson Yuhala
 */

#ifndef ROMULUS_HELPER_
#define ROMULUS_HELPER_
 
/* For pmem */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <new>
#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include <atomic>

#include "Types.h" //persistent header

#define MMAP_FILENAME "/dev/shm/romuluslog_shared"
#define MAX_SIZE 10*1024*1024 
#define MAGIC_ID 0x1337BAB2

#if defined(__cplusplus)

extern "C" {
#endif

void rom_init();
void create_file();
void close_file();


#if defined(__cplusplus)
}
#endif




#endif /* ROMULUS_HELPER_ */