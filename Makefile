TARGET=kicc
PREFIX=/usr
INSTALL_PATH=$(DESTDIR)$(PREFIX)/bin/$(TARGET)

CFLAGS?=-pipe -O3 -march=native -mtune=generic
CFLAGS+=\
	-static
    -Werror=pedantic \
    -Werror=return-local-addr \
    -Werror=missing-prototypes \
    -Werror=strict-prototypes \
    -Wall \
    -Wextra \
    -std=c99 \
    -D_FORTIFY_SOURCE=2 \
    -D_POSIX_C_SOURCE=200809L

all: $(TARGET)
install: $(TARGET)
	install -Dm755 $(TARGET) $(INSTALL_PATH)
