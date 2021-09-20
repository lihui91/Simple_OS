/*��������ģ�飬��ʼ�����̾����ļ���myDisk.img��
ֱ�Ӹ�������ļ�ֱ�Ӷ�д*/

#pragma once

#include<iostream>
using namespace std;
class DiskManager
{
public:
	/*���̾����ļ���*/
	static const char* DISK_FILE_NAME;
public:
	DiskManager();
	~DiskManager();
	/*��龵���ļ��Ƿ����*/
	bool Exists();
	/*�򿪾����ļ�*/
	void open();
	/*д�����ļ�*/
	void write(const void* buffer, unsigned int size, int offset = -1, unsigned int origin = 0);
	void read(void* buffer, unsigned int size, int offset = -1, unsigned int origin = 0);
private:
	/*�����ļ�ָ��*/
	FILE* fp;
};