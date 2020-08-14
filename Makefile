TARGET?=kicc
PREFIX?=/usr
INSTALL_PATH?=$(DESTDIR)$(PREFIX)/bin/$(TARGET)

VERSION=0.0.0

CFLAGS?=-march=native -mtune=generic -O2 -pipe
CFLAGS+=-static \
    -Werror=pedantic \
    -Werror=return-local-addr \
    -Werror=missing-prototypes \
    -Werror=strict-prototypes \
    -Wall \
    -Wextra \
    -std=c99 \
    -D_FORTIFY_SOURCE=2 \
    -D_POSIX_C_SOURCE=200809L \
    -DVERSION="\"$(VERSION)\""

OBJECTS=src/kicc-utils.o src/kicc-main.o

all: $(TARGET)

install: $(TARGET)
	install -Dm755 $(TARGET) $(INSTALL_PATH)

clean:
	rm -f $(OBJECTS)

docs:
	./make_docs.sh > docs.txt

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean install docs
