#!/bin/bash

# ---
# Runs the tests
# ---

cd src/

# compile
make servidor
make tests

# link dynamic library
export LD_LIBRARY_PATH=lib/

# run server
./servidor 8080 &

# run tests
export IP_TUPLAS=localhost
export PORT_TUPLAS=8080

./test_linked_list
./test_conc
./test_seq