/*���ٻ������ģ�飬��Ҫ�������ϵͳ�����еĻ���飬����
���롢�ͷš���д�����һ�黺��Ĺ��ܺ����ӿڣ�
�Լ�ϵͳ�˳�ʱˢ�����л����*/

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
	static const int NBUF = 100;			/* ������ƿ顢����������������15�޸�Ϊ100 */
	static const int BUFFER_SIZE = 512;	/* ��������С�� ���ֽ�Ϊ��λ */

public:
	BufferManager();
	~BufferManager();


	Buf* GetBlk(int blkno);	/* ����һ�黺�棬���ڶ�д�豸dev�ϵ��ַ���blkno��*/
	void Brelse(Buf* bp);				/* �ͷŻ�����ƿ� buf */

	Buf* Bread(int blkno);	/* ��һ�����̿顣devΪ�������豸�ţ�blknoΪĿ����̿��߼���š� */

	void Bwrite(Buf* bp);				/* дһ�����̿� */
	void Bdwrite(Buf* bp);				/* �ӳ�д���̿� */

	void ClrBuf(Buf* bp);				/* ��ջ��������� */
	void Bflush();				/* ��devָ���豸�������ӳ�д�Ļ���ȫ����������� */

	void FormatBuffer();				/* ��ʽ������Buffer */
private:
	void InitList();					/*��ʼ������*/
	void DetachNode(Buf* pb);		/**�Ӷ���������ָ����buf*/
	void InsertTail(Buf* pb);		/*���뵽���������*/

private:
	Buf* bFreeList;						/* ���ɻ�����п��ƿ� */
	Buf m_Buf[NBUF];					/* ������ƿ����� */
	unsigned char Buffer[NBUF][BUFFER_SIZE];	/* ���������� */
	unordered_map<int, Buf*> map;

	DiskManager* m_DiskManager;		/* ָ���豸����ģ��ȫ�ֶ��� */
};

#endif
