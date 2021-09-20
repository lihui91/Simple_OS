/*高速缓存管理模块，主要负责管理系统中所有的缓存块，包括
申请、释放、读写、清空一块缓存的功能函数接口，
以及系统退出时刷新所有缓存块*/

#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "Buf.h"
#include "DiskManager.h"
#include<iostream>
#include<algorithm>
#include <unordered_map>
#include<string.h>
using namespace std;
class BufferManager
{
public:
	/* static const member */
	static const int NBUF = 100;			/* 缓存控制块、缓冲区的数量，由15修改为100 */
	static const int BUFFER_SIZE = 512;	/* 缓冲区大小。 以字节为单位 */

public:
	BufferManager();
	~BufferManager();


	Buf* GetBlk(int blkno);	/* 申请一块缓存，用于读写设备dev上的字符块blkno。*/
	void Brelse(Buf* bp);				/* 释放缓存控制块 buf */

	Buf* Bread(int blkno);	/* 读一个磁盘块。dev为主、次设备号，blkno为目标磁盘块逻辑块号。 */

	void Bwrite(Buf* bp);				/* 写一个磁盘块 */
	void Bdwrite(Buf* bp);				/* 延迟写磁盘块 */

	void ClrBuf(Buf* bp);				/* 清空缓冲区内容 */
	void Bflush();				/* 将dev指定设备队列中延迟写的缓存全部输出到磁盘 */

	void FormatBuffer();				/* 格式化所有Buffer */
private:
	void InitList();					/*初始化队列*/
	void DetachNode(Buf* pb);		/**从队列中在下指定的buf*/
	void InsertTail(Buf* pb);		/*插入到缓存队列中*/

private:
	Buf* bFreeList;						/* 自由缓存队列控制块 */
	Buf m_Buf[NBUF];					/* 缓存控制块数组 */
	unsigned char Buffer[NBUF][BUFFER_SIZE];	/* 缓冲区数组 */
	unordered_map<int, Buf*> map;

	DiskManager* m_DiskManager;		/* 指向设备管理模块全局对象 */
};

#endif
