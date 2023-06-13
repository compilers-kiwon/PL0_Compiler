CC = g++
CXXFLAGS = -Wall -I ./Inc
OBJS = codegen.o compile.o error.o lexer.o main.o parser.o table.o
TARGET = PL0.out

all : $(TARGET)

.PHONY: clean
clean :
	rm -f *.o
	rm -f $(TARGET)

%.o : %.cpp
	$(CC) $(CXXFLAGS) -c $<

$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) -o $@ $(OBJS)

-include $(OBJS:.o=.d)