OUTPUT_ARCH( "riscv" )
ENTRY(_start)

MEMORY
{
    RAM (xrw)       : ORIGIN = 0x80080000, LENGTH = 0x000800
}

SECTIONS
{
  . = ORIGIN(RAM);
  .text :
  {
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    *(.srodata)        /* .rodata sections (constants, strings, etc.) */
    *(.srodata*)       /* .rodata* sections (constants, strings, etc.) */
  } >RAM

  . = ALIGN(0x400);
  .data :
  {
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    *(.sdata)          /* .sdata sections */
    *(.sdata*)         /* .sdata* sections */
  } >RAM

  .bss :
  {
    *(.bss)
    *(.bss*)
    *(.sbss)
    *(.sbss*)
  } >RAM
  _end = .;

  PROVIDE(_stack = ORIGIN(RAM) + LENGTH(RAM) - 4);
}

