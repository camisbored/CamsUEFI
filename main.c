/*
   main.c- the main logic for our UEFI shell. This will init library
   and call other features based on input.
   @author Cameron Grande
*/

#include <efi.h>
#include <efilib.h>
#include "functions.c"
 
EFI_STATUS
EFIAPI
//main function- program start
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable){
	InitializeLib(ImageHandle, SystemTable);
	SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);
	//logic start
	clearScreen();
	printInitialMenu();
	//main loop- check for key input forever, handle based on input
	while(1){
		printPrompt();
		getInput();
		  if (strcmp(L"newline", userString))
		  	newLine();
		  else if (strcmp(L"clear", userString))
			clearScreen();
		  else if (strcmp(L"color", userString))
			changeColor();
		  else if (strcmp(L"help", userString))
			printHelp();
		  else if (strcmp(L"draw", userString))
			drawCat();
		  else if (strcmp(L"exit", userString))
			return EFI_SUCCESS;
		  else if (strcmp(L"graphics", userString))
			graphicsTest();
		  else
		        printUnknown();
		}
}
