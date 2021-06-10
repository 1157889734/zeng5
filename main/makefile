#编译器设置
#交叉编译需要加前缀
#CPPTESTPATH = /parasoft/cpptest/9.6/cpptestscan
CPPTESTPATH = /opt/HiSTBLinuxV100R005C00SPC041B020/tools/linux/toolchains/arm-histbv310-linux/bin/arm-histbv310-linux-
CC            = $(CPPTESTPATH)gcc
CXX           = $(CPPTESTPATH)g++
CCX          = $(CXX)
#CROSS =
#CC = $(CROSS)gcc
#CXX = $(CROSS)g++
DEBUG = -w
CFLAGS = $(DEBUG) -c
LINK = -lpthread
#命令加参数
MV = mv -f
RM = rm -fr
LN = ln -sf

#目录初始化
TOP_PATH = $(shell pwd)
SRC_PATH = $(TOP_PATH)
DIRS = $(shell find $(SRC_PATH) -maxdepth 3 -type d)
#源文件初始化
SRCS += $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))
#依赖文件初始化
OBJS = $(patsubst %.cpp,%.o,$(SRCS))
#头文件初始化
INC_PATH += $(foreach dir,$(DIRS),$(addprefix -I,$(dir)))

#设置终极目标，也就是执行文件
TARGET = main

#自动编译所有.c文件，生成.o文件,所有(.o)文件依赖(.cpp)文件
%.o : %.cpp
	@$(CCX) $(CFLAGS) $< -o $@ $(INC_PATH)

#根据所有.o文件链接成终极目标
$(TARGET) : $(OBJS)
	$(CCX) $^ -o $@ $(LINK)
	@echo Create $(TARGET) ok...
	
	cp main /home/nfs_share/
	
#清除
TEMP = $(OBJS)
.PHONY :
clean:
	@ $(RM) $(TARGET) $(TEMP)
	@echo clean all...
