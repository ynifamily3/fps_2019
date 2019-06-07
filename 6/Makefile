TARGET   = a.out


CC      ?= gcc
CFLAGS   = -W -Wall -std=gnu11
LDFLAGS  = -W -Wall -std=gnu11

SRCDIR   = .
OBJDIR   = .
BINDIR   = .

SOURCES  := $(wildcard $(SRCDIR)/*.c)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJECTS) $(BINDIR)/$(TARGET)

.PHONY: format
format:
	clang-format -style="file" -i src/*.c src/*.h -verbose
	
.PHONY: test
test:
	make docker
	make docker-run
	
.PHONY: docker
docker:
	make clean
	docker build -t fileio .
	
.PHONY: docker-run
docker-run:
	docker run -i -t --rm --name="fileio" fileio
