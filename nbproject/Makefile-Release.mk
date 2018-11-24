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
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/AppDakiines.o \
	${OBJECTDIR}/src/CtxDakiines.o \
	${OBJECTDIR}/src/blit_c.o \
	${OBJECTDIR}/src/cal.o \
	${OBJECTDIR}/src/cd_unix.o \
	${OBJECTDIR}/src/coleco.o \
	${OBJECTDIR}/src/cpu6280.o \
	${OBJECTDIR}/src/cpu6280opc.o \
	${OBJECTDIR}/src/cpu6502.o \
	${OBJECTDIR}/src/cpu6502opc.o \
	${OBJECTDIR}/src/dt68000.o \
	${OBJECTDIR}/src/emu68k.o \
	${OBJECTDIR}/src/event.o \
	${OBJECTDIR}/src/gen_vdp.o \
	${OBJECTDIR}/src/genesis.o \
	${OBJECTDIR}/src/keyboard_dakii.o \
	${OBJECTDIR}/src/mappers.o \
	${OBJECTDIR}/src/msx.o \
	${OBJECTDIR}/src/mz80.o \
	${OBJECTDIR}/src/nes.o \
	${OBJECTDIR}/src/nes_ppu.o \
	${OBJECTDIR}/src/nes_psg.o \
	${OBJECTDIR}/src/palette16.o \
	${OBJECTDIR}/src/palette32.o \
	${OBJECTDIR}/src/palette8.o \
	${OBJECTDIR}/src/pce.o \
	${OBJECTDIR}/src/pce_cd.o \
	${OBJECTDIR}/src/pce_vdp.o \
	${OBJECTDIR}/src/sg1000.o \
	${OBJECTDIR}/src/sms.o \
	${OBJECTDIR}/src/sms9918.o \
	${OBJECTDIR}/src/sms_psg.o \
	${OBJECTDIR}/src/snd_dakii.o \
	${OBJECTDIR}/src/snd_unix.o \
	${OBJECTDIR}/src/test.o \
	${OBJECTDIR}/src/tiledraw.o \
	${OBJECTDIR}/src/tms9918.o \
	${OBJECTDIR}/src/tool.o \
	${OBJECTDIR}/src/torr_cpu.o \
	${OBJECTDIR}/src/ui_dakii.o \
	${OBJECTDIR}/src/video_dakii.o


# C Compiler Flags
CFLAGS=

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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dakiines

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dakiines: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dakiines ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/AppDakiines.o: src/AppDakiines.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AppDakiines.o src/AppDakiines.cpp

${OBJECTDIR}/src/CtxDakiines.o: src/CtxDakiines.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CtxDakiines.o src/CtxDakiines.cpp

${OBJECTDIR}/src/blit_c.o: src/blit_c.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/blit_c.o src/blit_c.cpp

${OBJECTDIR}/src/cal.o: src/cal.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cal.o src/cal.cpp

${OBJECTDIR}/src/cd_unix.o: src/cd_unix.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cd_unix.o src/cd_unix.cpp

${OBJECTDIR}/src/coleco.o: src/coleco.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/coleco.o src/coleco.cpp

${OBJECTDIR}/src/cpu6280.o: src/cpu6280.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cpu6280.o src/cpu6280.cpp

${OBJECTDIR}/src/cpu6280opc.o: src/cpu6280opc.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cpu6280opc.o src/cpu6280opc.cpp

${OBJECTDIR}/src/cpu6502.o: src/cpu6502.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cpu6502.o src/cpu6502.cpp

${OBJECTDIR}/src/cpu6502opc.o: src/cpu6502opc.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/cpu6502opc.o src/cpu6502opc.cpp

${OBJECTDIR}/src/dt68000.o: src/dt68000.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/dt68000.o src/dt68000.cpp

${OBJECTDIR}/src/emu68k.o: src/emu68k.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/emu68k.o src/emu68k.cpp

${OBJECTDIR}/src/event.o: src/event.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/event.o src/event.cpp

${OBJECTDIR}/src/gen_vdp.o: src/gen_vdp.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/gen_vdp.o src/gen_vdp.cpp

${OBJECTDIR}/src/genesis.o: src/genesis.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/genesis.o src/genesis.cpp

${OBJECTDIR}/src/keyboard_dakii.o: src/keyboard_dakii.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/keyboard_dakii.o src/keyboard_dakii.cpp

${OBJECTDIR}/src/mappers.o: src/mappers.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/mappers.o src/mappers.cpp

${OBJECTDIR}/src/msx.o: src/msx.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/msx.o src/msx.cpp

${OBJECTDIR}/src/mz80.o: src/mz80.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/mz80.o src/mz80.cpp

${OBJECTDIR}/src/nes.o: src/nes.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/nes.o src/nes.cpp

${OBJECTDIR}/src/nes_ppu.o: src/nes_ppu.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/nes_ppu.o src/nes_ppu.cpp

${OBJECTDIR}/src/nes_psg.o: src/nes_psg.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/nes_psg.o src/nes_psg.cpp

${OBJECTDIR}/src/palette16.o: src/palette16.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/palette16.o src/palette16.cpp

${OBJECTDIR}/src/palette32.o: src/palette32.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/palette32.o src/palette32.cpp

${OBJECTDIR}/src/palette8.o: src/palette8.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/palette8.o src/palette8.cpp

${OBJECTDIR}/src/pce.o: src/pce.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/pce.o src/pce.cpp

${OBJECTDIR}/src/pce_cd.o: src/pce_cd.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/pce_cd.o src/pce_cd.cpp

${OBJECTDIR}/src/pce_vdp.o: src/pce_vdp.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/pce_vdp.o src/pce_vdp.cpp

${OBJECTDIR}/src/sg1000.o: src/sg1000.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/sg1000.o src/sg1000.cpp

${OBJECTDIR}/src/sms.o: src/sms.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/sms.o src/sms.cpp

${OBJECTDIR}/src/sms9918.o: src/sms9918.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/sms9918.o src/sms9918.cpp

${OBJECTDIR}/src/sms_psg.o: src/sms_psg.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/sms_psg.o src/sms_psg.cpp

${OBJECTDIR}/src/snd_dakii.o: src/snd_dakii.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/snd_dakii.o src/snd_dakii.cpp

${OBJECTDIR}/src/snd_unix.o: src/snd_unix.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/snd_unix.o src/snd_unix.cpp

${OBJECTDIR}/src/test.o: src/test.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/test.o src/test.cpp

${OBJECTDIR}/src/tiledraw.o: src/tiledraw.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/tiledraw.o src/tiledraw.cpp

${OBJECTDIR}/src/tms9918.o: src/tms9918.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/tms9918.o src/tms9918.cpp

${OBJECTDIR}/src/tool.o: src/tool.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/tool.o src/tool.cpp

${OBJECTDIR}/src/torr_cpu.o: src/torr_cpu.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/torr_cpu.o src/torr_cpu.cpp

${OBJECTDIR}/src/ui_dakii.o: src/ui_dakii.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ui_dakii.o src/ui_dakii.cpp

${OBJECTDIR}/src/video_dakii.o: src/video_dakii.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/video_dakii.o src/video_dakii.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
