#!/bin/sh

set -xe

uid=$1
gid=$2

cd /tmp
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=~/.local \
      -DCMAKE_CXX_FLAGS="-Wall -Wextra -Werror -D_GLIBCXX_ASSERTIONS" \
      /io
make install
make tests
ctest --output-on-failure --timeout 50 ${MAKEFLAGS}

if test -n "${uid}" -a -n "${gid}"
then
  sudo cp -rv ~/.local/share/otagrum/doc/html /io
  sudo chown -R ${uid}:${gid} /io/html
fi

python /io/python/doc/examples/torque_model.py
