CFLAGS = -Wall -g
CC = gcc

all: tail

tail: tail.c
	$(CC) -o $@ $(CFLAGS) $(CPPFLAGS) $<

clean:
	rm -rf tail

check: tail
	for f in testfiles/t*; do \
		./tail $$f | diff -u - goldens/$$(basename $$f) || exit 1; \
	done
	./tail / 2>&1 1>/dev/null | diff -u - goldens/root
	./tail /etc/shadow 2>&1 1>/dev/null | diff -u - goldens/shadow

.PHONY: clean check
