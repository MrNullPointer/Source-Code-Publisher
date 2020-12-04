/////////////////////////////////////////////////////////////////////
// DFS.h - Depth First Search using provider model                 //
// ver 1.1                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018         //
/////////////////////////////////////////////////////////////////////

#ifdef TEST_DFS

#include "DFS.h"
#include "Provider.h"
#include "../Utilities/StringUtilities/StringUtilities.h"
#include "../Utilities/CodeUtilities/CodeUtilities.h"

using namespace Utilities;
using namespace Provider;

int main(int argc, char* argv[])
{
  Title("Demonstrate DFS with Directory Provider Model");

  ProcessCmdLine pcl(argc, argv);
  if (pcl.parseError())
    return 1;

  preface("Command Line: ");
  pcl.showCmdLine();
  putline();

  DirProvider dp(pcl.path());

  for (auto patt : pcl.patterns())
  {
    dp.addPattern(patt);
  }

  if(pcl.hasOption('h'))
    dp.hideEmptyDirectories(true);

  Search::DFS<IProvider> dfs(&dp);
  dfs.search();

  std::cout << "\n\n";
  return 0;
}
#endif
