/////////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp - Console App that processes incoming messages  //
// ver 1.0                                                             //
// Author: Parikshit Dubey                                             //
// Source: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2      //
/////////////////////////////////////////////////////////////////////////

#include "ServerPrototype.h"
#include "../CppParser/FileSystem/FileSystem.h"
#include <chrono>
#include "../executive/SourcePublisher.h"

namespace MsgPassComm = MsgPassingCommunication;

using namespace ServerRepository;
using namespace FileSystem;
using Msg = MsgPassingCommunication::Message;

MultipleFiles Server::getFiles(const ServerRepository::SearchPath& path)
{
  return Directory::getFiles(path);
}

Dirs Server::getDirs(const ServerRepository::SearchPath& path)
{
  return Directory::getDirectories(path);
}

template<typename T>
void show(const T& t, const std::string& msg)
{
  std::cout << "\n  " << msg.c_str();
  for (auto item : t)
  {
    std::cout << "\n    " << item.c_str();
  }
}

std::function<Msg(Msg)> echo = [](Msg msg) {
  Msg reply = msg;
  reply.to(msg.from());
  reply.from(msg.to());
  return reply;
};

std::function<Msg(Msg)> downloadFiles = [](Msg msg) {

	std::cout << std::endl << " Requirement 6 - This project supports the conversion of source code in server and that is done when a separate request is instatiated";
	std::cout << std::endl << "one or more of the converted files are sent back to the local client in downloaded folder, using the communication channel";
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("downloadFiles");
	reply.attribute("name","../DownloadedPages/" + msg.value("fileName"));
	return reply;
};

std::function<Msg(Msg)> convertFiles = [](Msg msg) {
	
	std::vector<std::string> args;
	std::vector<std::string> cmd;
	std::string delimiter = "##";

	size_t pos = 0;
	std::string token;
	std::string s = msg.value("strArgs");
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		cmd.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	char array[20][100] = { {0} };
	strcpy_s(array[0], "");
	std::cout << std::endl << "-------- Command line parameters are received in the cpp  --------" << std::endl;
	for (size_t i = 1; i <= cmd.size(); i++) {
		strcpy_s(array[i], cmd[i - 1].c_str());
		std::cout << cmd[i - 1] << std::endl;
	}
	// converting into char** so that can be passed as argv
	char *ptr_array[20];
	for (int i = 0; i < 20; i++)
		ptr_array[i] = array[i];
	char **tmp_array = ptr_array;

	factobject factory;
	SourcePublisher *obj = factory.ClientCreation();
	std::vector<std::string> files;
	files = obj->initialization(cmd.size()+1, tmp_array);
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("convertFiles");
	size_t count = 0;
	for (auto item : files)
	{
		std::string countStr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("file" + countStr, item);
	}
	return reply;
};

std::function<Msg(Msg)> getFiles = [](Msg msg) {
	
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getFiles");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    MultipleFiles files = Server::getFiles(searchPath);
    size_t count = 0;
    for (auto item : files)
    {
      std::string countStr = Utilities::Converter<size_t>::toString(++count);
      reply.attribute("file" + countStr, item);
    }
  }
  else
  {
    std::cout << std::endl << " getFiles message did not define a path attribute" << std::endl;
  }
  return reply;
};

std::function<Msg(Msg)> getDirs = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getDirs");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    MultipleFiles dirs = Server::getDirs(searchPath);
    size_t count = 0;
    for (auto item : dirs)
    {
      if (item != ".." && item != ".")
      {
        std::string countStr = Utilities::Converter<size_t>::toString(++count);
        reply.attribute("dir" + countStr, item);
      }
    }
  }
  else
  {
    std::cout << std::endl << "path attribute is not defined in the getDirs message";
  }
  return reply;
};

void ProjectRequirementsDisplay() {
	std::cout << std::endl << " -------------- Parikshit Dubey Source Code Publisher Project 4 --------------";
	std::cout << std::endl << " Requirement 1 - This project utilized Microsoft Visual Studio Community 2017 and its C++ Windows Console Projects";
	std::cout << std::endl << " Requrement 2 - This project used Windows Presentation Foundation (WPF) for the Client's user display";
	std::cout << std::endl << " Requirement 3 - The assembeled parts of Project 1, Project 2 and Project 3 are used in this project in a client server configuration";
	std::cout << std::endl << " Remote server is in communication with client for conversion";
	std::cout << std::endl << " Requirement 4 - This project uses a Graphical User Interface (GUI) for the client that supports navigating remote directories to find a project for conversion,";
	std::cout << std::endl << " and supports displaying the converted results in the client side in web browser.";
	std::cout << std::endl << " Requirement 8 - This project included an automated test that accepts the server url and remote path3 to your project directory on its command line, ";
	std::cout << std::endl << "   invokes the Code Publisher, through its interface, to convert all the project files matching a pattern that accepts *.h and *.cpp files, and then opens the Client GUI's Display view.";
	std::cout << std::endl << " NOTE: Please close all the instances of browser before using the gui";
	std::cout << std::endl << " NOTE: Please navigate to the directory OODProject4_ParikshitDubey in the GUI and click publish";
	std::cout << std::endl << " NOTE: The converted cpp files can be seen in the converted files tab";
}

int main()
{

  ProjectRequirementsDisplay();
  Server server(serverEndPoint, "ServerPrototype");
  server.start();
  std::cout << std::endl << " Requirement 5 - Message processing test";
  std::cout << std::endl <<" ------------------------------------------------------------- ";
  server.addMsgProc("echo", echo);
  server.addMsgProc("getFiles", getFiles);
  server.addMsgProc("getDirs", getDirs);
  server.addMsgProc("serverQuit", echo);
  server.addMsgProc("convertFiles",convertFiles);
  server.addMsgProc("downloadFiles", downloadFiles);
  server.processMessages();
  
  Msg msg(serverEndPoint, serverEndPoint); 
  msg.name("msgToSelf");
  msg.command("echo");
  msg.attribute("verbose", "show me");
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  msg.command("getFiles");
  msg.remove("verbose");
  msg.attributes()["path"] = storageRoot;
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  msg.command("getDirs");
  msg.attributes()["path"] = storageRoot;
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  std::cout << std::endl << " Please press enter to exit";
  std::cin.get();
  std::cout << std::endl;

  msg.command("serverQuit");
  server.postMessage(msg);
  server.stop();

  std::cout << std::endl << " Requirement 7 - Two or more concerrent operations from run.bat are displayed sucessfully in the project";

  return 0;
}

