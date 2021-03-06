#include <QtCore/QCoreApplication>
#include <QList>
#include <iostream>
#include <ctime>

using namespace std;

#define PAGE_NUM  (0xFF + 1)
#define FRAME_NUM (0x04)
#define FP_NONE   (-1)

struct FrameItem
{
    int pid;    // the task which use the frame
    int pnum;   // the page which the frame hold

    FrameItem()
    {
        pid = FP_NONE;
        pnum = FP_NONE;
    }
};

class PageTable
{
    int m_pt[PAGE_NUM];
public:
    PageTable()
    {
        for(int i=0; i<PAGE_NUM; i++)
        {
            m_pt[i] = FP_NONE;
        }
    }

    int& operator[] (int i)
    {
        if( (0 <= i) && (i < length()) )
        {
            return m_pt[i];
        }
        else
        {
            QCoreApplication::exit(-1);
        }
    }

    int length()
    {
        return PAGE_NUM;
    }
};

class PCB
{
    int m_pid;               // task id
    PageTable m_pageTable;   // page table for the task
    int* m_pageSerial;       // simulate the page serial access
    int m_pageSerialCount;   // page access count
    int m_next;              // the next page index to access
public:
    PCB(int pid)
    {
        m_pid = pid;
        m_pageSerialCount = qrand() % 5 + 5;
        m_pageSerial = new int[m_pageSerialCount];

        for(int i=0; i<m_pageSerialCount; i++)
        {
            m_pageSerial[i] = qrand() % 8;
        }

        m_next = 0;
    }

    int getPID()
    {
        return m_pid;
    }

    PageTable& getPageTable()
    {
        return m_pageTable;
    }

    int getNextPage()
    {
        int ret = m_next++;

        if( ret < m_pageSerialCount )
        {
            ret = m_pageSerial[ret];
        }
        else
        {
            ret = FP_NONE;
        }

        return ret;
    }

    bool running()
    {
        return (m_next < m_pageSerialCount);
    }

    void printPageSerial()
    {
        QString s = "";

        for(int i=0; i<m_pageSerialCount; i++)
        {
            s += QString::number(m_pageSerial[i]) + " ";
        }

        cout << ("Task" + QString::number(m_pid) + " : " + s).toStdString() << endl;
    }

    ~PCB()
    {
        delete[] m_pageSerial;
    }
};

FrameItem FrameTable[FRAME_NUM];
QList<PCB*> TaskTable;

int GetFrameItem();
void AccessPage(PCB& pcb);
int RequestPage(int pid, int page);
int SwapPage();
void PrintLog(QString log);
void PrintPageMap(int pid, int page, int frame);
void PrintFatalError(QString s, int pid, int page);

int GetFrameItem()
{
    int ret = FP_NONE;

    for(int i=0; i<FRAME_NUM; i++)
    {
        if( FrameTable[i].pid == FP_NONE )
        {
            ret = i;
            break;
        }
    }

    return ret;
}

void AccessPage(PCB& pcb)
{
    int pid = pcb.getPID();
    PageTable& pageTable = pcb.getPageTable();
    int page = pcb.getNextPage();

    if( page != FP_NONE )
    {
        PrintLog("Access Task" + QString::number(pid) + " for Page" + QString::number(page));

        if( pageTable[page] != FP_NONE )
        {
            PrintLog("Find target page in page table.");
            PrintPageMap(pid, page, pageTable[page]);
        }
        else
        {
            PrintLog("Target page is NOT found, need to request page ...");

            pageTable[page] = RequestPage(pid, page);

            if( pageTable[page] != FP_NONE )
            {
                PrintPageMap(pid, page, pageTable[page]);
            }
            else
            {
                PrintFatalError("Can NOT request page from disk...", pid, page);
            }
        }
    }
    else
    {
        PrintLog("Task" + QString::number(pid) + " is finished!");
    }
}

int RequestPage(int pid, int page)
{
    int frame = GetFrameItem();

    if( frame != FP_NONE )
    {
        PrintLog("Get a frame to hold page content: Frame" + QString::number(frame));
    }
    else
    {
        PrintLog("No free frame to allocate, need to swap page out.");

        frame = SwapPage();

        if( frame != FP_NONE )
        {
            PrintLog("Succeed to swap lazy page out.");
        }
        else
        {
            PrintFatalError("Failed to swap page out.", pid, FP_NONE);
        }
    }

    PrintLog("Load content from disk to Frame" + QString::number(frame));

    FrameTable[frame].pid = pid;
    FrameTable[frame].pnum = page;

    return frame;
}

int Random()
{
    // just random select
    int obj = qrand() % FRAME_NUM;

    PrintLog("Random select a frame to swap page content out: Frame" + QString::number(obj));
    PrintLog("Write the selected page content back to disk.");

    FrameTable[obj].pid = FP_NONE;
    FrameTable[obj].pnum = FP_NONE;

    for(int i=0, f=0; (i<TaskTable.count()) && !f; i++)
    {
        PageTable& pt = TaskTable[i]->getPageTable();

        for(int j=0; j<pt.length(); j++)
        {
            if( pt[j] == obj )
            {
                pt[j] = FP_NONE;
                f = 1;
                break;
            }
        }
    }

    return obj;
}

int SwapPage()
{
    return Random();
}

void PrintLog(QString log)
{
    cout << log.toStdString() << endl;
}

void PrintPageMap(int pid, int page, int frame)
{
    QString s = "Task" + QString::number(pid) + " : ";

    s += "Page" + QString::number(page) + " ==> Frame" + QString::number(frame);

    cout << s.toStdString() << endl;
}

void PrintFatalError(QString s, int pid, int page)
{
    s += " Task" + QString::number(pid) + ": Page" + QString::number(page);

    cout << s.toStdString() << endl;

    QCoreApplication::exit(-2);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    int index = 0;

    qsrand(time(NULL));

    TaskTable.append(new PCB(1));
    TaskTable.append(new PCB(2));

    PrintLog("Task Page Serial:");

    for(int i=0; i<TaskTable.count(); i++)
    {
        TaskTable[i]->printPageSerial();
    }

    PrintLog("==== Running ====");

    while( true )
    {
        if( TaskTable[index]->running() )
        {
            AccessPage(*TaskTable[index]);
        }

        index = (index + 1) % TaskTable.count();

        cin.get();
    }
    
    return a.exec();
}
