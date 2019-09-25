------------------------
Purpose of sgx-romulus
------------------------
The project demonstrates how to use cache line flush commands e.g: clfush, clwb, clfushopt inside sgx enclaves. A file on persistent memory (`/dev/shm/pool` by default) 
is first memory mapped into the untrusted runtime's virtual memory. The flush commands are then executed on some mapped addresses within the enclave. The long term goal 
of this project is to port the romulus library into sgx.

------------------------------------
How to Build/Execute the Sample Code
------------------------------------
1. Install Intel(R) SGX SDK for Linux* OS
2. Build the project with the prepared Makefile:
    a. Hardware Mode, Debug build:
        $ make SGX_MODE=HW SGX_DEBUG=1
    b. Hardware Mode, Pre-release build:
        $ make SGX_MODE=HW SGX_PRERELEASE=1
    c. Hardware Mode, Release build:
        $ make SGX_MODE=HW
3. Execute the binary directly:
    $ ./app


