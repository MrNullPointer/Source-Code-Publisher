/////////////////////////////////////////////////////////////////////////////////////////////////
// Executive.h - contains function declaration of requirements and demos                       //
// ver 1.1                                                                                     //										                                                 
// Platform:    MacBook Pro Intel Core i7, Win10                                               //
// Environment: Microsoft Visual Studio 2017                                                   //
// Author:      Parikshit Dubey                                                                //
/////////////////////////////////////////////////////////////////////////////////////////////////
/*
*
* Package Operations:
* ------------------
* Linking of converter, display, directory navigator, utilities and process packages is carried out by this package
* This module works as the main invocation point of all the other modules where other modules are called
* An object of convert is created and executive package calls it, in this the transfer of files in vector takes place
* Regular expressions are used to find and match the files and convert them
* Regex command line argument is passed, the command line argument with regex expression detects the file names with .cpp and .h extension
* Each package demonstration is done separately
* The web pages converted are stored in ConvertedWebpages folder and by the use of display package contents of directory are displayed on browser
*
* Required files:
* --------------
* - CodeUtilities.h
* - Converter.h, Converter.cpp
* - DirExplorerN.h, DirExplorerN.cpp
* - Display.h, Display.cpp
* - Utilities.h
* - ConfigureParser.h
* - Parser.h
* - ActionsAndRules.h
* - AbsSynTree.h
* - ScopeStack.h
* - DependencyAnalyzer.h
* - SourcePublisher.h
*
* Build commands:
* --------------
* devenv SourCodePublisherS2019.sln
*
* Maintenance History:
* ------------------
* ver 1.1 : 20th Mar 2019
* ver 1.0 : 14th Feb 2019
* - first release
*/

#pragma once
#include "../Converter/Converter.h"
#include "../display/display.h"
#include "../DirectoryNavigator/Utilities/CodeUtilities/CodeUtilities.h"
#include "../DirectoryNavigator/DirExplorer-Naive/DirExplorerN.h"
#include "../CppCommWithFileXfer/Utilities/Utilities.h"
#include "../CppParser/Parser/ConfigureParser.h"
#include "../CppParser/Parser/Parser.h"
#include "../CppParser/Parser/ActionsAndRules.h"
#include "../CppParser/AbstractSyntaxTree/AbstrSynTree.h"
#include "../CppParser/ScopeStack/ScopeStack.h"
#include "../dependencies/dependencyAnalyzer.h"
#include "SourcePublisher.h"
#include<conio.h>
#include<vector>
#define Util StringHelper


class Executive : public SourcePublisher
{
public:
	Executive();
	~Executive();
	void Req3();
	std::vector<std::string> initialization(int argc, char ** argv) override;
	void displayFiles(std::vector<std::string> Files);
	//int publish(int argc, char** argv);
};

SourcePublisher* factobject::ClientCreation()
{
	return new Executive;
}