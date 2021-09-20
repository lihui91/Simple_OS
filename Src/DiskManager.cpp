/*��������ģ�飬��ʼ�����̾����ļ���myDisk.img��
ֱ�Ӹ�������ļ�ֱ�Ӷ�д*/


#define _CRT_SECURE_NO_WARNINGS
#include "DiskManager.h"

const char* DiskManager::DISK_FILE_NAME = "myDisk.img"; //���̾����ļ���

DiskManager::DiskManager()
{
    this->fp = fopen(DISK_FILE_NAME, "rb+"); //rb+ ��д��һ���������ļ���ֻ�����д����
}

DiskManager::~DiskManager()
{
    if (this->fp)
    {
        fclose(this->fp);
        /* code */
    }

}
/* ��龵���ļ��Ƿ���� */
bool DiskManager::Exists() {
    return fp != NULL;
}


/* �򿪾����ļ� */
void DiskManager::open() {
    fp = fopen(DISK_FILE_NAME, "wb+"); // wb+ ��д�򿪻���һ���������ļ����������д
    if (fp == NULL) {
        printf("�򿪻��½��ļ�%sʧ�ܣ�", DISK_FILE_NAME);
        exit(-1);
    }
}

/* д���̺��� */
void DiskManager::write(const void* buffer, unsigned int size, int offset, unsigned int origin) {
    if (offset >= 0) {
        fseek(fp, offset, origin);
    }
    fwrite(buffer, size, 1, fp);
}

/* �����̺��� */
void DiskManager::read(void* buffer, unsigned int size, int offset, unsigned int origin) {
    if (offset >= 0) {
        fseek(fp, offset, origin);
    }
    fread(buffer, size, 1, fp);
}
