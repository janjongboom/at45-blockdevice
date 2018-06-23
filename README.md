# AT45 Block Device

Mbed OS 5 [BlockDevice](https://os.mbed.com/docs/latest/reference/blockdevice.html) driver for the AT45 SPI Dataflash chip. Based off [Steen Jørgensen's AT45 library](https://os.mbed.com/users/stjo2809/code/AT45/). You can use this driver together with the file system APIs in Mbed OS to mount a file system on your external flash, or just use the driver directly. Note that you can only do aligned operations on this block device.

## Deinitialization

Mbed OS does not have a way to destruct a SPI interface once created. This causes issues with the AT45 when initializing it multiple times, like in a bootloader and then in an application. For this a `DeconstructableSPI` interface is used in this library. If you call `deinit` on the block device it will automatically uninitialize the SPI interface. Do this before jumping to the main program from a bootloader.
