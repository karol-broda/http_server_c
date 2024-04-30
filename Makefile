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

run: $(TARGET)
	@echo "Running http_server on port 4221 with base directory /var/www"
	./$(TARGET) -p 4221 -d /var/www

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
