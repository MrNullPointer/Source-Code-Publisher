#pragma once
///////////////////////////////////////////////////////////////////////////
// Display.h     : defines webpage display using browser functions       //
// ver 1.0                                                               //                         
// Platform      : MacBook Pro Intel Core i7, Win10                      //
// Author        : Parikshit Dubey                                       //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package defines Display class which accepts a list of files as a
*  vector<string> and uses the default internet browser to display them
*  one by one. Please note that the functionality has limiations:
*   1) Opera/MS Edge: will pop-up all tabs instantly.
*   2) Chrome/Firefox: will pop-up windows separately only if no
*      already existing Chrome/Firefox window is opened (all must be
*      closed before running this).
*
*  Required Files:
* =======================
*  Display.h Display.cpp Process.h Process.cpp
*  Logger.h Logger.cpp FileSystem.h FileSystem.cpp
*
*  Maintainence History:
* =======================
*  ver 1.0 - 14 Feb 2019
*  - first release
*/

#include <vector>
#include <string>

class display
{
public:
	display();
	~display();
	//a function to display valid HTMl files using process class in the web browser
	void showWebPage(std::vector<std::string> convertedFiles);
};

