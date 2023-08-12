CC   = clang

wallbox: wallbox.c
	$(CC) wallbox.c -o wallbox -Wall -Wextra -pedantic `curl-config --libs` `pkg-config --cflags --libs json-c` 

clean:
	rm -rf wallbox