#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

bool starts_with(const string& s1, const string& s2) {
    return s2.size() <= s1.size() && s1.compare(0, s2.size(), s2) == 0;
}

bool ends_with(const string& s1, const string& s2) {
    return s2.size() <= s1.size() && s1.compare(s1.size() - s2.size(), s2.size(), s2) == 0;
}

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

struct RootEntry
{
    char DIR_Name[11];
    unsigned char DIR_Attr;
    unsigned char reserve[10];
    unsigned short DIR_WrtTime;
    unsigned short DIR_WrtDate;
    unsigned short DIR_FstClus;
    unsigned int DIR_FileSize;
};

#pragma pack(pop)

#if 1
void PrintHeader(Fat12Header& rf, string p)
{
    ifstream file(p.c_str());

    if (!file.is_open())
    {
        cout << "open file :" << p << "has failed!!" << endl;
        return;
    }

    file.seekg(3);

    file.read(reinterpret_cast<char*>(&rf), sizeof(rf));

    rf.BS_OEMName[7] = 0;
    rf.BS_VolLab[10] = 0;
    rf.BS_FileSysType[7] = 0;

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

    ifstream file(p.c_str());
    if (!file.is_open())
    {
        cout << "open file :" << p << "has failed!!" << endl;
        return ret;
    }

    if( (0 <= i) && (i < rf.BPB_RootEntCnt) )
    {
        file.seekg(19 * rf.BPB_BytsPerSec + i * sizeof(RootEntry));
        file.read(reinterpret_cast<char*>(&ret), sizeof(ret));
    }

    file.close();

    return ret;
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

    return ret;
}
#endif

/* 打印所有非空的目录项 */
void PrintRootEntry(Fat12Header& rf, string p)
{
    for(int i = 0; i < rf.BPB_RootEntCnt; i++)
    {
        RootEntry re = FindRootEntry(rf, p, i);

        if( re.DIR_Name[0] != '\0' )
        {
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
#endif

/* 读取指定文件的内容 */
#if 1
/* 读 fat 表, 存的内容是簇号 */
vector<unsigned short> ReadFat(Fat12Header& rf, string p)
{
    ifstream file(p.c_str());
    int size = rf.BPB_BytsPerSec * 9;
    unsigned char* fat = new unsigned char[size];
    vector<unsigned short> ret(size * 2 / 3, 0xFFFF); // 初始值为 0xFFFF

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

    if( re.DIR_Name[0] != '\0' )
    {
        vector<unsigned short> vec = ReadFat(rf, p);
        ifstream file(p.c_str());

        if (!file.is_open()) {
            cout << "open file :" << p << "has failed!!" << endl;
            return ret;
        }

        char buf[512] = {0};
        int count = 0;

        ret.resize(re.DIR_FileSize);

        /* 一个扇区一个扇区地读 */
        for(int i = 0, j = re.DIR_FstClus; j < 0xFF7; i += 512, j = vec[j])
        {
            /* 数据区起始地址所对应的编号(簇号)为2 ( 不为0 ) */
            file.seekg(rf.BPB_BytsPerSec * (33 + j - 2));

            file.read(buf, sizeof(buf));

            for(unsigned int k = 0; k < sizeof(buf); k++)
            {
                if( count < ret.size() )
                {
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

#if 1
    cout << "*** Read Header ***" << endl;
    PrintHeader(f12, img);
    cout << endl;
#endif


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
    cout << "DIR_Name: " << hex << re.DIR_Name << endl;
    cout << "DIR_Attr: " << hex << static_cast<short>(re.DIR_Attr) << endl;
    cout << "DIR_WrtDate: " << hex << re.DIR_WrtDate << endl;
    cout << "DIR_WrtTime: " << hex << re.DIR_WrtTime << endl;
    cout << "DIR_FstClus: " << hex << re.DIR_FstClus << endl;
    cout << "DIR_FileSize: " << hex << re.DIR_FileSize << endl;
    cout << endl;
#endif
#endif

#if 1
    cout << "*** Print File Content ***" << endl;
    //string content = ReadFileContent(f12, img, "TEST.TXT"); // exist
    string content = ReadFileContent(f12, img, "LOAD.BIN"); // exist
    //string content = ReadFileContent(f12, img, "load.bin"); // not exist
    cout << content << endl;
#endif

    return 0;
}
