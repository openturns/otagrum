#!/bin/sh

set -xe

cd /tmp
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=~/.local \
      -DCMAKE_UNITY_BUILD=ON \
      -DCMAKE_CXX_FLAGS="-Wall -Wextra -Werror -Wno-unused-parameter -D_GLIBCXX_ASSERTIONS" \
      -DSPHINX_FLAGS="-W -T -j4" \
      /io
make install
make tests
ctest --output-on-failure --timeout 100 ${MAKEFLAGS}

uid=$1
gid=$2
if test -n "${uid}" -a -n "${gid}"
then
  sudo cp -r ~/.local/share/*/doc/html /io
  sudo chown -R ${uid}:${gid} /io/html
fi

python /io/python/doc/examples/torque_model.py
