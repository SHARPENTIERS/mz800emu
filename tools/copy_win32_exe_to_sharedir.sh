#!/bin/sh

#
# This script is only for my development desktop
#

DESKTOP_NAME="arrakis.ordoz.com"
#SRC_FILE="dist/Release-Win32/i686-w64-mingw32-Linux-x86/mz800emu.exe"
SRC_FILE="dist/${1}/${2}/mz800emu.exe"
DST_DIR=~/share/mz800emu/


UNAME_EXE=uname

#if [ ! -x ${UNAME_EXE} ]; then
#	echo -e "$0 - ERROR\nUNAME_EXE not found '${UNAME_EXE}'\n"
#	exit 0
#fi

FULL_HOSTNAME=`${UNAME_EXE} -n`

if [ $? -ne 0 ]; then
	echo -e "$0 - ERROR\nUNAME_EXE not found '${UNAME_EXE}'\n"
	exit 0
fi

if [ "${1}" != "Release-Win32" ]; then exit 0; fi

if [ "${FULL_HOSTNAME}" != "${DESKTOP_NAME}" ]; then exit 0; fi

if [ ! -f "${SRC_FILE}" ]; then exit 0; fi

if [ ! -d "${DST_DIR}" ]; then exit 1; fi

echo -e "\n\nCopy mz800emu.exe to WIN32 share directory...\n"

/bin/cp "${SRC_FILE}" "${DST_DIR}/"

exit $?