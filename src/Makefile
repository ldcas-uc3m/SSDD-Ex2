BIN_FILES  = cliente servidor
TEST_FILES = test test_client test_server
VPATH = lib tests  # path with source files not found in current dir
CC = gcc

CFLAGS = -g -I$(INSTALL_PATH)/include -Wall -fPIC
LDFLAGS = -L$(INSTALL_PATH)/lib/
LDLIBS = -pthread -lm -lrt


all: $(BIN_FILES)
.PHONY : all

cliente: cliente.o libclaves.so linked_list.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -L$(INSTALL_PATH) -lclaves $^ -o $@
	
servidor: servidor.o server_impl.o linked_list.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

test_client: test_client.o linked_list.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@
	
test_server: test_server.o linked_list.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

test: test_server.o test_client.o servidor libclaves.so
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -L$(INSTALL_PATH) -lclaves $^ -o $@
	
# TODO: libclaves.so
libclaves.so: claves.o
	$(CC) -shared $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -f $(BIN_FILES) $(TEST_FILES) *.o *.so

.SUFFIXES:
.PHONY : clean