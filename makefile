all : bsdrsvr test_bsdr
BIN      = bsdrsvr
BIN_TEST = test_bsdr

CC       = g++

LIBS     = -lpthread \
           -lndn-cxx \
           -lboost_system \
           -lcrypto \
           -ljsoncpp \
           -lcurl \
           -lmysqlclient \
           -llog4cxx \
           -lssl \
           -lgtest \
           -ls3 \
           -lcommonLib \
           
CIFLAGS  = -I/usr/local/include/common \
           -I/usr/local/lib  \
           -I/usr/include/mysql \
           -I/usr/include/gtest \
           -I/usr/include/rapidjson 

CPPFLAGS = -Wall \
           -g \
           -std=c++11 \
           -I./ \
           -I./mysql

CXXFLAGS = -Wall \
           -g \
           -std=c++11 \
           $(CIFLAGS)
           
CFLAGS   = $(CIFLAGS) \
           -g

ROOTSRC  = $(wildcard *.c) $(wildcard *.cpp)
ROOTOBJ  = $(patsubst %c,%o,$(patsubst %cpp,%o,$(ROOTSRC)))

SUBDIR   = $(shell ls -d */ | grep -v test)
SUBSRC   = $(shell find $(SUBDIR) -name '*.c') $(shell find $(SUBDIR) -name '*.cpp')
SUBOBJ   = $(patsubst %c,%o,$(patsubst %cpp,%o,$(SUBSRC)))

TESTDIR   = ./
TESTSRC   = $(shell find $(TESTDIR) -name '*.c') $(shell find $(TESTDIR) -name '*.cpp' | grep -v bsdrsvr)
TESTOBJ   = $(patsubst %c,%o,$(patsubst %cpp,%o,$(TESTSRC)))

$(BIN):$(ROOTOBJ) $(SUBOBJ)
	$(CC) $(CXXFLAGS) $(ROOTOBJ) $(SUBOBJ) -o $(BIN) $(LIBS)
	
$(BIN_TEST):$(TESTOBJ)
	$(CC) $(CPPFLAGS) $(TESTOBJ) -o $(BIN_TEST) $(LIBS)

clean:
	rm -rf $(ROOTOBJ) $(SUBOBJ) $(TESTOBJ)

distclean:
	rm -rf $(BIN) $(BIN_TEST) $(ROOTOBJ) $(SUBOBJ) $(TESTOBJ)
