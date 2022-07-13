/*
  functions.c- contains data and functions called by out main
  function. This needs to be organized and broken down further.
  @author Cameron Grande
*/

#include "images.c"
//globals and strucutres
uint32_t base = 0;
int pixelsPerLine = 0; 
EFI_INPUT_KEY key;
CHAR16 *userString =  L"___DEFAULT_STRING___";
int userStringIndex = 0;
CHAR16 *currentChar =  L" ";
CHAR16 *invalidCmd = L"!!!Invalid command!!!\r\n";
CHAR16 *menu = L"Enter commands or type [help]!\r\n";
unsigned char color = 1;
int x = 50;
int y = 50;
uint32_t pixelColor = 0x0000FF00;
EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
EFI_STATUS status;
EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
EFI_TIME* time;
UINTN SizeOfInfo, numModes, nativeMode;

CHAR16 *helpMenu = L" -----------------------------------------------------------------------\r\n\
		 |                           Help Menu                                 |\r\n\
		 -----------------------------------------------------------------------\r\n\
		 | clear- clears screen                                                |\r\n\
		 -----------------------------------------------------------------------\r\n\
		 | exit- leave UEFI (Will return to machine default or BIOS menu)      |\r\n\
		 -----------------------------------------------------------------------\r\n\
		 | color- change color of text on screen                               |\r\n\
		 -----------------------------------------------------------------------\r\n\
		 | draw- will draw an image of a cat on a screen using different modes |\r\n\
		 -----------------------------------------------------------------------\r\n\
		 | graphics- will cycle through graphics modes/print statistics/draw   |\r\n\
		 -----------------------------------------------------------------------\r\n\
		 | newline- prints new line                                            |\r\n\
		 -----------------------------------------------------------------------\r\n";


//generic shell features
void clearScreen(){
	uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
}

void newLine(){
	uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, L" \r\n");
}

void printInitialMenu(){
	uefi_call_wrapper(ST->ConOut->SetAttribute, 1, ST->ConOut, EFI_YELLOW);
	uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, L"***********************************\
			  \r\n*Welcome to Cam's UEFI Bootloader!*\r\n***********************************\r\n");
	uefi_call_wrapper(ST->ConOut->SetAttribute, 1, ST->ConOut, EFI_GREEN);
	uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, menu);
	uefi_call_wrapper(ST->ConOut->SetAttribute, 1, ST->ConOut, EFI_RED);
}

void printHelp(){
	uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, helpMenu);
}

void printUnknown(){
        uefi_call_wrapper(ST->ConOut->SetAttribute, 1, ST->ConOut, EFI_BLUE);
        uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, invalidCmd);
        uefi_call_wrapper(ST->ConOut->SetAttribute, 1, ST->ConOut, color);
        uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, menu);
}

void printPrompt(){
	uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, L"$>");
}

//draws pixel with bpp bytes per pixel
void drawPixel(int x, int y, uint32_t color, int bpp){
	*((uint32_t*)(base + (bpp * pixelsPerLine * y) + (bpp * x))) = color;
}


void drawBox(int topLeftX, int topLeftY, int bottomRightX, int bottomRightY, uint32_t color, int bpp){
	int x = topLeftX, y = topLeftY;
	while (y++ < bottomRightY){
		while (x < bottomRightX)
			drawPixel(x++,y,color,bpp);
		x=topLeftX;
	}
}

void changeColor(){
	uefi_call_wrapper(ST->ConOut->SetAttribute, 1, ST->ConOut, color++);
}

void drawRainboxBox(int topLeftX, int topLeftY, int bottomRightX, int bottomRightY, int bpp){
	uint8_t red = 0x00, green = 0x00, blue = 0x00;
	int x = topLeftX, y = topLeftY;
	while (y++ < bottomRightY){
		while (x < bottomRightX)
				drawPixel(x++,y,(((((0x00000000 | red++) << 8) | green++) << 8) | blue++), bpp);
		x=topLeftX;
	}
}


//strcmp implementation- will compare two strings, return 1 if equal, 2 if not
int strcmp(CHAR16 *str1, CHAR16 *str2){
    CHAR16* pt1 = &str1[0];
    CHAR16* pt2 = &str2[0];
    while (*pt1 != 0x00)
    		if (*pt1++ != *pt2++)
    			return 0;
    return 1;
}


//getCh implementation- will wait until key pressed and load char value into key variable
void getCh(){
	while(1){
  	    for(int i=0;i<500;i++) 
	       if(!uefi_call_wrapper(BS->CheckEvent, 1, ST->ConIn->WaitForKey)) {           
	          uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);
	          return;
	    }
	    uefi_call_wrapper(BS->Stall, 1, 1000);
	}
}

//draws image with bpp bytes per pixel where img is a pointer to the image
 void draw_icon(int x, int y, int w, int h, unsigned char* img, int bpp) {
  int  l, i, j = 0;
     for (l = 0; l < h; l++) 
         for (i = 0; i < w; i++)
            *((uint32_t*)(base + (bpp * pixelsPerLine * (y+l)) + (bpp * (x+i)))) =(((((0x00000000 | img[j++]) << 8) | img[j++]) << 8) | img[j++]);
 }
 
//getInput- loads string input into userInput buffer. registers by pressing enter key. 
void getInput(){
    while(1){
	getCh();
	currentChar[0]=(UINTN)key.UnicodeChar;
	//if backspace pressed
	if (currentChar[0] == 0x08)
	   userStringIndex--;
	//if enter pressed
	else if (currentChar[0] == 0x0D){
		userString[userStringIndex]=0x00;
		userStringIndex=0;
		newLine();
	    	return;
	 }
	 //if within valid range
	else if (userStringIndex < 19)
  		userString[userStringIndex++] = currentChar[0];
  	//print key pressed to screen
  	if (userStringIndex < 19)
		uefi_call_wrapper(ST->ConOut->OutputString, 2, ST->ConOut, currentChar);
   }
}

void drawCat(){
	//init graphics mode if not already in
	 status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
	  status = uefi_call_wrapper(gop->QueryMode, 4, gop, gop->Mode==NULL?0:gop->Mode->Mode, &SizeOfInfo, &info);
	  if (status == EFI_NOT_STARTED)
	    status = uefi_call_wrapper(gop->SetMode, 2, gop, 0);
	  if(EFI_ERROR(status)) {
	    Print(L"Unable to get native mode");
	  } else {
	    nativeMode = gop->Mode->Mode;
	    numModes = gop->Mode->MaxMode;
	 }
	 int mode = gop->Mode->MaxMode-1;
	 status = uefi_call_wrapper(gop->SetMode, 2, gop, mode);
	 base = gop->Mode->FrameBufferBase;
	 pixelsPerLine = gop->Mode->Info->PixelsPerScanLine;
	 
	//draw image 
	Print(L"For the following screens, press [ESC] to continue after image is displayed\n");
	Print(L"Press any key to start.\n");
	getCh();
	clearScreen();
	draw_icon(x, y, GEORGE_WIDTH, GEORGE_HEIGHT, GEORGE_IMG, 3);
	//allow image to be moved aroudn screen until ESC pressed
	 while(1){
	 	getCh();
	 	clearScreen();
	 	if ((UINTN)key.ScanCode == 0x17){
	 		clearScreen();
	 		printInitialMenu();
	 		break;
	 	} else if (key.ScanCode == 0x01)
			y-=10;
		else if (key.ScanCode == 0x02)
			y+=10;
		else if (key.ScanCode == 0x03)
			x+=10;
		else if (key.ScanCode == 0x04)
			x-=10;
		draw_icon(x, y, GEORGE_WIDTH, GEORGE_HEIGHT, GEORGE_IMG, 3);
	 }
	clearScreen();
	
	//do same for 4 byte per pixel systems
	draw_icon(x, y, GEORGE_WIDTH, GEORGE_HEIGHT, GEORGE_IMG, 4);
	 while(1){
	 	getCh();
	 	clearScreen();
	 	if ((UINTN)key.ScanCode == 0x17){
	 		clearScreen();
	 		printInitialMenu();
	 		break;
	 	}else if (key.ScanCode == 0x01)
			y-=10;
		else if (key.ScanCode == 0x02)
			y+=10;
		else if (key.ScanCode == 0x03)
			x+=10;
		else if (key.ScanCode == 0x04)
			x-=10;
		draw_icon(x, y, GEORGE_WIDTH, GEORGE_HEIGHT, GEORGE_IMG, 4);
	 }
}

void graphicsTest(){
	Print(L"Querying graphics... your specs are as follows:\n");
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

	for (int i = 0; i < numModes; i++) {
	  status = uefi_call_wrapper(gop->QueryMode, 4, gop, i, &SizeOfInfo, &info);
	  Print(L"mode %03d width %d height %d format %x%s\n",
	    i,
	    info->HorizontalResolution,
	    info->VerticalResolution,
	    info->PixelFormat,
	    i == nativeMode ? "(current)" : ""
	  );
	}
	Print(L"Press any key to set mode/continue\n");
	getCh();

	//current mode is set here
	  int mode = gop->Mode->MaxMode-1;
	  status = uefi_call_wrapper(gop->SetMode, 2, gop, mode);
	  Print(L"Setting mode... your selected specs are as follows\n");
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
	Print(L"Start Address: %x\n", address);
	Print(L"Press any key to continue/draw");
	getCh();
	 uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
	 for (int i=0;i<gop->Mode->FrameBufferSize-10240;i+=3,address+=3)
	 	*((uint32_t*)(address)) = pixel;
	 x=50;
	 y=50;
	 address = base + (3 * pixelsPerLine * y) + (3 * x);
	 pixel=0x000000FF;
	 for (int i=0;i<327680;i+=3,address+=3)
	 	*((uint32_t*)(address)) = pixel;
	 	
	 drawBox(200,200,250,250,0x00FF0000, 3);
	 drawBox(300,200,350,250,0x0000FF00, 3);
	 drawBox(400,200,450,250,0x000000FF, 3);
	 drawRainboxBox(500,200,550,250, 3);
	 
	 Print(L"Previous was mode 1, press key for mode 2");	 
	 getCh();
	 clearScreen();
	 address = base + (4 * pixelsPerLine * y) + (4 * x);
	 pixel = 0x00FF0000;
	 for (int i=0;i<327680;i+=4,address+=4)
	 	*((uint32_t*)(address)) = pixel;
	 	
	 drawBox(200,200,250,250,0x00FF0000, 4);
	 drawBox(300,200,350,250,0x0000FF00, 4);
	 drawBox(400,200,450,250,0x000000FF, 4);
	 drawRainboxBox(500,200,550,250, 4);
	 	
	 Print(L"Press key for mode 3");	 
	 getCh();
	 address = base + (2 * pixelsPerLine * y) + (2 * x);
	 uint16_t pixel2 = 0xFF00;
	 for (int i=0;i<327680;i+=2,address+=2)
	 	*((uint32_t*)(address)) = pixel2;
	 	
	 Print(L"Press key for mode 4");	 
	 getCh();
	 address = base + (pixelsPerLine * y) + (x);
	 uint8_t pixel3 = 0xF2;
	 for (int i=0;i<327680;i+=1,address+=1)
	 	*((uint32_t*)(address)) = pixel3;
	 	
	 Print(L"Ok cool\n");
	 Print(L"Use arrow keys to move, b to set color black, c to change color, or press [ESC] to return\n");

	 while(1){
	 	getCh();
	 	if ((UINTN)key.ScanCode == 0x17){
	 		clearScreen();
	 		printInitialMenu();
	 		return;
	 	} else if (key.UnicodeChar == 0x62)
			pixelColor=0x00000000;
		else if (key.UnicodeChar == 0x63)
			pixelColor+= 100;
	 	else if (key.ScanCode == 0x01)
			y--;
		else if (key.ScanCode == 0x02)
			y++;
		else if (key.ScanCode == 0x03)
			x++;
		else if (key.ScanCode == 0x04)
			x--;
		drawPixel(x,y, pixelColor, 3);
		drawPixel(x,y, pixelColor, 4);
	 }
}
