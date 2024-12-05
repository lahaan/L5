# Defines the part type that this project uses.
PART=TM4C123GH6PM

# The base directory for TivaWare.
ROOT=../../..

COMPILER=gcc

PREFIX=arm-none-eabi

# The command for calling the compiler.
CC=${PREFIX}-gcc

# The location of the C compiler
# ARMGCC_ROOT is used by some makefiles that need to know where the compiler
# is installed.
ARMGCC_ROOT:=${shell dirname '${shell sh -c "which ${CC}"}'}/..

# Set the compiler CPU/FPU options.
CPU=-mcpu=cortex-m4
FPU=-mfpu=fpv4-sp-d16 -mfloat-abi=softfp

# The flags passed to the compiler.
CFLAGS=-mthumb             \
       ${CPU}              \
       ${FPU}              \
       -ffunction-sections \
       -fdata-sections     \
       -std=c99            \
       -Wall               \
       -pedantic           \
       -DPART_${PART}      \
       -c

# The command for calling the library archiver.
AR=${PREFIX}-ar

# The command for calling the linker.
LD=${PREFIX}-ld

# The flags passed to the linker.
LDFLAGS=--gc-sections

# Get the location of libgcc.a from the GCC front-end.
LIBGCC:=${shell ${CC} ${CFLAGS} -print-libgcc-file-name}

# Get the location of libc.a from the GCC front-end.
LIBC:=${shell ${CC} ${CFLAGS} -print-file-name=libc.a}

# Get the location of libm.a from the GCC front-end.
LIBM:=${shell ${CC} ${CFLAGS} -print-file-name=libm.a}

# The command for extracting images from the linked executables.
OBJCOPY=${PREFIX}-objcopy

CFLAGS+=-D DEBUG

# Add the tool specific CFLAGS.
CFLAGS+=-DTARGET_IS_BLIZZARD_RB1

# Where to find source files that do not live in this directory.
VPATH=../../../utils

# Where to find header files that do not live in the source directory.
IPATH=../../..

# Add the include file paths to CFLAGS.
CFLAGS+=${patsubst %,-I%,${subst :, ,${IPATH}}}

# The default rule, which causes the main to be built.
all: ${COMPILER}
all: ${COMPILER}/main.axf

GRADER=verify
UART=uartstdio
STARTUP=startup

# The rule to clean out all the build products, keep grader files.
clean:
	@find ${COMPILER} -maxdepth 1 ! -name '${GRADER}.o' ! -name '${STARTUP}.o' ! -name '${UART}.o' -type f -delete

# The rule to create the target directory (will do nothing if already exists).
${COMPILER}:
	@mkdir -p ${COMPILER}

# Linker script is common for all projects.
SCATTERgcc=../config.ld

# From startup.c
ENTRY=ResetISR

# Rules for building the main.
${COMPILER}/main.axf: ${COMPILER}/main.o
${COMPILER}/main.axf: ${COMPILER}/analog_temp.o
${COMPILER}/main.axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
${COMPILER}/main.axf: ${SCATTERgcc}

# The rule for building the OBJECT file from each C source file.
${COMPILER}${SUFFIX}/%.o: %.c
	${CC} ${CFLAGS} -g -O0 -D${COMPILER} -o ${@} ${<}

# The rule for creating an OBJECT LIBRARY.
${COMPILER}${SUFFIX}/%.a:
	${AR} -cr ${@} ${^}

# The rule for linking the application (ALSO LINKS WITH GRADER).
${COMPILER}${SUFFIX}/%.axf:
	${LD} -T ${SCATTERgcc}                                                   \
	      --entry ${ENTRY}                                                   \
	      ${LDFLAGS} -o ${@} ${COMPILER}/${GRADER}.o ${COMPILER}/${UART}.o   \
		  ${COMPILER}/${STARTUP}.o $(filter %.o %.a, ${^}) 				     \
	      '${LIBM}' '${LIBC}' '${LIBGCC}'
	@${OBJCOPY} -O binary ${@} ${@:.axf=.bin}
