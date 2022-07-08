/*
  main.c- @author Cameron Grande
  contains our main logic to be executed upon boot. Currently contains basic I/O 
  functionality and ability to draw graphics to screen. This is currently
  the only source file in this project.
*/
#include <efi.h>
#include <efilib.h>
 
//globals and prototypes
uint32_t base = 0;
int pixelsPerLine = 0; 

void clearScreen();
void newLine();
void functionOne();
void functionTwo();
void drawPixel(int x, int y, uint32_t color);
void drawBox(int topLeftX, int topLeftY, int bottomRightX, int bottomRightY, uint32_t color);
void drawRainboxBox(int topLeftX, int topLeftY, int bottomRightX, int bottomRightY);
 
EFI_STATUS
EFIAPI
//main function- program start
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable){
  InitializeLib(ImageHandle, SystemTable);
     //define variables and structures
     EFI_INPUT_KEY key;
     EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
     EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
     EFI_STATUS status;
     EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
     UINTN SizeOfInfo, numModes, nativeMode;
     key.ScanCode = SCAN_NULL;
     
     CHAR16 *invalidCmd = L"!!!Invalid command!!!\r\n";
     CHAR16 *menu = L"Press [1] or [2] for functions, [3] for new line, [4] to clearscreen, [a] for text mode (then . to return), [5] for gfx, or [ESC] to exit!\r\n";
     CHAR16 *currentChar =  L" ";

     //logic start
     clearScreen();
     uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 1, SystemTable->ConOut, EFI_YELLOW);
     uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, L"***********************************\r\n*Welcome to Cam's UEFI Bootloader!*\r\n***********************************\r\n");
     uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 1, SystemTable->ConOut, EFI_GREEN);
     uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, menu);
     uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 1, SystemTable->ConOut, EFI_RED);
     CHAR16 *string =  L" ";
     	  //main loop- check for key input forever, handle based on input
	  while(1){
	  	exitInputLoop:
	  	    for(int i=0;i<500;i++) {
		       if(!uefi_call_wrapper(BS->CheckEvent, 1, ST->ConIn->WaitForKey)) {           
		          uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);
		          if ((UINTN)key.UnicodeChar == 0x31)
		          	functionOne();
		          else if ((UINTN)key.UnicodeChar == 0x32)
				functionTwo();
		          else if ((UINTN)key.UnicodeChar == 0x33)
		          	newLine();
		          else if ((UINTN)key.UnicodeChar == 0x34)
				clearScreen();
			  else if ((UINTN)key.ScanCode == 0x17)
				return EFI_SUCCESS;
			  /*
			  Pressing the 5 key will run the graphics mode test. It will check for current settings,
			  print data to screen, then draw pixels and text to screen.
			  */
		          else if ((UINTN)key.UnicodeChar == 0x35){
		                status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
	  		        if(EFI_ERROR(status))
	    				Print(L"Unable to locate GOP");
	    				
	    			  status = uefi_call_wrapper(gop->QueryMode, 4, gop, gop->Mode==NULL?0:gop->Mode->Mode, &SizeOfInfo, &info);
				  // this is needed to get the current video mode
				  if (status == EFI_NOT_STARTED)
				    status = uefi_call_wrapper(gop->SetMode, 2, gop, 0);
				  if(EFI_ERROR(status)) {
				    Print(L"Unable to get native mode");
				  } else {
				    nativeMode = gop->Mode->Mode;
				    numModes = gop->Mode->MaxMode;
	  			}	
	  			
	  			for (i = 0; i < numModes; i++) {
				  status = uefi_call_wrapper(gop->QueryMode, 4, gop, i, &SizeOfInfo, &info);
				  Print(L"mode %03d width %d height %d format %x%s\n",
				    i,
				    info->HorizontalResolution,
				    info->VerticalResolution,
				    info->PixelFormat,
				    i == nativeMode ? "(current)" : ""
				  );
				}
	    				
	    			//current mode is set here
	    		          int mode = gop->Mode->MaxMode-1;
	    			  status = uefi_call_wrapper(gop->SetMode, 2, gop, mode);
				  if(EFI_ERROR(status)) {
				    Print(L"Unable to set mode %03d", mode);
				  } else {
				    // get framebuffer
				    Print(L"Framebuffer address %x size %d, width %d height %d pixelsperline %d\n",
				      gop->Mode->FrameBufferBase,
				      gop->Mode->FrameBufferSize,
				      gop->Mode->Info->HorizontalResolution,
				      gop->Mode->Info->VerticalResolution,
				      gop->Mode->Info->PixelsPerScanLine
				    );
				  }	
	    			
	    			//here we begin drawing to screen
	    			int x = 0;
	    			int y = 0;
	    			base = gop->Mode->FrameBufferBase;
	    			pixelsPerLine = gop->Mode->Info->PixelsPerScanLine;
	    			Print(L"Base: %x, Pixels: %d\n", base, pixelsPerLine);
	    			uint32_t pixel = 0x00FFFFFF;
	    			uint32_t address = base + (3 * pixelsPerLine * y) + (3 * x);
	    			Print(L"Address: %x\n", address);
	    			 uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
	    			 for (int i=0;i<gop->Mode->FrameBufferSize-10240;i+=3,address+=3)
	    			 	*((uint32_t*)(address)) = pixel;
	    			 x=50;
	    			 y=50;
	    			 address = base + (3 * pixelsPerLine * y) + (3 * x);
	    			 pixel=0x000000FF;
	    			 for (int i=0;i<327680;i+=3,address+=3)
	    			 	*((uint32_t*)(address)) = pixel;
	    			 	
	    			 drawPixel(0, 0, 0x0000FF00);
	    			 drawBox(200,200,250,250,0x00FF0000);
	    			 drawBox(300,200,350,250,0x0000FF00);
	    			 drawBox(400,200,450,250,0x000000FF);
	    			 drawRainboxBox(500,200,550,250);
	    			 	
	    			 Print(L"Ok cool");

			  }
			  //if the A key is pressed, we enter a mode where the system will print key pressed to screen
			  //we can then use the . key to return to menu
			  else if ((UINTN)key.UnicodeChar == 0x61){
			    while(1){
	  	    		for(int i=0;i<500;i++) {
		      		 if(!uefi_call_wrapper(BS->CheckEvent, 1, ST->ConIn->WaitForKey)) {           
		          		uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);
			  		currentChar[0]=(UINTN)key.UnicodeChar;
					if (currentChar[0] == '.')
			  		    goto exitInputLoop;
			  		else
					    uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, currentChar);
				    }
				  break;
		    		}
		    	     uefi_call_wrapper(BS->Stall, 1, 1000);
		   	   }
			} else {
			     uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 1, SystemTable->ConOut, EFI_BLUE);
			     uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, invalidCmd);
			     uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 1, SystemTable->ConOut, EFI_RED);
			     uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, menu);
			}
		       break;
		    }
		    uefi_call_wrapper(BS->Stall, 1, 1000);
		 }
	  	}
}

void clearScreen(){
	uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
}

void newLine(){
	uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, L" \r\n");
}

void functionOne(){
	uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, L"Function 1 was hit!");
}

void functionTwo(){
	uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, L"Function 2 was hit!");
}

void drawPixel(int x, int y, uint32_t color){
	*((uint32_t*)(base + (3 * pixelsPerLine * y) + (3 * x))) = color;
}

void drawBox(int topLeftX, int topLeftY, int bottomRightX, int bottomRightY, uint32_t color){
	int x = topLeftX, y = topLeftY;
	while (y++ < bottomRightY){
		while (x < bottomRightX)
			drawPixel(x++,y,color);
		x=topLeftX;
	}
}

void drawRainboxBox(int topLeftX, int topLeftY, int bottomRightX, int bottomRightY){
	uint8_t red = 0x00, green = 0x00, blue = 0x00;
	int x = topLeftX, y = topLeftY;
	while (y++ < bottomRightY){
		while (x < bottomRightX)
				drawPixel(x++,y,(((((0x00000000 | red++) << 8) | green++) << 8) | blue++));
		x=topLeftX;
	}
}
