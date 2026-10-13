ENTRY(reset_handler)

MEMORY
{
	FLASH (rx): ORIGIN = 0x08000000, LENGTH = 60K
  USER_FLASH (rx) : ORIGIN = 0x08000000 + 60K, LENGTH = 4K
	SRAM (rwx): ORIGIN = 0x20000000, LENGTH = 8K
}

/* highest address of the user mode stack */
_estack = 0x20002000;

SECTIONS
{
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } >FLASH

  .text :
  {
    . = ALIGN(4);
		
    *(.text)
    *(.text.*)
    *(.rodata)
    *(.rodata.*)
    KEEP(*(.init))
    KEEP(*(.fini))
    *(.eh_frame)
    *(.ARM.exidx)
		
    . = ALIGN(4);
    _etext = .;
  } >FLASH

  _sidata = LOADADDR(.data);

  .data :
  {
    . = ALIGN(4);
    _sdata = .;
		
    *(.data)
    *(.data.*)
    KEEP(*(.init_array))
    KEEP(*(.fini_array))

    . = ALIGN(4);
    _edata = .;
  } >SRAM AT> FLASH

  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    __bss_start__ = _sbss;
		
    *(.bss)
    *(.bss.*)
		
    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = _ebss;
    end = __bss_end__;
  } >SRAM

  .user_data :
  {
    . = ALIGN(4);
    _suser_data = .;     /* Define a global symbol for the start address */
    KEEP(*(.user_data))  /* Keep variables marked with this section */
    . = ALIGN(4);
    _euser_data = .;     /* Define a global symbol for the end address */
  } >USER_FLASH
}
