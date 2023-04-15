BIN_FILES  = cliente servidor testing
TEST_FILES = test_seq test_conc test_linked_list
VPATH = lib tests  # path with source files not found in current dir
CC = gcc

CFLAGS = -g -I$(INSTALL_PATH)/include -Wall -fPIC
LDFLAGS = -L$(INSTALL_PATH)/lib/ -Wl,-R$(DIR)/lib/linux '-Wl,-R$$ORIGIN'
LDLIBS = -pthread -lm -lrt


all: $(BIN_FILES)
.PHONY : all

cliente: cliente.o libclaves.so log.o log.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -L$(INSTALL_PATH) -lclaves $^ -o $@
	
servidor: servidor.o server_impl.o linked_list.o lines.o log.o log.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@
	
libclaves.so: claves.o lines.o
	$(CC) -shared $(CFLAGS) $(LDFLAGS) $(LDLIBS) $^ -o $@

testing: testing.o libclaves.so
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -L$(INSTALL_PATH) -lclaves $^ -o $@


%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<


tests: $(TEST_FILES)
.SUFFIXES:
.PHONY : tests


clean:
	rm -f $(BIN_FILES) $(TEST_FILES) *.o *.so lib/*.so


.SUFFIXES:
.PHONY : clean