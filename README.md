# CamsUEFI
A UEFI Bootloader with I/O and graphical capabilities.

Being that BIOs is deprecated and my recent OS dev work uses BIOs, I thought it might be more useful to start looking into UEFI development. 
This project is a bootable UEFI program that takes input from mouse and keyboard and performs some Graphical and I/O functions.
*THIS IS A WORK IN PROGRESS*

![IMG_20220801_083101538](https://user-images.githubusercontent.com/81730723/182148829-592622fa-1d9d-4547-9549-f1ffa44aa00f.jpg)

Currenly this-
1. Boots and displays welcome message/menu  
![image](https://user-images.githubusercontent.com/81730723/182147369-7a96e148-f5b0-4215-a226-202d8f467031.png)
2. Takes keyboard input for the function you want to call.
3. Has printing, new line, clear screen functions.
4. Will launch GUI with mouse capabilities if no key pressed within 5 seconds on launch. 
![image](https://user-images.githubusercontent.com/81730723/182147661-7a9ff64f-8711-4675-9d16-7b89c0237e2a.png)
5. Enters a graphics mode, where it will draw some shapes and colors to the screen. 


![image](https://user-images.githubusercontent.com/81730723/182154467-beecf1cd-0be7-43c4-874e-ca59be4d12a8.png)
![image](https://user-images.githubusercontent.com/81730723/182154546-044759f5-b184-4381-965b-4fd308e0961b.png)
![image](https://user-images.githubusercontent.com/81730723/182154598-f8875013-832b-4ab7-a39d-6ca4417ce95e.png)

6. Upon typing exit, will exit and return to your machines default landing location.
7. Prints invalid command message and help menu if invalid input is given.
8. Renders image of Cat to the screen via draw command (or clicking pic in GUI, can be closed with X). 

![image](https://user-images.githubusercontent.com/81730723/182147826-c87ef7c9-a67d-4194-906d-6e9aca9e60a6.png)

![IMG_20220801_083159157_HDR](https://user-images.githubusercontent.com/81730723/182149239-b53092d5-8636-4b9d-a35f-6756a9c7e56a.jpg)

9. Plays video of Cat based on bytes per pixel given (with sound)
10. Provides sound mode where you can input keys to buzz different frequencies.
11. Will print time (inaccurately- somethings off)
12. Starting to read file system (claims to read file but buffer unreadable)

*NOTE* Mouse input will not work in QEMU but works on real hadrware, which limits a good amount of what can be used there.

Todo-
1. Make this do something more useful, maybe Snake or Pong game, interact with file system, interact with network stack.
2. Break this out into a different file structure/organize? (Currently just c files in root folder, functions.c needs to be cleaned.).


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
