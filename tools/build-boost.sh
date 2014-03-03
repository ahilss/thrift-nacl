# Script for building boost library
#

set -e

ABSPATH=$(cd "$(dirname "$0")" && pwd)
TOOLS_DIR=${ABSPATH}
SRC_DIR=${ABSPATH}/../build/src
DST_DIR=${ABSPATH}/../build/usr

BOOST_TAR_GZ="boost_1_54_0.tar.gz"
BOOST_SHA1="069501636097d3f40ddfd996d29748bb23591c53"
BOOST_URL="http://sourceforge.net/projects/boost/files/boost/1.54.0/${BOOST_TAR_GZ}"
BOOST_SRC_DIR="${SRC_DIR}/${BOOST_TAR_GZ%.tar.gz}"

${TOOLS_DIR}/download_and_extract.sh ${BOOST_URL} ${BOOST_SHA1} ${SRC_DIR}

mkdir -p ${DST_DIR}/include
ln -s -f ${BOOST_SRC_DIR}/boost ${DST_DIR}/include
