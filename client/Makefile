# 定义编译器
GXX = g++

# 定义编译选项
CFLAGS = -Wall -g

# 获取当前路径
CURRENT_POS := $(shell pwd)
BUILD_DIR = $(CURRENT_POS)/build
$(shell mkdir -p $(BUILD_DIR))
# 定义包含路径
COMMON_PATH=$(CURRENT_POS)/../Common
INCLUDE_PATH = $(CURRENT_POS)/include
UTIL_PATH=$(CURRENT_POS)/../util
INCLUDE = -I$(INCLUDE_PATH) -I$(CURRENT_POS) -I$(COMMON_PATH) -I$(UTIL_PATH)
LDFLAGS = -lpthread

# 追加包含路径到 CFLAGS
CFLAGS += $(INCLUDE)

# 定义目标
TARGET = client

# 定义源文件路径
SRCS_PATH1 = $(CURRENT_POS)/include
SRCS_PATH2 = $(CURRENT_POS)/src


# 获取所有 .cpp 文件
SRCS=$(wildcard $(SRCS_PATH1)/*.cpp) 
SRCS1=$(wildcard $(SRCS_PATH2)/*.cpp)
SRCS2=$(wildcard $(COMMON_PATH)/*.cpp)
SRCS3=$(wildcard $(UTIL_PATH)/*.cpp)
# 定义对象文件
# 确保 OBJECT 变量只包含 .o 文件路径
OBJECT = $(SRCS:$(SRCS_PATH1)/%.cpp=$(BUILD_DIR)/%.o)
OBJECT += $(SRCS1:$(SRCS_PATH2)/%.cpp=$(BUILD_DIR)/%.o)
OBJECT += $(SRCS2:$(COMMON_PATH)/%.cpp=$(BUILD_DIR)/%.o)
OBJECT += $(SRCS3:$(UTIL_PATH)/%.cpp=$(BUILD_DIR)/%.o)
# 默认目标
all: $(TARGET)

# 链接目标
$(TARGET): $(OBJECT)
	$(GXX) -o $@ $(OBJECT) $(LDFLAGS)

# 编译源文件为对象文件
$(BUILD_DIR)/%.o: $(SRCS_PATH1)/%.cpp
	$(GXX) $(CFLAGS) -c $< -o $@
$(BUILD_DIR)/%.o: $(SRCS_PATH2)/%.cpp
	$(GXX) $(CFLAGS) -c $< -o $@
$(BUILD_DIR)/%.o: $(COMMON_PATH)/%.cpp
	$(GXX) $(CFLAGS) -c $< -o $@
$(BUILD_DIR)/%.o: $(UTIL_PATH)/%.cpp
	$(GXX) $(CFLAGS) -c $< -o $@
# 清理目标
clean:
	@rm -f $(TARGET) $(OBJECT)

# 调试输出 OBJECT
debug:
	@echo "OBJECT = $(OBJECT)"
