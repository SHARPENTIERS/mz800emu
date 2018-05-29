#!/bin/sh



if [ "${1}" != "Release-Linux" ]; then
	exit 0;
fi

CND_CONF="Release-Linux"

#
# EXE definitions
#
MKDIR_EXE=mkdir
CP_EXE=cp
FIND_EXE=find
BASENAME_EXE=basename


#for exe_tool in ${SED_EXE} ${WC_EXE} ${EGREP_EXE}; do
#	if [ ! -x ${exe_tool} ]; then
#		echo "ERROR: tool not found '${exe_tool}' !"
#		exit 1
#	fi
#done


DIST_DIR=./dist/Release-Linux/GNU-Linux-x86


if [ ! -e ${DIST_DIR} ]; then
	echo "ERROR: ${0} - DIST_DIR not exist '${DIST_DIR}'!"
	exit 1
fi

checkReturnCode () {
    rc=$?
    if [ $rc != 0 ]
    then
        exit $rc
    fi
}


if [ ! -e ${DIST_DIR}/ui_resources ]; then
	echo -e "Make copy ui_resources into DIST_DIR ...\n"
	${MKDIR_EXE} ${DIST_DIR}/ui_resources
	checkReturnCode
else
	echo -e "Updating ui_resources in DIST_DIR ...\n"
fi


${FIND_EXE} ./ui_resources/ -type f -name "*.glade" -or -name "*.css" -or -name "*.png" |\
while read src_file;do
	filename=`${BASENAME_EXE} ${src_file}`
	dst_file=${DIST_DIR}/ui_resources/${filename}
	if [ ! -e ${dst_file} ] || [ ${src_file} -nt ${dst_file} ]; then
		echo "COPY ${filename} into DIST_DIR ..."
		${CP_EXE} "${src_file}" "${dst_file}"
		checkReturnCode
	fi
done

exit 0
