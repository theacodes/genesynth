CXX := clang++
TARGET_EXEC ?= simulator

SRC_DIR := ./src ./../firmware/genesynth/
SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIR) -type d)
INC_DIRS := $(INC_DIRS) ./third_party/u8g2/cppsrc ./third_party/u8g2/csrc ./third_party/ym2612core
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
LDFLAGS := -stdlib=libc++ -l SDL2-2.0.0 -l RtMidi ./third_party/u8g2.a ./third_party/ym2612core.a

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
