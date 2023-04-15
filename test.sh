#!/bin/bash

# ---
# Runs the tests
# ---

cd src/

make

PORT=9990

# run tests
export IP_TUPLAS=localhost
export PORT_TUPLAS=$PORT

# link dynamic library
export LD_LIBRARY_PATH=lib/

# run server
./servidor $PORT &
pid_servidor=$!

sleep 1

./testing

echo "Killing server at $pid_servidor"
kill $pid_servidor