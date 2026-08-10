#!/bin/bash
set -e
INPUT_FILE=$1
: "${GCC_PATH}:="xpack-riscv-none-elf-gcc-15.2.0-1"}"
: "${GCC_PREFIX:="${GCC_PATH}/bin/riscv-none-elf"}"
: "${TEMP_FILE:="tmp.o"}"
: "${BIO_VARNAME:="bio_program"}"

${GCC_PREFIX}-as \
    -march=rv32izmmul \
    -mabi=ilp32e \
    -o ${TEMP_FILE} \
    ${INPUT_FILE}
CODE=`${GCC_PREFIX}-objcopy \
    -O binary \
    -j .text ${TEMP_FILE} \
    /dev/stdout \
    | hexdump -e '1/4 "    0x%08x," "\n"' \
    | sed '$s/,$//'`
rm -f ${TEMP_FILE}

echo -n "static const uint32_t ${BIO_VARNAME}_length = "
echo -n `echo "${CODE}" | wc -l`
echo ";"

echo "static const uint32_t ${BIO_VARNAME}[] = {"
echo "${CODE}"
echo "};"