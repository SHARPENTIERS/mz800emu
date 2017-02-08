#!/bin/sh


PROJECT_NAME="mz800emu"

if [ "${1}" != "Release-Win32" ]; then
	echo -e "\n\nERROR: not have package rules for '${1}' !\n"
	exit 0;
fi

PACKAGE_PLATFORM="win32"
PACKAGE_SURFIX="with_gtk_and_sdl_libs"

CND_CONF="Release-Win32"

echo -e "\n\nCreate package for: ${CND_CONF}"


#
# EXE definitions
#
SED_EXE=sed
WC_EXE=wc
EGREP_EXE=egrep
RAR_EXE=rar
FIND_EXE=find
RM_EXE=rm
MKDIR_EXE=mkdir
CP_EXE=cp
CHMOD_EXE=chmod
UNAME_EXE=uname


# Functions
checkReturnCode () {
    rc=$?
    if [ $rc != 0 ]
    then
        exit $rc
    fi
}


makeDirectory () {
# $1 directory path   
# $2 permission (optional)
    ${MKDIR_EXE} -p "$1"
    checkReturnCode
    if [ "$2" != "" ]
    then
      ${CHMOD_EXE} $2 "$1"
      checkReturnCode
    fi
}


copyFileToTmpDir () {
# $1 from-file path
# $2 to-file path  
# $3 permission    
    ${CP_EXE} "$1" "$2"
    checkReturnCode
    if [ "$3" != "" ]
    then
        ${CHMOD_EXE} $3 "$2"
        checkReturnCode
    fi
}


#for exe_tool in ${SED_EXE} ${WC_EXE} ${EGREP_EXE}; do
#	if [ ! -x ${exe_tool} ]; then
#		echo "ERROR: tool not found '${exe_tool}' !"
#		exit 1
#	fi
#done


EMULATOR_VERSION_DEF=`${EGREP_EXE} "^#define\s+EMULATOR_VERSION\s+\".*\"" src/cfgmain.h`

if [ $? -ne 0 ]; then
	echo "ERROR: Can't get EMULATOR_VERSION !"
	exit 1
fi

let LINES=`echo "${EMULATOR_VERSION_DEF}" | ${WC_EXE} -l`
checkReturnCode

if [ ${LINES} -ne 1 ]; then
	echo -e "ERROR: definition the  EMULATOR_VERSION has multiple lines:\n"
	echo -e "${EMULATOR_VERSION_DEF}\n"
	exit 1
fi

EMULATOR_VERSION_TXT=`echo "${EMULATOR_VERSION_DEF}" | ${SED_EXE} --regexp-extended -e 's/#define\s+EMULATOR_VERSION\s+\"//' -e 's/".*//'`
checkReturnCode
EMULATOR_VERSION_TXT=`echo ${EMULATOR_VERSION_TXT} | ${SED_EXE} --regexp-extended -e 's/^\s+//'`
checkReturnCode
EMULATOR_VERSION_TXT=`echo ${EMULATOR_VERSION_TXT} | ${SED_EXE} --regexp-extended -e 's/\s/_/g'`
checkReturnCode

if [ -z "${EMULATOR_VERSION_TXT}" ]; then
	echo "ERROR: definition the  EMULATOR_VERSION is empty !"
	exit 1
fi


echo -e "Release version: ${EMULATOR_VERSION_TXT}\n"

#
# Castecne zkopirovano z nbproject/Package-*.bash
#

# Macros
TOP=`pwd`
CND_PLATFORM=i686-w64-mingw32-Linux
CND_DISTDIR=dist   
CND_BUILDDIR=build
NBTMPDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tmp-packaging
TMPDIRNAME=tmp-packaging
OUTPUT_PATH=${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mz800emu-x86.exe
OUTPUT_BASENAME=mz800emu.exe
PACKAGE_TOP_DIR=${PROJECT_NAME}-${EMULATOR_VERSION_TXT}-${PACKAGE_PLATFORM}/
PACKAGE_ARCHIVE_NAME=${PROJECT_NAME}-${EMULATOR_VERSION_TXT}-${PACKAGE_PLATFORM}-${PACKAGE_SURFIX}

 

# Setup
cd "${TOP}"
checkReturnCode
${MKDIR_EXE} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package
${RM_EXE} -rf ${NBTMPDIR}
${MKDIR_EXE} -p ${NBTMPDIR}


# Copy files and create directories and links

PACKAGE_DIRS="
	${PACKAGE_TOP_DIR} \
	${PACKAGE_TOP_DIR}/ui_resources \
	${PACKAGE_TOP_DIR}/Documentation \
	${PACKAGE_TOP_DIR}/runtime/sdl-2 \
	${PACKAGE_TOP_DIR}/runtime/gtk-3/share/glib-2.0/schemas \
	${PACKAGE_TOP_DIR}/runtime/gtk-3/share/locale/pt_BR/LC_MESSAGES \
"


#
# mkdirs
#
cd "${TOP}"
for dirname in ${PACKAGE_DIRS}; do
	if [ ! -z ${dirname} ]; then
		#echo "MKDIR: '${NBTMPDIR}/${dirname}'"
		makeDirectory "${NBTMPDIR}/${dirname}"
	fi
done


#
# copyfiles
#
copyFileToTmpDir "${OUTPUT_PATH}" "${NBTMPDIR}/${PACKAGE_TOP_DIR}/${OUTPUT_BASENAME}" 0755
copyFileToTmpDir "src/windows_icon/mz800emu.ico" "${NBTMPDIR}/${PACKAGE_TOP_DIR}/mz800emu.ico" 0644


# ui_resources
for filename in mz800emu_cmt.glade  mz800emu.css  mz800emu_debugger.glade  mz800emu.glade  mz800emu_logo.png
do
	copyFileToTmpDir "ui_resources/${filename}" "${NBTMPDIR}/${PACKAGE_TOP_DIR}/ui_resources/${filename}" 0644
done


# Docs
for filename in Changelog_EN.txt Readme_CZ.txt Readme_EN.txt Runtime_libs_CZ.txt Runtime_libs_EN.txt
do
	copyFileToTmpDir "Documentation/${filename}" "${NBTMPDIR}/${PACKAGE_TOP_DIR}/Documentation/${filename}" 0644
done


#
# runtime files
#
cd ~/win32_mz800emu_runtime
${FIND_EXE} -type f | while read filename; do
	cd "${TOP}"
	copyFileToTmpDir ~/win32_mz800emu_runtime/${filename} "${NBTMPDIR}/${PACKAGE_TOP_DIR}/${filename}" 0644
	#echo "${filename} ${NBTMPDIR}/${PACKAGE_TOP_DIR}/${filename} 0644"
done


# Generate tar file
cd "${TOP}"
checkReturnCode

#${RM_EXE} -f ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package/${PACKAGE_ARCHIVE_NAME}.tar
#cd ${NBTMPDIR}
#tar -vcf ../../../../${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package/${PACKAGE_ARCHIVE_NAME}.tar *

${RM_EXE} -f ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package/${PACKAGE_ARCHIVE_NAME}.rar
checkReturnCode

cd ${NBTMPDIR}
checkReturnCode

${RAR_EXE} a ../../../../${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package/${PACKAGE_ARCHIVE_NAME}.rar *
checkReturnCode

#
# Only on my development desktop:
#
DESKTOP_NAME="arrakis.ordoz.com"

FULL_HOSTNAME=`${UNAME_EXE} -n`

if [ "${FULL_HOSTNAME}" = "${DESKTOP_NAME}" ]; then
	echo -e "\n\nCopy ${PACKAGE_ARCHIVE_NAME}.rar to WIN32 share directory...\n"
	cd "${TOP}"
	checkReturnCode
	${CP_EXE} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/package/${PACKAGE_ARCHIVE_NAME}.rar ~/share/
	checkReturnCode
fi


# Cleanup
cd "${TOP}"
checkReturnCode
${RM_EXE} -rf ${NBTMPDIR}
checkReturnCode

exit 0
