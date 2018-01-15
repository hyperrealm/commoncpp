#!/bin/sh
# Wrapper script to invoke configure with adjusted options.

`dirname $0`/configure "$@" --disable-cpp --disable-shared --with-pic \
    --enable-pcre16 --prefix=$PWD/noinstall
