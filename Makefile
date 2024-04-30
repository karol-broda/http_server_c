CC=gcc
CFLAGS=-I./include
LDFLAGS=-pthread
SRCDIR=src
OBJDIR=obj
BINDIR=bin
TESTDIR=tests
SOURCES=$(wildcard $(SRCDIR)/*.c)
OBJECTS=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
TARGET=$(BINDIR)/http_server
TEST_TARGET=$(BINDIR)/test_suite

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) -c $(CFLAGS) $< -o $@

test: $(TESTDIR)/run_tests.sh 
	@mkdir -p $(BINDIR)
	$(TESTDIR)/run_tests.sh

check: test

clean:
	rm -f $(OBJECTS) $(TARGET) $(TEST_TARGET)
	@rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all test check clean
