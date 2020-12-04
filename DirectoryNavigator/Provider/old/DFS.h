#pragma once
/////////////////////////////////////////////////////////////////////
// DFS.h - Depth First Search using provider model                 //
// ver 1.1                                                         //
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
    DFS(IProvider* pProvider) : pProvider_(pProvider) {}
    virtual ~DFS() {}
    void setMaxItems(size_t maxItems);

    void search();
    void find(IProvider* pProvider);
  private:
    bool done()
    {
      return (0 < maxItems_ && maxItems_ <= count_);
    }
    IProvider * pProvider_ = nullptr;
    static size_t maxItems_;
  };

  size_t DFS<Provider::IProvider>::maxItems_ = 0;

  template<typename IProvider>
  void DFS<IProvider>::setMaxItems(size_t maxItems)
  {
    maxItems_ = maxItems;
  }
  template<typename IProvider>
  void DFS<IProvider>::search()
  {
    find(pProvider_);
  }

  template<typename IProvider>
  void DFS<IProvider>::find(IProvider* pProvider)
  {
    size_t count = pProvider->count();
    if (pProvider->done())  // stop recursion
      return;
    pProvider->doOperation();
    for (auto pProv : *(pProvider->children()))
    {
      find(pProv.get());
    }
  }
}