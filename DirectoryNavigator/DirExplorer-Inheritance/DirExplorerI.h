#pragma once
/////////////////////////////////////////////////////////////////////
// DirExplorerI.h - Inherited directory explorer                   //
// ver 1.2                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018         //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* DirExplorerI provides a class, of the same name, that executes a
* depth first search of a directory tree rooted at a specified path.
* Each time it enters a directory, it invokes its member function
* doDir, and for every file in that directory, it invokes doFile.
*
* We call this a "Inherited Directory Explorer" because, it carries
* out its assigned responsibilities well, and allows the using
* application to modify what the doDir and doFiles do, by using
* a class, DerivedDirExplorerI, derived from DirExplorerI.
* This is an improvement over DirExplorerN because the application
* does not need to change any part of the DirExplorerI class.
*
* Other projects in this solution do just that, in different ways.

* - DirExplorer-Naive:
*     Implements basic processing well, but applications have to
*     change its code to affect how files and directories are
*     handled.
* - DirExplorer-Template:
*     Applications provide template class parameters to define file
*      and directory processing.
* - DirExplorer-Inheritance:
*     Applications subclass DirExplorerI, overriding virtual functions
*     doFile, doDir, and doQuit to define how files and directories
*     are handled, and whether processing should terminate before
*     visiting all directories.
* - DirExplorer-Events:
*     Applications use Event Interfaces, published by DirExplorerE,
*     by subscribing event-handler functions, and control terminate
*      processing using a plug-in interface.
* - DirExplorer-Provider:
*     Applications implement a file system provider that implements
*     a Provider interface published by DirExplorerP.
*
* We'll be using this solution to illustrate techniques for building
* flexible software.
*
* Required Files:
* ---------------
* DirExplorerI.h, DirExplorerI.cpp  // cpp file illustrates how app can modify processing
* FileSystem.h, FileSystem.cpp      // Directory and Path classes
* StringUtilities.h                 // Title function
* CodeUtilities.h                   // ProcessCmdLine class
*
* Maintenance History:
* --------------------
* ver 1.2 : 19 Aug 2018
* - moved methods to inline functions, below class declaration
* ver 1.1 : 16 Aug 2018
* - made no recursion default, added option /s for recursion
* ver 1.0 : 12 Aug 2018
* - first release
*
*/
#include <vector>
#include "../FileSystem/FileSystem.h"
#include "../Application/Application.h"

namespace FileSystem
{
  class DirExplorerI
  {
  public:
    using patterns = std::vector<std::string>;

    static std::string version() { return "ver1.2"; }

    DirExplorerI(const std::string& path);
    virtual ~DirExplorerI();

    void addPattern(const std::string& patt);
    void hideEmptyDirectories(bool hide);
    void showAllInCurrDir(bool showAllCurrDirFiles);
    bool showAllInCurrDir();
    void maxItems(size_t numFiles);
    void recurse(bool doRecurse = true);

    void search();
    virtual void find(const std::string& path);
    virtual void doFile(const std::string& filename);
    virtual void doDir(const std::string& dirname);
    size_t fileCount();
    size_t dirCount();
    void count(size_t maxItems);
    void showStats();

  protected:
    virtual bool done();

    std::string path_;
    patterns patterns_;
    bool hideEmptyDir_ = false;
    bool showAll_ = false;      // show files in current dir even if maxItems_ has been exceeded
    size_t maxItems_ = 0;
    size_t dirCount_ = 0;
    size_t fileCount_ = 0;
    bool recurse_ = false;
  };

  //----< construct DirExplorerN instance with default pattern >-----

  DirExplorerI::DirExplorerI(const std::string& path) : path_(path)
  {
    std::cout << "\n  constructing DirExplorerI";
    patterns_.push_back("*.*");
  }
  //----< virtual destructor supports substitution >-----------------

  DirExplorerI::~DirExplorerI() {}

  //----< add specified patterns for selecting file names >----------

  void DirExplorerI::addPattern(const std::string& patt)
  {
    if (patterns_.size() == 1 && patterns_[0] == "*.*")
      patterns_.pop_back();
    patterns_.push_back(patt);
  }
  //----< set option to hide empty directories >---------------------

  void DirExplorerI::hideEmptyDirectories(bool hide)
  {
    hideEmptyDir_ = hide;
  }
  //----< stop display before showing all files in dir >-------------

  void DirExplorerI::showAllInCurrDir(bool showAllCurrDirFiles)
  {
    showAll_ = showAllCurrDirFiles;
  }
  //----< stop display before showing all files in dir? >------------

  bool DirExplorerI::showAllInCurrDir()
  {
    return showAll_;
  }
  //----< set the maximum number of files to display >---------------

  void DirExplorerI::maxItems(size_t numFiles)
  {
    maxItems_ = numFiles;
  }
  //----< set option to recusively walk dir tree >-------------------

  void DirExplorerI::recurse(bool doRecurse)
  {
    recurse_ = doRecurse;
  }
  //----< start Depth First Search at path held in path_ >-----------

  void DirExplorerI::search()
  {
    find(path_);
  }
  //----< search for directories and their files >-------------------
  /*
  Recursively finds all dirs and files on the specified path,
  executing doDir when entering a directory and doFile when finding a file.
  */
  void DirExplorerI::find(const std::string& path)
  {
    if (done())  // stop searching
      return;

    bool hasFiles = false;
    std::string fpath = FileSystem::Path::getFullFileSpec(path);
    if (!hideEmptyDir_)
      doDir(fpath);

    for (auto patt : patterns_)
    {
      std::vector<std::string> files = FileSystem::Directory::getFiles(fpath, patt);
      if (!hasFiles && hideEmptyDir_)
      {
        if (files.size() > 0)
        {
          doDir(fpath);
          hasFiles = true;  // show dir only for the first pattern
        }
      }
      for (auto f : files)
      {
        doFile(f);
      }
    }

    if (done())  // stop recursion
      return;

    std::vector<std::string> dirs = FileSystem::Directory::getDirectories(fpath);
    for (auto d : dirs)
    {
      if (d == "." || d == "..")
        continue;
      std::string dpath = fpath + "\\" + d;
      if (recurse_)
      {
        find(dpath);
      }
      else
      {
        doDir(dpath);
      }
    }
  }
  //----< default file processing >----------------------------------

  void DirExplorerI::doFile(const std::string& filename)
  {
    if (!done() || showAllInCurrDir())
    {
      std::cout << "\n  fileI:    " << filename;
      ++fileCount_;
    }
  }
  //----< default dir processing >-----------------------------------

  void DirExplorerI::doDir(const std::string& dirname)
  {
    std::cout << "\n  dirI:  " << dirname;
    ++dirCount_;
  }
  //----< show number of files processed >---------------------------

  size_t DirExplorerI::fileCount()
  {
    return fileCount_;
  }
  //----< show number of directories processed >---------------------

  size_t DirExplorerI::dirCount()
  {
    return dirCount_;
  }
  //----< show final counts for files and dirs >---------------------

  void DirExplorerI::showStats()
  {
    std::cout << "\n\n  processed " << fileCount_ << " files in " << dirCount_ << " directories";
    if (done())
    {
      std::cout << "\n  stopped because max number of files exceeded";
    }
  }
  //----< default condition for stopping navigation >----------------

  bool DirExplorerI::done()
  {
    if (0 < maxItems_ && maxItems_ < fileCount_)
    {
      return true;
    }
    return false;
  }
}