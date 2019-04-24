#pragma once
/* This header file defines functions that are meant to look like
 * they belong to the freeglut API. In reality, they are simply
 * helper functions for the win32 API, spelled out as if they
 * were for freeGLUT. This can be very helpful if don't want to
 * bother learning win32 syntax and just want to get working
 * with freeglut. 
 * For obvious reasons, this is only supported on Windows. */
#ifndef __FREEGLUT32API_h__
#define __FREEGLUT32API_h__

#include <Windows.h>
#include <cstdlib>
#include <GL/glut.h>

namespace glut32 {
	//Converts a c-style string to a long-pointer-character-wide-string object, making a 
	//NEW one on the heap. Be sure to delete it when finished to avoid memory leaks.
	wchar_t* makeLPCWSTR(const char*& text) {
		// Based on the code from this stack overflow response
		//https://stackoverflow.com/questions/29847036/convert-char-to-wchar-t-using-mbstowcs-s
		std::size_t size = strlen(text) + 1;
		wchar_t* wideText = new wchar_t[size];
		//This argument isn't presently returned, but it's still here.
		std::size_t outSizeArg;
		mbstowcs_s(&outSizeArg, wideText, size, text, size - 1);
		return wideText;
	}

	//Get a handle to the top-level GLUT window with a particular name
	HWND getWindowHandle(const char* windowTitle) {
		//Convert string type
		wchar_t* windt = makeLPCWSTR(windowTitle);
		//Get the window handle
		HWND reth = FindWindow(NULL, windt);
		//Delete the new string
		delete windt;
		//Return the handle
		return reth;
	}

	//Provide the program name as the second argument (this should be argc[0])
	void setWindowIcon(const char* windowname, const char* execname, int iconID) {
		/* Thanks a ton to the following askers/answerers:
		 *	https://stackoverflow.com/questions/12748103/how-to-change-freeglut-main-window-icon-in-c
		 *	https://stackoverflow.com/questions/18064943/console-window-has-icon-but-opengl-glut-window-dont-have-why
		 */
		 //Get the window handler
		wchar_t* windt = makeLPCWSTR(windowname);
		HWND hwnd = FindWindow(NULL, windt);
		delete windt;
		//Get the height and width of an icon for this system
		SIZE smallIconSize = { GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON) };
		wchar_t* exect = makeLPCWSTR(execname);
		//Create the icon handle
		HANDLE icon = LoadImage(GetModuleHandle(exect), MAKEINTRESOURCE(iconID), IMAGE_ICON, 64, 64, LR_COLOR);
		delete exect;
		SendMessage(hwnd, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
	}


	//Maximizes a window by being passed its name
	void maximizeWindow(const char* windowName) {
		ShowWindow(getWindowHandle(windowName), SW_SHOWMAXIMIZED);
	}
}


#endif
