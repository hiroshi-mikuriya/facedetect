CXX       := g++
CXXFLAGS  := -Wall -std=c++0x -v

TARGET    := facedetect
PROJ_ROOT := $(realpath .)
INCLUDES  := `pkg-config opencv --cflags` \
	     -I $(PROJ_ROOT)/src \
	     -I $(PROJ_ROOT)/src/add \
	     -I $(PROJ_ROOT)/src/common \
	     -I $(PROJ_ROOT)/src/identify \
	     -I $(PROJ_ROOT)/src/list \
	     -I $(PROJ_ROOT)/src/remove \
	     -I $(PROJ_ROOT)/src/update
SRCS      := main.cpp \
		 $(shell find $(PROJ_ROOT)/src -name "*.cpp")
LIBS      := -lpthread \
	     -ldl \
	     `pkg-config opencv --libs` \
	     -lm

UNAME := ${shell uname}
ifeq ($(UNAME), Linux)
CXXFLAGS := $(CXXFLAGS) -DENABLE_RASPBERRY_PI_CAMERA
LIBS := $(LIBS) -lraspicamcv
endif

OBJS      := $(SRCS:.cpp=.o)

### Rules ######################################################################

.PHONY: all
all: $(TARGET)
.PHONY: make
make: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LIBS)

.PHONY: clean
clean:
	$(RM) -f $(TARGET) *.o

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $<
