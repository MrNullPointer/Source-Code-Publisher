///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// converter.h - This header file contains initializations of all the functions that will be required for conversion and creates dependency  //
// ver 1.0                                                                                                                                   //										                                                 
// Platform:    MacBook Pro Intel Core i7, Win10                                                                                             //
// Environment: Microsoft Visual Studio 2017                                                                                                 //
// Author:      Parikshit Dubey                                                                                                              //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* ------------------
* Provides a class of name converter that creates files which will be converted to HTML.
* The converted files include: The HTML tags.
* Vector of strings are then created as a container for the files being converted.
* This package contains two public member functions 'convert_html' and 'replace_char'
* The source codes are prepended with "<pre>" and postpended with "</pre>".
* The special characters '<' and '>' are being replaced with ther equivalent html notations
* Dependency Table is accessed by code convertor which creates webpages
* The webpages are linked to each other and points at their respective dependencies
*
* Required files:
* --------------
* Converter.h, Converter.cpp
* DependencyTable.h, DependencyTable.cpp
* LoggerP2.h, LoggerP2.cpp
* FileSystem.h, FileSystem.cpp
*
* Public Interface:
* ----------------
* Creates an object of the converter class which converts cpp files to valid HTML files
*
* New added functions are:
* HandlingComments, dependenciesFetch, SwitchFn
*
* Build commands:
* --------------
* devenv SourceCodePublisherS2019.sln
*
* Maintenance History:
* -------------------
* ver 1.1 : 19 Mar 2019
* ver 1.0 : 14 Feb 2019
* - first release
*
*/

#pragma once
#include<vector>
#include<iostream>
#include "../DirectoryNavigator/FileSystem/FileSystem.h"
#include "../display/display.h"
#include <fstream>
#include<string>
#include <algorithm>
#include <map>
#include"../dependencies/dependencyAnalyzer.h"

using TypeInfoT = std::map<std::string, std::map<std::size_t, dependencies::TypeInfo>>;
namespace HTML
{
	class converter
	{
	public:

		converter();
		~converter();

		std::string cppToHtml(std::string filesToProcess_, std::map<std::string, std::vector<std::string>> depT, const TypeInfoT &typeTable);
		void dependenciesFetch(std::string &filesToProcess, const std::map<std::string, std::vector<std::string>> &depTable, std::ofstream &webpageOutput);
		void HandlingComments(std::vector<std::string> &filedata, const TypeInfoT &typeTable);
		void DivTagsAdded(std::string &file, std::vector<std::string> &filedata, const TypeInfoT &typeTable);
		void SwitchFn(size_t &i, std::vector<std::string> &filedata, std::map<std::size_t, dependencies::TypeInfo>::const_iterator &iter2, size_t lineNo);
		void HandlingClasses(size_t &i, std::vector<std::string> &filedata, size_t &lineNo);
		void HandlingFunctions(size_t &i, std::vector<std::string> &filedata, size_t &lineNo);
		void HandlingSingleLnFOpen(size_t &i, std::vector<std::string> &filedata, size_t &lineNo);
		void HandlingSingleLnFClose(size_t &i, std::vector<std::string> &filedata, size_t &lineNo);
	};
}

