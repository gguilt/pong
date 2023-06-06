CC = clang
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lSDL2

SRCDIR = src
OBJDIR = obj
BINDIR = bin

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
EXEC = $(BINDIR)/pong

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(OBJS) $(EXEC) $(OBJDIR) $(BINDIR)