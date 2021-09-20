#include "INode.h"
#include "File.h"
#include "FileSystem.h"
#include "OpenFileManager.h"
#include "FileManager.h"
#include "User.h"
#include <iostream>
#include <istream>
#include<string>
#include<stdio.h>
#include<vector>

using namespace std;

DiskManager g_DiskManager;
BufferManager g_BufferManager;
OpenFileTable g_OpenFileTable;
SuperBlock g_SuperBlock;
FileSystem g_FileSystem;
InodeTable g_InodeTable;
FileManager g_FileManager;
User g_User;
void help()
{
    static string information =
        "    ------------------------The supported commands are as follows--------------------------\n\n\n"
        "   man          :  Help manual \n"
        "   fformat      :  system initialization\n"
        "   exit         :  Exit correctly \n"
        "   mkdir        :  New directory \n"
        "   cd           :  Change directory \n"
        "   ls           :  List directories and files \n"
        "   fcreate       :  Create a new file                                                eg: fcreate <name> -r/-w/-rw\n"
        "   fdelete      :  Delete Files                                                      eg: fdelete <name> \n"
        "   fopen        :  Open a file                                                       eg: fopen <name> -r/-w/-rw\n"
        "   fclose       :  Close file                                                        eg: fclose <fd>\n"
        "   flseek       :  Move the read and write pointer                                   eg: seek <fd> <offset> <origin>\n"
        "   fwrite       :  Read content from external file and write to internal file        eg: fwrite <fd> <InFileName> <size>\n"
        "   fread        :  Read the content of the internal file and output to the screen    eg: fread <fd> <size>\n"
        "   fin          :  Import host file                                                  eg: fin <filename> <InFileName>\n"
        "   fout         :  Export file to host                                               eg: fout <filename> <OutFileName>\n\n"
        ;
    cout << information;
}
void cmdArgs(const string& cmd, vector <string>& args) {
    args.clear();
    string str;
    unsigned int p, q;
    for (p = 0, q = 0; q < cmd.length(); p = q + 1) {
        q = cmd.find_first_of(" \n", p);
        str = cmd.substr(p, q - p);
        if (!str.empty()) {
            args.push_back(str);
        }
        if (q == string::npos)
            return;
    }
}

int main() {
    User* user = &g_User;

    string line = "";
    vector<string> args;
    string cmd, arg1, arg2, arg3;

    cout << "**************************** Secondary file system ******************************\n\n";
    help();
    while (1) {
        if (line == "")
            goto NEXT_INPUT;
        cmdArgs(line, args);
        cmd = args[0];
        arg1 = args.size() > 1 ? args[1] : "";
        arg2 = args.size() > 2 ? args[2] : "";
        arg3 = args.size() > 3 ? args[3] : "";

        if (cmd == "man") {
            help();
        }
        else if (cmd == "fformat") {
            user->Mkdir("root");
            user->Mkdir("bin");
            user->Mkdir("etc");
            user->Mkdir("home");
            user->Mkdir("usr");
            user->Mkdir("dev");
        }
        else if (cmd == "exit") {
            exit(0);
        }
        else if (cmd == "mkdir") {
            user->Mkdir(args[1]);
        }
        else if (cmd == "ls") {
            user->Ls();
        }
        else if (cmd == "cd") {
            user->Cd(arg1);
        }
        else if (cmd == "fcreate") {
            user->Create(arg1, arg2 + arg3);
        }
        else if (cmd == "fdelete") {
            user->Delete(arg1);
        }
        else if (cmd == "fopen") {
            user->Open(arg1, line);
        }
        else if (cmd == "fclose") {
            user->Close(arg1);
        }
        else if (cmd == "flseek") {
            user->Seek(arg1, arg2, arg3);
        }
        else if (cmd == "fread") {
            user->Read(arg1, "", arg2);
        }
        else if (cmd == "fwrite") {
            user->Write(arg1, arg2, arg3);
        }
        else if (cmd == "fin") {
            user->fin(arg1, arg2);
        }
        else if (cmd == "fout") {
            user->fout(arg1, arg2);
        }
        else if (cmd != "") {
            cout << "shell : " << cmd << " : don't find this commond \n";
        }

    NEXT_INPUT:
        cout << "[LH-2nd-FS" << user->curDirPath << "]$ ";
        getline(cin, line);
    }
    return 0;
}