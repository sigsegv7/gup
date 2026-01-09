include mk/defaults.mk

CFILES = $(shell find . -name "*.c" | grep -v "src/arch")
CFILES += src/arch/$(ARCH).c
OFILES = $(CFILES:.c=.o)
DFILES = $(CFILES:.c=.d)

.PHONY: all
all: $(OFILES)
	$(CC) $^ -o gup

%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

.PHONY: clean
clean:
	rm -f $(OFILES)
