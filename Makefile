#$Id: Makefile 2020-05-02 11:21:15Z sunwu $

CXX=$(GCC_PATH)/bin/c++8.3

BUILD_DIR_ROOT = $(PWD)

BUILD_PATH = $(BUILD_DIR_ROOT)/.obj
TARGET_PATH = $(BUILD_DIR_ROOT)/bin
PROTO_PATH = $(BUILD_DIR_ROOT)/protos
COMM_PATH = $(BUILD_DIR_ROOT)/Command
PROTOCPP_PATH = $(BUILD_DIR_ROOT)/protos


TARGET_PROJECTS := \
	SpdLog \


INC_PATH := \
	-I$(GCC_PATH)/include \
	-I$(PROTO_PATH) \
	-I$(THIRDPART)/curl/include \
	-I$(THIRDPART)/libxml2/include/libxml2 \
	-I$(THIRDPART)/protobuf/include/ \
	-I$(THIRDPART)/hiredis/include/hiredis \
	-I$(THIRDPART)/encdec/include/ \
	-I$(THIRDPART)/spdlog/include/ \
	-I$(THIRDPART)/monster/include/ \
		
LIB_DEBUG :=  -Wl,--end-group

LIB_PATH := \
	-L$(BUILD_PATH)/protos -lprotos \
	-L$(GCC_PATH)/lib/gcc/x86_64-redhat-linux/8.3.0/ \
	-L$(GCC_PATH)/lib64 \
	-L$(THIRDPART)/spdlog/lib  -lspdlog \
	-L$(THIRDPART)/curl/lib  -lcurl \
	-L$(THIRDPART)/libxml2/lib  -lxml2 \
	-L$(THIRDPART)/protobuf/lib  -lprotobuf \
	-L$(THIRDPART)/hiredis/lib  -lhiredis \
	-L$(THIRDPART)/monster/lib  -lmonster \
	-Wl,--start-group 


ALLTARGET := $(DEP_LIBS) $(TARGET_PROJECTS)

ADDRESS_SANITIZE=-fsanitize=address -fsanitize=undefined -fno-sanitize=vptr -fno-sanitize=alignment -fno-omit-frame-pointer -static-libasan -static-libubsan

FIR_IGNORE_WARN=-Wno-format-truncation -fno-var-tracking-assignments -Wno-frame-address -Wno-misleading-indentation -Wno-unused-function -Wno-permissive -Wno-unused-but-set-variable -Wno-unused-local-typedefs 
FIR_COMPILE_OPT= -std=gnu++17 -g -g3 -fno-strict-aliasing -Wall -Werror $(FIR_IGNORE_WARN) $(INC_PATH)
FIR_LINK_OPT= $(LIB_PATH) -lpthread -lrt -ldl -lcurl -lasan -lubsan -fsanitize=address

DEP_SCRIPT := \
	proto 

DEP_LIBS := \
	protos


all: $(TARGET_PROJECTS)

$(TARGET_PROJECTS): $(DEP_LIBS)
	@if [ ! -d "$(TARGET_PATH)" ]; then mkdir -pv $(TARGET_PATH); fi
	@$(MAKE) -C $@ \
		CXX='$(CXX)' \
		FIR_COMPILE_OPT='$(FIR_COMPILE_OPT) $(FIR_DEBUG_OPT) -D_SUNWU_DEBUG ' \
		FIR_LINK_OPT='$(FIR_LINK_OPT)' \
		THIRDPART='$(THIRDPART)' \
		BUILD_DIR_ROOT='$(BUILD_DIR_ROOT)' \
		BUILD_PATH='$(BUILD_PATH)/$@' \
		SRC_PATH='$(BUILD_DIR_ROOT)/$@'	\
		TARGET_PATH='$(TARGET_PATH)' \
		TARGET_NAME='$@'

$(DEP_LIBS): $(DEP_SCRIPT)
	@$(MAKE) -C $@ \
		CXX='$(CXX)' \
		FIR_COMPILE_OPT='$(FIR_COMPILE_OPT) $(FIR_DEBUG_OPT)' \
		FIR_LINK_OPT='$(FIR_LINK_OPT)' \
		THIRDPART='$(THIRDPART)' \
		BUILD_DIR_ROOT='$(BUILD_DIR_ROOT)' \
		BUILD_PATH='$(BUILD_PATH)/$@' \
		SRC_PATH='$(BUILD_DIR_ROOT)/$@'	\
		TARGET_PATH='$(TARGET_PATH)' \
		TARGET_NAME='$@'
		

proto:
	$(MAKE) -C $(PROTO_PATH) protoc COMM_PATH=$(COMM_PATH) THIRDPART=$(THIRDPART) PROTOCPP_PATH=$(PROTOCPP_PATH)

test:
	@echo $(PROTO_PATH)


clean:
	@rm -rf $(BUILD_PATH)
	@rm -rf $(TARGET_PATH)/*
	@rm -rf core.*

.PHONY : clean all $(ALLTARGET) proto test


