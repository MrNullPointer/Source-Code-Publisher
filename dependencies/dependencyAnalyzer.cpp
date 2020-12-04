
///////////////////////////////////////////////////////////////////////////////////////////////
// DependencyAnalyzer.cpp:	The function of this module do  perform analysis on files from   //
//							dependency table												 //
// ver 1.1                                                                                   //  
//Platform:					 MacBook Pro Intel Core i7, Windows10      			             //
// Environment:				 Microsoft Visual Studio 2017                                    //
// Author:					 Parikshit Dubey                                                 //
///////////////////////////////////////////////////////////////////////////////////////////////

#include "dependencyAnalyzer.h"

using namespace CodeAnalysis;
using namespace Utilities;

dependencies::dependencies()
{
}


dependencies::~dependencies()
{
}

std::map<std::string, std::vector<std::string>> dependencies::summonParser(std::vector<std::string> fileVec)
{
	std::string fileSpec;
	std::map<std::string, std::vector<std::string>> dependencyMap;

	for (size_t i = 0; i < fileVec.size(); ++i)  // iterate over files
	{
		fileSpec = FileSystem::Path::getFullFileSpec(fileVec[i]);
		ConfigParseForCodeAnal configure;
		Parser* pParser = configure.Build();

		std::string name;

		try
		{
			if (pParser)
			{
				name = FileSystem::Path::getName(fileVec[i]);
				if (!configure.Attach(fileSpec))
				{
					std::cout << "\n  could not open file " << name << std::endl;
					continue;
				}
			}
			else
			{
				std::cout << "\n\n  Parser not built\n\n";
				return std::map<std::string, std::vector<std::string>>();
			}

			// save current package name

			Repository* pRepo = Repository::getInstance();
			pRepo->package() = name;

			// parse the package

			while (pParser->next())
			{
				pParser->parse();
			}
			std::cout << "\n";

			// final AST operations
			ASTNode* pGlobalScope = pRepo->getGlobalScope();

			// walk AST, computing complexity for each node
			// and record in AST node's element

			DepTable(fileVec, pGlobalScope, fileSpec,dependencyMap);
			newInfoTable(pGlobalScope, fileSpec);

			analyzeDepT(dependencyMap);

			complexityEval(pGlobalScope);

		}
		catch (std::exception& ex)
		{
			std::cout << "\n\n    " << ex.what() << "\n\n";
			std::cout << "\n  exception caught at line " << __LINE__ << " ";
			std::cout << "\n  in package \"" << name << "\"";
		}
		std::cout << "\n";
	}
	std::cout << "\n";

	return dependencyMap;
}

//The depT table is analyzed here.
void dependencies::analyzeDepT(std::map<std::string, std::vector<std::string>>& depTmap)
{
	if (depTmap.size() == 0)
		return;
	auto iter = depTmap.begin();
	while (iter != depTmap.end()) {
		if (iter->second.size() != 0) {
			for (size_t i = 0; i < iter->second.size(); i++) {
				auto found = iter->second[i].find_last_of("/");
				auto fquote = iter->second[i].find_last_of("\"");
				if (found == iter->second[i].npos) {
					auto quote1 = iter->second[i].find_first_of("\"");
					iter->second[i] = iter->second[i].substr(quote1 + 1, fquote - quote1 - 1);
				}
				iter->second[i] = iter->second[i].substr(found + 1, fquote - found - 1);
			}
		}
		iter++;
	}
}

//Deptable is created here
void dependencies::DepTable(std::vector<std::string> files, CodeAnalysis::ASTNode * pGlobalScope, std::string fileSpec, std::map<std::string, std::vector<std::string>>& dependencyMap)
{
	auto iter = pGlobalScope->statements_.begin();
	std::vector<std::string> dep;
	while (iter != pGlobalScope->statements_.end()) {
		std::string str = (*iter)->ToString();
		int pos = str.find_first_of("\"");
		int posi = str.find_last_of("\"");
		std::string d = str.substr(pos + 1, posi - pos - 1);
		std::replace(d.begin(), d.end(), '/', '\\');
		std::string temp = FileSystem::Path::getPath(fileSpec);
		std::string filePath = temp + d;
		std::string fullyqualified = FileSystem::Path::getFullFileSpec(filePath);
		auto found = std::find(files.begin(), files.end(), fullyqualified);
		if (found != files.end()) {
			dep.push_back((*iter)->ToString());
			auto iter2 = dependencyMap.find(fileSpec);
			if (iter2 == dependencyMap.end()) {
				dependencyMap.insert(std::pair<std::string, std::vector<std::string>>(fileSpec, dep));
			}
			else {
				iter2->second.push_back((*iter)->ToString());
			}
		}
		iter++;
	}
}

//Infotable is created here
void dependencies::newInfoTable(CodeAnalysis::ASTNode * pGlobalScope, std::string file)
{

	static size_t indentLevel = 0;
	TypeInfo type = findTypeInfo(pGlobalScope);
	std::map<size_t, TypeInfo> temp;
	auto name = file;
	if (pGlobalScope->package_ != "" && pGlobalScope->type_ != "control" && pGlobalScope->type_ != "anonymous") {
		temp.insert(std::pair<size_t, TypeInfo>(pGlobalScope->startLineCount_, type));
		if (pGlobalScope->type_ == "class" || pGlobalScope->type_ == "struct") {
			temp.insert(std::pair<size_t, TypeInfo>(pGlobalScope->endLineCount_ - 1, end));
		}
		else {
			temp.insert(std::pair<size_t, TypeInfo>(pGlobalScope->endLineCount_, end));
		}
		auto find = typeInfoTable_.find(file);
		if (find == typeInfoTable_.end()) {
			typeInfoTable_.insert(std::pair<std::string, std::map<size_t, TypeInfo>>(name, temp));
		}
		else {
			find->second.insert(std::pair<size_t, TypeInfo>(pGlobalScope->startLineCount_, type));
			if (pGlobalScope->type_ == "class" || pGlobalScope->type_ == "struct")
				find->second.insert(std::pair<size_t, TypeInfo>(pGlobalScope->endLineCount_ - 1, end));
			else
				find->second.insert(std::pair<size_t, TypeInfo>(pGlobalScope->endLineCount_, end));
		}
	}
	auto iter = pGlobalScope->children_.begin();
	while (iter != pGlobalScope->children_.end()) {
		newInfoTable(*iter, file);
		++iter;
	}
	--indentLevel;
}

//To check if a given node is class or function
dependencies::TypeInfo dependencies::findTypeInfo(CodeAnalysis::ASTNode * pGlobalScope)
{
	if (pGlobalScope->type_ == "namespace")
		return none;
	if (pGlobalScope->type_ == "class")
		return classes;
	if (pGlobalScope->type_ == "function" && pGlobalScope->endLineCount_ - pGlobalScope->startLineCount_ == 0)
		return singleLineFn;
	if (pGlobalScope->type_ == "function")
		return function;
	if (pGlobalScope->type_ == "interface")
		return function;
	if (pGlobalScope->type_ == "struct")
		return classes;
	return none;
}

// Test Stub for dependencies package
#ifdef TEST_DEPENDENCY

int main()
{
	std::vector<std::string> files;
	files.pushback("../CppParser/FileSystem/FileSystem.h");
	files.pushback("../CppParser/Parser/ConfigureParser.h");
	files.pushback("../CppParser/Parser/Toker.h");
	files.pushback("../CppParser/Parser/Semi.h");
	files.pushback("../CppParser/Parser/Utilities.h");
	files.pushback("../CppParser/Parser/Logger.h");
	files.pushback("../CppParser/Parser/ScopeStack.h");
	Dependency ob;
	ob.summonparser(files);

	return 0;
}

#endif









