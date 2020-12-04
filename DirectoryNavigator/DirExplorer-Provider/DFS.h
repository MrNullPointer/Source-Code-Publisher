#pragma once
/////////////////////////////////////////////////////////////////////
// DFS.h - Depth First Search using provider model                 //
// ver 1.2                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018         //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  DFS implements Depth First Search, depending on its Provider to
*  implement branch and leaf operations.  
*
*  A directory provider has these corresponding parts:
*  - branch is a directory (holds files and subdirectories)
*  - leaf is a file
*  A graph provider would have these corresponding parts:
*  - branch is a vertex (holds edges)
*  - leaf is an edge (points to a single vertex)
*
*  The Provider is essentially an application's adapter that lets the 
*  DFS instance be ignorant of the details of its operations.
*
*  Required Files:
*  ---------------
*  DFS.h, DFS.cpp   // DFS.cpp is not needed by applications
*  Provider.h
*
*  Maintenance History:
*  --------------------
*  ver 1.3 : 19 Aug 2018
*  - minor refactoring of code layout
*  - changed done() to use Provider's done
*  ver 1.2 : 16 Aug 2018
*  - changed pProvider_ member from pointer to Provider::IProvider
*    to std::shared_ptr<Provider::IProvider>
*  - set no recursion as default
*  ver 1.1 : 15 Aug 2018
*  - added functionality for passing search and reporting parameters
*    to its provider
*  ver 1.0 : 12 Aug 2018
*  - first release
*/
#include "Provider.h"

namespace Search
{

  template<typename IProvider>
  class DFS
  {
  public:
    using sPtrProv = std::shared_ptr<IProvider>;

    DFS(sPtrProv pProvider) : pProvider_(pProvider) {}
    virtual ~DFS() {}
    void setMaxItems(size_t maxItems);

    void search();
    void find(sPtrProv pProvider);
    void recurse(bool doRecurse = true);

  private:
    bool done();
    sPtrProv pProvider_ = nullptr;
    static size_t maxItems_;
    static bool recurse_;
  };

  // define statics
  size_t DFS<Provider::IProvider>::maxItems_ = 0;
  bool DFS<Provider::IProvider>::recurse_ = false;

  //----< save upperbound on number of files to process >------------

  template<typename IProvider>
  void DFS<IProvider>::setMaxItems(size_t maxItems)
  {
    maxItems_ = maxItems;
  }
  //----< save navigation mode >-------------------------------------

  template<typename IProvider>
  void DFS<IProvider>::recurse(bool doRecurse)
  {
    recurse_ = doRecurse;
  }
  //----< start navigation using a stored DirProvider >--------------
  /*
  *  pProvider_ is a std::shared_ptr<IProvider> which refers
  *  to a DirProvider
  */
  template<typename IProvider>
  void DFS<IProvider>::search()
  {
    find(pProvider_);
  }
  //----< navigation using a very simple DFS process >---------------
  /*
  *  There are two Providers that implement the IProvider interface:
  *  - FileProvider handles all file operations
  *  - DirProvider handles all directory operations
  *  In order to navigate, you have to hand find() a DirProvider,
  *  otherwise you just display the input file name.  That has no
  *  children, so there won't be any recursion.
  */
  template<typename IProvider>
  void DFS<IProvider>::find(sPtrProv pProvider)
  {
    if (pProvider->done())  // stop recursion
      return;
    pProvider->doOperation(true);
    for (auto pProv : *(pProvider->children()))
    {
      if (recurse_)
        find(pProv);
      else
        pProv->doOperation(false);
    }
  }
  //----< defer evaluation of termination conditions to Provider >---

  template<typename IProvider>
  bool DFS<IProvider>::done()
  {
    pProvider_->done();
  }
}