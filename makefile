all: libqed.so libqed-static.a qed_test

qed: libqed.so
qed_static: libqed-static.a

OBJECTS=qed_batch.o qed_dependency.o qed_tinyhash.o qed_greedy.o

qed_batch.o: qed_batch.c qed_batch.h qed_callback.h qed_dependency.h qed_tinyhash.h
	$(CC) $(CFLAGS) -c qed_batch.c -o qed_batch.o

qed_greedy.o: qed_greedy.c qed_greedy.h qed_batch.h qed_callback.h qed_dependency.h qed_tinyhash.h
	$(CC) $(CFLAGS) -c qed_greedy.c -o qed_greedy.o

qed_dependency.o: qed_dependency.c qed_dependency.h qed_callback.h
	$(CC) $(CFLAGS) -c qed_dependency.c -o qed_dependency.o

qed_tinyhash.o: qed_tinyhash.c qed_tinyhash.h
	$(CC) $(CFLAGS) -c qed_tinyhash.c -o qed_tinyhash.o

libqed-static.a: $(OBJECTS)
	ar -rc libqed-static.a $(OBJECTS)
	ranlib libqed-static.a

libqed.so: $(OBJECTS)
	$(CC) $(CFLAGS) -shared -o libqed.so $(OBJECTS)

qed_test: libqed-static.a qed_test.c qed_test.h qed_batch.h qed_dependency.h
	$(CC) $(CFLAGS) qed_test.c libqed-static.a -o qed_test

clean:
	rm *.a *.o *.so

.PHONY: clean qed qed_static
