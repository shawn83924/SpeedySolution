
#--------------------------------------------------------------------------------
# Default rule
#--------------------------------------------------------------------------------
OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(filter %.c,$(SRCS))) \
        $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(filter %.cpp,$(SRCS))) \
        $(patsubst %.cp,$(BUILD_DIR)/%.o,$(filter %.cp,$(SRCS)))

DEPS := $(patsubst %.c,$(BUILD_DIR)/%.d,$(filter %.c,$(SRCS))) \
        $(patsubst %.cpp,$(BUILD_DIR)/%.d,$(filter %.cpp,$(SRCS))) \
        $(patsubst %.cp,$(BUILD_DIR)/%.d,$(filter %.cp,$(SRCS)))

default: builddir $(TARGET)
	@echo
	@echo "                    Target File"
	@echo "---------------------------------------------------"
	ls -l $(TARGET)
	@echo "--------------------- Make OK ---------------------"

all: default

builddir:
	-mkdir -p .. $(BUILD_DIR)/ $(BIN_DIR)/ $(TBIN_DIR)/ $(LIB_DIR)/

clean:
	rm -f $(OBJS) $(DEPS)

clobber: clean
	rm -f $(TARGET)
#--------------------------------------------------------------------------------
