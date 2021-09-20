/*���ļ�����ģ�飬����Դ��ļ��Ĺ���Ϊ�û����ļ�
�������ݽṹ֮��Ĺ�����ϵ��Ϊ�û��ṩֱ�Ӳ����ļ����ļ��������ӿ�*/


//���ļ��Ķ��������ݣ�ɾȥ�ܵ����ֵĴ��룬�������ʼ��format����
#include "OpenFileManager.h"
#include "User.h"
#include<ctime>

/* �����ʵ������*/
extern BufferManager g_BufferManager;
extern FileSystem g_FileSystem;
extern InodeTable g_InodeTable;
extern User g_User;

/*==============================class OpenFileTable===================================*/
/* ϵͳȫ�ִ��ļ������ʵ���Ķ��� */

OpenFileTable::OpenFileTable()
{
	//nothing to do here
}

OpenFileTable::~OpenFileTable()
{
	//nothing to do here
}

/*���ã����̴��ļ������������ҵĿ�����  ֮ �±�  д�� u_ar0[EAX]*/
File* OpenFileTable::FAlloc()//�˴������޸�
{
	int fd;
	//User& u = Kernel::Instance().GetUser();
	User& u = g_User;
	/* �ڽ��̴��ļ����������л�ȡһ�������� */
	fd = u.u_ofiles.AllocFreeSlot();

	if (fd < 0) /* ���Ѱ�ҿ�����ʧ�� */
	{
		return NULL;
	}

	for (int i = 0; i < OpenFileTable::NFILE; i++)
	{
		/* f_count==0��ʾ������� */
		if (this->m_File[i].f_count == 0)
		{
			/* ������������File�ṹ�Ĺ�����ϵ */
			u.u_ofiles.SetF(fd, &this->m_File[i]);
			/* ���Ӷ�file�ṹ�����ü��� */
			this->m_File[i].f_count++;
			/* ����ļ�����дλ�� */
			this->m_File[i].f_offset = 0;
			return (&this->m_File[i]);
		}
	}

	u.u_error = User::U_ENFILE;
	return NULL;
}

void OpenFileTable::CloseF(File* pFile)
{
	//ɾȥ�ܵ�����

	/* ���õ�ǰFile�Ľ�������1 */
	pFile->f_count--;
	//������õ�����С��0����رշŻ�
	if (pFile->f_count <= 0)
		g_InodeTable.IPut(pFile->f_inode);
}
/*���ļ���ĳ�ʼ��*/
void OpenFileTable::Format()
{
	File init;//�ն���
	for (int i = 0; i < OpenFileTable::NFILE; ++i)
	{
		memcpy(m_File + i, &init, sizeof(File));
	}
}

/*==============================class InodeTable===================================*/
/*  �����ڴ�Inode���ʵ�� */
InodeTable::InodeTable()
{
	//nothing to do here
	this->m_FileSystem = &g_FileSystem;
}

InodeTable::~InodeTable()
{
	//nothing to do here
}

Inode* InodeTable::IGet(int inumber)
{
	Inode* pInode;

	/* ���ָ���豸dev�б��Ϊinumber�����Inode�Ƿ����ڴ濽�� */
	int index = this->IsLoaded(inumber);
	if (index >= 0) /* �ҵ��ڴ濽�� */
	{
		pInode = &(this->m_Inode[index]);
		/* ������ڴ�Inode������ */
		//ֻ��һ���û���û����
		++pInode->i_count;
		return pInode;
	}

	/* ������ڴ�Inode�����������ļ�ϵͳ�����Ҹ�Inode��Ӧ��Mountװ��� */
	//û�����ļ�ϵͳ

	/* û��Inode���ڴ濽���������һ�������ڴ�Inode */

	pInode = this->GetFreeInode();
	/* ���ڴ�Inode���������������Inodeʧ�� */
	if (NULL == pInode)
	{
		printf("Inode Table Overflow !\n");
		g_User.u_error = User::U_ENFILE;
		return NULL;
	}
	/* �������Inode�ɹ��������Inode�����·�����ڴ�Inode */

	/* �����µ��豸�š����Inode��ţ��������ü������������ڵ����� */

	pInode->i_number = inumber;

	pInode->i_count++;

	BufferManager& bm = g_BufferManager;
	/* �������Inode���뻺���� */
	Buf* pBuf = bm.Bread(FileSystem::INODE_ZONE_START_SECTOR + inumber / FileSystem::INODE_NUMBER_PER_SECTOR);

	/* �������I/O���� */

	/* ���������е����Inode��Ϣ�������·�����ڴ�Inode�� */
	pInode->ICopy(pBuf, inumber);
	/* �ͷŻ��� */
	bm.Brelse(pBuf);
	return pInode;

}

/* close�ļ�ʱ�����Iput
 *      ��Ҫ���Ĳ������ڴ�i�ڵ���� i_count--����Ϊ0���ͷ��ڴ� i�ڵ㡢���иĶ�д�ش���
 * �����ļ�;��������Ŀ¼�ļ������������󶼻�Iput���ڴ�i�ڵ㡣·�����ĵ�����2��·������һ���Ǹ�
 *   Ŀ¼�ļ�������������д������ļ�������ɾ��һ�������ļ���������������д���ɾ����Ŀ¼����ô
 *   	���뽫���Ŀ¼�ļ�����Ӧ���ڴ� i�ڵ�д�ش��̡�
 *   	���Ŀ¼�ļ������Ƿ��������ģ����Ǳ��뽫����i�ڵ�д�ش��̡�
 * */
void InodeTable::IPut(Inode* pNode)
{
	/* ��ǰ����Ϊ���ø��ڴ�Inode��Ψһ���̣���׼���ͷŸ��ڴ�Inode */
	if (pNode->i_count == 1)
	{
		/*
		 * ��������Ϊ�������ͷŹ����п�����Ϊ���̲�����ʹ�øý���˯�ߣ�
		 * ��ʱ�п�����һ�����̻�Ը��ڴ�Inode���в������⽫�п��ܵ��´���
		 */
		 //û����

		 /* ���ļ��Ѿ�û��Ŀ¼·��ָ���� */
		if (pNode->i_nlink <= 0)
		{
			/* �ͷŸ��ļ�ռ�ݵ������̿� */
			pNode->ITrunc();
			pNode->i_mode = 0;
			/* �ͷŶ�Ӧ�����Inode */
			this->m_FileSystem->IFree(pNode->i_number);
		}

		/* �������Inode��Ϣ */
		pNode->IUpdate((int)time(NULL));
		/* ����ڴ�Inode�����б�־λ */
		pNode->i_flag = 0;
		/* �����ڴ�inode���еı�־֮һ����һ����i_count == 0 */
		pNode->i_number = -1;
	}

	/* �����ڴ�Inode�����ü��������ѵȴ����� */
	pNode->i_count--;
}

void InodeTable::UpdateInodeTable()
{
	for (int i = 0; i < InodeTable::NINODE; i++)
	{
		/*
		 * ���Inode����û�б�����������ǰδ����������ʹ�ã�����ͬ�������Inode��
		 * ����count������0��count == 0��ζ�Ÿ��ڴ�Inodeδ���κδ��ļ����ã�����ͬ����
		 */
		if (this->m_Inode[i].i_count != 0)
		{
			this->m_Inode[i].IUpdate((int)time(NULL));
		}
	}
}

int InodeTable::IsLoaded(int inumber)
{
	/* Ѱ��ָ�����Inode���ڴ濽�� */
	for (int i = 0; i < InodeTable::NINODE; i++)
	{
		if (this->m_Inode[i].i_number == inumber && this->m_Inode[i].i_count != 0)
		{
			return i;
		}
	}
	return -1;
}

Inode* InodeTable::GetFreeInode()
{
	for (int i = 0; i < InodeTable::NINODE; i++)
	{
		/* ������ڴ�Inode���ü���Ϊ�㣬���Inode��ʾ���� */
		if (this->m_Inode[i].i_count == 0)
		{
			return &(this->m_Inode[i]);
		}
	}
	return NULL; /* Ѱ��ʧ�� */
}

void InodeTable::Format()//��ʽ��inode��
{
	Inode init;//�ն���
	for (int i = 0; i < InodeTable::NINODE; ++i)
	{
		memcpy(m_Inode + i, &init, sizeof(Inode));
	}
}