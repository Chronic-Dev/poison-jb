go image load 0x69626F74 0x42000000
go patch 0x42000000 0x38000
go jump 0x42000040

go
go kernel load 0x42000000 0xF00000
go kernel patch 0x43000000 0xF00000
go jump 3 0x43001000 0x409fa000

go
go mw 0x5FF3B500 0xFFFFFFFF
go mw 0x5FF37014 0x44000000
go kernel load 0x42000000 0xF00000
go kernel boot

-v serial=1 serialbaud=115200 debug=12

# fsboot
go call 0x5FF00771

# ramdisk
go call 0x5FF01059
