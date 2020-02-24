# generation bin factory standalone
  Nb areas: 3, entry point 0x1C000094
  Area 0: offset: 0x400, base: 0x1c000000, size: 0x5fac, nbBlocks: 24
  Area 1: offset: 0x6400, base: 0x1c005fa8, size: 0xb20, nbBlocks: 3
  Area 2: offset: 0x7000, base: 0x1c006f84, size: 0x8, nbBlocks: 1
Generating boot loader data
dump 0x0  b'\x00\x00\x00\x00'
dump 0x94  b's%@\xf1'
dump 0x0  b'\x00\x00\x00\x00'
dump 0x94  b'\x07\x07\x07\x07'
dump 0x0  b'\x00\x00\x00\x00'
dump 0x94  b''
Partition boot loader size: 0x7400

Generating partition table:
Partition table offset: 0x7400
Partition table was not provided, generating generic table.
Creating an empty LittleFS partition, using the rest of the flash space: 0x3FC0000
Verifying table...

# GAP Partition Table
# Name, Type, SubType, Offset, Size, Flags
lfs,data,lfs,0x40000,65280K,

Dumping partition image::
lfs partition [None]

Writting output image to /g/app/ssbl/factory/BUILD/GAP8_V2/GCC_RISCV/flash.img, size 29KB.
dub @factory
# Generating elf2bin app factory 
  Nb areas: 3
  Area 0: offset: 0xd0, base: 0x1c000000, size: 0x5fac
  Area 1: offset: 0x6080, base: 0x1c005fa8, size: 0xb20
  Area 2: offset: 0x6ba0, base: 0x1c006f84, size: 0x8
Generating app data
ins 1  b'\x00\x00\x00\x00'
ins 1  b'\x00\x00\x00\x00'
ins 1  b'\x00\x00\x00\x00'
dub @ssbl

# run

App header: nbr of segments 3, entry point 0x1c000094
Load segment 0: flash offset 0xD0 - size 0x5FAC
Load segment to L2 memory at 0x1C000000
Load segment 1: flash offset 0x6080 - size 0xB20
Load segment to L2 memory at 0x1C005FA8
Load segment 2: flash offset 0x6BA0 - size 0x8
Load segment to L2 memory at 0x1C006F84
Boot to app entry point at 0x1C000094
@0x1C000094 0xF1402573
@0x1C000098 0x5938115


         *** PMSIS Factory App ***
