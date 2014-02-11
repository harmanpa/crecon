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
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/crecon_transform.o \
	${OBJECTDIR}/crecon_util.o \
	${OBJECTDIR}/crecon_wall.o \
	${OBJECTDIR}/crecon_wall_meta.o \
	${OBJECTDIR}/crecon_wall_object.o \
	${OBJECTDIR}/crecon_wall_table.o \
	${OBJECTDIR}/crecon_wall_table_row.o \
	${OBJECTDIR}/objectc.o \
	${OBJECTDIR}/unpack.o \
	${OBJECTDIR}/version.o \
	${OBJECTDIR}/vrefbuffer.o \
	${OBJECTDIR}/zone.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f1

# C Compiler Flags
CFLAGS=-m64

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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcrecon.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcrecon.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcrecon.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcrecon.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcrecon.a

${OBJECTDIR}/crecon_transform.o: crecon_transform.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/crecon_transform.o crecon_transform.c

${OBJECTDIR}/crecon_util.o: crecon_util.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/crecon_util.o crecon_util.c

${OBJECTDIR}/crecon_wall.o: crecon_wall.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/crecon_wall.o crecon_wall.c

${OBJECTDIR}/crecon_wall_meta.o: crecon_wall_meta.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/crecon_wall_meta.o crecon_wall_meta.c

${OBJECTDIR}/crecon_wall_object.o: crecon_wall_object.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/crecon_wall_object.o crecon_wall_object.c

${OBJECTDIR}/crecon_wall_table.o: crecon_wall_table.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/crecon_wall_table.o crecon_wall_table.c

${OBJECTDIR}/crecon_wall_table_row.o: crecon_wall_table_row.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/crecon_wall_table_row.o crecon_wall_table_row.c

${OBJECTDIR}/objectc.o: objectc.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/objectc.o objectc.c

${OBJECTDIR}/unpack.o: unpack.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/unpack.o unpack.c

${OBJECTDIR}/version.o: version.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/version.o version.c

${OBJECTDIR}/vrefbuffer.o: vrefbuffer.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/vrefbuffer.o vrefbuffer.c

${OBJECTDIR}/zone.o: zone.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/zone.o zone.c

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-conf ${TESTFILES}
${TESTDIR}/TestFiles/f1: ${TESTDIR}/tests/wallwritetest.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.c}   -o ${TESTDIR}/TestFiles/f1 $^ ${LDLIBSOPTIONS} 


${TESTDIR}/tests/wallwritetest.o: tests/wallwritetest.c 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} $@.d
	$(COMPILE.c) -g -I. -MMD -MP -MF $@.d -o ${TESTDIR}/tests/wallwritetest.o tests/wallwritetest.c


${OBJECTDIR}/crecon_transform_nomain.o: ${OBJECTDIR}/crecon_transform.o crecon_transform.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/crecon_transform.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -g -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/crecon_transform_nomain.o crecon_transform.c;\
	else  \
	    ${CP} ${OBJECTDIR}/crecon_transform.o ${OBJECTDIR}/crecon_transform_nomain.o;\
	fi

${OBJECTDIR}/crecon_util_nomain.o: ${OBJECTDIR}/crecon_util.o crecon_util.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/crecon_util.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -g -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/crecon_util_nomain.o crecon_util.c;\
	else  \
	    ${CP} ${OBJECTDIR}/crecon_util.o ${OBJECTDIR}/crecon_util_nomain.o;\
	fi

${OBJECTDIR}/crecon_wall_nomain.o: ${OBJECTDIR}/crecon_wall.o crecon_wall.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/crecon_wall.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -g -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/crecon_wall_nomain.o crecon_wall.c;\
	else  \
	    ${CP} ${OBJECTDIR}/crecon_wall.o ${OBJECTDIR}/crecon_wall_nomain.o;\
	fi

${OBJECTDIR}/crecon_wall_meta_nomain.o: ${OBJECTDIR}/crecon_wall_meta.o crecon_wall_meta.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/crecon_wall_meta.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -g -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/crecon_wall_meta_nomain.o crecon_wall_meta.c;\
	else  \
	    ${CP} ${OBJECTDIR}/crecon_wall_meta.o ${OBJECTDIR}/crecon_wall_meta_nomain.o;\
	fi

${OBJECTDIR}/crecon_wall_object_nomain.o: ${OBJECTDIR}/crecon_wall_object.o crecon_wall_object.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/crecon_wall_object.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -g -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/crecon_wall_object_nomain.o crecon_wall_object.c;\
	else  \
	    ${CP} ${OBJECTDIR}/crecon_wall_object.o ${OBJECTDIR}/crecon_wall_object_nomain.o;\
	fi

${OBJECTDIR}/crecon_wall_table_nomain.o: ${OBJECTDIR}/crecon_wall_table.o crecon_wall_table.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/crecon_wall_table.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -g -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/crecon_wall_table_nomain.o crecon_wall_table.c;\
	else  \
	    ${CP} ${OBJECTDIR}/crecon_wall_table.o ${OBJECTDIR}/crecon_wall_table_nomain.o;\
	fi

${OBJECTDIR}/crecon_wall_table_row_nomain.o: ${OBJECTDIR}/crecon_wall_table_row.o crecon_wall_table_row.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/crecon_wall_table_row.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -g -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/crecon_wall_table_row_nomain.o crecon_wall_table_row.c;\
	else  \
	    ${CP} ${OBJECTDIR}/crecon_wall_table_row.o ${OBJECTDIR}/crecon_wall_table_row_nomain.o;\
	fi

${OBJECTDIR}/objectc_nomain.o: ${OBJECTDIR}/objectc.o objectc.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/objectc.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -g -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/objectc_nomain.o objectc.c;\
	else  \
	    ${CP} ${OBJECTDIR}/objectc.o ${OBJECTDIR}/objectc_nomain.o;\
	fi

${OBJECTDIR}/unpack_nomain.o: ${OBJECTDIR}/unpack.o unpack.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/unpack.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -g -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/unpack_nomain.o unpack.c;\
	else  \
	    ${CP} ${OBJECTDIR}/unpack.o ${OBJECTDIR}/unpack_nomain.o;\
	fi

${OBJECTDIR}/version_nomain.o: ${OBJECTDIR}/version.o version.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/version.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -g -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/version_nomain.o version.c;\
	else  \
	    ${CP} ${OBJECTDIR}/version.o ${OBJECTDIR}/version_nomain.o;\
	fi

${OBJECTDIR}/vrefbuffer_nomain.o: ${OBJECTDIR}/vrefbuffer.o vrefbuffer.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/vrefbuffer.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -g -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/vrefbuffer_nomain.o vrefbuffer.c;\
	else  \
	    ${CP} ${OBJECTDIR}/vrefbuffer.o ${OBJECTDIR}/vrefbuffer_nomain.o;\
	fi

${OBJECTDIR}/zone_nomain.o: ${OBJECTDIR}/zone.o zone.c 
	${MKDIR} -p ${OBJECTDIR}
	@NMOUTPUT=`${NM} ${OBJECTDIR}/zone.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -g -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/zone_nomain.o zone.c;\
	else  \
	    ${CP} ${OBJECTDIR}/zone.o ${OBJECTDIR}/zone_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f1 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libcrecon.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
