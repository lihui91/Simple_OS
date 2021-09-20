/*ϵͳ�ļ���������ʵ��ģ�飬��Ҫ��װ�ļ�ϵͳ�ж��ļ�����Ĳ������̣������
�ļ�ϵͳ���ʵľ���ϸ�ڡ��������ļ��������ļ����ر��ļ���Seek�ļ�ָ�롢
��ȡ�ļ���д���ļ���ɾ���ļ���ϵͳ����ʵ��*/


#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "FileSystem.h"
#include "OpenFileManager.h"
#include "File.h"

/*
 * �ļ�������(FileManager)
 * ��װ���ļ�ϵͳ�ĸ���ϵͳ�����ں���̬�´�����̣�
 * ����ļ���Open()��Close()��Read()��Write()�ȵ�
 * ��װ�˶��ļ�ϵͳ���ʵľ���ϸ�ڡ�
 */
class FileManager
{
public:
	/* Ŀ¼����ģʽ������NameI()���� */
	enum DirectorySearchMode
	{
		OPEN = 0,		/* �Դ��ļ���ʽ����Ŀ¼ */
		CREATE = 1,		/* ���½��ļ���ʽ����Ŀ¼ */
		DELETE = 2		/* ��ɾ���ļ���ʽ����Ŀ¼ */
	};

	/* Functions */
	//ֻ������Ҫ�Ĺ��ܺ���
public:
	/* Constructors */
	FileManager();
	/* Destructors */
	~FileManager();

	/*
	 * @comment Open()ϵͳ���ô������
	 */
	void Open();

	/*
	 * @comment Creat()ϵͳ���ô������
	 */
	void Creat();

	/*
	 * @comment Open()��Creat()ϵͳ���õĹ�������
	 */
	void Open1(Inode* pInode, int mode, int trf);

	/*
	 * @comment Close()ϵͳ���ô������
	 */
	void Close();

	/*
	 * @comment Seek()ϵͳ���ô������
	 */
	void Seek();


	/*
	 * @comment Read()ϵͳ���ô������
	 */
	void Read();

	/*
	 * @comment Write()ϵͳ���ô������
	 */
	void Write();

	/*
	 * @comment ��дϵͳ���ù������ִ���
	 */
	void Rdwr(enum File::FileFlags mode);


	/*
	 * @comment Ŀ¼��������·��ת��Ϊ��Ӧ��Inode��
	 * �����������Inode
	 */
	Inode* NameI(enum DirectorySearchMode mode);
	/*
	 * @comment ��Creat()ϵͳ����ʹ�ã�����Ϊ�������ļ������ں���Դ
	 */
	Inode* MakNode(unsigned int mode);

	/*
	 * @comment ��Ŀ¼��Ŀ¼�ļ�д��һ��Ŀ¼��
	 */
	void WriteDir(Inode* pInode);


	/* �ı䵱ǰ����Ŀ¼ */
	void ChDir();

	/* ȡ���ļ� */
	void UnLink();

	/* �������г���ǰINode�ڵ���ļ��� */
	void Ls();

public:
	/* ��Ŀ¼�ڴ�Inode */
	Inode* rootDirInode;

	/* ��ȫ�ֶ���g_FileSystem�����ã��ö���������ļ�ϵͳ�洢��Դ */
	FileSystem* m_FileSystem;

	/* ��ȫ�ֶ���g_InodeTable�����ã��ö������ڴ�Inode��Ĺ��� */
	InodeTable* m_InodeTable;

	/* ��ȫ�ֶ���g_OpenFileTable�����ã��ö�������ļ�����Ĺ��� */
	OpenFileTable* m_OpenFileTable;
};

#endif
