
GXX49_VERSION := $(shell g++-4.9 --version 2>/dev/null)

ifdef GXX49_VERSION
	CXX_COMMAND := g++-4.9
else
	CXX_COMMAND := g++
endif

CXX = ${CXX_COMMAND} -std=c++14 -Wall

all: run_test

run_test: sender receiver


headers: msg.h

receiver: headers recv.cpp
	${CXX} recv.cpp -o receiver

sender: headers sender.cpp
	${CXX} sender.cpp -o sender

clean:
	rm -f sender receiver
