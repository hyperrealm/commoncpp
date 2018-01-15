#!/bin/sh
# Wrapper script to invoke configure with adjusted options.

`dirname $0`/configure "$@" --disable-shared --with-pic \
    --prefix=$PWD/noinstall CFLAGS=-DSQLITE_ENABLE_COLUMN_METADATA
