#!/bin/bash

set -e

if [[ -z "${NACL_SDK_ROOT}" ]]; then
  echo "Error: NACL_SDK_ROOT is not set"
  exit 1
fi

if [[ -z "${NACLPORTS_DIR}" ]]; then
  echo "Error: NACLPORTS_DIR is not set"
  exit 1
fi

if [[ -z "${NACL_ARCH}" ]]; then
   export NACL_ARCH=pnacl
fi

ABSPATH=$(cd "$(dirname "$0")" && pwd)

cp -r ${ABSPATH}/../naclports/thrift ${NACLPORTS_DIR}/src/ports

pushd ${NACLPORTS_DIR}/src
make thrift
popd
