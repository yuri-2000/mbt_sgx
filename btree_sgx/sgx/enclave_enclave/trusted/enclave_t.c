#include "enclave_t.h"

#include "sgx_trts.h" /* for sgx_ocalloc, sgx_is_outside_enclave */
#include "sgx_lfence.h" /* for sgx_lfence */

#include <errno.h>
#include <mbusafecrt.h> /* for memcpy_s etc */
#include <stdlib.h> /* for malloc/free etc */

#define CHECK_REF_POINTER(ptr, siz) do {	\
	if (!(ptr) || ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_UNIQUE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_ENCLAVE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_within_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define ADD_ASSIGN_OVERFLOW(a, b) (	\
	((a) += (b)) < (b)	\
)


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

static sgx_status_t SGX_CDECL sgx_ecall_enclave_sample(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_ecall_enclave_sample_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_ecall_enclave_sample_t* ms = SGX_CAST(ms_ecall_enclave_sample_t*, pms);
	sgx_status_t status = SGX_SUCCESS;



	ms->ms_retval = ecall_enclave_sample();


	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_createNewBTree(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_ecall_createNewBTree_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_ecall_createNewBTree_t* ms = SGX_CAST(ms_ecall_createNewBTree_t*, pms);
	sgx_status_t status = SGX_SUCCESS;



	ecall_createNewBTree(ms->ms_capacity);


	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_insertData(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_ecall_insertData_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_ecall_insertData_t* ms = SGX_CAST(ms_ecall_insertData_t*, pms);
	sgx_status_t status = SGX_SUCCESS;



	ecall_insertData(ms->ms_x0, ms->ms_id);


	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_pointLocationQuery(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_ecall_pointLocationQuery_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_ecall_pointLocationQuery_t* ms = SGX_CAST(ms_ecall_pointLocationQuery_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	unsigned long int* _tmp_pIndexIO = ms->ms_pIndexIO;
	size_t _len_pIndexIO = sizeof(unsigned long int);
	unsigned long int* _in_pIndexIO = NULL;
	unsigned long int* _tmp_pLeafIO = ms->ms_pLeafIO;
	size_t _len_pLeafIO = sizeof(unsigned long int);
	unsigned long int* _in_pLeafIO = NULL;

	CHECK_UNIQUE_POINTER(_tmp_pIndexIO, _len_pIndexIO);
	CHECK_UNIQUE_POINTER(_tmp_pLeafIO, _len_pLeafIO);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_pIndexIO != NULL && _len_pIndexIO != 0) {
		if ( _len_pIndexIO % sizeof(*_tmp_pIndexIO) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		if ((_in_pIndexIO = (unsigned long int*)malloc(_len_pIndexIO)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_pIndexIO, 0, _len_pIndexIO);
	}
	if (_tmp_pLeafIO != NULL && _len_pLeafIO != 0) {
		if ( _len_pLeafIO % sizeof(*_tmp_pLeafIO) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		if ((_in_pLeafIO = (unsigned long int*)malloc(_len_pLeafIO)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_pLeafIO, 0, _len_pLeafIO);
	}

	ecall_pointLocationQuery(ms->ms_x0, _in_pIndexIO, _in_pLeafIO);
	if (_in_pIndexIO) {
		if (memcpy_s(_tmp_pIndexIO, _len_pIndexIO, _in_pIndexIO, _len_pIndexIO)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_in_pLeafIO) {
		if (memcpy_s(_tmp_pLeafIO, _len_pLeafIO, _in_pLeafIO, _len_pLeafIO)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}

err:
	if (_in_pIndexIO) free(_in_pIndexIO);
	if (_in_pLeafIO) free(_in_pLeafIO);
	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_containsWhatQuery(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_ecall_containsWhatQuery_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_ecall_containsWhatQuery_t* ms = SGX_CAST(ms_ecall_containsWhatQuery_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	unsigned long int* _tmp_pIndexIO = ms->ms_pIndexIO;
	size_t _len_pIndexIO = sizeof(unsigned long int);
	unsigned long int* _in_pIndexIO = NULL;
	unsigned long int* _tmp_pLeafIO = ms->ms_pLeafIO;
	size_t _len_pLeafIO = sizeof(unsigned long int);
	unsigned long int* _in_pLeafIO = NULL;

	CHECK_UNIQUE_POINTER(_tmp_pIndexIO, _len_pIndexIO);
	CHECK_UNIQUE_POINTER(_tmp_pLeafIO, _len_pLeafIO);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_pIndexIO != NULL && _len_pIndexIO != 0) {
		if ( _len_pIndexIO % sizeof(*_tmp_pIndexIO) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		if ((_in_pIndexIO = (unsigned long int*)malloc(_len_pIndexIO)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_pIndexIO, 0, _len_pIndexIO);
	}
	if (_tmp_pLeafIO != NULL && _len_pLeafIO != 0) {
		if ( _len_pLeafIO % sizeof(*_tmp_pLeafIO) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		if ((_in_pLeafIO = (unsigned long int*)malloc(_len_pLeafIO)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_pLeafIO, 0, _len_pLeafIO);
	}

	ecall_containsWhatQuery(ms->ms_x0, ms->ms_y0, _in_pIndexIO, _in_pLeafIO);
	if (_in_pIndexIO) {
		if (memcpy_s(_tmp_pIndexIO, _len_pIndexIO, _in_pIndexIO, _len_pIndexIO)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_in_pLeafIO) {
		if (memcpy_s(_tmp_pLeafIO, _len_pLeafIO, _in_pLeafIO, _len_pLeafIO)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}

err:
	if (_in_pIndexIO) free(_in_pIndexIO);
	if (_in_pLeafIO) free(_in_pLeafIO);
	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_loadBTree(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_ecall_loadBTree_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_ecall_loadBTree_t* ms = SGX_CAST(ms_ecall_loadBTree_t*, pms);
	sgx_status_t status = SGX_SUCCESS;



	ecall_loadBTree(ms->ms_indexIdentifier);


	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_showTree(void* pms)
{
	sgx_status_t status = SGX_SUCCESS;
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	ecall_showTree();
	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_deleteTree(void* pms)
{
	sgx_status_t status = SGX_SUCCESS;
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	ecall_deleteTree();
	return status;
}

SGX_EXTERNC const struct {
	size_t nr_ecall;
	struct {void* ecall_addr; uint8_t is_priv; uint8_t is_switchless;} ecall_table[8];
} g_ecall_table = {
	8,
	{
		{(void*)(uintptr_t)sgx_ecall_enclave_sample, 0, 0},
		{(void*)(uintptr_t)sgx_ecall_createNewBTree, 0, 0},
		{(void*)(uintptr_t)sgx_ecall_insertData, 0, 0},
		{(void*)(uintptr_t)sgx_ecall_pointLocationQuery, 0, 0},
		{(void*)(uintptr_t)sgx_ecall_containsWhatQuery, 0, 0},
		{(void*)(uintptr_t)sgx_ecall_loadBTree, 0, 0},
		{(void*)(uintptr_t)sgx_ecall_showTree, 0, 0},
		{(void*)(uintptr_t)sgx_ecall_deleteTree, 0, 0},
	}
};

SGX_EXTERNC const struct {
	size_t nr_ocall;
	uint8_t entry_table[9][8];
} g_dyn_entry_table = {
	9,
	{
		{0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, },
	}
};


sgx_status_t SGX_CDECL ocall_print(const char* str)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_str = str ? strlen(str) + 1 : 0;

	ms_ocall_print_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_print_t);
	void *__tmp = NULL;


	CHECK_ENCLAVE_POINTER(str, _len_str);

	if (ADD_ASSIGN_OVERFLOW(ocalloc_size, (str != NULL) ? _len_str : 0))
		return SGX_ERROR_INVALID_PARAMETER;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_print_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_print_t));
	ocalloc_size -= sizeof(ms_ocall_print_t);

	if (str != NULL) {
		ms->ms_str = (const char*)__tmp;
		if (_len_str % sizeof(*str) != 0) {
			sgx_ocfree();
			return SGX_ERROR_INVALID_PARAMETER;
		}
		if (memcpy_s(__tmp, ocalloc_size, str, _len_str)) {
			sgx_ocfree();
			return SGX_ERROR_UNEXPECTED;
		}
		__tmp = (void *)((size_t)__tmp + _len_str);
		ocalloc_size -= _len_str;
	} else {
		ms->ms_str = NULL;
	}
	
	status = sgx_ocall(0, ms);

	if (status == SGX_SUCCESS) {
	}
	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_storeByteArray(long int* id, unsigned long int len, const unsigned char* data)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_id = sizeof(long int);
	size_t _len_data = len;

	ms_ocall_storeByteArray_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_storeByteArray_t);
	void *__tmp = NULL;

	void *__tmp_id = NULL;

	CHECK_ENCLAVE_POINTER(id, _len_id);
	CHECK_ENCLAVE_POINTER(data, _len_data);

	if (ADD_ASSIGN_OVERFLOW(ocalloc_size, (id != NULL) ? _len_id : 0))
		return SGX_ERROR_INVALID_PARAMETER;
	if (ADD_ASSIGN_OVERFLOW(ocalloc_size, (data != NULL) ? _len_data : 0))
		return SGX_ERROR_INVALID_PARAMETER;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_storeByteArray_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_storeByteArray_t));
	ocalloc_size -= sizeof(ms_ocall_storeByteArray_t);

	if (id != NULL) {
		ms->ms_id = (long int*)__tmp;
		__tmp_id = __tmp;
		if (_len_id % sizeof(*id) != 0) {
			sgx_ocfree();
			return SGX_ERROR_INVALID_PARAMETER;
		}
		if (memcpy_s(__tmp_id, ocalloc_size, id, _len_id)) {
			sgx_ocfree();
			return SGX_ERROR_UNEXPECTED;
		}
		__tmp = (void *)((size_t)__tmp + _len_id);
		ocalloc_size -= _len_id;
	} else {
		ms->ms_id = NULL;
	}
	
	ms->ms_len = len;
	if (data != NULL) {
		ms->ms_data = (const unsigned char*)__tmp;
		if (_len_data % sizeof(*data) != 0) {
			sgx_ocfree();
			return SGX_ERROR_INVALID_PARAMETER;
		}
		if (memcpy_s(__tmp, ocalloc_size, data, _len_data)) {
			sgx_ocfree();
			return SGX_ERROR_UNEXPECTED;
		}
		__tmp = (void *)((size_t)__tmp + _len_data);
		ocalloc_size -= _len_data;
	} else {
		ms->ms_data = NULL;
	}
	
	status = sgx_ocall(1, ms);

	if (status == SGX_SUCCESS) {
		if (id) {
			if (memcpy_s((void*)id, _len_id, __tmp_id, _len_id)) {
				sgx_ocfree();
				return SGX_ERROR_UNEXPECTED;
			}
		}
	}
	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_loadByteArray(long int id, unsigned long int len, unsigned char* data)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_data = len;

	ms_ocall_loadByteArray_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_loadByteArray_t);
	void *__tmp = NULL;

	void *__tmp_data = NULL;

	CHECK_ENCLAVE_POINTER(data, _len_data);

	if (ADD_ASSIGN_OVERFLOW(ocalloc_size, (data != NULL) ? _len_data : 0))
		return SGX_ERROR_INVALID_PARAMETER;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_loadByteArray_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_loadByteArray_t));
	ocalloc_size -= sizeof(ms_ocall_loadByteArray_t);

	ms->ms_id = id;
	ms->ms_len = len;
	if (data != NULL) {
		ms->ms_data = (unsigned char*)__tmp;
		__tmp_data = __tmp;
		if (_len_data % sizeof(*data) != 0) {
			sgx_ocfree();
			return SGX_ERROR_INVALID_PARAMETER;
		}
		memset(__tmp_data, 0, _len_data);
		__tmp = (void *)((size_t)__tmp + _len_data);
		ocalloc_size -= _len_data;
	} else {
		ms->ms_data = NULL;
	}
	
	status = sgx_ocall(2, ms);

	if (status == SGX_SUCCESS) {
		if (data) {
			if (memcpy_s((void*)data, _len_data, __tmp_data, _len_data)) {
				sgx_ocfree();
				return SGX_ERROR_UNEXPECTED;
			}
		}
	}
	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_getDataLength(long int id, unsigned long int* len)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_len = sizeof(unsigned long int);

	ms_ocall_getDataLength_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_getDataLength_t);
	void *__tmp = NULL;

	void *__tmp_len = NULL;

	CHECK_ENCLAVE_POINTER(len, _len_len);

	if (ADD_ASSIGN_OVERFLOW(ocalloc_size, (len != NULL) ? _len_len : 0))
		return SGX_ERROR_INVALID_PARAMETER;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_getDataLength_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_getDataLength_t));
	ocalloc_size -= sizeof(ms_ocall_getDataLength_t);

	ms->ms_id = id;
	if (len != NULL) {
		ms->ms_len = (unsigned long int*)__tmp;
		__tmp_len = __tmp;
		if (_len_len % sizeof(*len) != 0) {
			sgx_ocfree();
			return SGX_ERROR_INVALID_PARAMETER;
		}
		memset(__tmp_len, 0, _len_len);
		__tmp = (void *)((size_t)__tmp + _len_len);
		ocalloc_size -= _len_len;
	} else {
		ms->ms_len = NULL;
	}
	
	status = sgx_ocall(3, ms);

	if (status == SGX_SUCCESS) {
		if (len) {
			if (memcpy_s((void*)len, _len_len, __tmp_len, _len_len)) {
				sgx_ocfree();
				return SGX_ERROR_UNEXPECTED;
			}
		}
	}
	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL sgx_oc_cpuidex(int cpuinfo[4], int leaf, int subleaf)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_cpuinfo = 4 * sizeof(int);

	ms_sgx_oc_cpuidex_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_sgx_oc_cpuidex_t);
	void *__tmp = NULL;

	void *__tmp_cpuinfo = NULL;

	CHECK_ENCLAVE_POINTER(cpuinfo, _len_cpuinfo);

	if (ADD_ASSIGN_OVERFLOW(ocalloc_size, (cpuinfo != NULL) ? _len_cpuinfo : 0))
		return SGX_ERROR_INVALID_PARAMETER;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_sgx_oc_cpuidex_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_sgx_oc_cpuidex_t));
	ocalloc_size -= sizeof(ms_sgx_oc_cpuidex_t);

	if (cpuinfo != NULL) {
		ms->ms_cpuinfo = (int*)__tmp;
		__tmp_cpuinfo = __tmp;
		if (_len_cpuinfo % sizeof(*cpuinfo) != 0) {
			sgx_ocfree();
			return SGX_ERROR_INVALID_PARAMETER;
		}
		memset(__tmp_cpuinfo, 0, _len_cpuinfo);
		__tmp = (void *)((size_t)__tmp + _len_cpuinfo);
		ocalloc_size -= _len_cpuinfo;
	} else {
		ms->ms_cpuinfo = NULL;
	}
	
	ms->ms_leaf = leaf;
	ms->ms_subleaf = subleaf;
	status = sgx_ocall(4, ms);

	if (status == SGX_SUCCESS) {
		if (cpuinfo) {
			if (memcpy_s((void*)cpuinfo, _len_cpuinfo, __tmp_cpuinfo, _len_cpuinfo)) {
				sgx_ocfree();
				return SGX_ERROR_UNEXPECTED;
			}
		}
	}
	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL sgx_thread_wait_untrusted_event_ocall(int* retval, const void* self)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_sgx_thread_wait_untrusted_event_ocall_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_sgx_thread_wait_untrusted_event_ocall_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_sgx_thread_wait_untrusted_event_ocall_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_sgx_thread_wait_untrusted_event_ocall_t));
	ocalloc_size -= sizeof(ms_sgx_thread_wait_untrusted_event_ocall_t);

	ms->ms_self = self;
	status = sgx_ocall(5, ms);

	if (status == SGX_SUCCESS) {
		if (retval) *retval = ms->ms_retval;
	}
	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL sgx_thread_set_untrusted_event_ocall(int* retval, const void* waiter)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_sgx_thread_set_untrusted_event_ocall_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_sgx_thread_set_untrusted_event_ocall_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_sgx_thread_set_untrusted_event_ocall_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_sgx_thread_set_untrusted_event_ocall_t));
	ocalloc_size -= sizeof(ms_sgx_thread_set_untrusted_event_ocall_t);

	ms->ms_waiter = waiter;
	status = sgx_ocall(6, ms);

	if (status == SGX_SUCCESS) {
		if (retval) *retval = ms->ms_retval;
	}
	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL sgx_thread_setwait_untrusted_events_ocall(int* retval, const void* waiter, const void* self)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_sgx_thread_setwait_untrusted_events_ocall_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_sgx_thread_setwait_untrusted_events_ocall_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_sgx_thread_setwait_untrusted_events_ocall_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_sgx_thread_setwait_untrusted_events_ocall_t));
	ocalloc_size -= sizeof(ms_sgx_thread_setwait_untrusted_events_ocall_t);

	ms->ms_waiter = waiter;
	ms->ms_self = self;
	status = sgx_ocall(7, ms);

	if (status == SGX_SUCCESS) {
		if (retval) *retval = ms->ms_retval;
	}
	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL sgx_thread_set_multiple_untrusted_events_ocall(int* retval, const void** waiters, size_t total)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_waiters = total * sizeof(void*);

	ms_sgx_thread_set_multiple_untrusted_events_ocall_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_sgx_thread_set_multiple_untrusted_events_ocall_t);
	void *__tmp = NULL;


	CHECK_ENCLAVE_POINTER(waiters, _len_waiters);

	if (ADD_ASSIGN_OVERFLOW(ocalloc_size, (waiters != NULL) ? _len_waiters : 0))
		return SGX_ERROR_INVALID_PARAMETER;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_sgx_thread_set_multiple_untrusted_events_ocall_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_sgx_thread_set_multiple_untrusted_events_ocall_t));
	ocalloc_size -= sizeof(ms_sgx_thread_set_multiple_untrusted_events_ocall_t);

	if (waiters != NULL) {
		ms->ms_waiters = (const void**)__tmp;
		if (_len_waiters % sizeof(*waiters) != 0) {
			sgx_ocfree();
			return SGX_ERROR_INVALID_PARAMETER;
		}
		if (memcpy_s(__tmp, ocalloc_size, waiters, _len_waiters)) {
			sgx_ocfree();
			return SGX_ERROR_UNEXPECTED;
		}
		__tmp = (void *)((size_t)__tmp + _len_waiters);
		ocalloc_size -= _len_waiters;
	} else {
		ms->ms_waiters = NULL;
	}
	
	ms->ms_total = total;
	status = sgx_ocall(8, ms);

	if (status == SGX_SUCCESS) {
		if (retval) *retval = ms->ms_retval;
	}
	sgx_ocfree();
	return status;
}

