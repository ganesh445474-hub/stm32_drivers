/* filepath: /home/user-linux/cortex-m3-bootloader/bootloader/linker/bootloader.ld */
ENTRY(Reset_Handler)

/* top of RAM, used for initial stack pointer */
/* STM32F103C8 has 20KB SRAM at 0x20000000 */
_estack = 0x20005000;              /* End of 20KB RAM */

MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

SECTIONS
{
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } >FLASH

  .text :
  {
    *(.text*)
    *(.rodata*)
    KEEP(*(.init))
    KEEP(*(.fini))
  } >FLASH

  .data : AT(ADDR(.text) + SIZEOF(.text))
  {
    _sidata = LOADADDR(.data);
    _sdata  = .;
    *(.data*)
    _edata  = .;
  } >RAM

  .bss :
  {
    _sbss = .;
    *(.bss*)
    *(.common)
    _ebss = .;
  } >RAM

  end = .;

  /* stack space, not necessary but makes the symbol available */
  .stack (NOLOAD):
  {
    _estack = ORIGIN(RAM) + LENGTH(RAM);
  } >RAM
}
