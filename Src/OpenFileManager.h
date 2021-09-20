/*���ļ�����ģ�飬����Դ��ļ��Ĺ���Ϊ�û����ļ�
�������ݽṹ֮��Ĺ�����ϵ��Ϊ�û��ṩֱ�Ӳ����ļ����ļ��������ӿ�*/

//���ļ�����Ķ�
#ifndef OPEN_FILE_MANAGER_H
#define OPEN_FILE_MANAGER_H

#include "INode.h"
#include "File.h"
#include "FileSystem.h"


/* ����2������ʵ�������ڵ�OpenFileManager.cpp�ļ��� */
//extern InodeTable g_IodeTable;
//extern OpenFileTable g_OpenFileTable;

/*
 * ���ļ�������(OpenFileManager)����
 * �ں��жԴ��ļ������Ĺ���Ϊ����
 * ���ļ������ں����ݽṹ֮��Ĺ���
 * ��ϵ��
 * ������ϵָ����u���д��ļ�������ָ��
 * ���ļ����е�File���ļ����ƽṹ��
 * �Լ���File�ṹָ���ļ���Ӧ���ڴ�Inode��
 */
class OpenFileTable
{
public:
	/* static consts */
	//static const int NINODE	= 100;	/* �ڴ�Inode������ */
	static const int NFILE = 100;	/* ���ļ����ƿ�File�ṹ������ */

	/* Functions */
public:
	/* Constructors */
	OpenFileTable();
	/* Destructors */
	~OpenFileTable();

	// /* 
	 // * @comment �����û�ϵͳ�����ṩ���ļ�����������fd��
	 // * �ҵ���Ӧ�Ĵ��ļ����ƿ�File�ṹ
	 // */
	// File* GetF(int fd);
	/*
	 * @comment ��ϵͳ���ļ����з���һ�����е�File�ṹ
	 */
	File* FAlloc();
	/*
	 * @comment �Դ��ļ����ƿ�File�ṹ�����ü���f_count��1��
	 * �����ü���f_countΪ0�����ͷ�File�ṹ��
	 */
	void CloseF(File* pFile);

	/*
	* ��ʽ���ļ���
	*/
	void Format();
	/* Members */

public:
	File m_File[NFILE];			/* ϵͳ���ļ���Ϊ���н��̹������̴��ļ���������
								�а���ָ����ļ����ж�ӦFile�ṹ��ָ�롣*/
};

/*
 * �ڴ�Inode��(class InodeTable)
 * �����ڴ�Inode�ķ�����ͷš�
 */
class InodeTable
{
	/* static consts */
public:
	static const int NINODE = 100;	/* �ڴ�Inode������ */

	/* Functions */
public:
	/* Constructors */
	InodeTable();
	/* Destructors */
	~InodeTable();

	/*
	 * @comment ��ʼ����g_FileSystem���������
	 * �ŵ��˹��캯������ʹ��
	 */
	 //void Initialize();
	 /*
	  * @comment ����ָ���豸��dev�����Inode��Ż�ȡ��Ӧ
	  * Inode�������Inode�Ѿ����ڴ��У��������������ظ��ڴ�Inode��
	  * ��������ڴ��У���������ڴ�����������ظ��ڴ�Inode
	  * ֻ��һ���豸�����ȡ��dev�Ĳ���
	  */
	Inode* IGet(int inumber);
	/*
	 * @comment ���ٸ��ڴ�Inode�����ü����������Inode�Ѿ�û��Ŀ¼��ָ������
	 * ���޽������ø�Inode�����ͷŴ��ļ�ռ�õĴ��̿顣
	 */
	void IPut(Inode* pNode);

	/*
	 * @comment �����б��޸Ĺ����ڴ�Inode���µ���Ӧ���Inode��
	 */
	void UpdateInodeTable();

	/*
	 * @comment ����豸dev�ϱ��Ϊinumber�����inode�Ƿ����ڴ濽����
	 * ������򷵻ظ��ڴ�Inode���ڴ�Inode���е�����
	 */
	int IsLoaded(int inumber);
	/*
	 * @comment ���ڴ�Inode����Ѱ��һ�����е��ڴ�Inode
	 */
	Inode* GetFreeInode();

	/* Members */
	void Format();
public:
	Inode m_Inode[NINODE];		/* �ڴ�Inode���飬ÿ�����ļ�����ռ��һ���ڴ�Inode */

	FileSystem* m_FileSystem;	/* ��ȫ�ֶ���g_FileSystem������ */
};

#endif
