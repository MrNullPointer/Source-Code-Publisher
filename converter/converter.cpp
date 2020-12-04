//////////////////////////////////////////////////////////////////////////////////////
// converter.cpp - class used for conversion of .cpp source code files to HTML      //
// ver 1.1                                                                          //
// Platform:    MacBook Pro Intel Core i7, Windows10                                //
// Environment: Microsoft Visual Studio 2017                                        //
// Author:      Parikshit Dubey                                                     //
//////////////////////////////////////////////////////////////////////////////////////

#include "converter.h"

namespace HTML {

	//default constructor
	converter::converter()
	{
	}

	//default destructor
	converter::~converter()
	{
	}

	// A function that converts cpp file to valid HTML file
	std::string converter::cppToHtml(std::string filesToProcess_,std::map<std::string, std::vector<std::string>> depT, const TypeInfoT &typeTable)
	{
		std::string fileName = FileSystem::Path::getName(filesToProcess_);
		std::ifstream in(filesToProcess_);
		std::vector<std::string> fileContent;

		auto checkIter = depT.find(fileName);

		for (std::string line; std::getline(in, line);) {

			// Special characters are replaced
			while (line.find("<") != -1)
				line.replace(line.find("<"), 1, "&lt;");
			while (line.find(">") != -1)
				line.replace(line.find(">"), 1, "&gt;");
			fileContent.push_back(line);
		}

		HandlingComments(fileContent, typeTable);
		DivTagsAdded(filesToProcess_, fileContent, typeTable);

		std::string pathDir = "..\\ConvertedPages";
		std::string webPath = pathDir + "//" + fileName + ".html";
		std::ofstream OutputWebpage(webPath);

		OutputWebpage << "<!Document html>\n";
		OutputWebpage << "<html>\n";
		OutputWebpage << "<head>\n";
		OutputWebpage << "<link rel = \"stylesheet\" href = \"style.css\">\n";
		OutputWebpage << "<script src=\"showHideClass.js\"></script>\n";
		OutputWebpage << "</head>\n";
		OutputWebpage << "<body>\n";
		OutputWebpage << "<button onclick = \"showHideClass(\'classes\')\">Show or Hide Class Contents</button>\n";
		OutputWebpage << "<button onclick = \"showHideClass(\'functions\')\">Show or Hide Function Contents</button>\n";
		OutputWebpage << "<button onclick = \"showHideClass(\'comments\')\">Show or Hide Comments</button><br>";
		dependenciesFetch(filesToProcess_, depT, OutputWebpage);
		OutputWebpage << "<pre>\n";
		for (auto content : fileContent)
			OutputWebpage << content << "\n";
		OutputWebpage << "</pre>\n";
		OutputWebpage << "</body\n>";
		OutputWebpage << "</html\n>";

		OutputWebpage.close();
		return webPath;
	}
	
	// Dependencies of each file is received
	void converter::dependenciesFetch(std::string & filesToProcess, const std::map<std::string, std::vector<std::string>>& depTable, std::ofstream & webpageOutput)
	{
		auto iter = depTable.find(filesToProcess);
		if (iter == depTable.end()) {
			webpageOutput << "This file does not have any dependency";
			return;
		}
		else {
			auto iter2 = iter->second;
			if (iter2.size() == 0)
				webpageOutput << "This file does not have any dependency";
			else {
				for (size_t i = 0; i < iter2.size(); i++) {
					webpageOutput << "<a href=\"" + iter2[i] + ".html \">" + iter2[i] + "</a>" + "&nbsp&nbsp&nbsp&nbsp";
				}
			}
		}
	}
	
	// Comments are hidden and shown using this function
	void converter::HandlingComments(std::vector<std::string>& filedata, const TypeInfoT & typeTable)
	{

		bool isComment = false;
		size_t found;
		for (size_t i = 0; i < filedata.size(); i++) {
			found = filedata[i].find("//");
			if (found != filedata[i].npos) {
				filedata[i].replace(found, 2, "<div class=\"comments\">//");
				filedata[i].replace(filedata[i].size(), filedata[i].size(), "</div>");
			}
			found = filedata[i].find("/*");
			if (found != filedata[i].npos) {
				isComment = true;
				filedata[i].replace(found, 2, "<div class=\"comments\">/*");
			}
			if (isComment) {
				size_t muLineC = filedata[i].find("*/");
				if (muLineC != filedata[i].npos) {
					filedata[i].replace(muLineC, 2, "*/</div>");
					isComment = false;
				}
			}
		}
	}

	// DivTags implementation is done in this function
	void converter::DivTagsAdded(std::string & file, std::vector<std::string>& filedata, const TypeInfoT & typeTable)
	{
		auto iter = typeTable.find(file);
		if (iter != typeTable.end()) {
			auto iter2 = iter->second.begin();
			size_t i;
			while (iter2 != iter->second.end())
			{
				size_t lineNo = iter2->first;
				SwitchFn(i, filedata, iter2, lineNo);
				iter2++;
			}
		}
	}
	

	// Function  that takes care of the switch functionality
	void converter::SwitchFn(size_t & i, std::vector<std::string>& filedata, std::map<std::size_t, dependencies::TypeInfo>::const_iterator & iter2, size_t lineNo)
	{
		switch (iter2->second) {
		case dependencies::TypeInfo::classes:
			HandlingClasses(i, filedata, lineNo);
			break;
		case dependencies::TypeInfo::function:
			HandlingFunctions(i, filedata, lineNo);
			break;
		case dependencies::TypeInfo::singleLineFn:
			HandlingSingleLnFOpen(i, filedata, lineNo);
			HandlingSingleLnFClose(i, filedata, lineNo);
			break;
		case dependencies::TypeInfo::end:
			i = filedata[lineNo - 1].find("\t};");
			if (i != filedata[lineNo - 1].npos) {
				filedata[lineNo - 1].replace(i, 3, "\t};</div>"); break;
			}
			i = filedata[lineNo - 1].find("};");
			if (i != filedata[lineNo - 1].npos) {
				filedata[lineNo - 1].replace(i, 2, "};</div>"); break;
			}
			i = filedata[lineNo - 1].find("\t}");
			if (i != filedata[lineNo - 1].npos) {
				filedata[lineNo - 1].replace(i, 2, "\t}</div>"); break;
			}
			i = filedata[lineNo - 1].find("}");
			if (i != filedata[lineNo - 1].npos) {
				filedata[lineNo - 1].replace(i, 1, "}</div>"); break;
			}
			i = filedata[lineNo - 1].find("\t*/");
			if (i != filedata[lineNo - 1].npos) {
				filedata[lineNo - 1].replace(i, 3, "\t*/</div>"); break;
			}
			i = filedata[lineNo - 1].find("*/");
			if (i != filedata[lineNo - 1].npos) {
				filedata[lineNo - 1].replace(i, 2, "*/</div>"); break;
			}break;
		default: break;
		}
	}

	// Function that does the process of showing and hiding of classes
	void converter::HandlingClasses(size_t & i, std::vector<std::string>& filedata, size_t & lineNo)
	{
		i = filedata[lineNo - 1].find("\t{");
		if (i != filedata[lineNo - 1].npos) {
			filedata[lineNo - 1].replace(i, 2, "<div class=\"classes\">\t{");
		}
		else {
			i = filedata[lineNo - 1].find("{");
			if (i != filedata[lineNo - 1].npos)
				filedata[lineNo - 1].replace(i, 1, "<div class=\"classes\">{");
		}
	}
	
	// In this function, functions are shown or hidden
	void converter::HandlingFunctions(size_t & i, std::vector<std::string>& filedata, size_t & lineNo)
	{
		i = filedata[lineNo - 1].find("\t{");
		if (i != filedata[lineNo - 1].npos) {
			filedata[lineNo - 1].replace(i, 2, "<div class=\"functions\">\t{");
		}
		else {
			i = filedata[lineNo - 1].find("{");
			if (i != filedata[lineNo - 1].npos)
				filedata[lineNo - 1].replace(i, 1, "<div class=\"functions\">{");
		}
	}
	
	// In this function, single line functions are taken care of

	void converter::HandlingSingleLnFOpen(size_t & i, std::vector<std::string>& filedata, size_t & lineNo)
	{

		i = filedata[lineNo - 1].find("\t{");
		if (i != filedata[lineNo - 1].npos) {
			filedata[lineNo - 1].replace(i, 2, "<div class=\"functions\">\t{");
		}
		else {
			i = filedata[lineNo - 1].find("{");
			if (i != filedata[lineNo - 1].npos) {
				filedata[lineNo - 1].replace(i, 1, "<div class=\"functions\">{");
			}
		}
	}
// This function handles single line function close
	void converter::HandlingSingleLnFClose(size_t & i, std::vector<std::string>& filedata, size_t & lineNo)
	{
		i = filedata[lineNo - 1].find("\t}");
		if (i != filedata[lineNo - 1].npos) {
			filedata[lineNo - 1].replace(i, 2, "\t}</div>");
		}
		else {
			i = filedata[lineNo - 1].find("}");
			if (i != filedata[lineNo - 1].npos) {
				filedata[lineNo - 1].replace(i, 1, "}</div>");
			}
		}
	}

	

	
}

//Test Stub for converter package
#ifdef TEST_converter

int main()
{
	std::cout << "\tTesting independent package implementation from test stub." << "\n";
	std::cout << "\t**********************************************************" << "\n\n\n";
	std::cout << "converter package is dependent on Dependencies package and Dependencies package is dependent on parser." << "\n";
	std::cout << "Thus it is not possible to demonstrate converter as an independent Package" << "\n";
}

#endif