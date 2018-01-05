CXX       := g++
CXXFLAGS  := -Wall -std=c++0x -v

TARGET    := facedetect
PROJ_ROOT := $(realpath)
VPATH     := $(PROJ_ROOT)/block_identifier
INCLUDES  := `pkg-config opencv --cflags` \
	     -I $(PROJ_ROOT)/block_identifier
SRCS      := main.cpp
LIBS      := -lpthread \
	     -ldl \
	     `pkg-config opencv --libs` \
	     -lm

UNAME := ${shell uname}
ifeq ($(UNAME), Linux)
CXXDEFS := -DENABLE_REAL_3D_LED_CUBE
LIBS := $(LIBS) -lraspicamcv
endif

OBJS      := $(SRCS:.cpp=.o)

### Rules ######################################################################

.PHONY: all
all: $(TARGET)
.PHONY: make
make: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXDEFS) -o $@ $(OBJS) $(LIBS)

.PHONY: clean
clean:
	$(RM) -f $(TARGET) *.o

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $<
