/*�û������ӿ�ģ�飬���û����������ת��Ϊ��
��Ӧ�����ĵ��ã�������������д���ʹ�����*/

#ifndef USER_H
#define USER_H

#include <string.h>
#include "FileManager.h"

using namespace std;

/*
 * @comment ������Unixv6�� struct user�ṹ��Ӧ�����ֻ�ı�
 * ���������޸ĳ�Ա�ṹ���֣������������͵Ķ�Ӧ��ϵ����:
 */
class User
{
public:
	static const int EAX = 0;	/* u.u_ar0[EAX]�������ֳ���������EAX�Ĵ�����ƫ���� */

	/* u_error's Error Code */
	/* 1~32 ����linux ���ں˴����е�/usr/include/asm/errno.h, ����for V6++ */
	enum ErrorCode
	{
		U_NOERROR = 0,	/* No error */
		U_EPERM = 1,	/* Operation not permitted */
		U_ENOENT = 2,	/* No such file or directory */
		U_ESRCH = 3,	/* No such process */
		U_EINTR = 4,	/* Interrupted system call */
		U_EIO = 5,	/* I/O error */
		U_ENXIO = 6,	/* No such device or address */
		U_E2BIG = 7,	/* Arg list too long */
		U_ENOEXEC = 8,	/* Exec format error */
		U_EBADF = 9,	/* Bad file number */
		U_ECHILD = 10,	/* No child processes */
		U_EAGAIN = 11,	/* Try again */
		U_ENOMEM = 12,	/* Out of memory */
		U_EACCES = 13,	/* Permission denied */
		U_EFAULT = 14,	/* Bad address */
		U_ENOTBLK = 15,	/* Block device required */
		U_EBUSY = 16,	/* Device or resource busy */
		U_EEXIST = 17,	/* File exists */
		U_EXDEV = 18,	/* Cross-device link */
		U_ENODEV = 19,	/* No such device */
		U_ENOTDIR = 20,	/* Not a directory */
		U_EISDIR = 21,	/* Is a directory */
		U_EINVAL = 22,	/* Invalid argument */
		U_ENFILE = 23,	/* File table overflow */
		U_EMFILE = 24,	/* Too many open files */
		U_ENOTTY = 25,	/* Not a typewriter(terminal) */
		U_ETXTBSY = 26,	/* Text file busy */
		U_EFBIG = 27,	/* File too large */
		U_ENOSPC = 28,	/* No space left on device */
		U_ESPIPE = 29,	/* Illegal seek */
		U_EROFS = 30,	/* Read-only file system */
		U_EMLINK = 31,	/* Too many links */
		U_EPIPE = 32,	/* Broken pipe */
		U_ENOSYS = 100,
		//EFAULT	= 106
	};

	static const int NSIG = 32;	/* �źŸ��� */

	/* p_sig�н��ܵ����źŶ��� */
	//ȡ���ź�
public:
	//ֻ��һ������
	/* ϵͳ������س�Ա */
	unsigned int	u_ar0[5];		/* ָ�����ջ�ֳ���������EAX�Ĵ���
								��ŵ�ջ��Ԫ�����ֶδ�Ÿ�ջ��Ԫ�ĵ�ַ��
								��V6��r0���ϵͳ���õķ���ֵ���û�����
								x86ƽ̨��ʹ��EAX��ŷ���ֵ�����u.u_ar0[R0] */

	long u_arg[5];				/* ��ŵ�ǰϵͳ���ò��� */
	string u_dirp;				/* ϵͳ���ò���(һ������Pathname)��ָ�� */


	/* �ļ�ϵͳ��س�Ա */
	Inode* u_cdir;		/* ָ��ǰĿ¼��Inodeָ�� */
	Inode* u_pdir;		/* ָ��Ŀ¼��Inodeָ�� */

	DirectoryEntry u_dent;					/* ��ǰĿ¼��Ŀ¼�� */
	char u_dbuf[DirectoryEntry::DIRSIZ];	/* ��ǰ·������ */
	string curDirPath;					/* ��ǰ����Ŀ¼����·�� */

	ErrorCode u_error;			/* ��Ŵ����� */

	/* �ļ�ϵͳ��س�Ա */
	OpenFiles u_ofiles;		/* ���̴��ļ������������ */

	/* �ļ�I/O���� */
	IOParameter u_IOParam;	/* ��¼��ǰ����д�ļ���ƫ�������û�Ŀ�������ʣ���ֽ������� */

	FileManager* fileManager;

	string ls;
	/* Member Functions */

public:
	User();
	~User();

	void Ls();
	void Cd(string dirName);
	void Mkdir(string dirName);
	void Create(string fileName, string mode);
	void Delete(string fileName);
	void Open(string fileName, string mode);
	void Close(string fd);
	void Seek(string fd, string offset, string origin);
	void Write(string fd, string inFile, string size);
	void Read(string fd, string outFile, string size);
	void fin(string file, string inFile);
	void fout(string file, string outFile);

private:
	bool IsError();
	void EchoError(enum ErrorCode err);
	int INodeMode(string mode);
	int FileMode(string mode);
	bool checkPathName(string path);
};

#endif

