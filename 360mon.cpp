#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include "windows.h"
#include "conio.h"

#define ESC 27

using namespace std;

int main(int argc, char **argv) {
    
    
    //////////////////////////////////////////////////
    // Init Serial Comms //
    HANDLE hSerial;
	COMMTIMEOUTS timeout = {0};
	COMMCONFIG cfg;
	BOOL ret;
    char readbuff[2], writebuff[2] = {0};
    char string[2] = {0};
    DWORD dwBytesRead, dwBytesWrite = 0;
    
    char key;
    char byte;
	
	if (argc < 2) {
		fprintf(stderr, "Usage:   serial_read.exe <port>\n");
		fprintf(stderr, "Example: serial_read.exe COM3:\n");
		return 1;
	}
	
	//create the serial port
	hSerial = CreateFile(argv[1], GENERIC_READ | GENERIC_WRITE,
		0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		
    
    if (hSerial == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Unable to open %s\n", argv[1]);
		return 1;
	}
    
	// Configure the port
	GetCommConfig(hSerial, &cfg, &dwBytesRead);
	cfg.dcb.BaudRate = 115200;
	cfg.dcb.fBinary = TRUE;
	cfg.dcb.fParity = FALSE;
	cfg.dcb.fOutxCtsFlow = FALSE;
	cfg.dcb.fOutxDsrFlow = FALSE;
	cfg.dcb.fDtrControl = DTR_CONTROL_ENABLE;
	cfg.dcb.fDsrSensitivity = FALSE;
	cfg.dcb.fTXContinueOnXoff = TRUE;
	cfg.dcb.fOutX = FALSE;
	cfg.dcb.fInX = FALSE;
	cfg.dcb.fErrorChar = FALSE;
	cfg.dcb.fNull = FALSE;
	cfg.dcb.fRtsControl = RTS_CONTROL_ENABLE;
	cfg.dcb.fAbortOnError = FALSE;
	cfg.dcb.XonLim = 0x8000;
	cfg.dcb.XoffLim = 20;
	cfg.dcb.ByteSize = 8;
	cfg.dcb.Parity = NOPARITY;
	cfg.dcb.StopBits = ONESTOPBIT;
	SetCommConfig(hSerial, &cfg, dwBytesRead);
	GetCommTimeouts(hSerial, &timeout);
	timeout.ReadIntervalTimeout = 50;
	timeout.ReadTotalTimeoutConstant = 50;
	timeout.ReadTotalTimeoutMultiplier = 10;
	timeout.WriteTotalTimeoutConstant = 50;
	timeout.WriteTotalTimeoutMultiplier = 10;
	SetCommTimeouts(hSerial, &timeout);

    //Setup logging to file
    ofstream hexlog("hexlog.log");
    ofstream asciilog("asciilog.log");
        
    cout << "\n360 POST Monitor by Blackaddr\nEditted by Rumblpak for teensy\nPress ESC to quit\n\n";

    // MAIN POLLING LOOP
    for (;;) {
    
      if (kbhit()) {
        // key was pressed
        key = getch();
        if (key == 27) {break; } else { cout << "You pressed 0x" << hex << setw(2) << setfill('0') << (unsigned)key << endl; }
      }
      
      // check the serial port
      ret = ReadFile(hSerial, readbuff, 1, &dwBytesRead, NULL);
      int i = 0;
      ofstream x;
      while (dwBytesRead > 0) {
        // for (int i=0; i<dwBytesRead; i++) {
            //if((unsigned)readbuff[i] == ((char)13 || (char)10)) continue;
            string[i] += (unsigned)readbuff[i];
            //if( string[i] == (char)10 ) continue;  
            //if( string[i] == (char)13 ) continue;  
            //cout << "0x" << hex << setw(2) << setfill('0') << (unsigned)readbuff[i] << endl;
            //cout << string << endl;
            hexlog << "0x" << hex << setw(2) << setfill('0') << (unsigned)readbuff[i] << endl;
            //asciilog << (unsigned)readbuff[i] << endl;
        // }
        dwBytesRead--;
        i++;
      }
      //if( string[0] == (char)0) {}
     // else if(string[1] == (char)0 ){}
      //else {
           printf("%c",string[0]);
           asciilog << string[0];
           //fprintf(asciilog,"%c%c",string[0],string[1]);
           //asciilog << string << endl;
     // }
      string[0] = 0;
      string[1] = 0;
      //string[2] = (char)0;
      //printf("%02c%02c\n",string[0],string[1]);
      //cout << string << endl;
      //string = [];
    }


    cout << "\nExiting...\n" << endl; 
    hexlog.close();
    asciilog.close();
    CloseHandle(hSerial);
}
