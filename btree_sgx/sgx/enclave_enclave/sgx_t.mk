######## Intel(R) SGX SDK Settings ########
SGX_SDK ?= /opt/intel/sgxsdk
SGX_MODE ?= SIM
SGX_ARCH ?= x64
TRUSTED_DIR=trusted

ifeq ($(shell getconf LONG_BIT), 32)
	SGX_ARCH := x86
else ifeq ($(findstring -m32, $(CXXFLAGS)), -m32)
	SGX_ARCH := x86
endif

ifeq ($(SGX_ARCH), x86)
	SGX_COMMON_CFLAGS := -m32
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x86/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x86/sgx_edger8r
else
	SGX_COMMON_CFLAGS := -m64
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib64
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x64/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x64/sgx_edger8r
endif

ifeq ($(SGX_DEBUG), 1)
ifeq ($(SGX_PRERELEASE), 1)
$(error Cannot set SGX_DEBUG and SGX_PRERELEASE at the same time!!)
endif
endif

ifeq ($(SGX_DEBUG), 1)
        SGX_COMMON_CFLAGS += -O0 -g
else
        SGX_COMMON_CFLAGS += -O2
endif

ifneq ($(SGX_MODE), HW)
	Trts_Library_Name := sgx_trts_sim
	Service_Library_Name := sgx_tservice_sim
else
	Trts_Library_Name := sgx_trts
	Service_Library_Name := sgx_tservice
endif

Crypto_Library_Name := sgx_tcrypto

Enclave_Cpp_Files := $(TRUSTED_DIR)/enclave.cpp $(TRUSTED_DIR)/tools/Tools.cpp $(TRUSTED_DIR)/tools/Point.cpp $(TRUSTED_DIR)/tools/Region.cpp $(TRUSTED_DIR)/tools/LineSegment.cpp $(TRUSTED_DIR)/btree/Node.cpp $(TRUSTED_DIR)/btree/Index.cpp $(TRUSTED_DIR)/btree/Leaf.cpp $(TRUSTED_DIR)/btree/BTree.cpp $(TRUSTED_DIR)/btree/Statistics.cpp $(TRUSTED_DIR)/btree/Buffer.cpp $(TRUSTED_DIR)/btree/LruBuffer.cpp 
Enclave_C_Files := 
Enclave_Include_Paths := -IInclude -I$(TRUSTED_DIR) -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/libcxx 


Flags_Just_For_C := -Wno-implicit-function-declaration -std=c11
Common_C_Cpp_Flags := $(SGX_COMMON_CFLAGS) -nostdinc -fvisibility=hidden -fpie -fstack-protector $(Enclave_Include_Paths) -fno-builtin-printf -I.
Enclave_C_Flags := $(Flags_Just_For_C) $(Common_C_Cpp_Flags)
Enclave_Cpp_Flags :=  $(Common_C_Cpp_Flags) -std=c++11 -nostdinc++ -fno-builtin-printf -I.

Enclave_Cpp_Flags := $(Enclave_Cpp_Flags)  -fno-builtin-printf

Enclave_Link_Flags := $(SGX_COMMON_CFLAGS) -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_LIBRARY_PATH) \
	-Wl,--whole-archive -l$(Trts_Library_Name) -Wl,--no-whole-archive \
	-Wl,--start-group -lsgx_tstdc -lsgx_tcxx -l$(Crypto_Library_Name) -l$(Service_Library_Name) -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0 \
	-Wl,--version-script=$(TRUSTED_DIR)/enclave.lds

Enclave_Cpp_Objects := $(Enclave_Cpp_Files:.cpp=.o)
Enclave_C_Objects := $(Enclave_C_Files:.c=.o)

ifeq ($(SGX_MODE), HW)
ifneq ($(SGX_DEBUG), 1)
ifneq ($(SGX_PRERELEASE), 1)
Build_Mode = HW_RELEASE
endif
endif
endif


.PHONY: all run

ifeq ($(Build_Mode), HW_RELEASE)
all: enclave.so
	@echo "Build enclave enclave.so  [$(Build_Mode)|$(SGX_ARCH)] success!"
	@echo
	@echo "*********************************************************************************************************************************************************"
	@echo "PLEASE NOTE: In this mode, please sign the enclave.so first using Two Step Sign mechanism before you run the app to launch and access the enclave."
	@echo "*********************************************************************************************************************************************************"
	@echo 


else
all: enclave.signed.so
endif

run: all
ifneq ($(Build_Mode), HW_RELEASE)
	@$(CURDIR)/app
	@echo "RUN  =>  app [$(SGX_MODE)|$(SGX_ARCH), OK]"
endif


######## enclave Objects ########

$(TRUSTED_DIR)/enclave_t.c: $(SGX_EDGER8R) ./$(TRUSTED_DIR)/enclave.edl
	@cd ./$(TRUSTED_DIR) && $(SGX_EDGER8R) --trusted ../$(TRUSTED_DIR)/enclave.edl --search-path ../$(TRUSTED_DIR) --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

$(TRUSTED_DIR)/enclave_t.o: ./$(TRUSTED_DIR)/enclave_t.c
	@$(CC) $(Enclave_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

$(TRUSTED_DIR)/%.o: $(TRUSTED_DIR)/%.cpp
	@$(CXX) $(Enclave_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

$(TRUSTED_DIR)/%.o: $(TRUSTED_DIR)/%.c
	@$(CC) $(Enclave_C_Flags) -c $< -o $@
	@echo "CC  <=  $<"

enclave.so: $(TRUSTED_DIR)/enclave_t.o $(Enclave_Cpp_Objects) $(Enclave_C_Objects)
	@$(CXX) $^ -o $@ $(Enclave_Link_Flags)
	@echo "LINK =>  $@"

enclave.signed.so: enclave.so
	@$(SGX_ENCLAVE_SIGNER) sign -key $(TRUSTED_DIR)/enclave_private.pem -enclave enclave.so -out $@ -config $(TRUSTED_DIR)/enclave.config.xml
	@echo "SIGN =>  $@"
clean:
	@rm -f enclave.* $(TRUSTED_DIR)/enclave_t.* $(Enclave_Cpp_Objects) $(Enclave_C_Objects)