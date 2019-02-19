#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

/*----------------------------------------------------------------------------------------------------
 * open port
 *----------------------------------------------------------------------------------------------------
 */
static HANDLE
open_port (wchar_t * comport)
{
    HANDLE hPort;

    hPort = CreateFile (comport,              // Pointer to the name of the port
                        GENERIC_READ | GENERIC_WRITE,
                                                        // Access (read-write) mode
                        0,                              // Share mode
                        NULL,                           // Pointer to the security attribute
                        OPEN_EXISTING,                  // How to open the serial port
                        0,                              // Port attributes
                        NULL);                          // Handle to port with attribute

    return (hPort);
}

/*----------------------------------------------------------------------------------------------------
 * Configure serial port
 *----------------------------------------------------------------------------------------------------
 */
static int
init_port (HANDLE hPort, int baudrate)
{
    DCB PortDCB;

    // Initialize the DCBlength member.
    PortDCB.DCBlength = sizeof (DCB);

    // Get the default port setting information.
    GetCommState (hPort, &PortDCB);

    // Change the DCB structure settings.
    PortDCB.BaudRate            = baudrate;             // Current baud
    PortDCB.fBinary             = TRUE;                 // Binary mode; no EOF check
    PortDCB.fParity             = TRUE;                 // Enable parity checking
    PortDCB.fOutxCtsFlow        = FALSE;                // No CTS output flow control
    PortDCB.fOutxDsrFlow        = FALSE;                // No DSR output flow control
    PortDCB.fDtrControl         = DTR_CONTROL_ENABLE;   // DTR flow control type
    PortDCB.fDsrSensitivity     = FALSE;                // DSR sensitivity
    PortDCB.fTXContinueOnXoff   = TRUE;                 // XOFF continues Tx
    PortDCB.fOutX               = FALSE;                // No XON/XOFF out flow control
    PortDCB.fInX                = FALSE;                // No XON/XOFF in flow control
    PortDCB.fErrorChar          = FALSE;                // Disable error replacement
    PortDCB.fNull               = FALSE;                // Disable null stripping
    PortDCB.fRtsControl         = RTS_CONTROL_ENABLE;   // RTS flow control
    PortDCB.fAbortOnError       = FALSE;                // Do not abort reads/writes on error
    PortDCB.ByteSize            = 8;                    // Number of bits/byte, 4-8
    PortDCB.Parity              = NOPARITY;             // 0-4=no,odd,even,mark,space
    PortDCB.StopBits            = ONESTOPBIT;           // 1 stop bit
//  PortDCB.StopBits            = ONE5STOPBITS;         // 1.5 stop bits (does not work on some USARTs)
//  PortDCB.StopBits            = TWOSTOPBITS;          // 2 stop bits

    // Configure the port according to the specifications of the DCB
    // structure.
    if (!SetCommState (hPort, &PortDCB))
    {
        DWORD dwError;
        // Could not configure the serial port.
        dwError = GetLastError ();
        MessageBox (NULL, TEXT("Unable to configure the serial port"), TEXT("Error"), MB_OK);
        return (FALSE);
    }
    return (TRUE);
}

/*----------------------------------------------------------------------------------------------------
 * close serial port
 *----------------------------------------------------------------------------------------------------
 */
static int
close_port (HANDLE hPort)
{
    int rtc;

    rtc = CloseHandle (hPort);
    return (rtc);
}

int main(int argc, char ** argv)
{
    HANDLE      hPort;
    wchar_t     comport[256];

    if (argc == 2)
    {
        mbstowcs(comport, argv[1], 256);
        hPort = open_port (comport);

        if (hPort != (HANDLE) ERROR_INVALID_HANDLE)
        {
            if (init_port (hPort, 9600))
            {
                unsigned char sendbuf[256];
                int len;
                int n_written;

                strcpy ((char *) sendbuf, "Hello World\r\n");
                len = strlen ((char *) sendbuf);

                if (WriteFile(hPort, sendbuf, len, &n_written, NULL)
                {
                     printf ("Success!\n");
                }
            }
            close_port (hPort);
        }
    }
    return 0;
}
