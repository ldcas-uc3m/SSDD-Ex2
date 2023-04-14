#!/bin/bash
cd src/

# compile
make

# link dynamic library
export LD_LIBRARY_PATH=lib/

# run
./servidor 8080 &
env IP_TUPLAS=localhost PORT_TUPLAS=8080 ./cliente