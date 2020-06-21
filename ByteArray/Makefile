#$Id$

CXXFLAGS = $(FIR_COMPILE_OPT) $(patsubst %, -I%, $(shell find $(SRC_PATH) -type d))
CXXLFLAGS = $(FIR_LINK_OPT)
TARGET = $(TARGET_PATH)/$(TARGET_NAME)
OBJS = $(patsubst %.cpp, $(BUILD_PATH)/%.o, $(shell find . -name \*.cpp | sed 's/\.\///g'))
DEPS = $(OBJS:%.o=%.d)

all: $(TARGET)

-include $(DEPS)
$(DEPS): $(BUILD_PATH)/%.d: $(SRC_PATH)/%.cpp
	@if [ ! -d "$(dir $@)" ]; then mkdir -pv $(dir $@); fi
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($(notdir $*)\)\.o[ :]*,$(dir $@)\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$

$(TARGET): $(OBJS)
	@rm -rf $(TARGET)
	$(CXX) $(OBJS) -o $@ $(CXXLFLAGS)

$(OBJS): $(BUILD_PATH)/%.o: $(SRC_PATH)/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	@rm -rf $(BUILD_PATH) $(TARGET) core *~ 

.PHONY: all clean

