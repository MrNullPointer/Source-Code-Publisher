
////////////////////////////////////////////////////////////////////////
// Executive.cpp - class used to as entry point of project, manage	  //
//				   other packages like display and converter.         //                                                                              //
// Platform:    MacBook Pro Intel Core i7, Windows10                  //
// Environment: Microsoft Visual Studio 2017                          //
// Author:      Parikshit Dubey                                       //
////////////////////////////////////////////////////////////////////////

#include "Executive.h"
#include<regex>
#include "SourcePublisher.h"

using namespace CodeUtilities;


using namespace Utilities;
using namespace FileSystem;
using namespace HTML;
using namespace CodeAnalysis;

//default constructor
Executive::Executive()
{
	/*std::cout << "class excecutive created" << std::endl;*/
}

//default destructor
Executive::~Executive()
{
}

//function to display list of files
void Executive::displayFiles(std::vector<std::string> Files) {
	for (auto file : Files)
		std::cout << "\n" << file; 
	std::cout << "\n";
}

//function to display error message incase user enter invalid comand line argument
std::string customUsage()
{
	std::string usage;
	usage += "\n  Command Line: path [/option]* [/pattern]*";
	usage += "\n    path is relative or absolute path where processing begins";
	usage += "\n    [/option]* are one or more options of the form:";
	usage += "\n      /s - walk directory recursively";
	usage += "\n    [pattern]* are one or more pattern strings of the form:";
	usage += "\n      *.h *.cpp *.cs *.txt or *.*";
	usage += "\n";
	return usage;

}



//function to Processing comand line arguments and retrive files using regex
 inline std::vector<std::string> Executive::initialization(int argc, char** argv) {
	ProcessCmdLine pcl(argc, argv);
	pcl.usage(customUsage());

	preface("Command Line: ");
	pcl.showCmdLine();
	pcl.showRegexes();
	std::cout << "\n";

	if (pcl.parseError())
	{
		pcl.usage();
		std::cout << "\n\n";
		_getche();
		return std::vector<std::string>();
	}

	DirExplorerN de(pcl.path());
	for (auto patt : pcl.patterns())
	{
		de.addPattern(patt);
	}

	if (pcl.hasOption('s'))
	{
		de.recurse();
	}

	de.search();

	std::vector<std::string> filesRetreived = de.fileToProcess();
	std::vector<std::string> CppToHtmlFiles;

	//std::cout << "\n Demonstrating Req 5 and Req 6";
	std::cout << "\n List of files before applying regex";
	displayFiles(filesRetreived);
	if (pcl.regexes().size() > 0) {
		for (auto regeX : pcl.regexes()) {
			std::regex s(regeX);
			for (auto file : filesRetreived) {
				auto fileName = Path::getName(file);
				if (regex_match(fileName, s))
					CppToHtmlFiles.push_back(file);
			}
		}
	}
	else {
		CppToHtmlFiles = filesRetreived;
	}

	std::cout << "\n List of files after applying regex";
	displayFiles(CppToHtmlFiles);

	HTML::converter conv;  //converter object
	std::vector<std::string> htmlFiles;  //new vector

	dependencies object;
	std::map<std::string, std::vector<std::string>> depT;
	depT = object.summonParser(CppToHtmlFiles);
	std::map<std::string, std::map<std::size_t, dependencies::TypeInfo>> typeInfo = object.returnTypeInfoTable();

	for (auto file : CppToHtmlFiles) {
		htmlFiles.push_back(conv.cppToHtml(file, depT, typeInfo));
	}

	return htmlFiles;
}







////Test Stub for execution package
#ifdef TEST_EXECUTIVE


int main(int argc, char** argv) {


	dependencies object;

	Executive obj;
	obj.Req3();
	std::vector<std::string> initialization(int argc, char ** argv);
	std::vector<std::string> CppToHtmlFiles;
	CppToHtmlFiles = initialization(argc,argv);
	
	std::cout << "\n List of file after applying regex";

	if (CppToHtmlFiles.size() == 0) {
		std::cout << "\n No files found";
		_getche();
		return -1;
	}
	obj.displayFiles(CppToHtmlFiles);
	std::cout << "\n ";
	std::cout << "Demonstarting requirement 7 & 8\n\n";
	std::cout << "Converter::CppToHtml function converts all .cpp and .h file to .html\n";
	std::cout << "Dependency::summonParser is responsible for parsing the files and storing the dependency in a map\n";
	std::cout << "Converter::DivTagsAdded and Converter::HandlingFunctions are the functions that handle divtags and functions.\n\n";
	HTML::converter conv;  //converter object
	std::vector<std::string> htmlFiles;  //new vector
	
	std::map<std::string, std::vector<std::string>> depT;
	depT = object.summonParser(CppToHtmlFiles);
	std::map<std::string, std::map<std::size_t, dependencies::TypeInfo>> typeInfo = object.returnTypeInfoTable();

	for (auto file : CppToHtmlFiles) {
		htmlFiles.push_back(conv.cppToHtml(file, depT, typeInfo));
	}
	
	obj.displayFiles(CppToHtmlFiles);
	
	//std::string dirPath = "../ConvertedPages";
	std::vector<std::string> convertedFiles = FileSystem::Directory::getFiles();
	std::string x = FileSystem::Directory::getCurrentDirectory();

	std::cout << "\n Opening the files one by one";
	display d;
	d.showWebPage(htmlFiles);

	_getche();
	return 0;
}

#endif