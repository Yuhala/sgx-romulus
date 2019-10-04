#ifndef _TM_STACK
#define _TM_STACK

//All  your pmem objects could use this as a template
//Include this template file inside enclave routine to use the pmem object

#include "../romuluslog/RomulusLogSGX.hpp"

using namespace romuluslog;

// A simple persistent stack of integers (one integer per node)
class PStack {
    struct Node {
        persist<uint64_t> key;
        persist<Node*>    next;
    };

    persist<Node*> head {nullptr};

public:
    persist<uint64_t> EMPTY {999999999};

    void push(uint64_t key) {
        RomulusLog::updateTx([&] () {
            Node* node = RomulusLog::tmNew<Node>();
            node->next = head;
            node->key = key;
            head = node;
        });
    }

    // Returns EMPTY if the stack has no keys
    uint64_t pop(void) {
        uint64_t key = EMPTY;
        RomulusLog::updateTx([&] () {
           if (head == nullptr) return;
           Node* node = head;
           key = node->key;
           head = node->next;
           RomulusLog::tmDelete(node);
        });
        return key;
    }
};




#endif /* _TM_STACK */