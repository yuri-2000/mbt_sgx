#include "enclave_u.h"
#include <errno.h>

typedef struct ms_ecall_enclave_sample_t {
	int ms_retval;
} ms_ecall_enclave_sample_t;

typedef struct ms_ecall_createNewBTree_t {
	unsigned long int ms_capacity;
} ms_ecall_createNewBTree_t;

typedef struct ms_ecall_insertData_t {
	double ms_x0;
	long int ms_id;
} ms_ecall_insertData_t;

typedef struct ms_ecall_pointLocationQuery_t {
	double ms_x0;
	unsigned long int* ms_pIndexIO;
	unsigned long int* ms_pLeafIO;
} ms_ecall_pointLocationQuery_t;

typedef struct ms_ecall_containsWhatQuery_t {
	double ms_x0;
	double ms_y0;
	unsigned long int* ms_pIndexIO;
	unsigned long int* ms_pLeafIO;
} ms_ecall_containsWhatQuery_t;

typedef struct ms_ecall_loadBTree_t {
	long int ms_indexIdentifier;
} ms_ecall_loadBTree_t;

typedef struct ms_ocall_print_t {
	const char* ms_str;
} ms_ocall_print_t;

typedef struct ms_ocall_storeByteArray_t {
	long int* ms_id;
	unsigned long int ms_len;
	const unsigned char* ms_data;
} ms_ocall_storeByteArray_t;

typedef struct ms_ocall_loadByteArray_t {
	long int ms_id;
	unsigned long int ms_len;
	unsigned char* ms_data;
} ms_ocall_loadByteArray_t;

typedef struct ms_ocall_getDataLength_t {
	long int ms_id;
	unsigned long int* ms_len;
} ms_ocall_getDataLength_t;

typedef struct ms_sgx_oc_cpuidex_t {
	int* ms_cpuinfo;
	int ms_leaf;
	int ms_subleaf;
} ms_sgx_oc_cpuidex_t;

typedef struct ms_sgx_thread_wait_untrusted_event_ocall_t {
	int ms_retval;
	const void* ms_self;
} ms_sgx_thread_wait_untrusted_event_ocall_t;

typedef struct ms_sgx_thread_set_untrusted_event_ocall_t {
	int ms_retval;
	const void* ms_waiter;
} ms_sgx_thread_set_untrusted_event_ocall_t;

typedef struct ms_sgx_thread_setwait_untrusted_events_ocall_t {
	int ms_retval;
	const void* ms_waiter;
	const void* ms_self;
} ms_sgx_thread_setwait_untrusted_events_ocall_t;

typedef struct ms_sgx_thread_set_multiple_untrusted_events_ocall_t {
	int ms_retval;
	const void** ms_waiters;
	size_t ms_total;
} ms_sgx_thread_set_multiple_untrusted_events_ocall_t;

static sgx_status_t SGX_CDECL enclave_ocall_print(void* pms)
{
	ms_ocall_print_t* ms = SGX_CAST(ms_ocall_print_t*, pms);
	ocall_print(ms->ms_str);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL enclave_ocall_storeByteArray(void* pms)
{
	ms_ocall_storeByteArray_t* ms = SGX_CAST(ms_ocall_storeByteArray_t*, pms);
	ocall_storeByteArray(ms->ms_id, ms->ms_len, ms->ms_data);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL enclave_ocall_loadByteArray(void* pms)
{
	ms_ocall_loadByteArray_t* ms = SGX_CAST(ms_ocall_loadByteArray_t*, pms);
	ocall_loadByteArray(ms->ms_id, ms->ms_len, ms->ms_data);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL enclave_ocall_getDataLength(void* pms)
{
	ms_ocall_getDataLength_t* ms = SGX_CAST(ms_ocall_getDataLength_t*, pms);
	ocall_getDataLength(ms->ms_id, ms->ms_len);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL enclave_sgx_oc_cpuidex(void* pms)
{
	ms_sgx_oc_cpuidex_t* ms = SGX_CAST(ms_sgx_oc_cpuidex_t*, pms);
	sgx_oc_cpuidex(ms->ms_cpuinfo, ms->ms_leaf, ms->ms_subleaf);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL enclave_sgx_thread_wait_untrusted_event_ocall(void* pms)
{
	ms_sgx_thread_wait_untrusted_event_ocall_t* ms = SGX_CAST(ms_sgx_thread_wait_untrusted_event_ocall_t*, pms);
	ms->ms_retval = sgx_thread_wait_untrusted_event_ocall(ms->ms_self);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL enclave_sgx_thread_set_untrusted_event_ocall(void* pms)
{
	ms_sgx_thread_set_untrusted_event_ocall_t* ms = SGX_CAST(ms_sgx_thread_set_untrusted_event_ocall_t*, pms);
	ms->ms_retval = sgx_thread_set_untrusted_event_ocall(ms->ms_waiter);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL enclave_sgx_thread_setwait_untrusted_events_ocall(void* pms)
{
	ms_sgx_thread_setwait_untrusted_events_ocall_t* ms = SGX_CAST(ms_sgx_thread_setwait_untrusted_events_ocall_t*, pms);
	ms->ms_retval = sgx_thread_setwait_untrusted_events_ocall(ms->ms_waiter, ms->ms_self);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL enclave_sgx_thread_set_multiple_untrusted_events_ocall(void* pms)
{
	ms_sgx_thread_set_multiple_untrusted_events_ocall_t* ms = SGX_CAST(ms_sgx_thread_set_multiple_untrusted_events_ocall_t*, pms);
	ms->ms_retval = sgx_thread_set_multiple_untrusted_events_ocall(ms->ms_waiters, ms->ms_total);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[9];
} ocall_table_enclave = {
	9,
	{
		(void*)enclave_ocall_print,
		(void*)enclave_ocall_storeByteArray,
		(void*)enclave_ocall_loadByteArray,
		(void*)enclave_ocall_getDataLength,
		(void*)enclave_sgx_oc_cpuidex,
		(void*)enclave_sgx_thread_wait_untrusted_event_ocall,
		(void*)enclave_sgx_thread_set_untrusted_event_ocall,
		(void*)enclave_sgx_thread_setwait_untrusted_events_ocall,
		(void*)enclave_sgx_thread_set_multiple_untrusted_events_ocall,
	}
};
sgx_status_t ecall_enclave_sample(sgx_enclave_id_t eid, int* retval)
{
	sgx_status_t status;
	ms_ecall_enclave_sample_t ms;
	status = sgx_ecall(eid, 0, &ocall_table_enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t ecall_createNewBTree(sgx_enclave_id_t eid, unsigned long int capacity)
{
	sgx_status_t status;
	ms_ecall_createNewBTree_t ms;
	ms.ms_capacity = capacity;
	status = sgx_ecall(eid, 1, &ocall_table_enclave, &ms);
	return status;
}

sgx_status_t ecall_insertData(sgx_enclave_id_t eid, double x0, long int id)
{
	sgx_status_t status;
	ms_ecall_insertData_t ms;
	ms.ms_x0 = x0;
	ms.ms_id = id;
	status = sgx_ecall(eid, 2, &ocall_table_enclave, &ms);
	return status;
}

sgx_status_t ecall_pointLocationQuery(sgx_enclave_id_t eid, double x0, unsigned long int* pIndexIO, unsigned long int* pLeafIO)
{
	sgx_status_t status;
	ms_ecall_pointLocationQuery_t ms;
	ms.ms_x0 = x0;
	ms.ms_pIndexIO = pIndexIO;
	ms.ms_pLeafIO = pLeafIO;
	status = sgx_ecall(eid, 3, &ocall_table_enclave, &ms);
	return status;
}

sgx_status_t ecall_containsWhatQuery(sgx_enclave_id_t eid, double x0, double y0, unsigned long int* pIndexIO, unsigned long int* pLeafIO)
{
	sgx_status_t status;
	ms_ecall_containsWhatQuery_t ms;
	ms.ms_x0 = x0;
	ms.ms_y0 = y0;
	ms.ms_pIndexIO = pIndexIO;
	ms.ms_pLeafIO = pLeafIO;
	status = sgx_ecall(eid, 4, &ocall_table_enclave, &ms);
	return status;
}

sgx_status_t ecall_loadBTree(sgx_enclave_id_t eid, long int indexIdentifier)
{
	sgx_status_t status;
	ms_ecall_loadBTree_t ms;
	ms.ms_indexIdentifier = indexIdentifier;
	status = sgx_ecall(eid, 5, &ocall_table_enclave, &ms);
	return status;
}

sgx_status_t ecall_showTree(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 6, &ocall_table_enclave, NULL);
	return status;
}

sgx_status_t ecall_deleteTree(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 7, &ocall_table_enclave, NULL);
	return status;
}

