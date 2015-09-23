#!/bin/sh

#
# This script is only for my development desktop
#


DESKTOP_NAME="arrakis.ordoz.com"
SRC_FILE="dist/Release-Win32/i686-w64-mingw32-Linux-x86/mz800emu.exe"
DST_DIR=~/share//mz800emu/

FULL_HOSTNAME=`/usr/bin/uname -n`

if [ "${FULL_HOSTNAME}" != "${DESKTOP_NAME}" ]; then exit 0; fi

if [ ! -f "${SRC_FILE}" ]; then exit 0; fi

if [ ! -d "${DST_DIR}" ]; then exit 1; fi

echo -e "\n\nCopy mz800emu.exe to WIN32 share directory...\n"

/bin/cp "${SRC_FILE}" "${DST_DIR}/"

exit $?
