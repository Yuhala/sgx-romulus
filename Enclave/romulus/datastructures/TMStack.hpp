#ifndef _TM_STACK
#define _TM_STACK

//10 * 1024 * 1024
#define ROMULUS_LOG_PTM
#define ARRAY_SIZE 10 << 17
#define RUNTIME 20
//All  your pmem objects could use this as a template
//Include this template file inside enclave routine to use the pmem object

//#include "../romuluslog/RomulusLogSGX.hpp"
#include "../common/tm.h"
using namespace romuluslog;

//implementation of sps microbenchmark -->PYuhala

/**
 * An imprecise but fast random number generator
 */
uint64_t randomLong(uint64_t x)
{
    x ^= x >> 12; // a
    x ^= x << 25; // b
    x ^= x >> 27; // c
    return x * 2685821657736338717LL;
}

class PArray
{
    TM_TYPE<uint64_t *> array{nullptr};

public:
    void allocate()
    {
        TM_WRITE_TRANSACTION([&]() {
            array = (uint64_t *)TM_PMALLOC(ARRAY_SIZE * sizeof(TM_TYPE<uint64_t>));
            for (int i = 0; i < ARRAY_SIZE; i++)
                array[i] = i;
        });
    }

    void do_sps(long num_swaps, long *ops, double *timer)
    {
        uint64_t seed = 1234567890123456781ULL;
        long count = 0;
        ocall_start_clock();
        while (*timer <= RUNTIME)
        {
            TM_WRITE_TRANSACTION([&]() {
                for (int i = 0; i < num_swaps; i++)
                {
                    seed = randomLong(seed + i);
                    auto ia = seed % ARRAY_SIZE;
                    uint64_t tmp = array[ia];
                    seed = randomLong(seed + i);
                    auto ib = seed % ARRAY_SIZE;
                    array[ia] = array[ib];
                    array[ib] = tmp;
                }
            });
            //sgx_printf("completed trans\n");
            /*  count++;
            *ops = count; */
            (*ops) += num_swaps; //increase number of transactions by 1
            ocall_stop_clock();
        }
    }
};

// A simple persistent stack of integers (one integer per node)
class PStack
{
    struct Node
    {
        TM_TYPE<uint64_t> key;
        TM_TYPE<Node *> next;
    };

    TM_TYPE<Node *> head{nullptr};

public:
    TM_TYPE<uint64_t> EMPTY{999999999};

    void push(uint64_t key)
    {
        TM_WRITE_TRANSACTION([&]() {
            Node *node = (Node *)TM_PMALLOC(sizeof(struct Node));
            node->next = head;
            node->key = key;
            head = node;
        });
    }

    // Returns EMPTY if the stack has no keys
    uint64_t pop(void)
    {
        uint64_t key = 999999; //EMPTY;
        TM_WRITE_TRANSACTION([&]() {
            if (head == nullptr)
            {
                sgx_printf("Stack empty...\n");
                return;
            };
            Node *node = head;
            key = node->key;
            head = node->next;
            TM_PFREE(node);
        });
        return key;
    }
};

#endif /* _TM_STACK */