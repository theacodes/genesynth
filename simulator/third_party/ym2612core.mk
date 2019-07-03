CC := clang
CXX := clang++
TARGET_LIB ?= ym2612core.a

SRC_DIR := ./ym2612core
CSRCS := $(shell find $(SRC_DIR) -name "*.c")
CXXSRCS := $(shell find $(SRC_DIR) -name "*.cpp")

OBJS := $(CSRCS:.c=.o) $(CXXSRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS)) -I./stubs

CFLAGS ?= $(INC_FLAGS) -MMD -MP
CXXFLAGS ?= $(INC_FLAGS) -MMD -MP -std=c++11

$(TARGET_LIB): $(OBJS)
	ar -rv $@ $(OBJS)

# c source
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# cpp source
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(OBJS) $(DEPS) ${TARGET_LIB}

-include $(DEPS)
