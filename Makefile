#
#       !!!! Do NOT edit this makefile with an editor which replace tabs by spaces !!!!
#
##############################################################################################
#
# On command line:
#
# make all = Create project
#
# make clean = Clean project files.
#
# To rebuild project do "make clean" and "make all".
#
# Included originally in the yagarto projects. Original Author : Michael Fischer
# Modified to suit our purposes by Hussam Al-Hertani
# Use at your own risk!!!!!
##############################################################################################
# Start of default section
#
TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary -S
MCU  = cortex-m0

# List all default C defines here, like -D_DEBUG=1
# for STM32F103RCT6 micro
# DDEFS = -DSTM32F10X_HD
# for STM32F051C8T6 micro
# DDEFS = -DSTM32F051
# for STM32F030K6T6 micro or STM32F030R8T6
DDEFS = -DSTM32F030

# List all default ASM defines here, like -D_DEBUG=1
DADEFS =

# List all default directories to look for include files here
DINCDIR = ./src

# List the default directory to look for the libraries here
DLIBDIR =

# List all default libraries here
DLIBS =

#
# End of default section
##############################################################################################

##############################################################################################
# Start of user section
#

#
# Define project name here
PROJECT = Template_F030

# List C source files here
CORELIBDIR = ./cmsis_core
BOOTDIR = ./cmsis_boot

LINKER = ./cmsis_boot/startup

# Sources Files
SRC  = ./src/main.c
SRC += $(BOOTDIR)/system_stm32f0xx.c
SRC += $(BOOTDIR)/syscalls/syscalls.c

SRC += ./src/it.c
SRC += ./src/gpio.c
SRC += ./src/tim.c
SRC += ./src/dma.c
SRC += ./src/adc.c
SRC += ./src/uart.c
SRC += ./src/dsp.c
SRC += ./src/hard.c
SRC += ./src/utils.c
SRC += ./src/i2c.c
SRC += ./src/flash_program.c

SRC += ./src/dmx_transceiver.c
SRC += ./src/pwm.c
SRC += ./src/filters_and_offsets.c

SRC += ./src/ssd1306_display.c
SRC += ./src/ssd1306_gfx.c
SRC += ./src/display_utils.c
SRC += ./src/screen.c

SRC += ./src/test_functions.c

SRC += ./src/main_menu.c
SRC += ./src/options_menu.c

SRC += ./src/dmx1_mode.c
SRC += ./src/dmx2_mode.c
SRC += ./src/dmx_menu.c

SRC += ./src/manual_mode.c
SRC += ./src/manual_menu.c
SRC += ./src/fixed_menu.c
SRC += ./src/colors_menu.c
SRC += ./src/colors_functions.c

SRC += ./src/master_slave_mode.c
SRC += ./src/master_slave_menu.c
SRC += ./src/master_menu.c
SRC += ./src/slave_menu.c

SRC += ./src/reset_mode.c

SRC += ./src/hardware_mode.c
SRC += ./src/current_menu.c
SRC += ./src/limits_menu.c
SRC += ./src/channels_menu.c
SRC += ./src/temp_menu.c
SRC += ./src/version_menu.c

SRC += ./src/comm.c


## Core Support
SRC += $(CORELIBDIR)/core_cm0.c


## Other Peripherals libraries

# List ASM source files here
ASRC = ./cmsis_boot/startup/startup_stm32f0xx.s

# List all user directories here
UINCDIR = $(BOOTDIR) \
          $(CORELIBDIR)
			 #../paho.mqtt.embedded-c/MQTTPacket/src

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

# Define optimisation level here
# O1 optimiza size no significativo
# O2 size mas fuerte
# Os (size mas fuerte que O2)
# O3 el mas fuerte, seguro despues no corre
# O0 (no optimiza casi nada, mejor para debug)
OPT = -O0

#
# End of user defines
##############################################################################################
#
# Define linker script file here
#
LDSCRIPT = $(LINKER)/stm32_flash.ld
FULL_PRJ = $(PROJECT)_rom

INCDIR  = $(patsubst %,-I%,$(DINCDIR) $(UINCDIR))
LIBDIR  = $(patsubst %,-L%,$(DLIBDIR) $(ULIBDIR))

ADEFS   = $(DADEFS) $(UADEFS)

LIBS    = $(DLIBS) $(ULIBS)
MCFLAGS = -mcpu=$(MCU)

ASFLAGS = $(MCFLAGS) -g -gdwarf-2 -mthumb  -Wa,-amhls=$(<:.s=.lst) $(ADEFS)

# SIN INFO DEL DEBUGGER + STRIP CODE
# CPFLAGS = $(MCFLAGS) $(OPT) -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -fdata-sections -ffunction-sections -fverbose-asm -Wa,-ahlms=$(<:.c=.lst)

# CON INFO PARA DEBUGGER
#CPFLAGS = $(MCFLAGS) $(OPT) -g -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)

# CON INFO PARA DEBUGGER + STRIP CODE
CPFLAGS = $(MCFLAGS) $(OPT) -g -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -fdata-sections -ffunction-sections -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DDEFS)

# SIN DEAD CODE, hace el STRIP
LDFLAGS = $(MCFLAGS) -mthumb -lm --specs=nano.specs -Wl,--gc-sections -nostartfiles -T$(LDSCRIPT) -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch $(LIBDIR)
# CON DEAD CODE
#LDFLAGS = $(MCFLAGS) -mthumb --specs=nano.specs -nostartfiles -T$(LDSCRIPT) -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch $(LIBDIR)
#LDFLAGS = $(MCFLAGS) -mthumb -T$(LDSCRIPT) -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch $(LIBDIR)

#
# makefile rules
#

assemblersources = $(ASRC)
sources = $(SRC)
OBJS  = $(SRC:.c=.o) $(ASRC:.s=.o)

objects = $(sources:.c=.o)
assobjects = $(assemblersources:.s=.o)

all: $(objects) $(assobjects) $(FULL_PRJ).elf $(FULL_PRJ).bin
	arm-none-eabi-size $(FULL_PRJ).elf
	gtags -q

$(objects): %.o: %.c
	$(CC) -c $(CPFLAGS) -I. $(INCDIR) $< -o $@

$(assobjects): %.o: %.s
	$(AS) -c $(ASFLAGS) $< -o $@

%elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

%hex: %elf
	$(HEX) $< $@

%bin: %elf
	$(BIN)  $< $@

flash:
	sudo openocd -f stm32f0_flash.cfg

flash_lock:
	sudo openocd -f stm32f0_flash_lock.cfg

gdb:
	sudo openocd -f stm32f0_gdb.cfg

reset:
	sudo openocd -f stm32f0_reset.cfg

clean:
	rm -f $(OBJS)
	rm -f $(FULL_PRJ).elf
	rm -f $(FULL_PRJ).map
	rm -f $(FULL_PRJ).hex
	rm -f $(FULL_PRJ).bin
	rm -f $(SRC:.c=.lst)
	rm -f $(SRC:.c=.su)
#   rm $(ASRC:.s=.s.bak)
	rm -f $(ASRC:.s=.lst)
	rm -f *.o
	rm -f *.out

tests:
	# simple functions tests, copy functions to test into main
	gcc src/tests.c 
	./a.out

tests_colors_functions:
	gcc -c src/colors_functions.c -I. $(INCDIR)
	gcc src/tests_colors_functions.c colors_functions.o
	./a.out


tests_colors_menu:
	# primero objetos de los modulos a testear, solo si son tipo HAL sin dependencia del hard
	gcc -c src/colors_menu.c -I. $(INCDIR)
	gcc -c src/display_utils.c -I. $(INCDIR)
	gcc -c src/ssd1306_gfx.c -I. $(INCDIR)
	gcc src/tests_colors_menu.c colors_menu.o display_utils.o ssd1306_gfx.o -lpthread -lncurses
	./a.out

tests_options_menu:
	# primero objetos de los modulos a testear, solo si son tipo HAL sin dependencia del hard
	gcc -c src/options_menu.c -I. $(INCDIR)
	gcc -c src/display_utils.c -I. $(INCDIR)
	gcc -c src/ssd1306_gfx.c -I. $(INCDIR)
	gcc src/tests_options_menu.c options_menu.o display_utils.o ssd1306_gfx.o -lpthread -lncurses
	./a.out

tests_master_slave_menu:
	# primero objetos de los modulos a testear, solo si son tipo HAL sin dependencia del hard
	gcc -c src/master_slave_menu.c -I. $(INCDIR)
	gcc -c src/options_menu.c -I. $(INCDIR)
	gcc -c src/display_utils.c -I. $(INCDIR)
	gcc -c src/ssd1306_gfx.c -I. $(INCDIR)
	gcc src/tests_master_slave_menu.c master_slave_menu.o options_menu.o display_utils.o ssd1306_gfx.o -lpthread -lncurses
	./a.out

tests_master_menu:
	# primero objetos de los modulos a testear, solo si son tipo HAL sin dependencia del hard
	gcc -c src/master_slave_menu.c -I. $(INCDIR)
	gcc -c src/options_menu.c -I. $(INCDIR)
	gcc -c src/display_utils.c -I. $(INCDIR)
	gcc -c src/ssd1306_gfx.c -I. $(INCDIR)
	gcc src/tests_master_slave_menu.c master_slave_menu.o options_menu.o display_utils.o ssd1306_gfx.o -lpthread -lncurses
	./a.out

tests_slave_menu:
	# primero objetos de los modulos a testear, solo si son tipo HAL sin dependencia del hard
	gcc -c src/slave_menu.c -I. $(INCDIR)
	gcc -c src/options_menu.c -I. $(INCDIR)
	gcc -c src/display_utils.c -I. $(INCDIR)
	gcc -c src/ssd1306_gfx.c -I. $(INCDIR)
	gcc src/tests_slave_menu.c slave_menu.o options_menu.o display_utils.o ssd1306_gfx.o -lpthread -lncurses
	./a.out

tests_current_menu:
	# primero objetos de los modulos a testear, solo si son tipo HAL sin dependencia del hard
	gcc -c src/current_menu.c -I. $(INCDIR)
	gcc -c src/options_menu.c -I. $(INCDIR)
	gcc -c src/display_utils.c -I. $(INCDIR)
	gcc -c src/ssd1306_gfx.c -I. $(INCDIR)
	gcc src/tests_current_menu.c current_menu.o options_menu.o display_utils.o ssd1306_gfx.o -lpthread -lncurses
	./a.out

tests_limits_menu:
	# primero objetos de los modulos a testear, solo si son tipo HAL sin dependencia del hard
	gcc -c src/limits_menu.c -I. $(INCDIR)
	gcc -c src/options_menu.c -I. $(INCDIR)
	gcc -c src/display_utils.c -I. $(INCDIR)
	gcc -c src/ssd1306_gfx.c -I. $(INCDIR)
	gcc src/tests_limits_menu.c limits_menu.o options_menu.o display_utils.o ssd1306_gfx.o -lpthread -lncurses
	./a.out

tests_channels_menu:
	# primero objetos de los modulos a testear, solo si son tipo HAL sin dependencia del hard
	gcc -c src/channels_menu.c -I. $(INCDIR)
	gcc -c src/options_menu.c -I. $(INCDIR)
	gcc -c src/display_utils.c -I. $(INCDIR)
	gcc -c src/ssd1306_gfx.c -I. $(INCDIR)
	gcc src/tests_channels_menu.c channels_menu.o options_menu.o display_utils.o ssd1306_gfx.o -lpthread -lncurses
	./a.out

tests_temp_menu:
	# primero objetos de los modulos a testear, solo si son tipo HAL sin dependencia del hard
	gcc -c src/temp_menu.c -I. $(INCDIR)
	gcc -c src/options_menu.c -I. $(INCDIR)
	gcc -c src/display_utils.c -I. $(INCDIR)
	gcc -c src/ssd1306_gfx.c -I. $(INCDIR)
	gcc src/tests_temp_menu.c temp_menu.o options_menu.o display_utils.o ssd1306_gfx.o -lpthread -lncurses
	./a.out

tests_version_menu:
	# primero objetos de los modulos a testear, solo si son tipo HAL sin dependencia del hard
	gcc -c src/version_menu.c -I. $(INCDIR)
	gcc -c src/options_menu.c -I. $(INCDIR)
	gcc -c src/display_utils.c -I. $(INCDIR)
	gcc -c src/ssd1306_gfx.c -I. $(INCDIR)
	gcc src/tests_version_menu.c version_menu.o options_menu.o display_utils.o ssd1306_gfx.o -lpthread -lncurses
	./a.out

tests_limit_simulation:
	# simulate the limits functions
	gcc -c src/dsp.c -I. $(INCDIR)
	gcc src/tests_simul.c dsp.o
	./a.out
	# execute by hand python3 simul_limits.py

tests_comm:
	# first module objects to test
	gcc -c src/comm.c -I. $(INCDIR)
	# second auxiliary helper modules
	gcc -c src/tests_ok.c -I $(INCDIR)
	gcc src/tests_comm.c comm.o tests_ok.o
	./a.out

tests_comm_coverage:
	# first module objects to test and coverage
	gcc -c --coverage src/comm.c -I. $(INCDIR)
	# second auxiliary helper modules
	gcc -c src/tests_ok.c -I $(INCDIR)
	# compile the test and link with modules
	gcc --coverage src/tests_comm.c comm.o tests_ok.o
	# test execution
	./a.out
	# process coverage
	gcov comm.c -m


tests_temp_functions:
	# first module objects to test and coverage
	gcc -c --coverage src/temp_menu.c -I. $(INCDIR)
	gcc -c src/options_menu.c -I. $(INCDIR)
	gcc -c src/display_utils.c -I. $(INCDIR)
	gcc -c src/ssd1306_gfx.c -I. $(INCDIR)
	# second auxiliary helper modules
	gcc -c src/tests_ok.c -I $(INCDIR)
	# compile the test and link with modules
	gcc --coverage src/tests_temp_functions.c temp_menu.o tests_ok.o options_menu.o display_utils.o ssd1306_gfx.o
	# test execution
	./a.out
	# process coverage
	gcov temp_menu.c -m


tests_comm_profiling:
	# first module objects to test and profiling
	gcc -c -pg src/comm.c -I. $(INCDIR)
	# second auxiliary helper modules
	gcc -c -pg src/tests_ok.c -I $(INCDIR)
	# compile the test and link with modules
	gcc -pg src/tests_comm.c comm.o tests_ok.o
	# test execution
	./a.out
	# process profiling
	gprof a.out gmon.out > gprof.txt


tests_oled:
	# first compile common modules (modules to test and dependencies)
	# gcc -c src/lcd_utils.c -I. $(INCDIR)
	# the module that implements tests_lcd_application.h functions
	gcc -c `pkg-config --cflags gtk+-3.0` src/tests_oled_template.c -o tests_oled_template.o
	# then the gtk lib modules
	gcc -c `pkg-config --cflags gtk+-3.0` src/tests_glade_oled.c -o tests_glade_oled.o
	# link everything
	gcc tests_glade_oled.o tests_oled_template.o `pkg-config --libs gtk+-3.0` -o tests_gtk
	# run the simulation
	# ./tests_gtk


tests_oled_screen:
	# first compile common modules (modules to test and dependencies)
	gcc -c src/screen.c -I. $(INCDIR)
	gcc -c src/ssd1306_display.c -I. $(INCDIR) $(DDEFS)
	gcc -c src/ssd1306_gfx.c -I. $(INCDIR)
	# the module that implements tests_lcd_application.h functions
	gcc -c `pkg-config --cflags gtk+-3.0` src/tests_oled_app.c -o tests_oled_app.o
	# then the gtk lib modules
	gcc -c `pkg-config --cflags gtk+-3.0` src/tests_glade_oled.c -o tests_glade_oled.o
	# link everything
	gcc tests_glade_oled.o tests_oled_app.o screen.o ssd1306_display.o ssd1306_gfx.o `pkg-config --libs gtk+-3.0` -o tests_gtk
	# run global tags
	gtags -q
	# run the simulation
	# ./tests_gtk


tests_main_menu:
	# first compile common modules (modules to test and dependencies)
	gcc -c src/main_menu.c -I. $(INCDIR)
	gcc -c src/options_menu.c -I. $(INCDIR)
	gcc -c src/display_utils.c -I. $(INCDIR)
	gcc -c src/screen.c -I. $(INCDIR)
	gcc -c src/ssd1306_display.c -I. $(INCDIR) $(DDEFS)
	gcc -c src/ssd1306_gfx.c -I. $(INCDIR)
	# the module that implements application.h functions
	gcc -c `pkg-config --cflags gtk+-3.0` src/tests_oled_main_menu.c -o tests_oled_main_menu.o
	# then the gtk lib modules
	gcc -c `pkg-config --cflags gtk+-3.0` src/tests_glade_oled.c -o tests_glade_oled.o
	# link everything
	gcc tests_glade_oled.o tests_oled_main_menu.o main_menu.o options_menu.o display_utils.o screen.o ssd1306_display.o ssd1306_gfx.o `pkg-config --libs gtk+-3.0` -o tests_gtk
	# run global tags
	gtags -q
	# run the simulation
	./tests_gtk


tests_dmx_menu:
	# first compile common modules (modules to test and dependencies)
	gcc -c src/dmx_menu.c -I. $(INCDIR)
	gcc -c src/options_menu.c -I. $(INCDIR)
	gcc -c src/display_utils.c -I. $(INCDIR)
	gcc -c src/screen.c -I. $(INCDIR)
	gcc -c src/ssd1306_display.c -I. $(INCDIR) $(DDEFS)
	gcc -c src/ssd1306_gfx.c -I. $(INCDIR)
	# the module that implements application.h functions
	gcc -c `pkg-config --cflags gtk+-3.0` src/tests_oled_app_dmx1_menu.c -o tests_oled_app_dmx1_menu.o
	# then the gtk lib modules
	gcc -c `pkg-config --cflags gtk+-3.0` src/tests_glade_oled.c -o tests_glade_oled.o
	# link everything
	gcc tests_glade_oled.o tests_oled_app_dmx1_menu.o dmx1_menu.o options_menu.o display_utils.o screen.o ssd1306_display.o ssd1306_gfx.o `pkg-config --libs gtk+-3.0` -o tests_gtk
	# run global tags
	gtags -q
	# run the simulation
	./tests_gtk


tests_manual_menu:
	# first compile common modules (modules to test and dependencies)
	gcc -c src/manual_menu.c -I. $(INCDIR)
	gcc -c src/options_menu.c -I. $(INCDIR)
	gcc -c src/display_utils.c -I. $(INCDIR)
	gcc -c src/screen.c -I. $(INCDIR)
	gcc -c src/ssd1306_display.c -I. $(INCDIR) $(DDEFS)
	gcc -c src/ssd1306_gfx.c -I. $(INCDIR)
	# the module that implements application.h functions
	gcc -c `pkg-config --cflags gtk+-3.0` src/tests_oled_manual_menu.c -o tests_oled_manual_menu.o
	# then the gtk lib modules
	gcc -c `pkg-config --cflags gtk+-3.0` src/tests_glade_oled.c -o tests_glade_oled.o
	# link everything
	gcc tests_glade_oled.o tests_oled_manual_menu.o manual_menu.o options_menu.o display_utils.o screen.o ssd1306_display.o ssd1306_gfx.o `pkg-config --libs gtk+-3.0` -o tests_gtk
	# run global tags
	gtags -q
	# run the simulation
	./tests_gtk


tests_fixed_menu:
	# first compile common modules (modules to test and dependencies)
	gcc -c src/fixed_menu.c -I. $(INCDIR)
	gcc -c src/options_menu.c -I. $(INCDIR)
	gcc -c src/display_utils.c -I. $(INCDIR)
	gcc -c src/screen.c -I. $(INCDIR)
	gcc -c src/ssd1306_display.c -I. $(INCDIR) $(DDEFS)
	gcc -c src/ssd1306_gfx.c -I. $(INCDIR)
	# the module that implements application.h functions
	gcc -c `pkg-config --cflags gtk+-3.0` src/tests_oled_fixed_menu.c -o tests_oled_fixed_menu.o
	# then the gtk lib modules
	gcc -c `pkg-config --cflags gtk+-3.0` src/tests_glade_oled.c -o tests_glade_oled.o
	# link everything
	gcc tests_glade_oled.o tests_oled_fixed_menu.o fixed_menu.o options_menu.o display_utils.o screen.o ssd1306_display.o ssd1306_gfx.o `pkg-config --libs gtk+-3.0` -o tests_gtk
	# run global tags
	gtags -q
	# run the simulation
	./tests_gtk


# *** EOF ***
