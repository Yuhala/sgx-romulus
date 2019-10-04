/**
 * Author: Peterson Yuhala
 * Based of RomulusLog code by: Andreia Correia, Pascal Felber, Pedro Ramalhete
 */

#ifndef TYPES_H
#define TYPES_H
#include <cstdint>
#include <atomic>
#include <functional>
//--------------------------------------------------------------
#define CHUNK_SIZE_H 1024
#define MAX_SIZE 10 * 1024 * 1024 //100mb
#define MAGIC_ID_H 0x1337BAB2
#define CLPAD_H (128 / sizeof(uintptr_t)) //
#define NUM_ROOTS 100
#define MAX_THREADS 1 //single threading for now

/* Log states */
#define STATE_0 0 /* IDLE */
#define STATE_1 1 /* MUTATING */
#define STATE_2 2 /* COPYING */

/* Typedefs */
typedef void *mspace;

// One instance of this is at the start of base_addr, in persistent memory
typedef struct PersistentHeader
{
    uint64_t id {0};                  // Validates intialization
    std::atomic<int> state {STATE_0}; // Current state of consistency
    void **objects {};                // Objects directory

    // Original romulus code tests for ESLOCO; They never use ESLOCO so I ignore it for now..
    uint64_t used_size {0};
    mspace ms {}; // Pointer to allocator's metadata
                 // uint64_t used_size{0}; // It has to be the last, to calculate the used_size

} PersistentHeader;

typedef struct LogEntry
{
    size_t offset;   // Pointer offset in bytes, relative to main_addr
    uint64_t length; // Range length of data at pointer offset

} LogEntry;

typedef struct LogChunk
{
    LogEntry entries [CHUNK_SIZE_H];
    uint64_t num_entries {0};
    LogChunk *next {nullptr};

} LogChunk;

/* Contains untrusted romulus object attributes */
typedef struct romAttrib
{
    /* Attributes to be marshalled into enclave */

    //romuluslog attribs
    // Global with the 'main' size. Used by pload()
    uint64_t g_main_size;
    // Global with the 'main' addr. Used by pload()
    uint8_t *g_main_addr;

    bool dommap;
    //int fd = -1;
    uint8_t *base_addr;
    uint8_t *real_base_addr;
    uint64_t max_size;
    uint8_t *main_addr;
    uint8_t *back_addr;
    // There is always at least one (empty) chunk in the log, it's the head
    LogChunk *log_head;
    LogChunk *log_tail;

    // Volatile pointer to start of persistent memory
    PersistentHeader *per;
    uint64_t log_size;
    // Aligned just to make sure there is no false-sharing with rwlock
    alignas(128) bool logEnabled;
    alignas(128) std::atomic<std::function<void()> *> *fc;
    int *histo; //array of atomic pointers to functions
    int storecount;

} romAttrib;

#endif /*!ROMSGX_H*/