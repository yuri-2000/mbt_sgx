#include <string.h>
#include <assert.h>

#include <unistd.h>
#include <pwd.h>
#include <libgen.h>
#include <stdlib.h>

#define MAX_PATH FILENAME_MAX

#include <sgx_urts.h>
#include "sample.h"

#include "enclave_u.h"

//----tree----
#include "spatialindex/SpatialIndexImpl.h"
#include "../trusted/btree/BTree.h"

#include <cstring>

using namespace SpatialIndex;
using namespace std;

#define INSERT 1
#define DELETE 0
#define QUERY 2

StorageManager::IBuffer* file;

void print_bytes(byte* buffer, long len)
{
	for(long i = 0; i < len; i++)
    {
		printf("%.2x ", buffer[i]);
    }
	printf("\n");
}

//------------

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char *msg;
    const char *sug; /* Suggestion */
} sgx_errlist_t;

/* Error code returned by sgx_create_enclave */
static sgx_errlist_t sgx_errlist[] = {
    {
        SGX_ERROR_UNEXPECTED,
        "Unexpected error occurred.",
        NULL
    },
    {
        SGX_ERROR_INVALID_PARAMETER,
        "Invalid parameter.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_MEMORY,
        "Out of memory.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_LOST,
        "Power transition occurred.",
        "Please refer to the sample \"PowerTransition\" for details."
    },
    {
        SGX_ERROR_INVALID_ENCLAVE,
        "Invalid enclave image.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ENCLAVE_ID,
        "Invalid enclave identification.",
        NULL
    },
    {
        SGX_ERROR_INVALID_SIGNATURE,
        "Invalid enclave signature.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_EPC,
        "Out of EPC memory.",
        NULL
    },
    {
        SGX_ERROR_NO_DEVICE,
        "Invalid Intel(R) SGX device.",
        "Please make sure Intel(R) SGX module is enabled in the BIOS, and install Intel(R) SGX driver afterwards."
    },
    {
        SGX_ERROR_MEMORY_MAP_CONFLICT,
        "Memory map conflicted.",
        NULL
    },
    {
        SGX_ERROR_INVALID_METADATA,
        "Invalid enclave metadata.",
        NULL
    },
    {
        SGX_ERROR_DEVICE_BUSY,
        "Intel(R) SGX device was busy.",
        NULL
    },
    {
        SGX_ERROR_INVALID_VERSION,
        "Enclave version was invalid.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ATTRIBUTE,
        "Enclave was not authorized.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_FILE_ACCESS,
        "Can't open enclave file.",
        NULL
    },
};

/* Check error conditions for loading enclave */
void print_error_message(sgx_status_t ret)
{
    size_t idx = 0;
    size_t ttl = sizeof sgx_errlist/sizeof sgx_errlist[0];

    for (idx = 0; idx < ttl; idx++) {
        if(ret == sgx_errlist[idx].err) {
            if(NULL != sgx_errlist[idx].sug)
                printf("Info: %s\n", sgx_errlist[idx].sug);
            printf("Error: %s\n", sgx_errlist[idx].msg);
            break;
        }
    }
    
    if (idx == ttl)
        printf("Error code is 0x%X. Please refer to the \"Intel SGX SDK Developer Reference\" for more details.\n", ret);
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
void ocall_print(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    printf("%s", str);
    fflush(stdout);
}

//void storeByteArray(long& id, const unsigned long len, const byte* const data)
void ocall_storeByteArray(long* id, unsigned long len, const unsigned char* data){
	file->storeByteArray(*id, len, data);
	//printf("_id=%ld\n",_id);
	//print_bytes(data, len);
}

//void loadByteArray(const long id, unsigned long& len, byte** data)
void ocall_getDataLength(long id, unsigned long* len)
{
	byte* buffer;
	file->loadByteArray(id, *len, &buffer);
	delete[] buffer;
}

//void loadByteArray(const long id, unsigned long& len, byte** data)
void ocall_loadByteArray(long id, unsigned long len, unsigned char* data)
{
	unsigned long length=-1;
	byte* buffer;
	//printf("id=%ld\n",id);
	file->loadByteArray(id, length, &buffer);
	//printf("length=%ld\n",length);
	//print_bytes(buffer, length);
	for(long i=0;i<length;i++)
	{
	    data[i]=buffer[i];
	}
	delete[] buffer;
}
/*
//----tree----
// example of a Visitor pattern.
// see RTreeQuery for a more elaborate example.
class MyVisitor : public IVisitor
{
public:
	unsigned long m_indexIO;
	unsigned long m_leafIO;

public:
	MyVisitor() : m_indexIO(0), m_leafIO(0) {}

	void visitNode(const INode& n) {
		if (n.isLeaf()) m_leafIO++;
		else m_indexIO++;
	}

	void visitData(const IData& d)
	{
		IShape* pS;
		d.getShape(&pS);
			// do something.
		Point key;
		pS->getCenter(key);
		delete pS;

		cout << "visitData: key="<< key.m_pCoords[0] << "; id=" << d.getIdentifier() << endl;
		cout << endl;
	    // the ID of this data entry is an answer to the query. I will just print it to stdout.
	}

	void visitData(std::vector<const IData*>& v) {}
};
*/

long computeCapacity(long p)
{
	return (long)ceil((double)(p-4)/8.0);
}


int btree(int argc, char** argv)
{
	try
	{
		if (argc != 5)
		{
			cerr << "Usage: " << argv[0] << " input_file tree_file pagesize query_type [pointsearch | rangequery]." << endl;
			return -1;
		}

		int queryType = 0;
		long capacity=computeCapacity(atoi(argv[3]));
		//cerr<<"capacity: "<<capacity<<endl;

		if (strcmp(argv[4], "pointsearch") == 0) queryType = 0;
		else if (strcmp(argv[4], "rangequery") == 0) queryType = 1;
		else
		{
			cerr << "Unknown query type." << endl;
			return -1;
		}

		ifstream fin(argv[1]);
		if (! fin)
		{
			cerr << "Cannot open data file " << argv[1] << "." << endl;
			return -1;
		}

		// Create a new storage manager with the provided base name and a 4K page size.
		string baseName = argv[2];
		//***for Insert***
		//IStorageManager* diskfile = StorageManager::createNewDiskStorageManager(baseName, 1024);
		//***for Query***
		IStorageManager* diskfile = StorageManager::loadDiskStorageManager(baseName);


		//StorageManager::IBuffer* file = StorageManager::createNewRandomEvictionsBuffer(*diskfile, 10, false);
		//file = StorageManager::createNewRandomEvictionsBuffer(*diskfile, 10, false);
		file = StorageManager::createNewLruBuffer(*diskfile, 10, false);
		// Create a new, empty, RTree with dimensionality 1, minimum load 70%, using "file" as
		// the StorageManager and the RSTAR splitting policy.

		long indexIdentifier = 1;
		//ISpatialIndex* tree = BTree::createNewBTree(*file, 0.7, capacity, capacity, 1, SpatialIndex::BTree::BV_PLUS, indexIdentifier);
		//***for Insert***
		//ecall_createNewBTree(global_eid, capacity);

		//ISpatialIndex* tree = BTree::loadBTree(*file, 1);
		//***for Query***
		ecall_loadBTree(global_eid, 1);

		unsigned long count = 0;
		unsigned long indexIO = 0;
		unsigned long leafIO = 0;
		unsigned long id, op;
		double x[1];
		double y[1];

		Tools::ResourceUsage ru;
		ru.start();

		while (fin)
		{
			fin >> op >> id >> x[0] >> y[0];
			if (! fin.good()) continue; // skip newlines, etc.

			if (op == INSERT)
			{
				////Point p = Point(x, 1);

				//ostringstream os;
				//os << r;
				//string data = os.str();
				//tree->insertData(data.size() + 1, reinterpret_cast<const byte*>(data.c_str()), r, id);
				//cout<<"Insert "<<p.m_pCoords[0]<<endl;

				////tree->insertData(0, 0, p, id);
					// example of passing zero size and a null pointer as the associated data.
			    ecall_insertData(global_eid, x[0], id);
			}

			else if (op == DELETE)
			{
				Point p= Point(x,1);
				cout<<"delete "<<p.m_pCoords[0]<<endl;
				////if (tree->deleteData(p, id) == false)
				{
					cerr << "******ERROR******" << endl;
					cerr << "Cannot delete id: " << id << " , count: " << count << endl;
					return -1;
				}
			}
			else if (op == QUERY)
			{

				unsigned long _indexIO=0;
			    unsigned long _leafIO=0;
				//MyVisitor vis;
				cout << "query "<<id<<" "<<x[0]<<" "<<y[0]<<endl;
				if (queryType == 0)
				{
					////Point p = Point(x, 1);
				    ////tree->pointLocationQuery(p, vis);
					ecall_pointLocationQuery(global_eid, x[0], &_indexIO, &_leafIO);
				}
				else if (queryType == 1)
				{
					////LineSegment r = LineSegment(x, y, 1);
				    ////tree->containsWhatQuery(r, vis);
					ecall_containsWhatQuery(global_eid, x[0], y[0], &_indexIO, &_leafIO);
				}
				////indexIO += vis.m_indexIO;
				////leafIO += vis.m_leafIO;
				indexIO += _indexIO;
				leafIO += _leafIO;
			}

			if (((count+1) % 10000) == 0 && count!=0)
				cerr << count+1 << endl;
			count++;
		}  //while

		ru.stop();

		////tree->showTree();
		ecall_showTree(global_eid);

		////tree->computeFanout();    //compute m_avgFanout
		cerr << "Operations: " << count << endl;
		//cerr << "----tree----" << endl;
		//cerr << *tree;
		//cerr << "------------" << endl;
		cerr << "Index I/O: " << indexIO << endl;
		cerr << "Leaf I/O: " << leafIO << endl;
		cerr << "Buffer hits: " << file->getHits() << endl;
		cerr << "Seconds: " << ru.getTotalTime() << endl;
		cerr << "Index ID: " << indexIdentifier << endl;

		////bool ret = tree->isIndexValid();
		////if (ret == false) cerr << "ERROR: Structure is invalid!" << endl;
		////else cerr << "The stucture seems O.K." << endl;

		////delete tree;
		ecall_deleteTree(global_eid);
		delete file;
		delete diskfile;
			// delete the buffer first, then the storage manager
			// (otherwise the the buffer will fail trying to write the dirty entries).

	}  //try
	catch (Tools::Exception& e)
	{
		cerr << "******ERROR******" << endl;
		std::string s = e.what();
		cerr << s << endl;
		return -1;
	}
	catch (...)
	{
		cerr << "******ERROR******" << endl;
		cerr << "other exception" << endl;
		return -1;
	}

	return 0;
}


// ./sample i2 tree 20 pointsearch
// ./sample q tree 20 pointsearch
/* Application entry */
int SGX_CDECL main(int argc, char *argv[])
{
//    (void)(argc);
//    (void)(argv);

    /* Changing dir to where the executable is.*/
    char absolutePath[MAX_PATH];
    char *ptr = NULL;
    (void)(argv);
    ptr = realpath(dirname(argv[0]),absolutePath);
    if( chdir(absolutePath) != 0)
    	abort();
    /* Initialize the enclave */
    if(initialize_enclave() < 0){
        return -1; 
    }

    btree(argc, argv);
    
//    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
//    int ecall_return = 0;
//    ret = ecall_enclave_sample(global_eid, &ecall_return);
//    if (ret != SGX_SUCCESS)
//        abort();
//    if (ecall_return == 0)
//    {
//        printf("Application ran with success\n");
//    }
//    else
//    {
//        printf("Application failed %d \n", ecall_return);
//    }

    sgx_destroy_enclave(global_eid);

    return 0;
}
