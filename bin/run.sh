#!/bin/zsh

set -ue
BD="$(pwd)/$(dirname $0)/.."
. "${BD}/bin/variables.sh"

pushd "${BD}" >/dev/null

make
./build/${APP_NAME} $1

popd
