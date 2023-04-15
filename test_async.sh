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

sleep 2

# Be sure to have their ids with a higher difference than 2000 to be compliant with the tests (so we don't assume something doesn't exist)
ids=(1000 6000 9000 12000)
pids=()
length=${#ids[@]}
for (( j=0; j<length; j++ ));
do
    ./testing "${ids[$j]}" &
    pids+=($!)
done

echo "${pids[@]}"
wait "${pids[@]}"

kill $pid_servidor