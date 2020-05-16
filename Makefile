#$Id: Makefile 38624 2019-01-22 10:31:15Z dengmuyang $

CXX=$(GCC_PATH)/bin/c++8.3

BUILD_DIR_ROOT = $(PWD)

BUILD_PATH = $(BUILD_DIR_ROOT)/.obj
TARGET_PATH = $(BUILD_DIR_ROOT)/bin


TARGET_PROJECTS := \
	SpdLog \


INC_PATH := \
	-I$(GCC_PATH)/include \
	-I$(IVY_THIRDPATY)/curl/include \
	-I$(IVY_THIRDPATY)/libxml2/include/libxml2 \
	-I$(IVY_THIRDPATY)/protobuf/include/ \
	-I$(IVY_THIRDPATY)/hiredis/include/hiredis \
	-I$(IVY_THIRDPATY)/encdec/include/ \
	-I$(IVY_THIRDPATY)/spdlog/include/ \
		
LIB_DEBUG :=  -Wl,--end-group

LIB_PATH := \
	-L$(GCC_PATH)/lib/gcc/x86_64-redhat-linux/8.3.0/ \
	-L$(GCC_PATH)/lib64 \
	-L$(IVY_THIRDPATY)/spdlog/lib  -lspdlog \
	-L$(IVY_THIRDPATY)/curl/lib  -lcurl \
	-L$(IVY_THIRDPATY)/libxml2/lib  -lxml2 \
	-L$(IVY_THIRDPATY)/protobuf/lib  -lprotobuf \
	-L$(IVY_THIRDPATY)/hiredis/lib  -lhiredis \
	-Wl,--start-group 


ALLTARGET := $(DEP_LIBS) $(TARGET_PROJECTS)

ADDRESS_SANITIZE=-fsanitize=address -fsanitize=undefined -fno-sanitize=vptr -fno-sanitize=alignment -fno-omit-frame-pointer -static-libasan -static-libubsan

FIR_IGNORE_WARN=-Wno-format-truncation -fno-var-tracking-assignments -Wno-frame-address -Wno-misleading-indentation -Wno-unused-function -Wno-permissive -Wno-unused-but-set-variable -Wno-unused-local-typedefs 
FIR_COMPILE_OPT= -std=gnu++17 -g -g3 -fno-strict-aliasing -Wall -Werror $(FIR_IGNORE_WARN) $(INC_PATH)
FIR_LINK_OPT= $(LIB_PATH) -lpthread -lrt -ldl -lcurl -lasan -lubsan -fsanitize=address

all: $(TARGET_PROJECTS)

$(TARGET_PROJECTS):
	@if [ ! -d "$(TARGET_PATH)" ]; then mkdir -pv $(TARGET_PATH); fi
	@$(MAKE) -C $@ \
		CXX='$(CXX)' \
		FIR_COMPILE_OPT='$(FIR_COMPILE_OPT) $(FIR_DEBUG_OPT) -D_SUNWU_DEBUG ' \
		FIR_LINK_OPT='$(FIR_LINK_OPT)' \
		FIR_THIRDPART='$(FIR_THIRDPART)' \
		BUILD_DIR_ROOT='$(BUILD_DIR_ROOT)' \
		BUILD_PATH='$(BUILD_PATH)/$@' \
		SRC_PATH='$(BUILD_DIR_ROOT)/$@'	\
		TARGET_PATH='$(TARGET_PATH)' \
		TARGET_NAME='$@'



clean:
	@rm -rf $(BUILD_PATH)
	@rm -rf $(TARGET_PATH)/*
	@rm -rf core.*

.PHONY : clean all $(ALLTARGET)


