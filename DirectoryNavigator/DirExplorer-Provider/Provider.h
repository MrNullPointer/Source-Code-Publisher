#pragma once
/////////////////////////////////////////////////////////////////////
// Provider.h - Defines IProvider, FileProvider, and DirProvider   //
// ver 1.1                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018         //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package defines three classes:
*  - IProvider:
*    Abstract base for all providers, uses static member data to 
*    communicate between concrete instances.
*  - FileProvider:
*    Derives from IProvider, and wraps files to give them application
*    specific operations.
*  - DirProvider:
*    Derives from IProvider, and wraps directories to give them
*    application specific operations.
*  The way it is currently designed, every file and every directory
*  is wrapped in its own instance of a provider.  That's why static
*  data communication is needed.
*
*  This sharing of data between many providers may save a lot of memory
*  and makes communication much easier.
*
*  Required Files:
*  ---------------
*  Provider.h
*  FileSystem.h, FileSystem.cpp
*
*  Maintenance History:
*  --------------------
*  ver 1/2 : 19 Aug 2018
*  - minor changes to comments
*  ver 1.1 : 16 Aug 2018
*  - helped fix a memory leak in DFS by making small changes to the
*    way std::shared_ptrs are used.
*  - added these comments
*  Ver 1.0 : 8/12/2018
*  - first release
*
*/
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include "../FileSystem/FileSystem.h"

namespace Provider
{
  bool verbose = false;  // display creation and destruction

  /////////////////////////////////////////////////////////////////////
  // IProvider is an interface for this Provider family

  class IProvider
  {
  public:
    using sPtrProv = std::shared_ptr<IProvider>;
    using Children = std::vector<sPtrProv>;

    virtual ~IProvider() {}
    virtual void path(const std::string& path) = 0;
    virtual void doOperation(bool) = 0;
    virtual Children* children() = 0;
    virtual size_t fileCount() { return fileCount_; }
    virtual size_t dirCount() { return dirCount_; }
    virtual bool done()
    {
      return (0 < maxItems_ && maxItems_ <= fileCount_);
    }
  protected:
    static size_t fileCount_;
    static size_t dirCount_;
    static size_t maxItems_;
    static bool showAll_;    // show all files in directory before stopping
  };

  // Definition of static data
  size_t IProvider::fileCount_ = 0;
  size_t IProvider::dirCount_ = 0;
  size_t IProvider::maxItems_ = 0;
  bool IProvider::showAll_ = false;

  /////////////////////////////////////////////////////////////////////
  // FileProvider class defines provider representation for files
  // - instance represents a single file
  // - doOperation defines behavior of this provider for files

  class FileProvider : public IProvider
  {
  public:
    //----< save filename >--------------------------------------------

    FileProvider(const std::string& filename) : filename_(filename)
    {
      if(verbose)
        std::cout << "\n-- FileProvider created";
    }

    //----< defined only for demonstrations >--------------------------

    ~FileProvider()
    {
      if(verbose)
        std::cout << "\n-- FileProvider destroyed";
    }
    //----< defined to satisfy Interface >-----------------------------

    void path(const std::string& path) {}

    //----< define behavior of file providers >------------------------

    void doOperation(bool)
    {
      ++fileCount_;
      std::cout << "\n  file-->    " << filename_;
    }
    //----< defines required method by doing nothing >-----------------

    IProvider::Children* children()
    {
      return nullptr;
    }
  private:
    std::string filename_ = "";
  };

  /////////////////////////////////////////////////////////////////////
  // DirProvider class defines representation for directories
  // - instance represents a single directory
  // - doOperation defines the behavior of this provider

  class DirProvider : public IProvider
  {
  public:
    using File = std::string;
    using Files = std::vector<File>;
    using Dir = std::string;
    using Dirs = std::vector<Dir>;
    using Child = std::shared_ptr<IProvider>;
    using Pattern = std::string;
    using Patterns = std::vector<Pattern>;

    //----< save dir name and define default pattern >-----------------

    DirProvider(const Dir& dirname = "")
    {
      if (verbose)
        std::cout << "\n-- DirProvider created";
      dirname_ = FileSystem::Path::getFullFileSpec(dirname);
      if (patterns_.size() == 0)
        patterns_.push_back("*.*");
    }
    //----< defined only for demonstrations >--------------------------

    ~DirProvider()
    {
      if(verbose)
        std::cout << "\n-- DirProvider destroyed";
    }
    //----< supply stored root path >----------------------------------

    void path(const Dir& dirname)
    {
      dirname_ = FileSystem::Path::getFullFileSpec(dirname);
    }
    //----< adds pattern used for file searching >---------------------

    void addPattern(const Pattern& pattern)
    {
      if (patterns_.size() == 1 && patterns_[0] == "*.*")
        patterns_.pop_back();
      patterns_.push_back(pattern);
    }
    //----< get stored patterns >--------------------------------------

    Patterns& patterns()
    {
      return patterns_;
    }
    //----< set option to hide empty directories >---------------------

    void hideEmptyDirectories(bool hide)
    {
      hideEmptyDirectories_ = hide;
    }
    //----< set the maximum number of files to display >---------------

    void maxItems(size_t numFiles)
    {
      maxItems_ = numFiles;
    }
    //----< stop display before showing all files in dir >-------------

    void showAllInCurrDir(bool showAllCurrDirFiles)
    {
      showAll_ = showAllCurrDirFiles;
    }
    //----< stop display before showing all files in dir? >------------

    bool showAllInCurrDir()
    {
      return showAll_;
    }
    //----< stores all files and subdirectories of this directory >----

    void getDirContents()
    {
      files();
      dirs();
    }
    //----< finds and stores names of all files in this directory >----

    void files()
    {
      for (auto patt : patterns_)
      {
        std::vector<File> files = FileSystem::Directory::getFiles(dirname_, patt);
        for (auto f : files)
        {
          fileprovs_.push_back(std::shared_ptr<IProvider>(new FileProvider(f)));
        }
      }
    }
    //----< finds and stores the names of all subdirectories >---------

    void dirs()
    {
      std::vector<Dir> dirs = FileSystem::Directory::getDirectories(dirname_);
      for (auto d : dirs)
      {
        if (d != ".." && d != ".")
        {
          std::string fullname = dirname_ + "\\" + d;
          dirprovs_.push_back(std::shared_ptr<IProvider>(new DirProvider(fullname)));
        }
      }
    }
    //----< displays dir name and invokes file doOperation >-----------

    void doOperation(bool dofiles)
    {
      ++dirCount_;
      getDirContents();
      if (!hideEmptyDirectories_ || numberFiles() > 0)
        std::cout << "\n  dir--->  " << dirname_;
      if (dofiles)
      {
        for (auto fp : fileprovs_)
        {
          if (showAll_ || !done())
            fp->doOperation(true);
        }
      }
    }
    //----< returns provider representations of all subdirectories >---

    IProvider::Children* children()
    {
      return &dirprovs_;
    }
    //----< returns number of files in this directory >----------------

    size_t numberFiles()
    {
      return fileprovs_.size();
    }
  private:
    std::string dirname_ = "";
    std::vector<sPtrProv> fileprovs_;
    std::vector<sPtrProv> dirprovs_;
    static Patterns patterns_;
    static bool hideEmptyDirectories_;
  };

  //----< define storage for static data >-----------------------------

  DirProvider::Patterns DirProvider::patterns_;
  bool DirProvider::hideEmptyDirectories_ = false;
}