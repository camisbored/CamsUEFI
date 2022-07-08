# CamsUEFI
A UEFI Bootloader with I/O and graphical capabilities.

Being that BIOs is deprecated and my recent OS dev work uses BIOs, I thought it might be more useful to start looking into UEFI development. 
This project is a bootable UEFI program that takes input from keyboard and performs some basic functions.
*THIS IS A WORK IN PROGRESS*

Currenly this-
1. Boots and displays welcome message/menu
2. Takes keyboard input for the function you want to call.
3. Has printing, new line, clear screen functions.
4. Enters a text mode, where whatever is typed is echoed back on screen.
5. Enters a graphics mode, where it will draw some shapes and colors to the screen.
6. Upon pressing escape, will exit and return to your machines default landing location.
7. Prints invalid command message and help menu if invalid input is given.


Todo-
1. Add mouse functionality.
2. Change navigation to GUI maybe? At least give more developed shell.
3. Make this do something more useful, maybe Snake or Pong game.
4. Break this out into a different file structure after expanding? (Currently 1 c file).


This was developed in native Linux (Ubuntu 20.04) and should be able to be built standard from a similar environment.
I've included a build script and a run script. All that should be needed to build is gcc and having the 
gnu-efi library in your include folder (or modify script/path). To run, you will need to have qemu working.
If working from a CLI only environment, you can add the -nographics option to the build script, which will 
work but remove any graphical capabilities. The OVMF.fd file is a required driver for QEMU to work with UEFI properly.

To boot from this on your machine, one level into the image folder you can copy the entire efi folder to a flash drive.
This means that from a FAT32 usb, you would have the following path and file contained: /efi/boot/BOOTX64.efi
The BOOTX64.efi is the executable file, and UEFI systems look for that specific file in that path and attempt to boot it.
Upon machine startup, hold down F12 (or F2/F10 depending on your system), and you select UEFI USB (or something of that nature).
This will boot this program.

Inqueries and requests can be sent to camerongrande95@gmail.com.
