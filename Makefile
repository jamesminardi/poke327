CC = gcc
CXX = g++
ECHO = echo
RM = rm -f


CFLAGS = -Wall -ggdb -funroll-loops
CXXFLAGS = -Wall -ggdb -funroll-loops

LDFLAGS = -lstdc++ -lpanel -lncurses

BIN = poke327
OBJS = main.o world.o map.o heap.o db_parse.o character.o

all: $(BIN)

$(BIN): $(OBJS)
	@$(ECHO) Linking $@
	@$(CC) $^ -o $@ $(LDFLAGS)

-include $(OBJS:.o=.d)

%.o: %.c
	@$(ECHO) Compiling $<
	@$(CC) $(CFLAGS) -MMD -MF $*.d -c $<

%.o: %.cpp
	@$(ECHO) Compiling $<
	@$(CXX) $(CXXFLAGS) -MMD -MF $*.d -c $<

.PHONY: all clean clobber

clean:
	@$(ECHO) Removing all generated files
	@$(RM) *.o $(BIN) *.d core vgcore.* gmon.out

clobber: clean
	@$(ECHO) Removing backup files
	@$(RM) *~ \#* *pgm
