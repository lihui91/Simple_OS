/*磁盘驱动模块，初始化磁盘镜像文件（myDisk.img）
直接负责磁盘文件直接读写*/


#define _CRT_SECURE_NO_WARNINGS
#include "DiskManager.h"

const char* DiskManager::DISK_FILE_NAME = "myDisk.img"; //磁盘镜像文件名

DiskManager::DiskManager()
{
    this->fp = fopen(DISK_FILE_NAME, "rb+"); //rb+ 读写打开一个二进制文件，只允许读写数据
}

DiskManager::~DiskManager()
{
    if (this->fp)
    {
        fclose(this->fp);
        /* code */
    }

}
/* 检查镜像文件是否存在 */
bool DiskManager::Exists() {
    return fp != NULL;
}


/* 打开镜像文件 */
void DiskManager::open() {
    fp = fopen(DISK_FILE_NAME, "wb+"); // wb+ 读写打开或建立一个二进制文件，允许读和写
    if (fp == NULL) {
        printf("打开或新建文件%s失败！", DISK_FILE_NAME);
        exit(-1);
    }
}

/* 写磁盘函数 */
void DiskManager::write(const void* buffer, unsigned int size, int offset, unsigned int origin) {
    if (offset >= 0) {
        fseek(fp, offset, origin);
    }
    fwrite(buffer, size, 1, fp);
}

/* 读磁盘函数 */
void DiskManager::read(void* buffer, unsigned int size, int offset, unsigned int origin) {
    if (offset >= 0) {
        fseek(fp, offset, origin);
    }
    fread(buffer, size, 1, fp);
}
