CC=gcc

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
INCLUDES=$(foreach d,$(call rwildcard,./include,/),-I$d)

CFLAGS=$(INCLUDES)

LDFLAGS=-pthread -lncurses

SRCDIR=src
OBJDIR=build/obj
BINDIR=build/bin
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

run: $(TARGET)
	./$(TARGET) -p 4221 -d ./tests/data --no-ui

test: $(TESTDIR)/run_tests.sh 
	@mkdir -p $(BINDIR)
	$(TESTDIR)/run_tests.sh

check: test

clean:
	rm -f $(OBJECTS) $(TARGET) $(TEST_TARGET)
	@rm -rf $(OBJDIR) $(BINDIR)

stop:
	@echo "Stopping http_server..."
	@pkill -f $(TARGET) || echo "http_server is not running"

.PHONY: all test check clean run stop
