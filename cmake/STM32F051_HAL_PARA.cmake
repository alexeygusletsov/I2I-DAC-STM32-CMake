set(CPU_PARAMETERS ${CPU_PARAMETERS}
    -mthumb
    -mcpu=cortex-m0
    -mfpu=fpv4-sp-d16
    -mfloat-abi=soft
)

set(compiler_define ${compiler_define}
    "USE_HAL_DRIVER"
    "STM32F051x8"
)