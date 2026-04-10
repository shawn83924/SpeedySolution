#####################################################################
# Project Name : MDS (MemoryDataStore)
# Author       : Allen Lee
#####################################################################

#--------------------------------------------------------------------------------
# *********************************
#--------------------------------------------------------------------------------
include ../commDefine.mak

#--------------------------------------------------------------------------------
# Setting variable
#--------------------------------------------------------------------------------
LDLIBS := $(LDLIBS)
BUILD_DIR := $(BUILD_DIR)/MDS
SRCS := MDSException.cpp \
        MDSCommObj.cpp \
        MColumn.cpp \
        MRow.cpp \
        MRowSet.cpp \
        MTable.cpp \
        MTransactionStore.cpp \
        MIndex.cpp \
        MSchema.cpp

TARGET := $(BIN_DIR)/MDS.a
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
# *********************************
#--------------------------------------------------------------------------------
include ../commRule.mak

#--------------------------------------------------------------------------------
# Program rule
#--------------------------------------------------------------------------------
$(BIN_DIR)/MDS.a: $(OBJS)
	@echo
	@echo "  Make Indexed library" : $@
	@echo "---------------------------------------------------"
	ar -r $@ $^
	@echo "---------------------------------------------------"

$(BUILD_DIR)/MDSException.o: MDSException.cpp MDSException.h
	$(CXX) -c -o $@ $(CXXFLAG) $<

$(BUILD_DIR)/MDSCommObj.o: MDSCommObj.cpp MDSCommObj.h
	$(CXX) -c -o $@ $(CXXFLAG) $<

$(BUILD_DIR)/MColumn.o: MColumn.cpp MColumn.h
	$(CXX) -c -o $@ $(CXXFLAG) $<

$(BUILD_DIR)/MRow.o: MRow.cpp MRow.h
	$(CXX) -c -o $@ $(CXXFLAG) $<

$(BUILD_DIR)/MRowSet.o: MRowSet.cpp MRowSet.h
	$(CXX) -c -o $@ $(CXXFLAG) $<

$(BUILD_DIR)/MTable.o: MTable.cpp MTable.h
	$(CXX) -c -o $@ $(CXXFLAG) $<

$(BUILD_DIR)/MTransactionStore.o: MTransactionStore.cpp MTransactionStore.h
	$(CXX) -c -o $@ $(CXXFLAG) $<

$(BUILD_DIR)/MIndex.o: MIndex.cpp MIndex.h
	$(CXX) -c -o $@ $(CXXFLAG) $<

$(BUILD_DIR)/MSchema.o: MSchema.cpp MSchema.h
	$(CXX) -c -o $@ $(CXXFLAG) $<

#--------------------------------------------------------------------------------
