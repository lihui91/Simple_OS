/*ϵͳ�̿����ģ�飬��Ҫ����Ծ����ļ��Ĵ洢�ռ����
����SuperBlock �ռ�ռ�á�Inode�ռ�ֲ������ݿ����ռ�
�ֲ��Ĺ����ṩ���䡢����Inode�ڵ㡢���ݿ�ڵ��Լ�
��ʽ�������ļ��Ľӿ�*/



//ɾȥmountģ�飬���Ӹ�ʽ�������鼰�ļ�ϵͳ����
#include "FileSystem.h"
#include "User.h"
#include<ctime>

/*==============================class SuperBlock===================================*/
/* ϵͳȫ�ֳ�����SuperBlock���� */

extern DiskManager g_DiskManager;
extern BufferManager g_BufferManager;
extern SuperBlock g_SuperBlock;
extern InodeTable g_InodeTable;
extern User g_User;
SuperBlock::SuperBlock()
{
	//nothing to do here
}

SuperBlock::~SuperBlock()
{
	//nothing to do here
}

/*==============================class FileSystem===================================*/
FileSystem::FileSystem()
{
	//nothing to do here

	m_DeviceManager = &g_DiskManager;
	m_SuperBlock = &g_SuperBlock;
	m_BufferManager = &g_BufferManager;

	if (!m_DeviceManager->Exists())
	{
		FormatDevice();
	}
	else
	{
		LoadSuperBlock();
	}
}

FileSystem::~FileSystem()
{
	//nothing to do here
}
/* ��ʽ��SuperBlock */
void FileSystem::FormatSuperBlock()
{
	m_SuperBlock->s_isize = FileSystem::INODE_ZONE_SIZE;
	m_SuperBlock->s_fsize = FileSystem::DISK_SIZE;
	m_SuperBlock->s_nfree = 0;
	m_SuperBlock->s_free[0] = -1;
	m_SuperBlock->s_ninode = 0;
	m_SuperBlock->s_flock = 0;
	m_SuperBlock->s_ilock = 0;
	m_SuperBlock->s_fmod = 0;
	m_SuperBlock->s_ronly = 0;
	time((time_t*)&m_SuperBlock->s_time);
}

/* ��ʽ�������ļ�ϵͳ */
void FileSystem::FormatDevice()
{
	FormatSuperBlock();
	m_DeviceManager->open();

	//���ļ�����д��superblockռ�ݿռ䣬δ�����ļ���С
	m_DeviceManager->write(m_SuperBlock, sizeof(SuperBlock), 0);

	DiskInode emptyDINode, rootDINode;
	//��Ŀ¼DiskNode
	rootDINode.d_mode |= Inode::IALLOC | Inode::IFDIR;
	rootDINode.d_nlink = 1;
	m_DeviceManager->write(&rootDINode, sizeof(rootDINode));

	//�ӵ�1��DiskINode��ʼ������0���̶����ڸ�Ŀ¼"/"�����ɸı�
	for (int i = 1; i < FileSystem::INode_NUMBERS; ++i)
	{
		if (m_SuperBlock->s_ninode < SuperBlock::MAX_NINODE)
		{
			m_SuperBlock->s_inode[m_SuperBlock->s_ninode++] = i;
		}
		m_DeviceManager->write(&emptyDINode, sizeof(emptyDINode));
	}

	//�����̿��ʼ��
	char freeBlock[BLOCK_SIZE], freeBlock1[BLOCK_SIZE];
	memset(freeBlock, 0, BLOCK_SIZE);
	memset(freeBlock1, 0, BLOCK_SIZE);

	for (int i = 0; i < FileSystem::DATA_ZONE_SIZE; ++i)
	{
		if (m_SuperBlock->s_nfree >= SuperBlock::MAX_NFREE)
		{
			memcpy(freeBlock1, &m_SuperBlock->s_nfree, sizeof(int) + sizeof(m_SuperBlock->s_free));
			m_DeviceManager->write(&freeBlock1, BLOCK_SIZE);
			m_SuperBlock->s_nfree = 0;
		}
		else
		{
			m_DeviceManager->write(freeBlock, BLOCK_SIZE);
		}
		m_SuperBlock->s_free[m_SuperBlock->s_nfree++] = i + DATA_ZONE_START_SECTOR;
	}

	time((time_t*)&m_SuperBlock->s_time);
	//�ٴ�д��superblock
	m_DeviceManager->write(m_SuperBlock, sizeof(SuperBlock), 0);
}

void FileSystem::LoadSuperBlock()
{
	//ֻ��Ҫ���豸�ж��뼴��
	m_DeviceManager->read(m_SuperBlock, sizeof(SuperBlock), SUPERBLOCK_START_SECTOR * BLOCK_SIZE);
}


void FileSystem::Update()
{
	//int i;

	Buf* pBuf;

	/* ͬ��SuperBlock������ */

	/* ��SuperBlock�޸ı�־ */
	m_SuperBlock->s_fmod = 0;
	/* д��SuperBlock�����ʱ�� */
	m_SuperBlock->s_time = (int)time(NULL);

	/*
			 * Ϊ��Ҫд�ص�������ȥ��SuperBlock����һ�黺�棬���ڻ�����СΪ512�ֽڣ�
			 * SuperBlock��СΪ1024�ֽڣ�ռ��2��������������������Ҫ2��д�������
			 */
	for (int j = 0; j < 2; j++)
	{
		/* ��һ��pָ��SuperBlock�ĵ�0�ֽڣ��ڶ���pָ���512�ֽ� */
		int* p = (int*)m_SuperBlock + j * 128;

		/* ��Ҫд�뵽�豸dev�ϵ�SUPER_BLOCK_SECTOR_NUMBER + j������ȥ */
		pBuf = this->m_BufferManager->GetBlk(FileSystem::SUPERBLOCK_START_SECTOR + j);

		/* ��SuperBlock�е�0 - 511�ֽ�д�뻺���� */
		memcpy(pBuf->b_addr, p, BLOCK_SIZE);
		/* ���������е�����д�������� */
		this->m_BufferManager->Bwrite(pBuf);
	}

	/* ͬ���޸Ĺ����ڴ�Inode����Ӧ���Inode */
	g_InodeTable.UpdateInodeTable();

	/* ���ӳ�д�Ļ����д�������� */
	this->m_BufferManager->Bflush();
}

Inode* FileSystem::IAlloc()
{
	//SuperBlock *sb;
	Buf* pBuf;
	Inode* pNode;
	User& u = g_User;
	int ino; /* ���䵽�Ŀ������Inode��� */

	/* ��ȡ��Ӧ�豸��SuperBlock�ڴ渱�� */
	//ֻ��һ���豸��ֱ�Ӳ���superblock����

	/*
	 * SuperBlockֱ�ӹ���Ŀ���Inode�������ѿգ�
	 * ���뵽��������������Inode���ȶ�inode�б�������
	 * ��Ϊ�����³����л���ж��̲������ܻᵼ�½����л���
	 * ���������п��ܷ��ʸ����������ᵼ�²�һ���ԡ�
	 */
	 /* ����ȡ���̿���Ϊ�㣬���ʾ�ѷ��価���еĿ��д��̿顣*/

	if (m_SuperBlock->s_ninode <= 0)
	{
		/* ����Inode���������� */
		/* ���Inode��Ŵ�0��ʼ���ⲻͬ��Unix V6�����Inode��1��ʼ��� */
		ino = -1;

		/* ���ζ������Inode���еĴ��̿飬�������п������Inode���������Inode������ */
		for (int i = 0; i < m_SuperBlock->s_isize; i++)
		{
			pBuf = this->m_BufferManager->Bread(FileSystem::INODE_ZONE_START_SECTOR + i);

			/* ��ȡ��������ַ */
			int* p = (int*)pBuf->b_addr;

			/* ���û�������ÿ�����Inode��i_mode != 0����ʾ�Ѿ���ռ�� */
			for (int j = 0; j < FileSystem::INODE_NUMBER_PER_SECTOR; j++)
			{
				ino++;

				int mode = *(p + j * sizeof(DiskInode) / sizeof(int));

				/* �����Inode�ѱ�ռ�ã����ܼ������Inode������ */
				if (mode != 0)
				{
					continue;
				}

				/*
				 * ������inode��i_mode==0����ʱ������ȷ��
				 * ��inode�ǿ��еģ���Ϊ�п������ڴ�inodeû��д��
				 * ������,����Ҫ���������ڴ�inode���Ƿ�����Ӧ����
				 */
				if (g_InodeTable.IsLoaded(ino) == -1)
				{
					/* �����Inodeû�ж�Ӧ���ڴ濽��������������Inode������ */
					m_SuperBlock->s_inode[m_SuperBlock->s_ninode++] = ino;

					/* ��������������Ѿ�װ�����򲻼������� */
					if (m_SuperBlock->s_ninode >= SuperBlock::MAX_NINODE)
					{
						break;
					}
				}
			}

			/* �����Ѷ��굱ǰ���̿飬�ͷ���Ӧ�Ļ��� */
			this->m_BufferManager->Brelse(pBuf);

			/* ��������������Ѿ�װ�����򲻼������� */
			if (m_SuperBlock->s_ninode >= SuperBlock::MAX_NINODE)
			{
				break;
			}
		}
		/* ����Կ������Inode�����������������Ϊ�ȴ�����˯�ߵĽ��� */

		/* ����ڴ�����û���������κο������Inode������NULL */
		if (m_SuperBlock->s_ninode <= 0)
		{
			printf("No space\n");
			u.u_error = User::U_ENOSPC;
			return NULL;
		}
	}

	/*
	 * ���沿���Ѿ���֤������ϵͳ��û�п������Inode��
	 * �������Inode�������бض����¼�������Inode�ı�š�
	 */

	ino = m_SuperBlock->s_inode[--m_SuperBlock->s_ninode];
	pNode = g_InodeTable.IGet(ino);
	if (NULL == pNode) {
		printf("�޿����ڴ�INode");
		return NULL;
	}

	pNode->Clean();

	m_SuperBlock->s_fmod = 1;
	return pNode;
	//return NULL; /* GCC likes it! */
}

void FileSystem::IFree(int number)
{
	//ֻ��һ���豸��ֱ�Ӳ���superblock
	/*
	 * ���������ֱ�ӹ���Ŀ���Inode��������
	 * ���ͷŵ����Inodeɢ���ڴ���Inode���С�
	 */


	 /*
	  * ���������ֱ�ӹ���Ŀ������Inode����100����
	  * ͬ�����ͷŵ����Inodeɢ���ڴ���Inode���С�
	  */
	if (m_SuperBlock->s_ninode >= SuperBlock::MAX_NINODE)
	{
		return;
	}

	m_SuperBlock->s_inode[m_SuperBlock->s_ninode++] = number;

	/* ����SuperBlock���޸ı�־ */
	m_SuperBlock->s_fmod = 1;
}

Buf* FileSystem::Alloc()
{
	int blkno; /* ���䵽�Ŀ��д��̿��� */
	//SuperBlock *sb;
	Buf* pBuf;
	User& u = g_User;

	/* ��ȡSuperBlock������ڴ渱�� */
	//ֱ�Ӳ���superblock���ɣ�ֻ��һ���豸

	/*
	 * ������д��̿����������ڱ���������������������
	 * ���ڲ������д��̿����������������������ͨ��
	 * ������������̵���Free()��Alloc()��ɵġ�
	 */

	 /* ��������ջ������ȡ���д��̿��� */
	blkno = m_SuperBlock->s_free[--m_SuperBlock->s_nfree];

	/*
	 * ����ȡ���̿���Ϊ�㣬���ʾ�ѷ��価���еĿ��д��̿顣
	 * ���߷��䵽�Ŀ��д��̿��Ų����������̿�������(��BadBlock()���)��
	 * ����ζ�ŷ�����д��̿����ʧ�ܡ�
	 */
	if (0 >= blkno)
	{
		m_SuperBlock->s_nfree = 0;
		printf("no space on device\n");
		u.u_error = User::U_ENOSPC;
		return NULL;
	}


	/*
	 * ջ�ѿգ��·��䵽���д��̿��м�¼����һ����д��̿�ı��,
	 * ����һ����д��̿�ı�Ŷ���SuperBlock�Ŀ��д��̿�������s_free[100]�С�
	 */
	if (m_SuperBlock->s_nfree <= 0)
	{
		/* ����ÿ��д��̿� */
		pBuf = this->m_BufferManager->Bread(blkno);

		/* �Ӹô��̿��0�ֽڿ�ʼ��¼����ռ��4(s_nfree)+400(s_free[100])���ֽ� */
		int* p = (int*)pBuf->b_addr;

		/* ���ȶ��������̿���s_nfree */
		m_SuperBlock->s_nfree = *p++;

		/* ��ȡ�����к���λ�õ����ݣ�д�뵽SuperBlock�����̿�������s_free[100]�� */
		memcpy(m_SuperBlock->s_free, p, sizeof(m_SuperBlock->s_free));
		/* ����ʹ����ϣ��ͷ��Ա㱻��������ʹ�� */
		this->m_BufferManager->Brelse(pBuf);

		/* ����Կ��д��̿������������������Ϊ�ȴ�����˯�ߵĽ��� */
		//ֻ��һ������
	}

	/* ��ͨ����³ɹ����䵽һ���д��̿� */
	pBuf = this->m_BufferManager->GetBlk(blkno); /* Ϊ�ô��̿����뻺�� */
	if (pBuf)
	{
		this->m_BufferManager->ClrBuf(pBuf);			  /* ��ջ����е����� */
	}
	this->m_SuperBlock->s_fmod = 1;									  /* ����SuperBlock���޸ı�־ */

	return pBuf;
}

void FileSystem::Free(int blkno)
{
	//SuperBlock *sb;
	Buf* pBuf;
	User& u = g_User;


	/*
	 * ��������SuperBlock���޸ı�־���Է�ֹ���ͷ�
	 * ���̿�Free()ִ�й����У���SuperBlock�ڴ渱��
	 * ���޸Ľ�������һ�룬�͸��µ�����SuperBlockȥ
	 */

	 /* SuperBlock��ֱ�ӹ�����д��̿�ŵ�ջ���� */
	if (m_SuperBlock->s_nfree >= SuperBlock::MAX_NINODE)
	{
		m_SuperBlock->s_flock++;

		/*
		 * ʹ�õ�ǰFree()������Ҫ�ͷŵĴ��̿飬���ǰһ��100������
		 * ���̿��������
		 */
		pBuf = this->m_BufferManager->GetBlk(blkno); /* Ϊ��ǰ��Ҫ�ͷŵĴ��̿���仺�� */

		/* �Ӹô��̿��0�ֽڿ�ʼ��¼����ռ��4(s_nfree)+400(s_free[100])���ֽ� */
		int* p = (int*)pBuf->b_addr;

		/* ����д������̿��������˵�һ��Ϊ99�飬����ÿ�鶼��100�� */
		*p++ = m_SuperBlock->s_nfree;
		/* ��SuperBlock�Ŀ����̿�������s_free[100]д�뻺���к���λ�� */
		//Utility::DWordCopy(sb->s_free, p, 100);
		memcpy(p, m_SuperBlock->s_free, sizeof(int) * SuperBlock::MAX_NFREE);
		m_SuperBlock->s_nfree = 0;
		/* ����ſ����̿�������ġ���ǰ�ͷ��̿顱д����̣���ʵ���˿����̿��¼�����̿�ŵ�Ŀ�� */
		this->m_BufferManager->Bwrite(pBuf);

	}
	m_SuperBlock->s_free[m_SuperBlock->s_nfree++] = blkno; /* SuperBlock�м�¼�µ�ǰ�ͷ��̿�� */
	m_SuperBlock->s_fmod = 1;
}
