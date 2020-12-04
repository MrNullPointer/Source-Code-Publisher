#pragma once
///////////////////////////////////////////////////////////////////////////////////////////////
// DependencyAnalyzer.h:	The function of this class is to find the dependencies			 //
// ver 1.1                                                                                   //  
//Platform:					 MacBook Pro Intel Core i7, Windows10      			             //
// Environment:				 Microsoft Visual Studio 2017                                    //
// Author:					 Parikshit Dubey                                                 //
///////////////////////////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* ======================
* This package provides header file for dependency analysis
* The dependencies bwtween the files in dependency table are analysed
* Analysis of the gathered semi expression is done by Parser class.
* The major role of this analysis is to apply rule to semi expression
* Actions are then performed on the matches found
*
* Required Files:
* ==================
* Parser.h, Parser.cpp,
* ConfigureParser.h, ConfigureParser.cpp,
* Utilities.h, Utilities.cpp
* ActionsAndRules.h, ActionsAndRules.cpp
* AbsSynTree.h, ScopeStack.h
*
* Maintenance History:
* ==========================
* ver 1.1 : 19 Mar 2019
* ver 1.0 : 14 Feb 2019
* - first release
*/

#include <vector>
#include <map>
#include "../CppParser/Parser/ConfigureParser.h"
#include "../CppParser/Parser/Parser.h"
#include "../CppParser/Parser/ActionsAndRules.h"
#include "../CppParser/AbstractSyntaxTree/AbstrSynTree.h"
#include "../CppParser/ScopeStack/ScopeStack.h"
#include "../CppCommWithFileXfer/Utilities/Utilities.h"
#define Util StringHelper

class dependencies
{
public:
	dependencies();
	~dependencies();
	enum TypeInfo { none, classes, function, end, singleLineFn };
	std::map<std::string, std::vector<std::string>> summonParser(std::vector<std::string> files);
	void analyzeDepT(std::map < std::string, std::vector<std::string>> & depT);
	void DepTable(std::vector<std::string> files, CodeAnalysis::ASTNode* pGlobalScope, std::string fileSpec, std::map<std::string, std::vector<std::string>>& dependencyMap);
	void newInfoTable(CodeAnalysis::ASTNode * pGlobalScope, std::string file);
	TypeInfo findTypeInfo(CodeAnalysis::ASTNode* pGlobalScope);
	std::map<std::string, std::map<std::size_t, dependencies::TypeInfo>> returnTypeInfoTable() { return typeInfoTable_; }

private:
	std::map<std::string, std::map<std::size_t, dependencies::TypeInfo>> typeInfoTable_;
};

