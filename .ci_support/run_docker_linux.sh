#!/bin/sh

set -xe

cd /tmp
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=${HOME}/.local \
      -DCMAKE_UNITY_BUILD=ON \
      -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wpedantic -Wshadow -Werror -D_GLIBCXX_ASSERTIONS --coverage" \
      -DSWIG_COMPILE_FLAGS="-O1 -Wno-unused-parameter -Wno-shadow" \
      -DUSE_SPHINX=ON -DSPHINX_FLAGS="-W -T -j4" \
      /io
make install
make tests
ctest --output-on-failure --timeout 100 ${MAKEFLAGS}

# coverage
gcov `find lib/src/ -name "*.gcno"`
lcov --capture --directory . --output-file coverage.info --include "*.cxx"
genhtml --output-directory coverage coverage.info
cp -v coverage.info coverage

uid=$1
gid=$2
if test -n "${uid}" -a -n "${gid}"
then
  sudo cp -r ~/.local/share/doc/*/html /io
  sudo chown -R ${uid}:${gid} /io/html
fi

python /io/python/doc/examples/torque_model.py
