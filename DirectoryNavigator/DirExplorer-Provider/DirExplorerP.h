#pragma once
/////////////////////////////////////////////////////////////////////
// DirExplorerP.h - Provider-based directory explorer              //
// ver 1.3                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018         //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* DirExplorerP provides a class, of the same name, that executes a
* depth first search of a directory tree rooted at a specified path.
*
* We call this a "Provider-based Directory Explorer" because, it carries
* out its assigned responsibilities using the facilities of the DFS
* and Provider packages.  
*
* The Provider is application specific, deciding what to do when a
* file or a directory is encountered in DFS.  
*
* DFS controls how the search is executed.  All that the DirExplorerP
* package needs to do is to extract parameters from the command line
* and, using them, start DFS.

* This is an improvement over DirExplorerN because the application
* does not need to change any part of the DirExplorerP or DFS classes.
*
* Other projects in this solution do just that, in different ways.

* - DirExplorer-Naive:
*     Implements basic processing well, but applications have to
*     change its code to affect how files and directories are
*     handled.
* - DirExplorer-Template:
*     Applications provide template class parameters to define file
*     and directory processing.
* - DirExplorer-Inheritance:
*     Applications subclass DirExplorerI, overriding virtual functions
*     doFile, doDir, and doQuit to define how files and directories
*     are handled, and whether processing should terminate before
*     visiting all directories.
* - DirExplorer-Events:
*     Applications use Event Interfaces, published by DirExplorerE,
*     by subscribing event-handler functions, and control terminate
*     processing using a plug-in interface.
* - DirExplorer-Provider:
*     Applications build a file system provider that implements
*     an IProvider interface published by DirExplorerP.
*
* We'll be using this solution to illustrate techniques for building
* flexible software.
*
* Required Files:
* ---------------
* DirExplorerP.h, DirExplorerP.cpp
* Provider.h
* DFS.h
* FileSystem.h, FileSystem.cpp      // Directory and Path classes
* StringUtilities.h                 // Title function
* CodeUtilities.h                   // ProcessCmdLine class
*
* Maintenance History:
* --------------------
* ver 1.3 : 18 Aug 2018
* - moved methods to inlines below class declaration
* - eliminated some unnecessary state in DirExplorerP
* ver 1.2 : 16 Aug 2018
* - fixed bug reporting final status
* ver 1.1 : 16 Aug 2018
* - made no recursion default, added option /s for recursion
* ver 1.0 : 15 Aug 2018
* - first release
*
*/
#include <vector>
#include <memory>
#include "../FileSystem/FileSystem.h"
#include "Provider.h"
#include "DFS.h"

namespace FileSystem
{
  class DirExplorerP
  {
  public:
    using Patterns = std::vector<std::string>;

    static std::string version() { return "ver1.3"; }

    DirExplorerP(const std::string& path);

    // configure processing

    void addPattern(const std::string& patt);
    void hideEmptyDirectories(bool hide);
    void maxItems(size_t numFiles);
    void showAllInCurrDir(bool showAllCurrDirFiles);
    void recurse(bool doRecurse = true);
    
    void search();     // navigate using DFS instance,
                       // which uses Provider
    void showStats(); 

  private:
    // use dynamic_cast to access DirProvider's extended interface

    std::shared_ptr<Provider::DirProvider> dirProvider(Provider::IProvider::sPtrProv pProv)
    {
      std::shared_ptr<Provider::DirProvider> pDirProv 
        = std::dynamic_pointer_cast<Provider::DirProvider>(pProv);
      return pDirProv;
    }
    // Provider contains all knowledge about the file system

    Provider::IProvider::sPtrProv pProvider_;
    
    std::string path_;       // pass to Provider
    size_t maxItems_ = 0;    // pass to temp DFS instance
    bool recurse_ = false;   // pass to temp DFS instance
  };

  //----< construct DirExplorerN instance with default pattern >-----

  DirExplorerP::DirExplorerP(const std::string& path) 
    : path_(path), pProvider_(new Provider::DirProvider(path)) { }

  //----< add specified patterns for selecting file names >----------

  void DirExplorerP::addPattern(const std::string& patt)
  {
    auto pDirProvider = dirProvider(pProvider_);
    if (pDirProvider)
    {
      Patterns& provPatterns = pDirProvider->patterns();
      if (provPatterns.size() == 1 && provPatterns[0] == "*.*")
        provPatterns.pop_back();
      provPatterns.push_back(patt);
    }
  }
  //----< set option to hide empty directories >---------------------

  void DirExplorerP::hideEmptyDirectories(bool hide)
  {
    //hideEmptyDir_ = hide;
    auto pDirProvider = dirProvider(pProvider_);
    if (pDirProvider)
      pDirProvider->hideEmptyDirectories(hide);
  }
  //----< set the maximum number of files to display >---------------

  void DirExplorerP::maxItems(size_t numFiles)
  {
    //maxItems_ = numFiles;
    auto pDirProvider = dirProvider(pProvider_);
    if (pDirProvider)
      pDirProvider->maxItems(numFiles);
  }
  //----< stop display before showing all files in dir >-------------

  void DirExplorerP::showAllInCurrDir(bool showAllCurrDirFiles)
  {
    //showAll_ = showAllCurrDirFiles;
    auto pDirProvider = dirProvider(pProvider_);
    if (pDirProvider)
      pDirProvider->showAllInCurrDir(showAllCurrDirFiles);
  }
  //----< stop display before showing all files in dir? >------------

  //bool DirExplorerP::showAllInCurrDir()
  //{
  //  auto pDirProvider = dirProvider(pProvider_);
  //  if (pDirProvider)
  //    showAll_ = pDirProvider->showAllInCurrDir();
  //  return showAll_;
  //}
  //----< set option to recusively walk dir tree >-------------------

  void DirExplorerP::recurse(bool doRecurse)
  {
    recurse_ = doRecurse;
  }
  //----< start Depth First Search at path held in path_ >-----------

  void DirExplorerP::search()
  {
    pProvider_->path(path_);
    Search::DFS<Provider::IProvider> dfs(pProvider_);
    dfs.setMaxItems(maxItems_);
    dfs.recurse(recurse_);
    dfs.search();
  }
  //----< show final counts for files and dirs >---------------------

  void DirExplorerP::showStats()
  {
    size_t fileCount = pProvider_->fileCount();
    size_t dirCount = pProvider_->dirCount();
    std::cout << "\n\n  processed " << fileCount << " files in " << dirCount << " directories";
    if (pProvider_->done())
    {
      std::cout << "\n  stopped because max number of files exceeded";
    }
  }
}