# emacs_dexel_rgb_6ch
Placa Dexel 6CH modelo nuevo V1.0

micro STM32F030R8T6

#antes de empezar revisar seleccion del micro
stm32f0_flash.cfg		//work area size y flash image

stm32f0_flash_lock.cfg		//work area size y flash image

stm32f0_gdb.cfg		//work area size

stm32f0_reset.cfg		//work area size


./cmsis_boot/startup/stm32_flash.ld		//end of ram; stack size; memory lenght

./cmsis_boot/stm32f0xx.h					//linea 68 elegir micro


