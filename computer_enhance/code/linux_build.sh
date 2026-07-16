COMMON_CMD_FLAGS="-g"
COMMON_LINKER_FLAGS=""

if [ ! -d ../../build ]; then
  mkdir ../../build
fi

pushd ../../build

rm -rdf *

gcc $COMMON_CMD_FLAGS ../computer_enhance/code/linux_enhance.c  $COMMON_LINKER_FLAGS

popd
