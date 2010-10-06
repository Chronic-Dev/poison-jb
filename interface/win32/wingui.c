#include <windows.h>
#include <wininet.h>
#if defined __MINGW_H
#define _WIN32_IE 0x0400
#endif
#include <commctrl.h>
#include <direct.h>
#include <stdio.h>
#include "../../injector/libpois0n.h"

// Header fixes?
#define PBS_MARQUEE  0x08 
#define PBM_SETMARQUEE WM_USER + 10 


LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
char *szClassName = TEXT("WindowsApp");

BOOL MessageLoop(BOOL blocking) {
    MSG messages;

    if (!(blocking ? 
        GetMessage(&messages, NULL, 0, 0) :
        PeekMessage(&messages, NULL, 0, 0, PM_REMOVE)
    ))
        return FALSE;

    TranslateMessage(&messages);
    DispatchMessage(&messages);

    return TRUE;
}


			
HWND window = NULL;
HWND nButton = NULL, title = NULL, group = NULL, copyright = NULL, progress = NULL, subtitle = NULL;
HWND reset = NULL, seconds = NULL, counter = NULL, first = NULL, second = NULL, third = NULL, fourth = NULL, enter = NULL;


BOOL jbcomplete = FALSE;
int dfutimer = 0;
int dfuphase = 0;
int dfucountdown = 0;

LPCSTR dfutext[] = {
	"Get ready to enter DFU.",
	"Press and hold the sleep button.",
	"Continue holding sleep; press and hold home button.",
	"Release sleep button; continue holding home."
};

#define DFU_PHASE_NONE 0
#define DFU_PHASE_READY 1
#define DFU_PHASE_POWER 2
#define DFU_PHASE_BOTH 3
#define DFU_PHASE_HOME 4

#define DFU_TIMER_ID 1337


void BoldifyLabel(HWND label, BOOL bold) {
	SendMessage(label, WM_SETFONT, (WPARAM) CreateFont(14, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma")), TRUE);
}

void UpdateDFUStatusText() {	
	if (dfuphase != DFU_PHASE_NONE) {
		BoldifyLabel(first, FALSE);
		BoldifyLabel(second, FALSE);
		BoldifyLabel(third, FALSE);
		BoldifyLabel(fourth, FALSE);
	}
	
	if (dfuphase == DFU_PHASE_READY) BoldifyLabel(first, TRUE);
	if (dfuphase == DFU_PHASE_POWER) BoldifyLabel(second, TRUE);
	if (dfuphase == DFU_PHASE_BOTH) BoldifyLabel(third, TRUE);
	if (dfuphase == DFU_PHASE_HOME) BoldifyLabel(fourth, TRUE);
	
	char text[0x10];
	sprintf(text, "%d", dfucountdown);
	
	SendMessage(counter, WM_SETTEXT, 0, (LPARAM)text);
}

void ToggleDFUTimers(BOOL show) {
	if (dfutimer) KillTimer(window, dfutimer);
	dfutimer = 0;
	
	if (show) {
		ShowWindow(first, SW_SHOW);
		ShowWindow(second, SW_SHOW);
		ShowWindow(third, SW_SHOW);
		ShowWindow(fourth, SW_SHOW);
		ShowWindow(counter, SW_HIDE);
		ShowWindow(reset, SW_HIDE);
		ShowWindow(seconds, SW_HIDE);
		ShowWindow(enter, SW_HIDE);
		
		UpdateWindow(window);
		UpdateWindow(group);
		
		dfucountdown = 4;
		dfuphase = DFU_PHASE_READY;
		dfutimer = SetTimer(window, DFU_TIMER_ID, 1000, NULL);
		UpdateDFUStatusText();
	} else { 
		dfuphase = DFU_PHASE_NONE;
		
		ShowWindow(enter, SW_SHOW);
		ShowWindow(first, SW_HIDE);
		ShowWindow(second, SW_HIDE);
		ShowWindow(third, SW_HIDE);
		ShowWindow(fourth, SW_HIDE);
		ShowWindow(counter, SW_HIDE);
		ShowWindow(reset, SW_HIDE);
		ShowWindow(seconds, SW_HIDE);

		InvalidateRect(group, NULL, TRUE);
		InvalidateRect(window, NULL, TRUE);
		UpdateWindow(window);
		UpdateWindow(group);
	}
}

BOOL DeviceInDFU() {
    return pois0n_is_ready() == 0 && pois0n_is_compatible() == 0;
}

BOOL UpdateJailbreakStatus() {
	BOOL dfu = DeviceInDFU();
	
	if (dfu) {
		SendMessage(nButton, WM_SETTEXT, 0, TEXT("Jailbreak!"));
		SendMessage(enter, WM_SETTEXT, 0, TEXT("Ready to Jailbreak"));
		EnableWindow(nButton, TRUE);
		EnableWindow(enter, FALSE);
		
		ToggleDFUTimers(FALSE);
	} else {
		SendMessage(nButton, WM_SETTEXT, 0, TEXT("Waiting for DFU..."));
		EnableWindow(nButton, FALSE);
	}
	
	return dfu;
}

void PerformJailbreak() {
	EnableWindow(progress, TRUE);
	EnableWindow(nButton, FALSE);
	
	SendMessage(nButton, WM_SETTEXT, 0, TEXT("Jailbreaking..."));
	SendMessage(enter, WM_SETTEXT, 0, TEXT("Jailbreaking..."));
	
	pois0n_inject();
	
	/*SendMessage(nButton, WM_SETTEXT, 0, TEXT("Performing exploit..."));
	
	SendMessage(nButton, WM_SETTEXT, 0, TEXT("Uploading data..."));
	SendMessage(progress, PBM_SETPOS, 20, 0);

	SendMessage(nButton, WM_SETTEXT, 0, TEXT("Waiting for reboot..."));
	SendMessage(progress, PBM_SETPOS, 40, 0);

	SendMessage(nButton, WM_SETTEXT, 0, TEXT("Uploading ramdisk..."));
	SendMessage(progress, PBM_SETPOS, 60, 0);

	SendMessage(nButton, WM_SETTEXT, 0, TEXT("Jailbreaking..."));
	SendMessage(progress, PBM_SETPOS, 80, 0);*/
	
	SendMessage(nButton, WM_SETTEXT, 0, TEXT("Jailbreak Complete!"));
	SendMessage(progress, PBM_SETPOS, 100, 0);
	
	EnableWindow(progress, FALSE);
	jbcomplete = TRUE;
	SendMessage(enter, WM_SETTEXT, 0, TEXT("Quit"));
	EnableWindow(enter, TRUE);
}

void ProgressCallback(double percent) {
    SendMessage(progress, PBM_SETPOS, (int) percent, 0);
    
    // Actually, like, show the update.
    MessageLoop(TRUE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{   
    // Register window class
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WindowProcedure;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(GetModuleHandle(NULL), TEXT("ID"));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szClassName;
    wc.hIconSm = (HICON)LoadImage(GetModuleHandle(NULL), TEXT("ID"), IMAGE_ICON, 16, 16, 0);
    if(!RegisterClassEx(&wc)) return 0;
    
    pois0n_init();
    pois0n_set_callback(&ProgressCallback, NULL);

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_PROGRESS_CLASS;
	InitCommonControlsEx(&icex);

    // Main window
    window = CreateWindowEx(0, szClassName, TEXT("greenpoison"), WS_OVERLAPPED | WS_SYSMENU | SS_OWNERDRAW, CW_USEDEFAULT, CW_USEDEFAULT, 480 + GetSystemMetrics(SM_CXFIXEDFRAME), 260 + GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYCAPTION), HWND_DESKTOP, NULL, hInstance, NULL);

	// Jailbreak button
	nButton = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Jailbreak"), BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 20, 205, 138, 25, window, (HMENU) 1, NULL, NULL);
	SendMessage(nButton, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);    
	
	// Progress bar
	progress = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 165, 206, 295, 23, window, NULL, NULL, NULL);
	EnableWindow(progress, FALSE);

	// Title
	title = CreateWindowEx(0, TEXT("STATIC"), TEXT("greenpoison"), WS_VISIBLE | WS_CHILD | SS_CENTER, 111, 2, 257, 44, window, NULL, NULL, NULL);
    SendMessage(title, WM_SETFONT, (WPARAM) CreateFont(/*the*/42/*answer*/, 0, 0, 0, FW_EXTRALIGHT, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma")), TRUE);

	// Subtitle
	subtitle = CreateWindowEx(0, TEXT("STATIC"), TEXT("Please power off your device and connect it to begin."), WS_VISIBLE | WS_CHILD | SS_CENTER, 20, 52, 440, 17, window, NULL, NULL, NULL);
	SendMessage(subtitle, WM_SETFONT, (WPARAM) CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma")), TRUE);
	
	// Copyright warning
	copyright = CreateWindowEx(0, TEXT("STATIC"), TEXT("(c) 2009-2010 chronic dev team (http://chronic-dev.org). Beware the copyright monster!"), WS_VISIBLE | WS_CHILD | SS_CENTER, 20, 236, 440, 13, window, NULL, NULL, NULL);
	SendMessage(copyright, WM_SETFONT, (WPARAM) CreateFont(12, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma")), TRUE);
	
	
	// DFU group box
	group = CreateWindowEx(0, TEXT("BUTTON"), TEXT(""), BS_GROUPBOX | WS_VISIBLE | WS_CHILD, 20, 70, 440, 125, window, NULL, NULL, NULL);

	// Label #1
	first = CreateWindowEx(0, TEXT("STATIC"), dfutext[0], WS_VISIBLE | WS_CHILD | SS_CENTER, 5, 20, 330, 17, group, NULL, NULL, NULL);
	SendMessage(first, WM_SETFONT, (WPARAM) CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma")), TRUE);

	// Label #2
	second = CreateWindowEx(0, TEXT("STATIC"), dfutext[1], WS_VISIBLE | WS_CHILD | SS_CENTER, 5, 45, 330, 17, group, NULL, NULL, NULL);
	SendMessage(second, WM_SETFONT, (WPARAM) CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma")), TRUE);

	// Label #3
	third = CreateWindowEx(0, TEXT("STATIC"), dfutext[2], WS_VISIBLE | WS_CHILD | SS_CENTER, 5, 70, 330, 17, group, NULL, NULL, NULL);
	SendMessage(third, WM_SETFONT, (WPARAM) CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma")), TRUE);

	// Label #4
	fourth = CreateWindowEx(0, TEXT("STATIC"), dfutext[3], WS_VISIBLE | WS_CHILD | SS_CENTER, 5, 95, 330, 17, group, NULL, NULL, NULL);
	SendMessage(fourth, WM_SETFONT, (WPARAM) CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma")), TRUE);

	// Countdown timer
	counter = CreateWindowEx(0, TEXT("STATIC"), TEXT(""), WS_VISIBLE | WS_CHILD | SS_CENTER, 350, 15, 60, 60, group, NULL, NULL, NULL);
    SendMessage(counter, WM_SETFONT, (WPARAM) CreateFont(64, 0, 0, 0, FW_EXTRALIGHT, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma")), TRUE);

	// Seconds label
	seconds = CreateWindowEx(0, TEXT("STATIC"), TEXT("Seconds"), WS_VISIBLE | WS_CHILD | SS_CENTER, 350, 75, 60, 15, group, NULL, NULL, NULL);
	SendMessage(seconds, WM_SETFONT, (WPARAM) CreateFont(12, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma")), TRUE);

	// Reset button
	reset = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Reset"), BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 20 + 350, 70 + 95, 60, 20, window, (HMENU) 2, NULL, NULL);
	SendMessage(reset, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);
	
	// Enter button
	enter = CreateWindowEx(0, TEXT("BUTTON"), TEXT("Prepare to Jailbreak (DFU)"), BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 20 + 136, 70 + 50, 160, 25, window, (HMENU) 3, NULL, NULL);
	SendMessage(enter, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), FALSE);


    // Show the window
    ShowWindow(window, nFunsterStil);

	UpdateJailbreakStatus();
	ToggleDFUTimers(FALSE);
    
    // Run the main runloop.
    while(MessageLoop(TRUE));
    
    pois0n_exit();

    return 0;
}


LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND: {	
	    if (LOWORD(wParam) == 1) {
			PerformJailbreak();
		} else if (LOWORD(wParam) == 2) {
			ToggleDFUTimers(TRUE);
		} else if (LOWORD(wParam) == 3) {
			if (jbcomplete) PostQuitMessage(0);
			else ToggleDFUTimers(TRUE);
		}
        
        break;
    } case WM_DESTROY: {
        PostQuitMessage(0);
            
        break;
	} case WM_TIMER: {
		ShowWindow(counter, SW_SHOW);
		ShowWindow(seconds, SW_SHOW);
		ShowWindow(reset, SW_SHOW);
		
		if (wParam == DFU_TIMER_ID) {
			dfucountdown -= 1;
			
			if (dfucountdown <= 0) {
				if (dfuphase == DFU_PHASE_HOME || dfuphase == DFU_PHASE_NONE) {
					dfuphase = DFU_PHASE_NONE;
					SendMessage(enter, WM_SETTEXT, 0, TEXT("Try Again"));
					ToggleDFUTimers(FALSE);
				} else {
					dfuphase += 1;
					if (dfuphase == DFU_PHASE_POWER) {
						dfucountdown = 2;
					} else if (dfuphase == DFU_PHASE_BOTH) {
						dfucountdown = 10;
					} else if (dfuphase == DFU_PHASE_HOME) {
						dfucountdown = 15;
					}
				}
			}
			
			UpdateDFUStatusText();
			UpdateJailbreakStatus();
		}
	} default: 
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}