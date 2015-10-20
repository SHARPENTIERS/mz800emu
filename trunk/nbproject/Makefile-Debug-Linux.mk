#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug-Linux
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/audio.o \
	${OBJECTDIR}/src/build_time.o \
	${OBJECTDIR}/src/cfgfile/cfgelement.o \
	${OBJECTDIR}/src/cfgfile/cfgmodule.o \
	${OBJECTDIR}/src/cfgfile/cfgroot.o \
	${OBJECTDIR}/src/cfgmain.o \
	${OBJECTDIR}/src/cmt/cmt.o \
	${OBJECTDIR}/src/cmt/cmt_hack.o \
	${OBJECTDIR}/src/ctc8253/ctc8253.o \
	${OBJECTDIR}/src/debugger/breakpoints.o \
	${OBJECTDIR}/src/debugger/debugger.o \
	${OBJECTDIR}/src/debugger/inline_asm.o \
	${OBJECTDIR}/src/display.o \
	${OBJECTDIR}/src/fdc/fdc.o \
	${OBJECTDIR}/src/fdc/wd279x.o \
	${OBJECTDIR}/src/gdg/framebuffer.o \
	${OBJECTDIR}/src/gdg/gdg.o \
	${OBJECTDIR}/src/gdg/hwscroll.o \
	${OBJECTDIR}/src/gdg/vramctrl.o \
	${OBJECTDIR}/src/iface_sdl/iface_sdl.o \
	${OBJECTDIR}/src/iface_sdl/iface_sdl_audio.o \
	${OBJECTDIR}/src/iface_sdl/iface_sdl_keyboard.o \
	${OBJECTDIR}/src/iface_sdl/iface_sdl_log.o \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/memory/ROM/ROM_CGROM.o \
	${OBJECTDIR}/src/memory/ROM/ROM_MZ700.o \
	${OBJECTDIR}/src/memory/ROM/ROM_MZ800.o \
	${OBJECTDIR}/src/memory/memory.o \
	${OBJECTDIR}/src/mz800.o \
	${OBJECTDIR}/src/pio8255/pio8255.o \
	${OBJECTDIR}/src/pioz80/pioz80.o \
	${OBJECTDIR}/src/port.o \
	${OBJECTDIR}/src/psg/psg.o \
	${OBJECTDIR}/src/ramdisk/ramdisk.o \
	${OBJECTDIR}/src/sharpmz_ascii.o \
	${OBJECTDIR}/src/ui/debugger/ui_breakpoints.o \
	${OBJECTDIR}/src/ui/debugger/ui_debugger.o \
	${OBJECTDIR}/src/ui/debugger/ui_debugger_callbacks.o \
	${OBJECTDIR}/src/ui/debugger/ui_debugger_iasm.o \
	${OBJECTDIR}/src/ui/ui_cmt.o \
	${OBJECTDIR}/src/ui/ui_display.o \
	${OBJECTDIR}/src/ui/ui_fdc.o \
	${OBJECTDIR}/src/ui/ui_main.o \
	${OBJECTDIR}/src/ui/ui_ramdisk.o \
	${OBJECTDIR}/src/ui/ui_utils.o \
	${OBJECTDIR}/src/z80ex/z80ex.o \
	${OBJECTDIR}/src/z80ex/z80ex_dasm.o


# C Compiler Flags
CFLAGS=-pedantic -pipe -Wl,--export-dynamic `${LINUX_SDL2_CONFIG} --cflags` `${LINUX_PKGCONFIG} --cflags ${PKG_OBJECTS}` 

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mz800emu

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mz800emu: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	gcc -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mz800emu ${OBJECTFILES} ${LDLIBSOPTIONS} -lm ${LINUX_SDL2_LIBS} `${LINUX_PKGCONFIG} --libs ${PKG_OBJECTS}`

${OBJECTDIR}/src/audio.o: src/audio.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/audio.o src/audio.c

${OBJECTDIR}/src/build_time.o: src/build_time.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/build_time.o src/build_time.c

${OBJECTDIR}/src/cfgfile/cfgelement.o: src/cfgfile/cfgelement.c 
	${MKDIR} -p ${OBJECTDIR}/src/cfgfile
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cfgfile/cfgelement.o src/cfgfile/cfgelement.c

${OBJECTDIR}/src/cfgfile/cfgmodule.o: src/cfgfile/cfgmodule.c 
	${MKDIR} -p ${OBJECTDIR}/src/cfgfile
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cfgfile/cfgmodule.o src/cfgfile/cfgmodule.c

${OBJECTDIR}/src/cfgfile/cfgroot.o: src/cfgfile/cfgroot.c 
	${MKDIR} -p ${OBJECTDIR}/src/cfgfile
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cfgfile/cfgroot.o src/cfgfile/cfgroot.c

${OBJECTDIR}/src/cfgmain.o: src/cfgmain.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cfgmain.o src/cfgmain.c

${OBJECTDIR}/src/cmt/cmt.o: src/cmt/cmt.c 
	${MKDIR} -p ${OBJECTDIR}/src/cmt
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cmt/cmt.o src/cmt/cmt.c

${OBJECTDIR}/src/cmt/cmt_hack.o: src/cmt/cmt_hack.c 
	${MKDIR} -p ${OBJECTDIR}/src/cmt
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cmt/cmt_hack.o src/cmt/cmt_hack.c

${OBJECTDIR}/src/ctc8253/ctc8253.o: src/ctc8253/ctc8253.c 
	${MKDIR} -p ${OBJECTDIR}/src/ctc8253
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ctc8253/ctc8253.o src/ctc8253/ctc8253.c

${OBJECTDIR}/src/debugger/breakpoints.o: src/debugger/breakpoints.c 
	${MKDIR} -p ${OBJECTDIR}/src/debugger
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/debugger/breakpoints.o src/debugger/breakpoints.c

${OBJECTDIR}/src/debugger/debugger.o: src/debugger/debugger.c 
	${MKDIR} -p ${OBJECTDIR}/src/debugger
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/debugger/debugger.o src/debugger/debugger.c

${OBJECTDIR}/src/debugger/inline_asm.o: src/debugger/inline_asm.c 
	${MKDIR} -p ${OBJECTDIR}/src/debugger
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/debugger/inline_asm.o src/debugger/inline_asm.c

${OBJECTDIR}/src/display.o: src/display.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/display.o src/display.c

${OBJECTDIR}/src/fdc/fdc.o: src/fdc/fdc.c 
	${MKDIR} -p ${OBJECTDIR}/src/fdc
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/fdc/fdc.o src/fdc/fdc.c

${OBJECTDIR}/src/fdc/wd279x.o: src/fdc/wd279x.c 
	${MKDIR} -p ${OBJECTDIR}/src/fdc
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/fdc/wd279x.o src/fdc/wd279x.c

${OBJECTDIR}/src/gdg/framebuffer.o: src/gdg/framebuffer.c 
	${MKDIR} -p ${OBJECTDIR}/src/gdg
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/gdg/framebuffer.o src/gdg/framebuffer.c

${OBJECTDIR}/src/gdg/gdg.o: src/gdg/gdg.c 
	${MKDIR} -p ${OBJECTDIR}/src/gdg
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/gdg/gdg.o src/gdg/gdg.c

${OBJECTDIR}/src/gdg/hwscroll.o: src/gdg/hwscroll.c 
	${MKDIR} -p ${OBJECTDIR}/src/gdg
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/gdg/hwscroll.o src/gdg/hwscroll.c

${OBJECTDIR}/src/gdg/vramctrl.o: src/gdg/vramctrl.c 
	${MKDIR} -p ${OBJECTDIR}/src/gdg
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/gdg/vramctrl.o src/gdg/vramctrl.c

${OBJECTDIR}/src/iface_sdl/iface_sdl.o: src/iface_sdl/iface_sdl.c 
	${MKDIR} -p ${OBJECTDIR}/src/iface_sdl
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/iface_sdl/iface_sdl.o src/iface_sdl/iface_sdl.c

${OBJECTDIR}/src/iface_sdl/iface_sdl_audio.o: src/iface_sdl/iface_sdl_audio.c 
	${MKDIR} -p ${OBJECTDIR}/src/iface_sdl
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/iface_sdl/iface_sdl_audio.o src/iface_sdl/iface_sdl_audio.c

${OBJECTDIR}/src/iface_sdl/iface_sdl_keyboard.o: src/iface_sdl/iface_sdl_keyboard.c 
	${MKDIR} -p ${OBJECTDIR}/src/iface_sdl
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/iface_sdl/iface_sdl_keyboard.o src/iface_sdl/iface_sdl_keyboard.c

${OBJECTDIR}/src/iface_sdl/iface_sdl_log.o: src/iface_sdl/iface_sdl_log.c 
	${MKDIR} -p ${OBJECTDIR}/src/iface_sdl
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/iface_sdl/iface_sdl_log.o src/iface_sdl/iface_sdl_log.c

${OBJECTDIR}/src/main.o: src/main.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.c

${OBJECTDIR}/src/memory/ROM/ROM_CGROM.o: src/memory/ROM/ROM_CGROM.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/ROM_CGROM.o src/memory/ROM/ROM_CGROM.c

${OBJECTDIR}/src/memory/ROM/ROM_MZ700.o: src/memory/ROM/ROM_MZ700.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/ROM_MZ700.o src/memory/ROM/ROM_MZ700.c

${OBJECTDIR}/src/memory/ROM/ROM_MZ800.o: src/memory/ROM/ROM_MZ800.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/ROM_MZ800.o src/memory/ROM/ROM_MZ800.c

${OBJECTDIR}/src/memory/memory.o: src/memory/memory.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/memory.o src/memory/memory.c

${OBJECTDIR}/src/mz800.o: src/mz800.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/mz800.o src/mz800.c

${OBJECTDIR}/src/pio8255/pio8255.o: src/pio8255/pio8255.c 
	${MKDIR} -p ${OBJECTDIR}/src/pio8255
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/pio8255/pio8255.o src/pio8255/pio8255.c

${OBJECTDIR}/src/pioz80/pioz80.o: src/pioz80/pioz80.c 
	${MKDIR} -p ${OBJECTDIR}/src/pioz80
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/pioz80/pioz80.o src/pioz80/pioz80.c

${OBJECTDIR}/src/port.o: src/port.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/port.o src/port.c

${OBJECTDIR}/src/psg/psg.o: src/psg/psg.c 
	${MKDIR} -p ${OBJECTDIR}/src/psg
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/psg/psg.o src/psg/psg.c

${OBJECTDIR}/src/ramdisk/ramdisk.o: src/ramdisk/ramdisk.c 
	${MKDIR} -p ${OBJECTDIR}/src/ramdisk
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ramdisk/ramdisk.o src/ramdisk/ramdisk.c

${OBJECTDIR}/src/sharpmz_ascii.o: src/sharpmz_ascii.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/sharpmz_ascii.o src/sharpmz_ascii.c

${OBJECTDIR}/src/ui/debugger/ui_breakpoints.o: src/ui/debugger/ui_breakpoints.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui/debugger
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/debugger/ui_breakpoints.o src/ui/debugger/ui_breakpoints.c

${OBJECTDIR}/src/ui/debugger/ui_debugger.o: src/ui/debugger/ui_debugger.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui/debugger
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/debugger/ui_debugger.o src/ui/debugger/ui_debugger.c

${OBJECTDIR}/src/ui/debugger/ui_debugger_callbacks.o: src/ui/debugger/ui_debugger_callbacks.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui/debugger
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/debugger/ui_debugger_callbacks.o src/ui/debugger/ui_debugger_callbacks.c

${OBJECTDIR}/src/ui/debugger/ui_debugger_iasm.o: src/ui/debugger/ui_debugger_iasm.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui/debugger
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/debugger/ui_debugger_iasm.o src/ui/debugger/ui_debugger_iasm.c

${OBJECTDIR}/src/ui/ui_cmt.o: src/ui/ui_cmt.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/ui_cmt.o src/ui/ui_cmt.c

${OBJECTDIR}/src/ui/ui_display.o: src/ui/ui_display.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/ui_display.o src/ui/ui_display.c

${OBJECTDIR}/src/ui/ui_fdc.o: src/ui/ui_fdc.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/ui_fdc.o src/ui/ui_fdc.c

${OBJECTDIR}/src/ui/ui_main.o: src/ui/ui_main.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/ui_main.o src/ui/ui_main.c

${OBJECTDIR}/src/ui/ui_ramdisk.o: src/ui/ui_ramdisk.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/ui_ramdisk.o src/ui/ui_ramdisk.c

${OBJECTDIR}/src/ui/ui_utils.o: src/ui/ui_utils.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/ui_utils.o src/ui/ui_utils.c

${OBJECTDIR}/src/z80ex/z80ex.o: src/z80ex/z80ex.c 
	${MKDIR} -p ${OBJECTDIR}/src/z80ex
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/z80ex/z80ex.o src/z80ex/z80ex.c

${OBJECTDIR}/src/z80ex/z80ex_dasm.o: src/z80ex/z80ex_dasm.c 
	${MKDIR} -p ${OBJECTDIR}/src/z80ex
	${RM} "$@.d"
	$(COMPILE.c) -g -Wall -DLINUX -DMZ800_DEBUGGER -D_XOPEN_SOURCE=500 -Isrc -Isrc/z80ex/ -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/z80ex/z80ex_dasm.o src/z80ex/z80ex_dasm.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mz800emu

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
