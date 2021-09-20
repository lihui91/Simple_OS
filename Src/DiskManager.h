/*磁盘驱动模块，初始化磁盘镜像文件（myDisk.img）
直接负责磁盘文件直接读写*/

#pragma once

#include<iostream>
using namespace std;
class DiskManager
{
public:
	/*磁盘镜像文件名*/
	static const char* DISK_FILE_NAME;
public:
	DiskManager();
	~DiskManager();
	/*检查镜像文件是否存在*/
	bool Exists();
	/*打开镜像文件*/
	void open();
	/*写磁盘文件*/
	void write(const void* buffer, unsigned int size, int offset = -1, unsigned int origin = 0);
	void read(void* buffer, unsigned int size, int offset = -1, unsigned int origin = 0);
private:
	/*磁盘文件指针*/
	FILE* fp;
};