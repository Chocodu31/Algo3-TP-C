# Specific part of the Makefile
EXEC = exec

# Begin generic part of the Makefile
CC = gcc
CFLAGS = -std=c99 -Werror -Wextra -Wall -pedantic
LDFLAGS = 
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

ECHO = @
ifeq ($(VERBOSE),1)
	ECHO=
endif

ifeq ($(DEBUG),yes)
	CFLAGS += -g
	LDFLAGS =
else
	CFLAGS += -O3 -DNDEBUG
	LDFLAGS =
endif

#doc:
#	doxygen documentation/TP1

all:
ifeq ($(DEBUG),yes)
	@echo "Generating in debug mode"
else
	@echo "Generating in release mode"
endif
	@$(MAKE) $(EXEC)

%.o: %.c
	$(ECHO)$(CC) -o $@ -c $< $(CFLAGS) -lm

$(EXEC): $(OBJ)
	$(ECHO)$(CC) -o $@ $^ $(LDFLAGS) -lm

.PHONY: clean mrproper

clean:
	$(ECHO)rm -rf *.o

mrproper: clean
	$(ECHO)rm -rf $(EXEC)
# End generic part of the makefile

# List of target dependencies
main.o: stack.h token.h queue.h
