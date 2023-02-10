#ifndef ENCLAVE_T_H__
#define ENCLAVE_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */


#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

int ecall_enclave_sample(void);
void ecall_createNewBTree(unsigned long int capacity);
void ecall_insertData(double x0, long int id);
void ecall_pointLocationQuery(double x0, unsigned long int* pIndexIO, unsigned long int* pLeafIO);
void ecall_containsWhatQuery(double x0, double y0, unsigned long int* pIndexIO, unsigned long int* pLeafIO);
void ecall_loadBTree(long int indexIdentifier);
void ecall_showTree(void);
void ecall_deleteTree(void);

sgx_status_t SGX_CDECL ocall_print(const char* str);
sgx_status_t SGX_CDECL ocall_storeByteArray(long int* id, unsigned long int len, const unsigned char* data);
sgx_status_t SGX_CDECL ocall_loadByteArray(long int id, unsigned long int len, unsigned char* data);
sgx_status_t SGX_CDECL ocall_getDataLength(long int id, unsigned long int* len);
sgx_status_t SGX_CDECL sgx_oc_cpuidex(int cpuinfo[4], int leaf, int subleaf);
sgx_status_t SGX_CDECL sgx_thread_wait_untrusted_event_ocall(int* retval, const void* self);
sgx_status_t SGX_CDECL sgx_thread_set_untrusted_event_ocall(int* retval, const void* waiter);
sgx_status_t SGX_CDECL sgx_thread_setwait_untrusted_events_ocall(int* retval, const void* waiter, const void* self);
sgx_status_t SGX_CDECL sgx_thread_set_multiple_untrusted_events_ocall(int* retval, const void** waiters, size_t total);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
