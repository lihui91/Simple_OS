/*���ٻ������ģ�飬��Ҫ�������ϵͳ�����еĻ���飬����
���롢�ͷš���д�����һ�黺��Ĺ��ܺ����ӿڣ�
�Լ�ϵͳ�˳�ʱˢ�����л����*/

#include "BufferManager.h"

extern DiskManager g_DiskManager;

BufferManager::BufferManager()
{
	//nothing to do here
	this->bFreeList = new Buf;
	InitList();
	m_DiskManager = &g_DiskManager;
}

BufferManager::~BufferManager()
{
	//nothing to do here
	this->Bflush();
	delete this->bFreeList;
}

/* ����һ�黺��*/

Buf* BufferManager::GetBlk(int blkno)
{
	Buf* bp;

	/*
	 * ����豸�������Ѿ�������Ӧ���棬�򷵻ظû��棻
	 * ��������ɶ����з����µĻ��������ַ����д��
	 */
	if (map.find(blkno) != map.end()) {
		bp = map[blkno];
		DetachNode(bp);
		return bp;
	}

	bp = bFreeList->b_back;
	if (bp == bFreeList) {
		printf("��Bufferʹ��\n");;
		return NULL;
	}
	DetachNode(bp);
	map.erase(bp->b_blkno);
	//�ͷŻ�����У�ʹ��map������ÿ�α���һ�λ������
	//�ӳ�д
	if (bp->b_flags & Buf::B_DELWRI) {
		m_DiskManager->write(bp->b_addr, BUFFER_SIZE, bp->b_blkno * BUFFER_SIZE);
	}
	bp->b_flags &= ~(Buf::B_DELWRI | Buf::B_DONE);
	bp->b_blkno = blkno;
	map[blkno] = bp;
	return bp;

}

void BufferManager::Brelse(Buf* bp)
{
	//�ͷ�һ������飬��������β��
	InsertTail(bp);
}

Buf* BufferManager::Bread(int blkno)
{
	Buf* bp;
	/* �����豸�ţ��ַ�������뻺�� */
	bp = this->GetBlk(blkno);
	/* ������豸�������ҵ����軺�棬��B_DONE�����ã��Ͳ������I/O���� */
	if (bp->b_flags & (Buf::B_DONE | Buf::B_DELWRI)) {
		return bp;
	}
	/* û���ҵ���Ӧ���棬����I/O������� */
	this->m_DiskManager->read(bp->b_addr, BUFFER_SIZE, bp->b_blkno * BUFFER_SIZE);
	/*
	 * ��I/O�����������Ӧ�豸I/O������У���������I/O����������ִ�б���I/O����
	 * ����ȴ���ǰI/O����ִ����Ϻ����жϴ����������ִ�д�����
	 * ע��Strategy()������I/O����������豸������к󣬲���I/O����ִ����ϣ���ֱ�ӷ��ء�
	 */

	 /* ͬ�������ȴ�I/O�������� */


	bp->b_flags |= Buf::B_DONE;
	return bp;
}

void BufferManager::Bwrite(Buf* bp)
{
	bp->b_flags &= ~(Buf::B_DELWRI);
	this->m_DiskManager->write(bp->b_addr, BUFFER_SIZE, bp->b_blkno * BUFFER_SIZE);
	bp->b_flags |= (Buf::B_DONE);
	this->Brelse(bp);

	/*
	 * ��������ӳ�д��������󣻷��򲻼�顣
	 * ������Ϊ����ӳ�д������п��ܵ�ǰ���̲���
	 * ������һ�����Ľ��̣�����GetError()��Ҫ��
	 * ����ǰ���̸��ϴ����־��
	 */


	return;
}

void BufferManager::Bdwrite(Buf* bp)
{
	/* �ӳ�д���̿� */
	/* ����B_DONE������������ʹ�øô��̿����� */
	bp->b_flags |= (Buf::B_DELWRI | Buf::B_DONE);
	this->Brelse(bp);
	return;
}


void BufferManager::ClrBuf(Buf* bp)
{
	/* ������������������ */
	memset(bp->b_addr, 0, BufferManager::BUFFER_SIZE);
	return;
}

void BufferManager::Bflush()
{
	Buf* bp;
	/* ע�⣺����֮����Ҫ��������һ����֮�����¿�ʼ������
	 * ��Ϊ��bwite()���뵽����������ʱ�п��жϵĲ���������
	 * �ȵ�bwriteִ����ɺ�CPU�Ѵ��ڿ��ж�״̬�����Ժ�
	 * �п��������ڼ���������жϣ�ʹ��bfreelist���г��ֱ仯��
	 * �����������������������������¿�ʼ������ô�ܿ�����
	 * ����bfreelist���е�ʱ����ִ���
	 */
	for (int i = 0; i < NBUF; ++i) {
		bp = m_Buf + i;
		if ((bp->b_flags & Buf::B_DELWRI)) {
			bp->b_flags &= ~(Buf::B_DELWRI);
			this->m_DiskManager->write(bp->b_addr, BUFFER_SIZE, bp->b_blkno * BUFFER_SIZE);
			bp->b_flags |= (Buf::B_DONE);
		}
	}
}
/* ����LRU Cache �㷨��ÿ�δ�ͷ��ȡ����ʹ�ú�ŵ�β��
*/
void BufferManager::DetachNode(Buf* bp) {
	if (bp->b_back == NULL) {
		return;
	}
	bp->b_forw->b_back = bp->b_back;
	bp->b_back->b_forw = bp->b_forw;
	bp->b_back = NULL;
	bp->b_forw = NULL;
}

void BufferManager::InsertTail(Buf* bp) {
	if (bp->b_back != NULL) {
		return;
	}
	bp->b_forw = bFreeList->b_forw;
	bp->b_back = bFreeList;
	bFreeList->b_forw->b_back = bp;
	bFreeList->b_forw = bp;
}

void BufferManager::FormatBuffer() {
	Buf emptyBuffer;
	for (int i = 0; i < NBUF; ++i) {
		memcpy(this->m_Buf + i, &emptyBuffer, sizeof(Buf));
	}
	InitList();
}

void BufferManager::InitList() {
	for (int i = 0; i < NBUF; ++i) {
		if (i) {
			this->m_Buf[i].b_forw = this->m_Buf + i - 1;
		}
		else {
			this->m_Buf[i].b_forw = bFreeList;
			bFreeList->b_back = this->m_Buf + i;
		}

		if (i + 1 < NBUF) {
			this->m_Buf[i].b_back = this->m_Buf + i + 1;
		}
		else {
			this->m_Buf[i].b_back = bFreeList;
			bFreeList->b_forw = this->m_Buf + i;
		}
		this->m_Buf[i].b_addr = Buffer[i];

	}
}