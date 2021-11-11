.PHONY: all
all: libcjson.a

libds.a:
	make -C ./ds/ && mv ./ds/libds.a ./libds.a

cjson.o: cjson.c
	$(CC) -c -o cjson.o cjson.c

libcjson.a: cjson.o libds.a
	ar -rc libcjson.a cjson.o

.PHONY: test
test: test.c libcjson.a
	$(CC) -o test test.c -L. -lcjson -lds

.PHONY: run
run: test
	./test

.PHONY: clean
clean:
	make -C ./ds/ clean; \
	rm -f ./libds.a; \
	rm -f ./cjson.o; \
	rm -f ./libcjson.a; \
	rm -f ./test;
