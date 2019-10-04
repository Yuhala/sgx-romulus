/*
 * Copyright (C) 2011-2019 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>

#include <unistd.h>
#include <pwd.h>

#include <sgx_urts.h>
#include <sgx_uswitchless.h> //for switchless calls
#include "App.h"
#include "Enclave_u.h"
#include "ErrorSupport.h"

/* For pmem */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

/* For romulus */

#define MAX_PATH FILENAME_MAX

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

uint8_t *base_addr = (uint8_t *)0x7fdd40000000;
static int fd = -1;

/* Create file and do mmap */
void create_file()
{
   
    struct stat buf;
    if (stat(MMAP_FILENAME, &buf) == 0)
    {
        // File exists
        //std::cout << "Re-using memory region\n";
        fd = open(MMAP_FILENAME, O_RDWR | O_CREAT, 0755);
        assert(fd >= 0);
        // mmap() memory range
        uint8_t *got_addr = (uint8_t *)mmap(base_addr,MAX_SIZE, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0);
        if (got_addr == MAP_FAILED)
        {
            printf("got_addr = %p  %p\n", got_addr, MAP_FAILED);
            perror("ERROR: mmap() is not working !!! ");
            assert(false);
        }
    }
    else
    {
        // File doesn't exist
        fd = open(MMAP_FILENAME, O_RDWR | O_CREAT, 0755);
        assert(fd >= 0);
        if (lseek(fd, MAX_SIZE - 1, SEEK_SET) == -1)
        {
            perror("lseek() error");
        }
        if (write(fd, "", 1) == -1)
        {
            perror("write() error");
        }
        // mmap() memory range
        uint8_t *got_addr = (uint8_t *)mmap(base_addr, MAX_SIZE, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0);
        if (got_addr == MAP_FAILED)
        {
            printf("got_addr = %p  %p\n", got_addr, MAP_FAILED);
            perror("ERROR: mmap() is not working !!! ");
            assert(false);
        }
    }
    printf("mmapped file created\n");
}

/* Do munmap and close file */
void my_ocall_close(){
    assert(fd >= 0);
    munmap(base_addr,MAX_SIZE);
    close(fd);

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
    if (ret != SGX_SUCCESS) {
        print_error_message(ret);
        return -1;
    }

    return 0;
}


/* OCall functions */
void ocall_createFile()
{
    printf("ocall create file called\n");    
    create_file();//maybe you could delete the file here to be sure it doesn't exist..
    ecall_init(global_eid,base_addr);
}

void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate
     * the input string to prevent buffer overflow.
     */
    printf("%s", str);
}


/* Application entry */
int SGX_CDECL main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    sgx_status_t ret;

    /* Initialize the enclave */
    if(initialize_enclave() < 0){
        printf("Enter a character before exit ...\n");
        getchar();
        return -1; 
    }   
    //Create file and do mmap
    //create_file();
    printf("base addr is : %p\n",base_addr);

    //Initialize enclave with base addr.
    empty_ecall(global_eid);
    //could launch worker inside ecall

    sgx_destroy_enclave(global_eid);
    return 0;
}
