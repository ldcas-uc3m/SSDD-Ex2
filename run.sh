#!/bin/bash
cd src/

# compile
make

# link dynamic library
export LD_LIBRARY_PATH=lib/

# run
./servidor 9990 &
env IP_TUPLAS=localhost PORT_TUPLAS=9990 ./cliente