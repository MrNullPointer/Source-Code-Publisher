#pragma once
/////////////////////////////////////////////////////////////////////
// DirExplorerN.h - Naive directory explorer                       //
// ver 1.3                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018         //
/////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * DirExplorerN provides a class, of the same name, that executes a
 * depth first search of a directory tree rooted at a specified path.
 * Each time it enters a directory, it invokes its member function
 * doDir, and for every file in that directory, it invokes doFile.
 *
 * We call this a "Naive Directory Explorer" because, while it carries
 * out its assigned responsibilities well, there is no way to change
 * what its doDir and doFile functions do, without changing the class
 * itself.  It would be much better to provide a mechanism to allow
 * a using application to supply this processing without changing
 * the Directory Navigator. 
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
 * DirExplorerN.h, DirExplorerN.cpp
 * FileSystem.h, FileSystem.cpp      // Directory and Path classes
 * StringUtilities.h                 // Title function
 * CodeUtilities.h                   // ProcessCmdLine class
 *
 * Maintenance History:
 * --------------------
 * ver 1.4 : 04th Feb 2019
 * - Added _filesToProcess which contains std::vector<std::string> of files.
 * - Added method filesToProcess() which returns _filesToProcess
 * ver 1.3 : 19 Aug 2018
 * - Removed some options to make this version simple.  Those are
 *   implemented in the more advanced navigators, presented here.
 * ver 1.2 : 17 Aug 2018
 * - Moved method definitions to inlines, below the class declaration.
 * ver 1.1 : 16 Aug 2018
 * - Made no recursion default, added option /s for recursion.
 * ver 1.0 : 11 Aug 2018
 * - first release
 *
*/
#include <vector>
#include <iostream>
#include "../FileSystem/FileSystem.h"

namespace FileSystem
{
  class DirExplorerN
  {
  public:
    using patterns = std::vector<std::string>;

    static std::string version() { return "ver 1.3"; }

    DirExplorerN(const std::string& path);

    // set options for analysis

    void addPattern(const std::string& patt);
    void recurse(bool doRecurse = true);

    // conduct depth first search

    void search();
    void find(const std::string& path);

    // define what happens when a file or dir is encountered

    void doFile(const std::string& filename);
    void doDir(const std::string& dirname);

    // extract traversal statistics 

    size_t fileCount();
    size_t dirCount();
    void showStats();
	const std::vector<std::string>& fileToProcess() const {
		return filesToProcess_;
	}
  
  private:
    std::string path_;
    patterns patterns_;
    size_t dirCount_ = 0;
    size_t fileCount_ = 0;
    bool recurse_ = false;
	std::vector <std::string> filesToProcess_;

	

  };

  //----< construct DirExplorerN instance with default pattern >-----

  DirExplorerN::DirExplorerN(const std::string& path) : path_(path)
  {
    patterns_.push_back("*.*");
  }
  //----< add specified patterns for selecting file names >----------

  void DirExplorerN::addPattern(const std::string& patt)
  {
    if (patterns_.size() == 1 && patterns_[0] == "*.*")
      patterns_.pop_back();
    patterns_.push_back(patt);
  }
  //----< set option to recusively walk dir tree >-------------------

  void DirExplorerN::recurse(bool doRecurse)
  {
    recurse_ = doRecurse;
  }
  //----< start Depth First Search at path held in path_ >-----------

  void DirExplorerN::search()
  {
    find(path_);
  }
  //----< search for directories and their files >-------------------
  /*
    Recursively finds all the dirs and files on the specified path,
    executing doDir when entering a directory and doFile when finding a file
  */
  void DirExplorerN::find(const std::string& path)
  {
    // show current directory

    std::string fpath = FileSystem::Path::getFullFileSpec(path);
    //doDir(fpath);

    for (auto patt : patterns_)
    {
      std::vector<std::string> files = FileSystem::Directory::getFiles(fpath, patt);
      for (auto f : files)
      {
       // doFile(f);  // show each file in current directory
		  filesToProcess_.push_back(fpath + "\\" + f);
      }
    }

    std::vector<std::string> dirs = FileSystem::Directory::getDirectories(fpath);
    for (auto d : dirs)
    {
      if (d == "." || d == "..")
        continue;
      std::string dpath = fpath + "\\" + d;
      if (recurse_)
      {
        find(dpath);   // recurse into subdirectories
      }
      else
      {
       // doDir(dpath);  // show subdirectories
      }
    }
  }
  //----< an application changes to enable specific file ops >-------

  void DirExplorerN::doFile(const std::string& filename)
  {
    ++fileCount_;
    std::cout << "\n  --   " << filename;
  }
  //----< an application changes to enable specific dir ops >--------

  void DirExplorerN::doDir(const std::string& dirname)
  {
    ++dirCount_;
    std::cout << "\n  ++ " << dirname;
  }
  //----< return number of files processed >-------------------------

  size_t DirExplorerN::fileCount()
  {
    return fileCount_;
  }
  //----< return number of directories processed >-------------------

  size_t DirExplorerN::dirCount()
  {
    return dirCount_;
  }
  //----< show final counts for files and dirs >---------------------

  void DirExplorerN::showStats()
  {
    std::cout << "\n\n  processed " << fileCount_ << " files in " << dirCount_ << " directories";
  }
}