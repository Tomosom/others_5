#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#pragma pack(push)
#pragma pack(1)
struct Fat12Header
{
    char BS_OEMName[8];
    unsigned short BPB_BytsPerSec;
    unsigned char BPB_SecPerClus;
    unsigned short BPB_RsvdSecCnt;
    unsigned char BPB_NumFATs;
    unsigned short BPB_RootEntCnt;
    unsigned short BPB_TotSec16;
    unsigned char BPB_Media;
    unsigned short BPB_FATSz16;
    unsigned short BPB_SecPerTrk;
    unsigned short BPB_NumHeads;
    unsigned int BPB_HiddSec;
    unsigned int BPB_TotSec32;
    unsigned char BS_DrvNum;
    unsigned char BS_Reserved1;
    unsigned char BS_BootSig;
    unsigned int BS_VolID;
    char BS_VolLab[11];
    char BS_FileSysType[8];
};

#if 0
struct end
{
    unsigned char b510;
    unsigned char b511;
};
#endif

#pragma pack(pop)


// 注: get()用来操作字符和字符串的, 应该替换为read()
// http://www.cplusplus.com/reference/istream/istream/get/
// http://www.cplusplus.com/reference/istream/istream/read/
void PrintHeader(Fat12Header& rf, string p)
{
    ifstream file(p.c_str());

    if (!file.is_open())
    {
        cout << "open file :" << p << "has failed!!" << endl;
        return;
    }

	/*
     * 偏移3个字节: 因为与文件系统相关的信息就是从第0扇区偏移3个字节的地方开始的.
     * 遵循fat12的数据组织方式, 遵循规定, (参考004 fat12主引导区表 前面3个是跳转指令)
	 */
    file.seekg(3);

    //file.get(reinterpret_cast<char*>(&rf), sizeof(rf) + 1); // error
    file.read(reinterpret_cast<char*>(&rf), sizeof(rf));

    rf.BS_OEMName[7] = 0;
    rf.BS_VolLab[10] = 0;
    rf.BS_FileSysType[7] = 0;

    cout << "BS_OEMName: " << rf.BS_OEMName << endl;
    cout << "BPB_BytsPerSec: " << hex << rf.BPB_BytsPerSec << endl;
    cout << "BPB_SecPerClus: " << hex << (static_cast<short>(rf.BPB_SecPerClus) & 0xff) << endl;
    cout << "BPB_RsvdSecCnt: " << hex << rf.BPB_RsvdSecCnt << endl;
    cout << "BPB_NumFATs: " << hex << (static_cast<short>(rf.BPB_NumFATs) & 0xff) << endl;
    cout << "BPB_RootEntCnt: " << hex << rf.BPB_RootEntCnt << endl;
    cout << "BPB_TotSec16: " << hex << rf.BPB_TotSec16 << endl;
    cout << "BPB_Media: " << hex << (static_cast<short>(rf.BPB_Media) & 0xff) << endl;
    cout << "BPB_FATSz16: " << hex << rf.BPB_FATSz16 << endl;
    cout << "BPB_SecPerTrk: " << hex << rf.BPB_SecPerTrk << endl;
    cout << "BPB_NumHeads: " << hex << rf.BPB_NumHeads << endl;
    cout << "BPB_HiddSec: " << hex << rf.BPB_HiddSec << endl;
    cout << "BPB_TotSec32: " << hex << rf.BPB_TotSec32 << endl;
    cout << "BS_DrvNum: " << hex << (static_cast<short>(rf.BS_DrvNum) & 0xff) << endl;
    cout << "BS_Reserved1: " << hex << (static_cast<short>(rf.BS_Reserved1) & 0xff) << endl;
    cout << "BS_BootSig: " << hex << (static_cast<short>(rf.BS_BootSig) & 0xff) << endl;
    cout << "BS_VolID: " << hex << rf.BS_VolID << endl;
    cout << "BS_VolLab: " << rf.BS_VolLab << endl;
    cout << "BS_FileSysType: " << rf.BS_FileSysType << endl;

    file.seekg(510);

    /* 方法1 : 不推荐 */
#if 0
    char b510, b511;
    file.get(b510);
    file.get(b511);

    cout << "Byte 510: " << hex << (static_cast<short>(b510) & 0xff) << endl;
    cout << "Byte 510: " << hex << (static_cast<short>(b511) & 0xff) << endl;
#endif

    /* 方法2 : 正确做法 */
    char b510, b511;
    file.read(&b510, sizeof(b510));
    file.read(&b511, sizeof(b511));

    cout << "Byte 510: " << hex << (static_cast<short>(b510) & 0xff) << endl;
    cout << "Byte 510: " << hex << (static_cast<short>(b511) & 0xff) << endl;

    /* error */
#if 0
    struct end e;

    file.get(reinterpret_cast<char*>(&e), sizeof(e) + 1);

    cout << "Byte 510: " << hex << (static_cast<short>(e.b510) & 0xff) << endl;
    cout << "Byte 511: " << hex << (static_cast<short>(e.b511) & 0xff) << endl;
#endif
    file.close();
}

int main(int argc, char *argv[])
{
    Fat12Header f12;

    PrintHeader(f12, "data.img");
    
    return 0;
}
