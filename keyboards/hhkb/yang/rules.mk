# MCU frequency
F_CPU = 8000000

# Build Options
#   change yes to no to disable
#
BOOTMAGIC_ENABLE = no       # Enable Bootmagic Lite
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = no         # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
NKRO_ENABLE = no            # USB Nkey Rollover
COMBO_ENABLE = yes          # Enable combos
DYNAMIC_MACRO_ENABLE = yes  # Enable dynamic macros
MAGIC_ENABLE = no

BLUETOOTH_ENABLE = yes

LTO_ENABLE = yes

# Custom matrix file for the HHKB
CUSTOM_MATRIX = lite
SRC += matrix.c
