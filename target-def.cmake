# specify the cross compiler
set(CMAKE_C_COMPILER ${CROSS_COMPILE_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${CROSS_COMPILE_PREFIX}-g++)
set(CMAKE_ASM_COMPILER ${CROSS_COMPILE_PREFIX}-gcc)
set(CMAKE_OBJCOPY ${CROSS_COMPILE_PREFIX}-objcopy)
set(CMAKE_OBJDUMP ${CROSS_COMPILE_PREFIX}-objdump)
# find additional toolchain executables
find_program(ARM_SIZE_EXECUTABLE ${CROSS_COMPILE_PREFIX}-size)
find_program(ARM_GDB_EXECUTABLE ${CROSS_COMPILE_PREFIX}-gdb)
find_program(ARM_OBJCOPY_EXECUTABLE ${CROSS_COMPILE_PREFIX}-objcopy)
find_program(ARM_OBJDUMP_EXECUTABLE ${CROSS_COMPILE_PREFIX}-objdump)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# search for program/library/include in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Target-specific flags
#型号
set(MCU_FAMILY STM32F427xx)
#布局文件
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F427IIHx_FLASH.ld)
#内核相关
set(CPU "-mcpu=cortex-m4")
set(FPU "-mfpu=fpv4-sp-d16")
set(FLOAT_ABI "-mfloat-abi=hard")

add_definitions(-D${MCU_FAMILY})

# generate flags from user variables
if(CMAKE_BUILD_TYPE MATCHES Debug)
set(DBG_FLAGS "-g3 -gdwarf-2 -O0")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
set(DBG_FLAGS "-O3")
endif()

set(MCU_FLAGS "${CPU} -mthumb ${FPU} ${FLOAT_ABI}") 

# compiler: language specific flags CFLAGS
set(CMAKE_C_FLAGS "${MCU_FLAGS} -std=gnu99 -Wall -fdata-sections -ffunction-sections ${DBG_FLAGS} -Wno-long-long" CACHE INTERNAL "C compiler flags")
#CPP 
set(CMAKE_CXX_FLAGS "${MCU_FLAGS} -fno-rtti -fno-exceptions -fno-builtin -Wall -fdata-sections -ffunction-sections ${DBG_FLAGS} -Wno-long-long" CACHE INTERNAL "Cxx compiler flags")
#ASFLAGS
set(CMAKE_ASM_FLAGS "${MCU_FLAGS} -x assembler-with-cpp ${DBG_FLAGS} " CACHE INTERNAL "ASM compiler flags")
#LDFLAGS -mcpu=cortex-m0plus -mthumb
set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} -specs=nano.specs -T${LINKER_SCRIPT} -Wl,-Map=${PROJECT_NAME}.map,--cref -Wl,--gc-sections" CACHE INTERNAL "Exe linker flags")
#要链接的库 对应makefile的 LIBS
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-lc -lm -lnosys" CACHE INTERNAL "Shared linker flags")