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
#pragma pack(pop)

void PrintHeader(Fat12Header& rf, string p)
{
    //QFile file(p);
    ifstream file(p.c_str());

    if (!file.is_open())
    {
        cout << "open file :" << p << "has failed!!" << endl;
        return;
    }

    file.seekg(3);

    file.get(reinterpret_cast<char*>(&rf), sizeof(rf));

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

    unsigned char b510 = 0;
    unsigned char b511 = 0;

    file.get(reinterpret_cast<char*>(&b510), sizeof(b510));
    file.get(reinterpret_cast<char*>(&b511), sizeof(b511));

    cout << static_cast<short>(b510) << endl;
    cout << static_cast<short>(b511) << endl;

    //cout << "Byte 510: " << hex << (static_cast<short>(b510) & 0xff) << endl;
    //cout << "Byte 511: " << hex << (static_cast<short>(b511) & 0xff) << endl;

    file.close();
}

int main(int argc, char *argv[])
{
    Fat12Header f12;

    PrintHeader(f12, "data.img");
    
    return 0;
}
