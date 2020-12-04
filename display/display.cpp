///////////////////////////////////////////////////////////////////////////////////////////
// display.cpp - class used for display of converted .cpp source code files to HTML      //
// ver 1.1                                                                               //
// Platform:    MacBook Pro Intel Core i7, Windows10                                     //
// Environment: Microsoft Visual Studio 2017                                             //
// Author:      Parikshit Dubey                                                          //
///////////////////////////////////////////////////////////////////////////////////////////

#include "display.h"
#include "../Process/Process.h"

//Default Constructor
display::display()
{
}

//Default Destructor
display::~display()
{
}

// A function which takes an argument of vector <string>and display a valid HTML file using process class
void display::showWebPage(std::vector<std::string> convertedFiles)
{
	std::cout << std::endl << "Code demonstration";
	std::cout << std::endl <<"======================================";

	Process p;
	p.title("Test Application");
	std::string appPath = "C:/Program Files/Mozilla Firefox/firefox.exe";  // path to application to start
	p.application(appPath);

	
	for (auto f:convertedFiles)
	{
		std::string cmdLine = "/A " + f; 
		p.commandLine(cmdLine);

		std::cout << std::endl << " starting process: \"" << appPath << "\"";
		std::cout << std::endl << " with this cmdlne: \"" << cmdLine << "\"";
		p.create();
		CBP callback = []() { std::cout << std::endl << "  <--- child process exited with this message --->"; };
		p.setCallBackProcessing(callback);
		p.registerCallback();

		WaitForSingleObject(p.getProcessHandle(), INFINITE);  // wait for created process to terminate
	}	
}

//Test stub for display package
#ifdef TEST_display

int main()
{
	std::cout << "\n testing Display Package";
	std::vector<std::string> htmlFiles;
	display d;
	htmlFiles.push_back("./display.cpp.html");
	d.showWebPage(htmlFiles);
	std::cout << "Printing converted file names" << "\n";

	return 0;

}

#endif