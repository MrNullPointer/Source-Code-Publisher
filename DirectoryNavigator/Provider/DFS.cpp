/////////////////////////////////////////////////////////////////////
// DFS.h - Depth First Search using provider model                 //
// ver 1.2                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018         //
/////////////////////////////////////////////////////////////////////

#ifdef TEST_DFS

#include "DFS.h"
#include "Provider.h"
#include "../Utilities/StringUtilities/StringUtilities.h"
#include "../Utilities/CodeUtilities/CodeUtilities.h"

using namespace Utilities;
using namespace Provider;
using namespace Search;

int main(int argc, char* argv[])
{
  Title("Demonstrate DFS with Directory Provider Model");

  ProcessCmdLine pcl(argc, argv);
  if (pcl.parseError())
    return 1;

  preface("Command Line: ");
  pcl.showCmdLine();
  putline();

  std::shared_ptr<DirProvider> ptrDP(new DirProvider(pcl.path()));

  for (auto patt : pcl.patterns())
  {
    ptrDP->addPattern(patt);
  }

  if (pcl.hasOption('h'))
    ptrDP->hideEmptyDirectories(true);

  if (pcl.maxItems() > 0)
  {
    ptrDP->maxItems(pcl.maxItems());
  }

  DFS<IProvider> dfs(ptrDP);

  if (pcl.hasOption('s'))
  {
    dfs.recurse(true);
  }

  if (pcl.hasOption('a'))
  {
    ptrDP->showAllInCurrDir(true);
  }

  dfs.search();

  std::cout << "\n\n";
  return 0;
}
#endif
