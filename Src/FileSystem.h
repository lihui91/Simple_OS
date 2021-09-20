/*ϵͳ�̿����ģ�飬��Ҫ����Ծ����ļ��Ĵ洢�ռ����
����SuperBlock �ռ�ռ�á�Inode�ռ�ֲ������ݿ����ռ�
�ֲ��Ĺ����ṩ���䡢����Inode�ڵ㡢���ݿ�ڵ��Լ�
��ʽ�������ļ��Ľӿ�*/

#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "INode.h"
#include "DiskManager.h"
#include "BufferManager.h"

/*
 * �ļ�ϵͳ�洢��Դ�����(Super Block)�Ķ��塣
 */
class SuperBlock
{
	/* Functions */
public:
	static const int MAX_NFREE = 100;
	static const int MAX_NINODE = 100;
	/* Constructors */
	SuperBlock();
	/* Destructors */
	~SuperBlock();

	/* Members */
public:
	int		s_isize;		/* ���Inode��ռ�õ��̿��� */
	int		s_fsize;		/* �̿����� */
	int		s_nfree;		/* ֱ�ӹ���Ŀ����̿����� */
	int		s_free[100];	/* ֱ�ӹ���Ŀ����̿������� */

	int		s_ninode;		/* ֱ�ӹ���Ŀ������Inode���� */
	int		s_inode[100];	/* ֱ�ӹ���Ŀ������Inode������ */

	int		s_flock;		/* ���������̿��������־ */
	int		s_ilock;		/* ��������Inode���־ */

	int		s_fmod;			/* �ڴ���super block�������޸ı�־����ζ����Ҫ��������Ӧ��Super Block */
	int		s_ronly;		/* ���ļ�ϵͳֻ�ܶ��� */
	int		s_time;			/* ���һ�θ���ʱ�� */
	int		padding[47];	/* ���ʹSuperBlock���С����1024�ֽڣ�ռ��2������ */
};

class DirectoryEntry {
public:
	static const int DIRSIZ = 28;	/* Ŀ¼����·�����ֵ�����ַ������� */

public:
	int m_ino;		    /* Ŀ¼����INode��Ų��� */
	char name[DIRSIZ];	/* Ŀ¼����·�������� */

};


/*
 * �ļ�ϵͳ��(FileSystem)�����ļ��洢�豸��
 * �ĸ���洢��Դ�����̿顢���INode�ķ��䡢
 * �ͷš�
 */
class FileSystem
{
public:
	/* static consts */
	static const int BLOCK_SIZE = 512;	/* Block���С*/
	static const int DISK_SIZE = 16384; /* ����������������*/
	static const int SUPERBLOCK_START_SECTOR = 0;/*����SuperBlockλ�ڴ����ϵ������ţ�ռ����������*/
	static const int INODE_ZONE_START_SECTOR = 2;/*���INode��λ�ڴ����ϵ���ʼ������*/
	static const int INODE_ZONE_SIZE = 1022;/*���������INode��ռ�ݵ�������*/
	static const int INODE_NUMBER_PER_SECTOR = BLOCK_SIZE / sizeof(DiskInode); /*���INode���󳤶�Ϊ64�ֽڣ�ÿ�����̿���Դ��512/64 = 8�����INode*/
	static const int ROOT_INODE_NO = 0;/*�ļ�ϵͳ��Ŀ¼���INode���*/
	static const int INode_NUMBERS = INODE_ZONE_SIZE * INODE_NUMBER_PER_SECTOR;/*���INode���ܸ���*/
	static const int DATA_ZONE_START_SECTOR = INODE_ZONE_START_SECTOR + INODE_ZONE_SIZE;/* ����������ʼ������*/
	static const int DATA_ZONE_END_SECTOR = DISK_SIZE - 1; /*�����������������*/
	static const int DATA_ZONE_SIZE = DISK_SIZE - DATA_ZONE_START_SECTOR;/* ������ռ�ݵ���������*/

	/* Functions */
public:
	/* Constructors */
	FileSystem();
	/* Destructors */
	~FileSystem();


	/*
	* @comment ϵͳ��ʼ��ʱ����SuperBlock
	*/
	void LoadSuperBlock();

	/*
	 * @comment ��SuperBlock������ڴ渱�����µ�
	 * �洢�豸��SuperBlock��ȥ
	 */
	void Update();

	/*
	 * @comment  �ڴ洢�豸dev�Ϸ���һ������
	 * ���INode��һ�����ڴ����µ��ļ���
	 */
	Inode* IAlloc();
	/*
	 * @comment  �ͷŴ洢�豸dev�ϱ��Ϊnumber
	 * �����INode��һ������ɾ���ļ���
	 */
	void IFree(int number);

	/*
	 * @comment �ڴ洢�豸dev�Ϸ�����д��̿�
	 */
	Buf* Alloc();
	/*
	 * @comment �ͷŴ洢�豸dev�ϱ��Ϊblkno�Ĵ��̿�
	 */
	void Free(int blkno);

	/* ��ʽ��SuperBlock */
	void FormatSuperBlock();

	/* ��ʽ�������ļ�ϵͳ */
	void FormatDevice();


private:
	BufferManager* m_BufferManager;		/* FileSystem����Ҫ�������ģ��(BufferManager)�ṩ�Ľӿ� */
	DiskManager* m_DeviceManager;
	SuperBlock* m_SuperBlock;
};

#endif
