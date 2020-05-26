#$Id: Makefile 2020-05-21 20:02:21 sunwu $
CXXFLAGS = $(FIR_COMPILE_OPT) 
TARGET = $(BUILD_PATH)/lib$(TARGET_NAME).a 
OBJS = $(patsubst %.cc, $(BUILD_PATH)/%.o, $(wildcard *.cc))
DEPS = $(OBJS:$(BUILD_PATH)/%.o=$(BUILD_PATH)/.%.d)

COMM_PROTO = $(wildcard $(COMM_PATH)/*.proto) 
LOCAL_PROTO = $(wildcard *.proto) 
 

COMM_PROTO_CPP = $(COMM_PROTO:$(COMM_PATH)/%.proto=%.pb.cc)
LOCAL_PROTO_CPP = $(LOCAL_PROTO:%.proto=%.pb.cc)
PROTOC=$(THIRDPART)/protobuf/bin/protoc


all: $(TARGET)

$(LOCAL_PROTO_CPP): %.pb.cc: %.proto
	$(PROTOC) -I=. --proto_path=$(COMM_PATH) --cpp_out=$(PROTOCPP_PATH) $<

$(COMM_PROTO_CPP): %.pb.cc: $(COMM_PATH)/%.proto
	 $(PROTOC) -I=$(COMM_PATH) --cpp_out=$(PROTOCPP_PATH) $<

$(TARGET): $(OBJS) 
	@rm -rf $(TARGET)
	@ar cru $(TARGET) $(OBJS)
	@ranlib $(TARGET)
	@rm -rf $(TARGET_PATH)/*

$(DEPS): $(BUILD_PATH)/.%.d: $(SRC_PATH)/%.cc
	@if [ ! -d "$(dir $@)" ]; then mkdir -pv $(dir $@); fi
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,$(BUILD_PATH)/\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$

$(OBJS): $(BUILD_PATH)/%.o: $(SRC_PATH)/%.cc $(BUILD_PATH)/.%.d
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	@rm -rf $(BUILD_PATH) $(TARGET) core *~ 

protoc: $(LOCAL_PROTO_CPP) $(COMM_PROTO_CPP)



.PHONY: all clean protoc

