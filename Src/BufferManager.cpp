/*高速缓存管理模块，主要负责管理系统中所有的缓存块，包括
申请、释放、读写、清空一块缓存的功能函数接口，
以及系统退出时刷新所有缓存块*/

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

/* 申请一块缓存*/

Buf* BufferManager::GetBlk(int blkno)
{
	Buf* bp;

	/*
	 * 如果设备队列中已经存在相应缓存，则返回该缓存；
	 * 否则从自由队列中分配新的缓存用于字符块读写。
	 */
	if (map.find(blkno) != map.end()) {
		bp = map[blkno];
		DetachNode(bp);
		return bp;
	}

	bp = bFreeList->b_back;
	if (bp == bFreeList) {
		printf("无Buffer使用\n");;
		return NULL;
	}
	DetachNode(bp);
	map.erase(bp->b_blkno);
	//释放缓存队列，使用map，则不用每次遍历一次缓存队列
	//延迟写
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
	//释放一个缓存块，则放入队列尾部
	InsertTail(bp);
}

Buf* BufferManager::Bread(int blkno)
{
	Buf* bp;
	/* 根据设备号，字符块号申请缓存 */
	bp = this->GetBlk(blkno);
	/* 如果在设备队列中找到所需缓存，即B_DONE已设置，就不需进行I/O操作 */
	if (bp->b_flags & (Buf::B_DONE | Buf::B_DELWRI)) {
		return bp;
	}
	/* 没有找到相应缓存，构成I/O读请求块 */
	this->m_DiskManager->read(bp->b_addr, BUFFER_SIZE, bp->b_blkno * BUFFER_SIZE);
	/*
	 * 将I/O请求块送入相应设备I/O请求队列，如无其它I/O请求，则将立即执行本次I/O请求；
	 * 否则等待当前I/O请求执行完毕后，由中断处理程序启动执行此请求。
	 * 注：Strategy()函数将I/O请求块送入设备请求队列后，不等I/O操作执行完毕，就直接返回。
	 */

	 /* 同步读，等待I/O操作结束 */


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
	 * 如果不是延迟写，则检查错误；否则不检查。
	 * 这是因为如果延迟写，则很有可能当前进程不是
	 * 操作这一缓存块的进程，而在GetError()主要是
	 * 给当前进程附上错误标志。
	 */


	return;
}

void BufferManager::Bdwrite(Buf* bp)
{
	/* 延迟写磁盘块 */
	/* 置上B_DONE允许其它进程使用该磁盘块内容 */
	bp->b_flags |= (Buf::B_DELWRI | Buf::B_DONE);
	this->Brelse(bp);
	return;
}


void BufferManager::ClrBuf(Buf* bp)
{
	/* 将缓冲区中数据清零 */
	memset(bp->b_addr, 0, BufferManager::BUFFER_SIZE);
	return;
}

void BufferManager::Bflush()
{
	Buf* bp;
	/* 注意：这里之所以要在搜索到一个块之后重新开始搜索，
	 * 因为在bwite()进入到驱动程序中时有开中断的操作，所以
	 * 等到bwrite执行完成后，CPU已处于开中断状态，所以很
	 * 有可能在这期间产生磁盘中断，使得bfreelist队列出现变化，
	 * 如果这里继续往下搜索，而不是重新开始搜索那么很可能在
	 * 操作bfreelist队列的时候出现错误。
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
/* 采用LRU Cache 算法，每次从头部取出，使用后放到尾部
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