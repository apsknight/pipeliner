CXX = g++
CXXFLAGS = -std=c++11 -Wall -g

SRCS = src/main.cpp

OBJS = ${SRCS:.cpp=.o}

MAIN = pipeliner

all: ${MAIN}
	@echo Pipeliner has been compiled succesfully.

${MAIN}: ${OBJS}
	${CXX} ${CXXFLAGS} ${OBJS} -o ${MAIN}

.cpp.o:
	${CXX} ${CXXFLAGS} -c $< -o $@

clean:
	${RM} ${PROGS} ${OBJS} *.o *~ src/*.o