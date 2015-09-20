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

# definice pro CROSS WIN32
MINGW32_PLATFORM	:= i686-w64-mingw32
MINGW32_TOOLS_PREFIX	:= ${MINGW32_PLATFORM}-
MINGW32_PKGCONFIG	:= ${MINGW32_TOOLS_PREFIX}pkg-config
MINGW32_SDL2_CONFIG	:= /usr/local/cross-tools/$(MINGW32_PLATFORM)/bin/sdl2-config
# ve win32 verzi odstranenim -mwindows vynutime vystupni consoli
MINGW32_SDL2_LIBS	:= ${shell ${MINGW32_SDL2_CONFIG} --libs | /bin/sed -e 's/-mwindows//' } -lSDL2
WINDRES			:= ${MINGW32_TOOLS_PREFIX}windres

# definice pro NATIVE LINUX
LINUX_TOOLS_PREFIX	:=
LINUX_PKGCONFIG		:= ${TOOLS_PREFIX}pkg-config 
LINUX_SDL2_CONFIG	:= /usr/local/bin/sdl2-config
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
	
	${shell echo -e "\n/* Do NOT edit this file! His content is created automatically. */\n\nchar* build_time_get ( void ) { return \"`/bin/date '+%Y-%m-%d %H:%M:%S'`\"; }\n" > src/build_time.c }


.build-post: .build-impl
	@# Add your post 'build' code here...
	@#echo ${CND_ARTIFACT_PATH_${CONF}}
	@# Zkopirujeme vysledek do adresare sdileneho s windows
	@${shell if [ "`/usr/bin/uname -n`" == "arrakis.ordoz.com" ]; then /bin/cp dist/Release-Win32/i686-w64-mingw32-Linux-x86/mz800emu.exe ~/share/mz800emu/; fi }


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
	$(WINDRES) src/windows_icon/app.rc -o ${CND_BUILDDIR}/${CONF}/${CND_PLATFORM_${CONF}}/$@

                