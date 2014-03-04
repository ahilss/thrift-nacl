#!/bin/bash
#
# Build thrift compiler and naclports

set -e

ABSPATH=$(cd "$(dirname "$0")" && pwd)
TOOLS_DIR=${ABSPATH}

${TOOLS_DIR}/build-boost.sh
${TOOLS_DIR}/build-thrift-compiler.sh
${TOOLS_DIR}/build-naclports.sh
