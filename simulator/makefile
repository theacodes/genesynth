CXX := clang++
TARGET_EXEC ?= simulator

SIMULATOR_SRC_DIR := ./src
SIMULATOR_SRCS := $(shell find $(SIMULATOR_SRC_DIR) -name "*.cpp")
GENESYNTH_SRC_DIR := ./../firmware/genesynth/
GENESYNTH_SRCS := $(shell find $(GENESYNTH_SRC_DIR) -name "*.cpp")

OBJS := $(GENESYNTH_SRCS:.cpp=.o) $(SIMULATOR_SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SIMULATOR_SRC_DIR) $(GENESYNTH_SRC_DIR) -type d)
INC_DIRS := $(INC_DIRS) ./third_party/u8g2/cppsrc ./third_party/u8g2/csrc
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
LDFLAGS := -stdlib=libc++ -l SDL2-2.0.0 u8g2.a

CPPFLAGS ?= $(INC_FLAGS) -g -MMD -MP -std=c++11 -stdlib=libc++ -DHARDWARE_CONSTANTS_OVERRIDE=1

$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# c++ source
%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(OBJS) $(DEPS) ${TARGET_EXEC}

-include $(DEPS)
