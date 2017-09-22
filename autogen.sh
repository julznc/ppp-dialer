#!/bin/sh

autoreconf --install --force --verbose && \
  echo "If there are no error messages above, type:" && \
  echo "  ./configure && make"

