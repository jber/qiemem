PACKAGES       = stages/test stmlib/utils stages stages/ramp stmlib/dsp

VPATH          = $(PACKAGES)

TARGET         = stages_test
BUILD_ROOT     = build/
BUILD_DIR      = $(BUILD_ROOT)$(TARGET)/
CC_FILES       = ramp_extractor.cc \
		stages_test.cc \
		segment_generator.cc \
		resources.cc \
		units.cc \
		random.cc
OBJ_FILES      = $(CC_FILES:.cc=.o)
OBJS           = $(patsubst %,$(BUILD_DIR)%,$(OBJ_FILES)) $(STARTUP_OBJ)
DEPS           = $(OBJS:.o=.d)
DEP_FILE       = $(BUILD_DIR)depends.mk
OPEN           = $(shell type -p open || echo xdg-open)

all:  stages_test

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)%.o: %.cc
	g++ -c -DTEST -g -Wall -Werror -msse2 -Wno-unused-variable -O2 -I. $< -o $@

$(BUILD_DIR)%.d: %.cc
	g++ -MM -DTEST -I. $< -MF $@ -MT $(@:.d=.o)

stages_test:  $(OBJS)
	g++ -g -o $(TARGET) $(OBJS) -lm -lprofiler -L/opt/local/lib

depends:  $(DEPS)
	cat $(DEPS) > $(DEP_FILE)

$(DEP_FILE):  $(BUILD_DIR) $(DEPS)
	cat $(DEPS) > $(DEP_FILE)

profile:	stages_test
	env CPUPROFILE_FREQUENCY=1000 CPUPROFILE=$(BUILD_DIR)/stages.prof ./stages_test && pprof --pdf ./stages_test $(BUILD_DIR)/stages.prof > profile.pdf && ${OPEN} profile.pdf

valgrind:	stages_test
	valgrind --tool=callgrind ./stages_test

clean:
	rm $(BUILD_DIR)*.*

include $(DEP_FILE)
