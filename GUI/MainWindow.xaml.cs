///////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - GUI for Project3HelpWPF                      //
// ver 1.0                                                           //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018         //
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package provides a WPF-based GUI for Project3HelpWPF demo.  It's 
 * responsibilities are to:
 * - Provide a display of directory contents of a remote ServerPrototype.
 * - It provides a subdirectory list and a filelist for the selected directory.
 * - You can navigate into subdirectories by double-clicking on subdirectory
 *   or the parent directory, indicated by the name "..".
 *   
 * Required Files:
 * ---------------
 * Mainwindow.xaml, MainWindow.xaml.cs
 * Translater.dll
 * 
 * Maintenance History:
 * --------------------
 * ver 1/1 : 07 Aug 2018
 * - fixed bug in DirList_MouseDoubleClick by returning when selectedDir is null
 * ver 1.0 : 30 Mar 2018
 * - first release
 * - Several early prototypes were discussed in class. Those are all superceded
 *   by this package.
 */

// Translater has to be statically linked with CommLibWrapper
// - loader can't find Translater.dll dependent CommLibWrapper.dll
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;
using MsgPassingCommunication;
using System.IO;
using Path = System.IO.Path;
using System.Threading;
using System.Diagnostics;


namespace WpfApp1
{
  public partial class MainWindow : Window
  {
        private string WrkDirectory { get; set; }

        private string Patterns { get; set; }

        private string Regex { get; set; }

        private List<String> files;

       private int test = 1;

        public MainWindow()
    {
      InitializeComponent();
    }

    private Stack<string> pathStack_ = new Stack<string>();
    private Translater translater;
    private CsEndPoint endPoint_;
    private Thread rcvThrd = null;
    private Dictionary<string, Action<CsMessage>> dispatcher_ 
      = new Dictionary<string, Action<CsMessage>>();

    //----< process incoming messages on child thread >----------------
    private void processMessages()
    {
      ThreadStart thrdProc = () => {
        while (true)
        {
          CsMessage msg = translater.getMessage();
          string msgId = msg.value("command");
          if (dispatcher_.ContainsKey(msgId))
            dispatcher_[msgId].Invoke(msg);
        }
      };
      rcvThrd = new Thread(thrdProc);
      rcvThrd.IsBackground = true;
      rcvThrd.Start();
    }

    //----< function dispatched by child thread to main thread >-------
    private void clearDirs()
    {
      lstFiles.Items.Clear();
    }

    //----< function dispatched by child thread to main thread >------
    private void addDir(string dir)
    {
      lstFiles.Items.Add(dir);
    }

    //----< function dispatched by child thread to main thread >-------
    private void insertParent()
    {
      lstFiles.Items.Insert(0, "..");
    }

    //----< function dispatched by child thread to main thread >-------
    private void clearFiles()
    {
      lstFiles.Items.Clear();
    }

    //----< function dispatched by child thread to main thread >-------
    private void addFile(string file)
    {
      lstFiles.Items.Add(file);
    }

    //----< add client processing for message with key >---------------
    private void addClientProc(string key, Action<CsMessage> clientProc)
    {
      dispatcher_[key] = clientProc;
    }

    //----< load getDirs processing into dispatcher dictionary >-------
    private void DispatcherLoadGetDirs()
    {
      Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
      {
        Action clrDirs = () =>
        {
          clearDirs();
        };
        Dispatcher.Invoke(clrDirs, new Object[] { });
        var enumer = rcvMsg.attributes.GetEnumerator();
        while (enumer.MoveNext())
        {
          string key = enumer.Current.Key;
          if (key.Contains("dir"))
          {
            Action<string> doDir = (string dir) =>
            {
              addDir(dir);
            };
            Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });
          }
        }
        Action insertUp = () =>
        {
          insertParent();
        };
        Dispatcher.Invoke(insertUp, new Object[] { });
      };
      addClientProc("getDirs", getDirs);
    }

    //----< load getFiles processing into dispatcher dictionary >------
    private void DispatcherLoadGetFiles()
    {
      Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
      {
        Action clrFiles = () =>
        {
        //  clearFiles();
        };
        Dispatcher.Invoke(clrFiles, new Object[] { });
        var enumer = rcvMsg.attributes.GetEnumerator();
        while (enumer.MoveNext())
        {
          string key = enumer.Current.Key;
          if (key.Contains("file"))
          {
            Action<string> doFile = (string file) =>
            {
              addFile(file);
            };
            Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
          }
        }
      };
      addClientProc("getFiles", getFiles);
    }

    //----< load Converted FIles into dispatcher dictionary >------
        private void DispatcherLoadConvertedFiles()
        {
            Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
            {
                Action clrFiles = () =>
                {
                    lstConverted.Items.Clear();
                };
                Dispatcher.Invoke(clrFiles, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("file"))
                    {
                        Action<string> doFile = (string file) =>
                        {
                            lstConverted.Items.Add(file);
                        };
                        Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                    }
                }
            };
            addClientProc("convertFiles", getFiles);
        }

   //----< load Downloaded FIles into dispatcher dictionary >------
        private void DispatcherLoadDownloadedFiles()
        {
            Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
            {
                Action clrFiles = () =>
                {
                    
                };
                Dispatcher.Invoke(clrFiles, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    if (key.Contains("name"))
                    {
                        Action<string> doFile = (string file) =>
                        {
                            var process = new Process();
                            process.StartInfo.FileName = "firefox";
                            process.StartInfo.Arguments = System.IO.Path.GetFullPath("../../../"+file);
                            process.Start();
                            process.WaitForExit();
                        };
                        Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                    }
                }
            };
            addClientProc("downloadFiles", getFiles);
        }


    //----< load all dispatcher processing >---------------------------
        private void loadDispatcher()
    {
      DispatcherLoadGetDirs();
      DispatcherLoadGetFiles();
            DispatcherLoadConvertedFiles();
            DispatcherLoadDownloadedFiles();
    }

    // mouse double click event handler - double click on the converted files list to open the HTML files 
        private void LstConverted_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";
            endPoint_.machineAddress = "localhost";
            string selectedItem = lstConverted.SelectedItem.ToString();

            selectedItem = System.IO.Path.Combine("..\\..\\..\\..\\ConvertedPages", selectedItem);
            selectedItem = Path.GetFullPath(selectedItem);
            serverEndPoint.port = 8080;
            endPoint_.port = 8082;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.remove("command");
            msg.add("command", "downloadFiles");
            msg.add("file", selectedItem);
            msg.add("fileName", System.IO.Path.GetFileName(selectedItem));
            translater.postMessage(msg);
        }
   
    //----< start Comm, fill window display with dirs and files >------
        private void Window_Loaded(object sender, RoutedEventArgs e)
    {
            // start Comm
            string[] pcmd = Environment.GetCommandLineArgs();
      endPoint_ = new CsEndPoint();
      endPoint_.machineAddress = pcmd[3];
      endPoint_.port = Int32.Parse(pcmd[4]);
      translater = new Translater();
      translater.listen(endPoint_);

      // start processing messages
      processMessages();

      // load dispatcher
      loadDispatcher();

      CsEndPoint serverEndPoint = new CsEndPoint();
      serverEndPoint.machineAddress = pcmd[1];
      serverEndPoint.port = Int32.Parse(pcmd[2]);
            WrkDirectory = Directory.GetCurrentDirectory() + "/../../../../";
            WrkDirectory = Path.GetFullPath(WrkDirectory);
            Patterns = "*.h *.cpp";
            Regex = "[A-B](.*)";

            txtPatterns.Text = Patterns;
            txtRegex.Text = Regex;

            txtPath.Text = System.IO.Path.GetFullPath("../../../../");
      pathStack_.Push("../");
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "getDirs");
      msg.add("path", pathStack_.Peek());
      translater.postMessage(msg);
      msg.remove("command");
      msg.add("command", "getFiles");
      translater.postMessage(msg);



        }

    //----< strip off name of first part of path >---------------------
    private string removeFirstDir(string path)
    {
      string modifiedPath = path;
      int pos = path.IndexOf("/");
      modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
      return modifiedPath;
    }

    //----< respond to mouse double-click on dir name >----------------
    private void DirList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
    {
      // build path for selected dir
      string selectedDir = (string)lstFiles.SelectedItem;
      if (selectedDir == null)
        return;
      string path;
      if(selectedDir == "..")
      {
        if (pathStack_.Count > 1)  // don't pop off "Storage"
          pathStack_.Pop();
        else
          return;
      }
      else
      {
        path = pathStack_.Peek() + "/" + selectedDir;
        pathStack_.Push(path);
      }
      // display path in Dir TextBlcok
      txtPath.Text = removeFirstDir(pathStack_.Peek());

            // build message to get dirs and post 
            string[] pcmd = Environment.GetCommandLineArgs();
      CsEndPoint serverEndPoint = new CsEndPoint();
      serverEndPoint.machineAddress = pcmd[1];
      serverEndPoint.port = Int32.Parse(pcmd[2]);
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "getDirs");
      msg.add("path", pathStack_.Peek());
      translater.postMessage(msg);
      
      // build message to get files and post it
      msg.remove("command");
      msg.add("command", "getFiles");
      translater.postMessage(msg);
    }

    //----< first test not completed >---------------------------------
    void test1()
    {
      MouseButtonEventArgs e = new MouseButtonEventArgs(null, 0, 0);
      lstFiles.SelectedIndex = 1;
      DirList_MouseDoubleClick(this, e);
    }

        

        //This function open converted files in browser 
        private void openInBrowser()
        {
            TabCtrl.SelectedIndex = 1;
            foreach (string file in files)
            {
                try
                {
                    var process = new Process();
                    process.StartInfo.FileName = "Firefox";
                    process.StartInfo.Arguments = file;
                    process.Start();
                    process.WaitForExit();
                }
                catch (SystemException ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
        }


        // This function execute Publish button event handler 
        private void BtnPublish_Click(object sender, RoutedEventArgs e)
        {

            List<String> args = new List<string>();
            args.Add(System.IO.Path.GetFullPath("../../../../../"+txtPath.Text.ToString()));
            args.Add("/s");
            Patterns = txtPatterns.Text.ToString();
            args.AddRange(Patterns.Split(' '));
            args.Add(txtRegex.Text.ToString());
            string strArgs = "";
            foreach (var a in args)
            {
                strArgs += a.ToString() + "##";
            }
            List<String> resultFiles = new List<String>();
            CsEndPoint serverEndPoint = new CsEndPoint();
            string[] pcmd = Environment.GetCommandLineArgs();
            serverEndPoint.machineAddress = pcmd[1];
               serverEndPoint.port = Int32.Parse(pcmd[2]);
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "convertFiles");
            msg.add("strArgs", strArgs);
            translater.postMessage(msg);

        }

        // Browse Button event handler
        private void BtnBrowse_Click(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.FolderBrowserDialog dialog;
            dialog = new System.Windows.Forms.FolderBrowserDialog();

            if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                WrkDirectory = dialog.SelectedPath;
                txtPath.Text = WrkDirectory;
            }

        }

        // to fetch the correct path
        private void TxtPath_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (translater != null)
            {
                lstFiles.Items.Clear();
                lstFiles.Items.Add("[..]");

                var dirs = Directory.GetDirectories(WrkDirectory);

                foreach (var dir in dirs)
                {
                    string dirName = "[" + Path.GetFileName(dir) + "]";
                    lstFiles.Items.Add(dirName);
                }

                List<String> filesMatchingPatterns = new List<string>();

                if (Patterns != null)
                {
                    var patterns = Patterns.Split(' ');
                    foreach (var patt in patterns)
                        filesMatchingPatterns.AddRange(Directory.GetFiles(WrkDirectory, patt));

                }
                else
                {
                    filesMatchingPatterns = Directory.GetFiles(WrkDirectory).ToList();
                }


                foreach (var file in filesMatchingPatterns) lstFiles.Items.Add(Path.GetFileName(file));

            }

        }

        // mouse double click event handler - double click on the list to open the files
        private void LstFiles_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (lstFiles.SelectedIndex == -1) return;

            string selectedItem = lstFiles.SelectedItem.ToString();
            selectedItem = selectedItem.Substring(1, selectedItem.Length - 2);
            selectedItem = Path.Combine(WrkDirectory, selectedItem);
            selectedItem = Path.GetFullPath(selectedItem);

            if (!Directory.Exists(selectedItem)) return;

            WrkDirectory = selectedItem;
            txtPath.Text = selectedItem;
        }

        // Pattern event handler - to handle the input of the extention we want
        private void TxtPatterns_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                Patterns = txtPatterns.Text;
                TxtPath_TextChanged(this, null);
            }
        }

        // Regex pattern event handler - to handle the regex pattern we give
        private void TxtRegex_KeyUP(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                Regex = txtRegex.Text;
                TxtPath_TextChanged(this, null);
            }
        }

        // This function opens the selected file
        private void Display_Click(object sender, RoutedEventArgs e)
        {
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";
            serverEndPoint.port = 8080;
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.remove("command");
            msg.add("command", "convertFiles");
            msg.add("file", lstConverted.Items[0].ToString());
            translater.postMessage(msg);
        }

        private void RcvConverted_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {

        }


    }
}


