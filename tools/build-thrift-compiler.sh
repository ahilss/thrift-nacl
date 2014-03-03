# Script for building thrift compiler
#

set -e

ABSPATH=$(cd "$(dirname "$0")" && pwd)
TOOLS_DIR=${ABSPATH}
SRC_DIR=${ABSPATH}/../build/src
DST_DIR=${ABSPATH}/../build/usr

THRIFT_VERSION="0.9.1"
THRIFT_TAR_GZ="thrift-${THRIFT_VERSION}.tar.gz"
THRIFT_SHA1="dc54a54f8dc706ffddcd3e8c6cd5301c931af1cc"
THRIFT_URL="http://mirrors.ibiblio.org/apache/thrift/${THRIFT_VERSION}/${THRIFT_TAR_GZ}"
THRIFT_SRC_DIR="${SRC_DIR}/${THRIFT_TAR_GZ%.tar.gz}"

if [ ! -e ${THRIFT_SRC_DIR} ]; then
  "${TOOLS_DIR}/download_and_extract.sh" ${THRIFT_URL} ${THRIFT_SHA1} ${SRC_DIR}

  pushd "${THRIFT_SRC_DIR}"
  patch -p1 -N < ${ABSPATH}/thrift-compiler.patch
  popd
fi

pushd "${THRIFT_SRC_DIR}"

./configure --prefix=${DST_DIR} --with-boost=${DST_DIR} --with-cpp=yes --with-c_glib=no --with-csharp=no --with-d=no --with-erlang=no --with-haskell=no --with-go=no --with-java=no --with-perl=no --with-php=no --with-php_extension=no --with-python=no --with-ruby=no
make clean
make
make install
popd
