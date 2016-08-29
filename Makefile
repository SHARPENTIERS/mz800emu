#
#  There exist several targets which are by default empty and which can be 
#  used for execution of your targets. These targets are usually executed 
#  before and after some main targets. They are: 
#
#     .build-pre:              called before 'build' target
#     .build-post:             called after 'build' target
#     .clean-pre:              called before 'clean' target
#     .clean-post:             called after 'clean' target
#     .clobber-pre:            called before 'clobber' target
#     .clobber-post:           called after 'clobber' target
#     .all-pre:                called before 'all' target
#     .all-post:               called after 'all' target
#     .help-pre:               called before 'help' target
#     .help-post:              called after 'help' target
#
#  Targets beginning with '.' are not intended to be called on their own.
#
#  Main targets can be executed directly, and they are:
#  
#     build                    build a specific configuration
#     clean                    remove built files from a configuration
#     clobber                  remove all built files
#     all                      build all configurations
#     help                     print help mesage
#  
#  Targets .build-impl, .clean-impl, .clobber-impl, .all-impl, and
#  .help-impl are implemented in nbproject/makefile-impl.mk.
#
#  Available make variables:
#
#     CND_BASEDIR                base directory for relative paths
#     CND_DISTDIR                default top distribution directory (build artifacts)
#     CND_BUILDDIR               default top build directory (object files, ...)
#     CONF                       name of current configuration
#     CND_PLATFORM_${CONF}       platform name (current configuration)
#     CND_ARTIFACT_DIR_${CONF}   directory of build artifact (current configuration)
#     CND_ARTIFACT_NAME_${CONF}  name of build artifact (current configuration)
#     CND_ARTIFACT_PATH_${CONF}  path to build artifact (current configuration)
#     CND_PACKAGE_DIR_${CONF}    directory of package (current configuration)
#     CND_PACKAGE_NAME_${CONF}   name of package (current configuration)
#     CND_PACKAGE_PATH_${CONF}   path to package (current configuration)
#
# NOCDDL

MY_HOME_SHAREDIR    := ${HOME}/share/mz800emu/
	
# Environment 
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin

# prozatim odlozene pkg baliky:
# gtk+-2.0
# libxml-2.0
PKG_OBJECTS		:= gtk+-3.0 gmodule-export-2.0

BUILD_HOST_OS		:= ${shell uname -o }


#
# Pokud kompilujeme ve Windows pod Msys
#
ifeq (${BUILD_HOST_OS},Msys)

# definice pro NATIVE WIN32
W32NAT_PKGCONFIG	:= pkg-config
# ve win32 verzi odstranenim -mwindows vynutime vystupni consoli
W32NAT_SDL2_LIBS	:= -lmingw32 -lSDL2main -lSDL2
W32NAT_SDL2_CFLAGS	:= -IC:/MinGW/include/SDL2 -Dmain=SDL_main
WINDRES			:= windres


#
# Pokud kompilujeme v Linuxu
#

else ifeq (${BUILD_HOST_OS},GNU/Linux)

# definice pro CROSS WIN_X86
WIN_X86_MINGW32_PLATFORM	:= i686-w64-mingw32
WIN_X86_MINGW32_TOOLS_PREFIX	:= ${WIN_X86_MINGW32_PLATFORM}-
WIN_X86_MINGW32_PKGCONFIG	:= ${WIN_X86_MINGW32_TOOLS_PREFIX}pkg-config
WIN_X86_MINGW32_SDL2_CONFIG	:= /usr/local/cross-tools/$(WIN_X86_MINGW32_PLATFORM)/bin/sdl2-config
# ve windows verzi odstranenim -mwindows vynutime vystupni consoli
WIN_X86_MINGW32_SDL2_LIBS	:= ${shell ${WIN_X86_MINGW32_SDL2_CONFIG} --libs | /bin/sed -e 's/-mwindows//' } -lSDL2
WIN_X86_WINDRES			:= ${WIN_X86_MINGW32_TOOLS_PREFIX}windres

# definice pro CROSS WIN_X64
WIN_X64_MINGW32_PLATFORM	:= x86_64-w64-mingw32
WIN_X64_MINGW32_TOOLS_PREFIX	:= ${WIN_X64_MINGW32_PLATFORM}-
WIN_X64_MINGW32_PKGCONFIG	:= ${WIN_X64_MINGW32_TOOLS_PREFIX}pkg-config
WIN_X64_MINGW32_SDL2_CONFIG	:= /usr/local/cross-tools/$(WIN_X64_MINGW32_PLATFORM)/bin/sdl2-config
# ve windows verzi odstranenim -mwindows vynutime vystupni consoli
WIN_X64_MINGW32_SDL2_LIBS	:= ${shell ${WIN_X64_MINGW32_SDL2_CONFIG} --libs | /bin/sed -e 's/-mwindows//' } -lSDL2
WIN_X64_WINDRES			:= ${WIN_X64_MINGW32_TOOLS_PREFIX}windres

# definice pro NATIVE LINUX
LINUX_TOOLS_PREFIX	:=
LINUX_PKGCONFIG		:= ${TOOLS_PREFIX}pkg-config 
LINUX_SDL2_CONFIG	:= sdl2-config
LINUX_SDL2_LIBS         := ${shell ${LINUX_SDL2_CONFIG} --libs} -lSDL2

else

$(error  "Unknovn build host OS! '${BUILD_HOST_OS}'")

endif



# build
build: .build-post

.build-pre:
	@# Add your pre 'build' code here...
	@echo -e "\n\n"
	@echo "Making:                   $@"
	@echo "Actual configuration:     ${CONF}"
	@echo "Actual platform:          ${CND_PLATFORM_${CONF}}"
	@echo "Build host OS:            ${BUILD_HOST_OS}"
	@echo -e "\n\n"
	./tools/create_buildtime.sh > src/build_time.c


.build-post: .build-impl
	@# Add your post 'build' code here...
	@#echo ${CND_ARTIFACT_PATH_${CONF}}
		
	@# vzdy nakopirujeme ui_resources do adresare s binarkou
	@echo -e "\n";
	@./tools/copy_ui_resources.sh ${CND_ARTIFACT_DIR_${CONF}}

	@# zobrazime si informaci o buildu
	@echo -e "`cat src/build_time.c | sed -e 's/";/\n/' -e 's/"/\nBuild time: /' | egrep "^Build time: "`\n"
	
	@# Zkopirujeme vysledek do adresare sdileneho s windows - jen na mem desktopu
	@if [ ! -z "`uname -n|egrep 'arrakis.ordoz.com|atreides.ordoz.com'`" ]; then \
	    if [ ! -z "`echo "${CONF}"|egrep 'Release-Win32|Release-Win64'`" ]; then \
		echo -e "Copy ${CND_ARTIFACT_PATH_${CONF}} to ${MY_HOME_SHAREDIR}\n"; \
		$(CP) ${CND_ARTIFACT_PATH_${CONF}} ${MY_HOME_SHAREDIR}; \
	    fi; \
	fi
		
	@# Pokud profilujeme
	@if [ "${CONF}" = "Gprof-Release-Linux" ]; then \
	    echo -e "\n\n********** ${CONF} **********\n"; \
	    echo -e "\nprof1) gprof - http://www.thegeekstuff.com/2012/08/gprof-tutorial/"; \
	    echo -e "prof2) valgrind --tool=callgrind ${CND_ARTIFACT_PATH_${CONF}}\n(visualise in KCachegrind)"; \
	    echo "Now:"; \
	    echo "1. Run '${CND_ARTIFACT_PATH_${CONF}}' to create gmon.out (exit the program)"; \
	    echo "2. Run gprof ${CND_ARTIFACT_PATH_${CONF}} gmon.out > analysis.txt"; \
	    echo -e "3. cat analysis.txt\n\n"; \
	    ${CND_ARTIFACT_PATH_${CONF}}; \
	    gprof ${CND_ARTIFACT_PATH_${CONF}} gmon.out > analysis.txt; \
	    cat analysis.txt; \
	fi

# clean
clean: .clean-post

.clean-pre:
# Add your pre 'clean' code here...

.clean-post: .clean-impl
# Add your post 'clean' code here...


# clobber
clobber: .clobber-post

.clobber-pre:
# Add your pre 'clobber' code here...

.clobber-post: .clobber-impl
# Add your post 'clobber' code here...


# all
all: .all-post

.all-pre:
# Add your pre 'all' code here...

.all-post: .all-impl
# Add your post 'all' code here...


# build tests
build-tests: .build-tests-post

.build-tests-pre:
# Add your pre 'build-tests' code here...

.build-tests-post: .build-tests-impl
# Add your post 'build-tests' code here...


# run tests
test: .test-post

.test-pre: build-tests
# Add your pre 'test' code here...

.test-post: .test-impl
# Add your post 'test' code here...


# help
help: .help-post
	@echo "Targets available only for WIN_X86:"
	@echo -e "\tpackage"
	@echo -e "\n"

.help-pre:
# Add your pre 'help' code here...

.help-post: .help-impl
# Add your post 'help' code here...



# include project implementation makefile
include nbproject/Makefile-impl.mk

# include project make variables
include nbproject/Makefile-variables.mk


#
#
#

# windows icon
src/windows_icon/app.o: src/windows_icon/app.rc src/windows_icon/mz800emu.ico
	${MKDIR} -p ${CND_BUILDDIR}/${CONF}/${CND_PLATFORM_${CONF}}/src/windows_icon
	@#$(WINDRES) src/windows_icon/app.rc -o ${CND_BUILDDIR}/${CONF}/${CND_PLATFORM_${CONF}}/$@
	
	@if [ "${CONF}" = "Release-Win32" ]; then \
	    $(WIN_X86_WINDRES) src/windows_icon/app.rc -o ${CND_BUILDDIR}/${CONF}/${CND_PLATFORM_${CONF}}/$@; \
	elif [ "${CONF}" = "Release-Win64" ]; then \
	    $(WIN_X64_WINDRES) src/windows_icon/app.rc -o ${CND_BUILDDIR}/${CONF}/${CND_PLATFORM_${CONF}}/$@; \
	else \
	    $(WINDRES) src/windows_icon/app.rc -o ${CND_BUILDDIR}/${CONF}/${CND_PLATFORM_${CONF}}/$@; \
	fi


package: build
	./tools/create_package-Release-Win32.sh "${CONF}"
	
	                
