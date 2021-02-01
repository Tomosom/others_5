#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

#pragma pack(push)
#pragma pack(1) /* 下述结构体设置为 1 字节对齐 */

/*
 * BS_OEMName: FreeDOS // 这张虚拟软盘是被FreeDOS格式化的
 * BPB_BytsPerSec: 200 // 512字节
 * BPB_SecPerClus: 1
 * BPB_RsvdSecCnt: 1
 * BPB_NumFATs: 2
 * BPB_RootEntCnt: e0
 * BPB_TotSec16: b40
 * BPB_Media: f0
 * BPB_FATSz16: 9
 * BPB_SecPerTrk: 12
 * BPB_NumHeads: 2
 * BPB_HiddSec: 0
 * BPB_TotSec32: 0
 * BS_DrvNum: 1 // 直接磁盘服务(Direct Disk Service——INT 13H)
 * BS_Reserved1: 0
 * BS_BootSig: 29
 * BS_VolID: 0
 * BS_VolLab:
 * BS_FileSysType: FAT12 // 标识文件系统的类型
 */
struct Fat12Header {
    char BS_OEMName[8];            /* OEM字符串，必须为8个字符，不足以空格填空 */
    unsigned short BPB_BytsPerSec; /* 每扇区字节数 */
    unsigned char BPB_SecPerClus;  /* 每簇占用的扇区数 */
    unsigned short BPB_RsvdSecCnt; /* Boot占用的扇区数 */
    unsigned char BPB_NumFATs;     /* FAT表的记录数 */
    unsigned short BPB_RootEntCnt; /* 最大根目录文件数 */
    unsigned short BPB_TotSec16;   /* 逻辑扇区总数 */
    unsigned char BPB_Media;       /* 媒体描述符 */
    unsigned short BPB_FATSz16;    /* 每个FAT占用扇区数 */
    unsigned short BPB_SecPerTrk;  /* 每个磁道扇区数 */
    unsigned short BPB_NumHeads;   /* 磁头数 */
    unsigned int BPB_HiddSec;      /* 隐藏扇区数 */
    unsigned int BPB_TotSec32;     /* 如果BPB_TotSecl6是0,则在这里记录扇区总数 */
    unsigned char BS_DrvNum;       /* 中断13的驱动器号 */
    unsigned char BS_Reserved1;    /* 未使用 */
    unsigned char BS_BootSig;      /* 扩展引导标志 */
    unsigned int BS_VolID;         /* 卷序列号 */
    char BS_VolLab[11];            /* 卷标，必须是11个字符，不足以空格填充 */
    char BS_FileSysType[8];        /* 文件系统类型，必须是8个字符，不足填充空格 */
};

struct RootEntry {
    char DIR_Name[11];          /* 文件名8字节,扩展名3字节 */
    unsigned char DIR_Attr;     /* 文件属性 */
    unsigned char reserve[10];  /* 保留位 */
    unsigned short DIR_WrtTime; /* 最后一次写入时间 */
    unsigned short DIR_WrtDate; /* 最后一次写入日期 */
    unsigned short DIR_FstClus; /* 文件开始的簇号 */
    unsigned int DIR_FileSize;  /* 文件大小 */
};
#pragma pack(pop)

bool starts_with(const string& s1, const string& s2)
{
    return s2.size() <= s1.size() && s1.compare(0, s2.size(), s2) == 0;
}

bool ends_with(const string& s1, const string& s2)
{
    return s2.size() <= s1.size() && s1.compare(s1.size() - s2.size(), s2.size(), s2) == 0;
}

#if 1
void PrintHeader(Fat12Header& rf, string p)
{
    ifstream file(p.c_str()); /* open file */
    if (!file.is_open()) {
        cout << "open file :" << p << "has failed!!" << endl;
        return;
    }

    file.seekg(3);

    file.read(reinterpret_cast<char*>(&rf), sizeof(rf));

    rf.BS_OEMName[7] = '\0';
    rf.BS_VolLab[10] = '\0';
    rf.BS_FileSysType[7] = '\0';

    cout << "BS_OEMName: " << rf.BS_OEMName << endl;
    cout << "BPB_BytsPerSec: " << hex << rf.BPB_BytsPerSec << endl;
    cout << "BPB_SecPerClus: " << hex << static_cast<short>(rf.BPB_SecPerClus) << endl;
    cout << "BPB_RsvdSecCnt: " << hex << rf.BPB_RsvdSecCnt << endl;
    cout << "BPB_NumFATs: " << hex << static_cast<short>(rf.BPB_NumFATs) << endl;
    cout << "BPB_RootEntCnt: " << hex << rf.BPB_RootEntCnt << endl;
    cout << "BPB_TotSec16: " << hex << rf.BPB_TotSec16 << endl;
    cout << "BPB_Media: " << hex << static_cast<short>(rf.BPB_Media) << endl;
    cout << "BPB_FATSz16: " << hex << rf.BPB_FATSz16 << endl;
    cout << "BPB_SecPerTrk: " << hex << rf.BPB_SecPerTrk << endl;
    cout << "BPB_NumHeads: " << hex << rf.BPB_NumHeads << endl;
    cout << "BPB_HiddSec: " << hex << rf.BPB_HiddSec << endl;
    cout << "BPB_TotSec32: " << hex << rf.BPB_TotSec32 << endl;
    cout << "BS_DrvNum: " << hex << static_cast<short>(rf.BS_DrvNum) << endl;
    cout << "BS_Reserved1: " << hex << static_cast<short>(rf.BS_Reserved1) << endl;
    cout << "BS_BootSig: " << hex << static_cast<short>(rf.BS_BootSig) << endl;
    cout << "BS_VolID: " << hex << rf.BS_VolID << endl;
    cout << "BS_VolLab: " << rf.BS_VolLab << endl;
    cout << "BS_FileSysType: " << rf.BS_FileSysType << endl;

    file.seekg(510);

    char b510, b511;
    file.read(&b510, sizeof(b510));
    file.read(&b511, sizeof(b511));

    cout << "Byte 510: " << hex << static_cast<short>(b510) << endl;
    cout << "Byte 510: " << hex << static_cast<short>(b511) << endl;

    file.close();
}
#endif

#if 1
/* 读取第 i 个目录项 */
RootEntry FindRootEntry(Fat12Header& rf, string p, int i)
{
    RootEntry ret = {{0}};

    ifstream file(p.c_str()); /* open file */
    if (!file.is_open()) {
        cout << "open file :" << p << "has failed!!" << endl;
        return ret;
    }

    if( (0 <= i) && (i < rf.BPB_RootEntCnt) ) {
        file.seekg(19 * rf.BPB_BytsPerSec + i * sizeof(RootEntry));
        file.read(reinterpret_cast<char*>(&ret), sizeof(ret));
    }

    file.close();

    return ret;
}

/* 打印所有非空的目录项 */
void PrintRootEntry(Fat12Header& rf, string p)
{
    for(int i = 0; i < rf.BPB_RootEntCnt; i++) {
        RootEntry re = FindRootEntry(rf, p, i);

        if( re.DIR_Name[0] != '\0' ) {
            cout << i << ":" << endl;
            cout << "DIR_Name: " << hex << re.DIR_Name << endl;
            cout << "DIR_Attr: " << hex << static_cast<short>(re.DIR_Attr) << endl;
            cout << "DIR_WrtDate: " << hex << re.DIR_WrtDate << endl;
            cout << "DIR_WrtTime: " << hex << re.DIR_WrtTime << endl;
            cout << "DIR_FstClus: " << hex << re.DIR_FstClus << endl;
            cout << "DIR_FileSize: " << hex << re.DIR_FileSize << endl;
        }
    }
}

#if 1
/* 根据文件名查找对应的文件项 */
RootEntry FindRootEntry(Fat12Header& rf, string p, string fn)
{
    RootEntry ret = {{0}};

    for(int i = 0; i < rf.BPB_RootEntCnt; i++) {
        RootEntry re = FindRootEntry(rf, p, i);

        if( re.DIR_Name[0] != '\0' ) {
            int d = fn.find_last_of(".");

            /* 去掉首尾空格 */
            string name = string(re.DIR_Name);
            name.erase(0,name.find_first_not_of(" "));
            name.erase(name.find_last_not_of(" ") + 1);

            if( d >= 0 ) {
                string pre = fn.substr(0, d);
                string post = fn.substr(d + 1);

                if( starts_with(name, pre) && ends_with(name, post) ) {
                    ret = re;
                    break;
                }
            } else { // 无后缀的文件名
                if( fn == name ) {
                    ret = re;
                    break;
                }
            }
        }
    }

    cout << "DIR_Name: " << hex << ret.DIR_Name << endl;
    cout << "DIR_Attr: " << hex << static_cast<short>(ret.DIR_Attr) << endl;
    cout << "DIR_WrtDate: " << hex << ret.DIR_WrtDate << endl;
    cout << "DIR_WrtTime: " << hex << ret.DIR_WrtTime << endl;
    cout << "DIR_FstClus: " << hex << ret.DIR_FstClus << endl;
    cout << "DIR_FileSize: " << hex << ret.DIR_FileSize << endl;

    return ret;
}
#endif
#endif

/* 读取指定文件的内容 */
#if 1
/* 读 fat 表, 存的内容是簇号 */
vector<unsigned short> ReadFat(Fat12Header& rf, string p)
{
    int size = rf.BPB_BytsPerSec * 9;
    unsigned char *fat = new unsigned char[size];
    vector<unsigned short> ret(size * 2 / 3, 0xFFFF); // 初始值为 0xFFFF
    ifstream file(p.c_str()); /* open file */
    if (!file.is_open()) {
        cout << "open file :" << p << "has failed!!" << endl;
        return ret;
    }

    file.seekg(rf.BPB_BytsPerSec * 1); // 读fat表1

    file.read(reinterpret_cast<char*>(fat), size);

    /* FAT表中的每个表项只占用12比特（1.5字节） */
    for(int i = 0, j = 0; i < size; i += 3, j += 2) { /* 3个字节表示2个fat表项 */
        ret[j] = static_cast<unsigned short>((fat[i + 1] & 0x0F) << 8) | fat[i];
        ret[j + 1] = static_cast<unsigned short>(fat[i + 2] << 4) | ((fat[i + 1] >> 4) & 0x0F);
    }

    file.close();

    delete[] fat;

    return ret;
}

/* 读指定文件的内容 */
string ReadFileContent(Fat12Header& rf, string p, string fn)
{
    string ret;
    RootEntry re = FindRootEntry(rf, p, fn);

    if( re.DIR_Name[0] != '\0' ) {
        vector<unsigned short> vec = ReadFat(rf, p);
        ifstream file(p.c_str()); /* open file */
        if (!file.is_open()) {
            cout << "open file :" << p << "has failed!!" << endl;
            return ret;
        }

        char buf[512] = {0};
        int count = 0;

        ret.resize(re.DIR_FileSize);

        /* 一个扇区一个扇区地读 */
        for(int i = 0, j = re.DIR_FstClus; j < 0xFF7; i += 512, j = vec[j]) {
            /* 数据区起始地址所对应的编号(簇号)为2 ( 不为0 ) */
            file.seekg(rf.BPB_BytsPerSec * (33 + j - 2));

            file.read(buf, sizeof(buf));

            for(unsigned int k = 0; k < sizeof(buf); k++) {
                if(count < ret.size()) {
                    ret[i + k] = buf[k];
                    count++;
                }
            }
        }

        file.close();
    }

    return ret;
}
#endif

int main(int argc, char *argv[])
{
    Fat12Header f12;
    string img = "data.img";

    /* 实验一:读取FAT12文件系统信息 */
    cout << "*** Read Header ***" << endl;
    PrintHeader(f12, img);
    cout << endl;

    /* 实验二:读取根目录信息 */
#if 1
    cout << "*** Print Root Entry ***" << endl;
    PrintRootEntry(f12, img);
    cout << endl;
#if 1
    /* 根据文件名查找对应的文件项 */
    cout << "*** Find Root Entry ***" << endl;
    //RootEntry re = FindRootEntry(f12, img, "TEST.TXT"); // exist
    RootEntry re = FindRootEntry(f12, img, "LOAD.BIN"); // exist
    //RootEntry re = FindRootEntry(f12, img, "load.bin"); // not exist
    cout << endl;
#endif
#endif

    /* 实验三:读取指定文件内容 */
#if 1
    cout << "*** Print File Content ***" << endl;
    //string content = ReadFileContent(f12, img, "TEST.TXT"); // exist
    string content = ReadFileContent(f12, img, "LOAD.BIN"); // exist
    //string content = ReadFileContent(f12, img, "load.bin"); // not exist
    cout << content << endl;
#endif

    return 0;
}
