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
CC=x86_64-w64-mingw32-gcc
CCC=x86_64-w64-mingw32-g++
CXX=x86_64-w64-mingw32-g++
FC=x86_64-w64-mingw32-gfortran
AS=x86_64-w64-mingw32-as

# Macros
CND_PLATFORM=x86_64-w64-mingw32-Linux
CND_DLIB_EXT=so
CND_CONF=Release-Win64
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
	${OBJECTDIR}/src/fs_layer.o \
	${OBJECTDIR}/src/gdg/framebuffer.o \
	${OBJECTDIR}/src/gdg/gdg.o \
	${OBJECTDIR}/src/gdg/hwscroll.o \
	${OBJECTDIR}/src/gdg/vramctrl.o \
	${OBJECTDIR}/src/iface_sdl/iface_sdl.o \
	${OBJECTDIR}/src/iface_sdl/iface_sdl_audio.o \
	${OBJECTDIR}/src/iface_sdl/iface_sdl_keyboard.o \
	${OBJECTDIR}/src/iface_sdl/iface_sdl_log.o \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/memory/ROM/JSS-1.3/ROM_JSS103_CGROM.o \
	${OBJECTDIR}/src/memory/ROM/JSS-1.3/ROM_JSS103_MZ700.o \
	${OBJECTDIR}/src/memory/ROM/JSS-1.3/ROM_JSS103_MZ800.o \
	${OBJECTDIR}/src/memory/ROM/JSS-1.5C/ROM_JSS105C_CGROM.o \
	${OBJECTDIR}/src/memory/ROM/JSS-1.5C/ROM_JSS105C_MZ700.o \
	${OBJECTDIR}/src/memory/ROM/JSS-1.5C/ROM_JSS105C_MZ800.o \
	${OBJECTDIR}/src/memory/ROM/JSS-1.6A/ROM_JSS106A_CGROM.o \
	${OBJECTDIR}/src/memory/ROM/JSS-1.6A/ROM_JSS106A_MZ700.o \
	${OBJECTDIR}/src/memory/ROM/JSS-1.6A/ROM_JSS106A_MZ800.o \
	${OBJECTDIR}/src/memory/ROM/JSS-1.8C/ROM_JSS108C_CGROM.o \
	${OBJECTDIR}/src/memory/ROM/JSS-1.8C/ROM_JSS108C_MZ700.o \
	${OBJECTDIR}/src/memory/ROM/JSS-1.8C/ROM_JSS108C_MZ800.o \
	${OBJECTDIR}/src/memory/ROM/ROM_CGROM.o \
	${OBJECTDIR}/src/memory/ROM/ROM_MZ700.o \
	${OBJECTDIR}/src/memory/ROM/ROM_MZ800.o \
	${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_MZ700.o \
	${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_en_CGROM.o \
	${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_en_MZ800.o \
	${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_ge_CGROM.o \
	${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_ge_MZ800.o \
	${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_jap_CGROM.o \
	${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_jap_MZ800.o \
	${OBJECTDIR}/src/memory/memory.o \
	${OBJECTDIR}/src/memory/rom.o \
	${OBJECTDIR}/src/mz800.o \
	${OBJECTDIR}/src/pio8255/pio8255.o \
	${OBJECTDIR}/src/pioz80/pioz80.o \
	${OBJECTDIR}/src/port.o \
	${OBJECTDIR}/src/psg/psg.o \
	${OBJECTDIR}/src/qdisk/qdisk.o \
	${OBJECTDIR}/src/ramdisk/ramdisk.o \
	${OBJECTDIR}/src/sharpmz_ascii.o \
	${OBJECTDIR}/src/ui/debugger/ui_breakpoints.o \
	${OBJECTDIR}/src/ui/debugger/ui_debugger.o \
	${OBJECTDIR}/src/ui/debugger/ui_debugger_callbacks.o \
	${OBJECTDIR}/src/ui/debugger/ui_debugger_iasm.o \
	${OBJECTDIR}/src/ui/debugger/ui_memdump.o \
	${OBJECTDIR}/src/ui/ui_cmt.o \
	${OBJECTDIR}/src/ui/ui_display.o \
	${OBJECTDIR}/src/ui/ui_fcbutton.o \
	${OBJECTDIR}/src/ui/ui_fdc.o \
	${OBJECTDIR}/src/ui/ui_main.o \
	${OBJECTDIR}/src/ui/ui_qdisk.o \
	${OBJECTDIR}/src/ui/ui_ramdisk.o \
	${OBJECTDIR}/src/ui/ui_rom.o \
	${OBJECTDIR}/src/ui/ui_utils.o \
	${OBJECTDIR}/src/z80ex/z80ex.o \
	${OBJECTDIR}/src/z80ex/z80ex_dasm.o


# C Compiler Flags
CFLAGS=-pedantic -pipe -Wl,--export-dynamic ${PROJECT_CFLAGS}

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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mz800emu-x64.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mz800emu-x64.exe: src/windows_icon/app.o

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mz800emu-x64.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mz800emu-x64.exe ${OBJECTFILES} ${LDLIBSOPTIONS} ${PROJECT_LIBS} ${CND_BUILDDIR}/${CONF}/${CND_PLATFORM_${CONF}}/src/windows_icon/app.o

${OBJECTDIR}/src/audio.o: src/audio.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/audio.o src/audio.c

${OBJECTDIR}/src/build_time.o: src/build_time.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/build_time.o src/build_time.c

${OBJECTDIR}/src/cfgfile/cfgelement.o: src/cfgfile/cfgelement.c 
	${MKDIR} -p ${OBJECTDIR}/src/cfgfile
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cfgfile/cfgelement.o src/cfgfile/cfgelement.c

${OBJECTDIR}/src/cfgfile/cfgmodule.o: src/cfgfile/cfgmodule.c 
	${MKDIR} -p ${OBJECTDIR}/src/cfgfile
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cfgfile/cfgmodule.o src/cfgfile/cfgmodule.c

${OBJECTDIR}/src/cfgfile/cfgroot.o: src/cfgfile/cfgroot.c 
	${MKDIR} -p ${OBJECTDIR}/src/cfgfile
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cfgfile/cfgroot.o src/cfgfile/cfgroot.c

${OBJECTDIR}/src/cfgmain.o: src/cfgmain.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cfgmain.o src/cfgmain.c

${OBJECTDIR}/src/cmt/cmt.o: src/cmt/cmt.c 
	${MKDIR} -p ${OBJECTDIR}/src/cmt
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cmt/cmt.o src/cmt/cmt.c

${OBJECTDIR}/src/cmt/cmt_hack.o: src/cmt/cmt_hack.c 
	${MKDIR} -p ${OBJECTDIR}/src/cmt
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cmt/cmt_hack.o src/cmt/cmt_hack.c

${OBJECTDIR}/src/ctc8253/ctc8253.o: src/ctc8253/ctc8253.c 
	${MKDIR} -p ${OBJECTDIR}/src/ctc8253
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ctc8253/ctc8253.o src/ctc8253/ctc8253.c

${OBJECTDIR}/src/debugger/breakpoints.o: src/debugger/breakpoints.c 
	${MKDIR} -p ${OBJECTDIR}/src/debugger
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/debugger/breakpoints.o src/debugger/breakpoints.c

${OBJECTDIR}/src/debugger/debugger.o: src/debugger/debugger.c 
	${MKDIR} -p ${OBJECTDIR}/src/debugger
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/debugger/debugger.o src/debugger/debugger.c

${OBJECTDIR}/src/debugger/inline_asm.o: src/debugger/inline_asm.c 
	${MKDIR} -p ${OBJECTDIR}/src/debugger
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/debugger/inline_asm.o src/debugger/inline_asm.c

${OBJECTDIR}/src/display.o: src/display.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/display.o src/display.c

${OBJECTDIR}/src/fdc/fdc.o: src/fdc/fdc.c 
	${MKDIR} -p ${OBJECTDIR}/src/fdc
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/fdc/fdc.o src/fdc/fdc.c

${OBJECTDIR}/src/fdc/wd279x.o: src/fdc/wd279x.c 
	${MKDIR} -p ${OBJECTDIR}/src/fdc
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/fdc/wd279x.o src/fdc/wd279x.c

${OBJECTDIR}/src/fs_layer.o: src/fs_layer.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/fs_layer.o src/fs_layer.c

${OBJECTDIR}/src/gdg/framebuffer.o: src/gdg/framebuffer.c 
	${MKDIR} -p ${OBJECTDIR}/src/gdg
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/gdg/framebuffer.o src/gdg/framebuffer.c

${OBJECTDIR}/src/gdg/gdg.o: src/gdg/gdg.c 
	${MKDIR} -p ${OBJECTDIR}/src/gdg
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/gdg/gdg.o src/gdg/gdg.c

${OBJECTDIR}/src/gdg/hwscroll.o: src/gdg/hwscroll.c 
	${MKDIR} -p ${OBJECTDIR}/src/gdg
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/gdg/hwscroll.o src/gdg/hwscroll.c

${OBJECTDIR}/src/gdg/vramctrl.o: src/gdg/vramctrl.c 
	${MKDIR} -p ${OBJECTDIR}/src/gdg
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/gdg/vramctrl.o src/gdg/vramctrl.c

${OBJECTDIR}/src/iface_sdl/iface_sdl.o: src/iface_sdl/iface_sdl.c 
	${MKDIR} -p ${OBJECTDIR}/src/iface_sdl
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/iface_sdl/iface_sdl.o src/iface_sdl/iface_sdl.c

${OBJECTDIR}/src/iface_sdl/iface_sdl_audio.o: src/iface_sdl/iface_sdl_audio.c 
	${MKDIR} -p ${OBJECTDIR}/src/iface_sdl
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/iface_sdl/iface_sdl_audio.o src/iface_sdl/iface_sdl_audio.c

${OBJECTDIR}/src/iface_sdl/iface_sdl_keyboard.o: src/iface_sdl/iface_sdl_keyboard.c 
	${MKDIR} -p ${OBJECTDIR}/src/iface_sdl
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/iface_sdl/iface_sdl_keyboard.o src/iface_sdl/iface_sdl_keyboard.c

${OBJECTDIR}/src/iface_sdl/iface_sdl_log.o: src/iface_sdl/iface_sdl_log.c 
	${MKDIR} -p ${OBJECTDIR}/src/iface_sdl
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/iface_sdl/iface_sdl_log.o src/iface_sdl/iface_sdl_log.c

${OBJECTDIR}/src/main.o: src/main.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.c

${OBJECTDIR}/src/memory/ROM/JSS-1.3/ROM_JSS103_CGROM.o: src/memory/ROM/JSS-1.3/ROM_JSS103_CGROM.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/JSS-1.3
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/JSS-1.3/ROM_JSS103_CGROM.o src/memory/ROM/JSS-1.3/ROM_JSS103_CGROM.c

${OBJECTDIR}/src/memory/ROM/JSS-1.3/ROM_JSS103_MZ700.o: src/memory/ROM/JSS-1.3/ROM_JSS103_MZ700.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/JSS-1.3
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/JSS-1.3/ROM_JSS103_MZ700.o src/memory/ROM/JSS-1.3/ROM_JSS103_MZ700.c

${OBJECTDIR}/src/memory/ROM/JSS-1.3/ROM_JSS103_MZ800.o: src/memory/ROM/JSS-1.3/ROM_JSS103_MZ800.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/JSS-1.3
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/JSS-1.3/ROM_JSS103_MZ800.o src/memory/ROM/JSS-1.3/ROM_JSS103_MZ800.c

${OBJECTDIR}/src/memory/ROM/JSS-1.5C/ROM_JSS105C_CGROM.o: src/memory/ROM/JSS-1.5C/ROM_JSS105C_CGROM.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/JSS-1.5C
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/JSS-1.5C/ROM_JSS105C_CGROM.o src/memory/ROM/JSS-1.5C/ROM_JSS105C_CGROM.c

${OBJECTDIR}/src/memory/ROM/JSS-1.5C/ROM_JSS105C_MZ700.o: src/memory/ROM/JSS-1.5C/ROM_JSS105C_MZ700.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/JSS-1.5C
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/JSS-1.5C/ROM_JSS105C_MZ700.o src/memory/ROM/JSS-1.5C/ROM_JSS105C_MZ700.c

${OBJECTDIR}/src/memory/ROM/JSS-1.5C/ROM_JSS105C_MZ800.o: src/memory/ROM/JSS-1.5C/ROM_JSS105C_MZ800.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/JSS-1.5C
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/JSS-1.5C/ROM_JSS105C_MZ800.o src/memory/ROM/JSS-1.5C/ROM_JSS105C_MZ800.c

${OBJECTDIR}/src/memory/ROM/JSS-1.6A/ROM_JSS106A_CGROM.o: src/memory/ROM/JSS-1.6A/ROM_JSS106A_CGROM.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/JSS-1.6A
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/JSS-1.6A/ROM_JSS106A_CGROM.o src/memory/ROM/JSS-1.6A/ROM_JSS106A_CGROM.c

${OBJECTDIR}/src/memory/ROM/JSS-1.6A/ROM_JSS106A_MZ700.o: src/memory/ROM/JSS-1.6A/ROM_JSS106A_MZ700.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/JSS-1.6A
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/JSS-1.6A/ROM_JSS106A_MZ700.o src/memory/ROM/JSS-1.6A/ROM_JSS106A_MZ700.c

${OBJECTDIR}/src/memory/ROM/JSS-1.6A/ROM_JSS106A_MZ800.o: src/memory/ROM/JSS-1.6A/ROM_JSS106A_MZ800.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/JSS-1.6A
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/JSS-1.6A/ROM_JSS106A_MZ800.o src/memory/ROM/JSS-1.6A/ROM_JSS106A_MZ800.c

${OBJECTDIR}/src/memory/ROM/JSS-1.8C/ROM_JSS108C_CGROM.o: src/memory/ROM/JSS-1.8C/ROM_JSS108C_CGROM.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/JSS-1.8C
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/JSS-1.8C/ROM_JSS108C_CGROM.o src/memory/ROM/JSS-1.8C/ROM_JSS108C_CGROM.c

${OBJECTDIR}/src/memory/ROM/JSS-1.8C/ROM_JSS108C_MZ700.o: src/memory/ROM/JSS-1.8C/ROM_JSS108C_MZ700.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/JSS-1.8C
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/JSS-1.8C/ROM_JSS108C_MZ700.o src/memory/ROM/JSS-1.8C/ROM_JSS108C_MZ700.c

${OBJECTDIR}/src/memory/ROM/JSS-1.8C/ROM_JSS108C_MZ800.o: src/memory/ROM/JSS-1.8C/ROM_JSS108C_MZ800.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/JSS-1.8C
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/JSS-1.8C/ROM_JSS108C_MZ800.o src/memory/ROM/JSS-1.8C/ROM_JSS108C_MZ800.c

${OBJECTDIR}/src/memory/ROM/ROM_CGROM.o: src/memory/ROM/ROM_CGROM.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/ROM_CGROM.o src/memory/ROM/ROM_CGROM.c

${OBJECTDIR}/src/memory/ROM/ROM_MZ700.o: src/memory/ROM/ROM_MZ700.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/ROM_MZ700.o src/memory/ROM/ROM_MZ700.c

${OBJECTDIR}/src/memory/ROM/ROM_MZ800.o: src/memory/ROM/ROM_MZ800.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/ROM_MZ800.o src/memory/ROM/ROM_MZ800.c

${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_MZ700.o: src/memory/ROM/WILLY/ROM_WILLY_MZ700.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/WILLY
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_MZ700.o src/memory/ROM/WILLY/ROM_WILLY_MZ700.c

${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_en_CGROM.o: src/memory/ROM/WILLY/ROM_WILLY_en_CGROM.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/WILLY
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_en_CGROM.o src/memory/ROM/WILLY/ROM_WILLY_en_CGROM.c

${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_en_MZ800.o: src/memory/ROM/WILLY/ROM_WILLY_en_MZ800.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/WILLY
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_en_MZ800.o src/memory/ROM/WILLY/ROM_WILLY_en_MZ800.c

${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_ge_CGROM.o: src/memory/ROM/WILLY/ROM_WILLY_ge_CGROM.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/WILLY
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_ge_CGROM.o src/memory/ROM/WILLY/ROM_WILLY_ge_CGROM.c

${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_ge_MZ800.o: src/memory/ROM/WILLY/ROM_WILLY_ge_MZ800.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/WILLY
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_ge_MZ800.o src/memory/ROM/WILLY/ROM_WILLY_ge_MZ800.c

${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_jap_CGROM.o: src/memory/ROM/WILLY/ROM_WILLY_jap_CGROM.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/WILLY
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_jap_CGROM.o src/memory/ROM/WILLY/ROM_WILLY_jap_CGROM.c

${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_jap_MZ800.o: src/memory/ROM/WILLY/ROM_WILLY_jap_MZ800.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory/ROM/WILLY
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/ROM/WILLY/ROM_WILLY_jap_MZ800.o src/memory/ROM/WILLY/ROM_WILLY_jap_MZ800.c

${OBJECTDIR}/src/memory/memory.o: src/memory/memory.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/memory.o src/memory/memory.c

${OBJECTDIR}/src/memory/rom.o: src/memory/rom.c 
	${MKDIR} -p ${OBJECTDIR}/src/memory
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/memory/rom.o src/memory/rom.c

${OBJECTDIR}/src/mz800.o: src/mz800.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/mz800.o src/mz800.c

${OBJECTDIR}/src/pio8255/pio8255.o: src/pio8255/pio8255.c 
	${MKDIR} -p ${OBJECTDIR}/src/pio8255
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/pio8255/pio8255.o src/pio8255/pio8255.c

${OBJECTDIR}/src/pioz80/pioz80.o: src/pioz80/pioz80.c 
	${MKDIR} -p ${OBJECTDIR}/src/pioz80
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/pioz80/pioz80.o src/pioz80/pioz80.c

${OBJECTDIR}/src/port.o: src/port.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/port.o src/port.c

${OBJECTDIR}/src/psg/psg.o: src/psg/psg.c 
	${MKDIR} -p ${OBJECTDIR}/src/psg
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/psg/psg.o src/psg/psg.c

${OBJECTDIR}/src/qdisk/qdisk.o: src/qdisk/qdisk.c 
	${MKDIR} -p ${OBJECTDIR}/src/qdisk
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/qdisk/qdisk.o src/qdisk/qdisk.c

${OBJECTDIR}/src/ramdisk/ramdisk.o: src/ramdisk/ramdisk.c 
	${MKDIR} -p ${OBJECTDIR}/src/ramdisk
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ramdisk/ramdisk.o src/ramdisk/ramdisk.c

${OBJECTDIR}/src/sharpmz_ascii.o: src/sharpmz_ascii.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/sharpmz_ascii.o src/sharpmz_ascii.c

${OBJECTDIR}/src/ui/debugger/ui_breakpoints.o: src/ui/debugger/ui_breakpoints.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui/debugger
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/debugger/ui_breakpoints.o src/ui/debugger/ui_breakpoints.c

${OBJECTDIR}/src/ui/debugger/ui_debugger.o: src/ui/debugger/ui_debugger.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui/debugger
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/debugger/ui_debugger.o src/ui/debugger/ui_debugger.c

${OBJECTDIR}/src/ui/debugger/ui_debugger_callbacks.o: src/ui/debugger/ui_debugger_callbacks.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui/debugger
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/debugger/ui_debugger_callbacks.o src/ui/debugger/ui_debugger_callbacks.c

${OBJECTDIR}/src/ui/debugger/ui_debugger_iasm.o: src/ui/debugger/ui_debugger_iasm.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui/debugger
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/debugger/ui_debugger_iasm.o src/ui/debugger/ui_debugger_iasm.c

${OBJECTDIR}/src/ui/debugger/ui_memdump.o: src/ui/debugger/ui_memdump.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui/debugger
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/debugger/ui_memdump.o src/ui/debugger/ui_memdump.c

${OBJECTDIR}/src/ui/ui_cmt.o: src/ui/ui_cmt.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/ui_cmt.o src/ui/ui_cmt.c

${OBJECTDIR}/src/ui/ui_display.o: src/ui/ui_display.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/ui_display.o src/ui/ui_display.c

${OBJECTDIR}/src/ui/ui_fcbutton.o: src/ui/ui_fcbutton.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/ui_fcbutton.o src/ui/ui_fcbutton.c

${OBJECTDIR}/src/ui/ui_fdc.o: src/ui/ui_fdc.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/ui_fdc.o src/ui/ui_fdc.c

${OBJECTDIR}/src/ui/ui_main.o: src/ui/ui_main.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/ui_main.o src/ui/ui_main.c

${OBJECTDIR}/src/ui/ui_qdisk.o: src/ui/ui_qdisk.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/ui_qdisk.o src/ui/ui_qdisk.c

${OBJECTDIR}/src/ui/ui_ramdisk.o: src/ui/ui_ramdisk.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/ui_ramdisk.o src/ui/ui_ramdisk.c

${OBJECTDIR}/src/ui/ui_rom.o: src/ui/ui_rom.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/ui_rom.o src/ui/ui_rom.c

${OBJECTDIR}/src/ui/ui_utils.o: src/ui/ui_utils.c 
	${MKDIR} -p ${OBJECTDIR}/src/ui
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui/ui_utils.o src/ui/ui_utils.c

${OBJECTDIR}/src/z80ex/z80ex.o: src/z80ex/z80ex.c 
	${MKDIR} -p ${OBJECTDIR}/src/z80ex
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/z80ex/z80ex.o src/z80ex/z80ex.c

${OBJECTDIR}/src/z80ex/z80ex_dasm.o: src/z80ex/z80ex_dasm.c 
	${MKDIR} -p ${OBJECTDIR}/src/z80ex
	${RM} "$@.d"
	$(COMPILE.c) -O2 -Wall -DWINDOWS -DWINDOWS_X64 -D_XOPEN_SOURCE=500 -Dmain=SDL_main -I. -Isrc -Isrc/z80ex -Isrc/z80ex/include -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/z80ex/z80ex_dasm.o src/z80ex/z80ex_dasm.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/mz800emu-x64.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
