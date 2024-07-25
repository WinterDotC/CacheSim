SRC = src/main.c src/LRUCache.c src/Common.c src/DSE.c

BUILD_DIR = ./build

CFLAGS = -I./include

app:
	gcc $(CFLAGS) $(SRC) -o CacheSim

run:
	./CacheSim

.PHONY: app run