/*用户操作接口模块，将用户输入的命令转化为对
相应函数的调用，对输入输出进行处理和错误检查*/


#include "User.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
extern FileManager g_FileManager;


User::User()
{
    u_error = User::U_NOERROR;
    fileManager = &g_FileManager;
    u_dirp = "/";
    curDirPath = "/";
    u_cdir = fileManager->rootDirInode;

    u_pdir = NULL;
    memset(u_arg, 0, sizeof(u_arg));

}

User::~User()
{
}

void User::Mkdir(string dirName) { //错误不在此处
    if (!checkPathName(dirName)) {
        return;
    }
    u_arg[1] = Inode::IFDIR;
    fileManager->Creat();
    IsError();
}

void User::Ls() {
    ls.clear();
    fileManager->Ls();
    if (IsError()) {
        return;
    }
    printf("%s\n", ls.c_str());
}

void User::Cd(string dirName) {
    if (!checkPathName(dirName)) {
        return;
    }
    fileManager->ChDir();
    IsError();
}

void User::Create(string fileName, string mode) {
    if (!checkPathName(fileName)) {
        return;
    }
    int md = INodeMode(mode);
    if (md == 0) {
        printf("no this mode !  \n");
        return;
    }
    u_arg[1] = md;
    fileManager->Creat();
    IsError();
}

void User::Delete(string fileName) {
    if (!checkPathName(fileName)) {
        return;
    }
    fileManager->UnLink();
    IsError();
}

void User::Open(string fileName, string mode) {
    if (!checkPathName(fileName)) {
        return;
    }
    int md = FileMode(mode);
    if (md == 0) {
        printf("no this mode ! \n");
        return;
    }
    u_arg[1] = md;
    fileManager->Open();
    if (IsError())
        return;
    printf("open success, fd is =%d\n", u_ar0[EAX]);
}

void User::Close(string sfd) {
    if (sfd.empty() || !isdigit(sfd.front())) {
        printf("close error.no such file! \n");
        return;
    }
    u_arg[0] = stoi(sfd);
    fileManager->Close();
    IsError();
}

void User::Seek(string sfd, string offset, string origin) {
    if (sfd.empty() || !isdigit(sfd.front())) {
        printf("seek error! \n");
        return;
    }
    if (offset.empty() || !isdigit(origin.front())) {
        printf("seek error\n");
        return;
    }
    if (origin.empty() || !isdigit(origin.front())) {
        printf("seek error\n");
        return;
    }
    u_arg[0] = stoi(sfd);
    u_arg[1] = stoi(offset);
    u_arg[2] = stoi(origin);
    fileManager->Seek();
    IsError();
}
void User::fin(string file, string inFile)//从宿主机导入文件到二级系统中
{
    int md = FileMode("-rw");
    u_arg[1] = md;
    fileManager->Open();
    if (IsError())
        return;
    string sfd = to_string(u_ar0[EAX]);
    fileManager->Close();
    ifstream in(inFile);
    in.seekg(0, ios::end);
    streampos ps = in.tellg();
    in.close();
    Write(sfd, inFile, to_string(ps));
}
void User::fout(string file, string outfile)//从二级系统导出文件到宿主机中
{
    int md = FileMode("-rw");
    u_arg[1] = md;
    fileManager->Open();
    if (IsError())
        return;
    string sfd = to_string(u_ar0[EAX]);
    fileManager->Close();
    int size = u_ofiles.GetF(u_ar0[EAX])->f_inode->i_size;//获取当前文件的长度
    Read(sfd, outfile, to_string(size));
}
void User::Write(string sfd, string inFile, string size) {
    if (sfd.empty() || !isdigit(sfd.front())) {
        printf("write error \n");
        return;
    }
    int fd = stoi(sfd);

    int usize;
    if (size.empty() || (usize = stoi(size)) < 0) {
        printf("write error! \n");
        return;
    }

    char* buffer = new char[usize];
    fstream fin(inFile, ios::in | ios::binary);
    if (!fin) {
        printf("file %s open failed ! \n", inFile.c_str());
        return;
    }
    fin.read(buffer, usize);
    fin.close();
    u_arg[0] = fd;
    u_arg[1] = (long)buffer;
    u_arg[2] = usize;
    fileManager->Write();

    if (IsError())
        return;
    cout << "write " << u_ar0[User::EAX] << " bytes success !" << endl;
    delete[]buffer;
}

void User::Read(string sfd, string outFile, string size) {
    if (sfd.empty() || !isdigit(sfd.front())) {
        printf("read error \n");
        return;
    }
    int fd = stoi(sfd);

    int usize;
    if (size.empty() || !isdigit(size.front()) || (usize = stoi(size)) < 0) {
        printf("read error \n");
        return;
    }
    char* buffer = new char[usize];
    u_arg[0] = fd;
    u_arg[1] = (long)buffer;
    u_arg[2] = usize;
    fileManager->Read();
    if (IsError())
        return;

    cout << "read " << u_ar0[User::EAX] << " bytes success : " << endl;
    if (outFile.empty()) {
        for (unsigned int i = 0; i < u_ar0[User::EAX]; ++i) {
            cout << (char)buffer[i];
        }
        cout << endl;
        return;
    }
    fstream fout(outFile, ios::out | ios::binary);
    if (!fout) {
        cout << "file " << outFile << " open failed ! " << endl;;
        return;
    }
    fout.write(buffer, u_ar0[User::EAX]);
    fout.close();
    cout << "read to " << outFile << " done ! " << endl;;
    delete[]buffer;
}

int User::INodeMode(string mode) {
    int md = 0;
    if (mode.find("-r") != string::npos) {
        md |= Inode::IREAD;
    }
    if (mode.find("-w") != string::npos) {
        md |= Inode::IWRITE;
    }
    if (mode.find("-rw") != string::npos) {
        md |= (Inode::IREAD | Inode::IWRITE);
    }
    return md;
}

int User::FileMode(string mode) {
    int md = 0;
    if (mode.find("-r") != string::npos) {
        md |= File::FREAD;
    }
    if (mode.find("-w") != string::npos) {
        md |= File::FWRITE;
    }
    if (mode.find("-rw") != string::npos) {
        md |= (File::FREAD | File::FWRITE);
    }
    return md;
}

bool User::checkPathName(string path) {
    if (path.empty()) {
        cout << "parameter path can't be empty !" << endl;
        return false;
    }

    if (path.substr(0, 2) != "..") {
        u_dirp = path;
    }
    else {
        string pre = curDirPath;
        unsigned int p = 0;
        //多重相对路径 .. ../ ../.. ../../
        for (; pre.length() > 3 && p < path.length() && path.substr(p, 2) == ".."; ) {
            pre.pop_back();
            pre.erase(pre.find_last_of('/') + 1);
            p += 2;
            p += p < path.length() && path[p] == '/';
        }
        u_dirp = pre + path.substr(p);
    }

    if (u_dirp.length() > 1 && u_dirp.back() == '/') {
        u_dirp.pop_back();
    }

    for (unsigned int p = 0, q = 0; p < u_dirp.length(); p = q + 1) {
        q = u_dirp.find('/', p);
        q = min(q, (unsigned int)u_dirp.length());
        if (q - p > DirectoryEntry::DIRSIZ) {
            cout << "the fileName or dirPath can't be greater than 28 size !\n";
            return false;
        }
    }
    return true;
}

bool User::IsError() {
    if (u_error != U_NOERROR) {
        cout << "errno = " << u_error << endl;
        EchoError(u_error);
        u_error = U_NOERROR;//重置错误码
        return true;
    }
    return false;
}

void User::EchoError(enum ErrorCode err) {
    string estr;
    switch (err) {
    case User::U_NOERROR:
        estr = " No u_error ";
        break;
    case User::U_ENOENT:
        estr = " No such file or directory ";
        break;
    case User::U_EBADF:
        estr = " Bad file number ";
        break;
    case User::U_EACCES:
        estr = " Permission denied ";
        break;
    case User::U_ENOTDIR:
        estr = " Not a directory ";
        break;
    case User::U_ENFILE:
        estr = " File table overflow ";
        break;
    case User::U_EMFILE:
        estr = " Too many open files ";
        break;
    case User::U_EFBIG:
        estr = " File too large ";
        break;
    case User::U_ENOSPC:
        estr = " No space left on device ";
        break;
    default:
        break;
    }
    cout << estr << endl;
}