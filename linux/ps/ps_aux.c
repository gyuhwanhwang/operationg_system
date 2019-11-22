#include <iostream>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <dirent.h>
#include <string.h>
#include <time.h>

using namespace std;

int viewProc();
// 각프로세스의 CPU 사용율을 얻어온다. 
int getcputime(ulong utime, ulong stime, ulong starttime, int seconds);
// OS의 uptime(부팅후 지금까지의 가동시간)을 얻어온다.  
int uptime();

// 프로세스 데이타 저장 
typedef struct _process_info
{
    char            username[32];  // 유저이름
    vector<string>  process;       // stat정보들이 들어간다 
    char            processTime[20];    
} process_info;

// 프로세스의 리스트를 유지하기 위한 자료구조
// 결국은 배열의 배열이다. 
typedef vector<process_info> Plist;

class Proc
{
    private:
        int         Processnum;     // 현재 프로세스 갯수
        Plist       ProcList;       // 프로세스정보 리스트

    public:
        // 생성자및 소멸자 
        Proc();
        ~Proc();
        void            MakeProcInfo();      // 프로세스 정보를 얻어온다.
        vector<string>  ProcParser(char *);  // stat파일을 파싱한다 
        int             ProcNum();           // 현재프로세스 갯수를 넘겨준다.
        int             IsDigit(char *);     // 문자열이 숫자인지 확인한다. 
        Plist           GetProcList();       // 프로세스정보 리스트를 되돌려준다.
};

/*
 * 프로세스정보 리스트를 얻어와서 보기좋게 출력한다.  
 */
int viewProc()
{
    Proc *mproc;
    unsigned i;
    int pcpu;
    int seconds = 0;

    Plist ProcList;
    mproc = new Proc;
    mproc->MakeProcInfo();

    // 프로세스정보 리스트를 얻어온다. 
    ProcList = mproc->GetProcList(); 

    int total_cpu = 0;  

    // OS의 uptime을 얻어온다. 
    // 얻어온 정보는 프로세스의 CPU 사용율을 구하기 위한 기초정보로 
    // 사용된다.  
    seconds = uptime();
    printf("%-10s %7s %7s %4s %2s %16s %7s %7s %5s %-12s\n", "USER", "PID", "PPID", "%CPU", 
                                              "STA", "VMEM", "RSS","DAY", "TIME", " COMMAND"); 
    printf("==========================================================================================\n");
    for (i = 0; i < mproc->ProcNum(); i++)
    {
        // CPU사용율을 얻어온다. 
        pcpu = getcputime(atol(ProcList[i].process[13].c_str()), // utime
            atol(ProcList[i].process[14].c_str()), // stime
            atol(ProcList[i].process[21].c_str()), seconds); // starttime, now
        
        

        // 보기좋게 출력한다. 
        printf("%-10s %7s %7s %2d.%d %3s %16s %7s %14s %-12s\n", ProcList[i].username, 
                                    ProcList[i].process[0].c_str(), // PID
                                    ProcList[i].process[3].c_str(), // PPID
                                    pcpu/10, pcpu%10,  //process id
                                    ProcList[i].process[2].c_str(),  // Stat
                                    ProcList[i].process[22].c_str(), // VMEM
                                    ProcList[i].process[23].c_str(), // RSS
                                    ProcList[i].processTime,
                                    ProcList[i].process[1].c_str()
                                    
                                    ); // COMMAND
                                    
    }   
    return 1;
}
/*
 * 프로세스의 CPU사용율을 얻기 위해서 사용한다. 
 * utime     : 유저모드에서 프로세스가 스케쥴링되면서 사용한 jiffies 수이다. 
 *             프로세스가 스케쥴링될때마다 증가되는 수치이다.  
 * stime     : 커널모드에서 프로세스가 스케쥴링되면서 사용한 jiffies 수이다. 
 * starttime : 운영체제가 시작되고나서 몇 jiffies가 지난후 
 *             프로세스가 시작되었는지 
 */
int getcputime(ulong utime, ulong stime, ulong starttime, int seconds)
{   
    unsigned long long total_time;
    int pcpu=0;

    // 유저 jiffies 와 커널 jiffies를 더한다.
    total_time = utime + stime;

    // 100은 HZ값이다. HZ이 작아질수록 context switching가 빨라진다.  
    // 이값은 /usr/include/asm/param.h에 선언되어 있다. 
    // 100. 대신에 (unsigned long long)HZ 정도로 코드를 
    // 작성하는게 좀더 안전할것이다. 여기에서는 직관적으로 설명하기 
    // 위해서 하드코딩했다.  
    seconds = seconds - (int)(starttime/100.);

    if(seconds)
    {
        pcpu = (int)(total_time * 1000ULL/100.)/seconds;
    }

    return pcpu;
}

/*
 * 운영체제가 부팅하고 나서 얼마의 시간이 지났는지
 */
int uptime()
{
    FILE *fp;
    char buf[36];
    double stime;
    double idletime;

    if ((fp = fopen("/proc/uptime", "r")) == NULL)
    {
        perror("fopen error : ");
        exit(0);
    }
    fgets(buf, 36, fp);
    sscanf(buf, "%lf %lf", &stime, &idletime);
    fclose(fp);

    return (int)stime;
}

Proc::Proc()
{
}

Proc::~Proc()
{
}

/*
 * 프로세스 정보를 가져온다. 
 * 그러기 위해서 /proc/[PID]/stat파일을 읽어들이고 이를 
 * 필드별로 파싱한다. 파싱은 " "문자를 기준으로 한다.  
 * 또한 프로세스를 생성한 유저 이름도 얻어온다. 
 */
void Proc::MakeProcInfo()
{
    DIR *directory;
    struct dirent *entry = NULL;
    char proc_file[40];
    vector<string> procinfo; 
    process_info lprocess_info;
    struct stat lstat;

    ProcList.clear();

    // proc 디렉토리를 열어서 파일(디렉토리포함)의 리스트를 
    // 얻어온다.
    if ((directory = opendir("/proc")) == NULL)
    {
        perror("opendir error :");
        exit(0);
    }

    while((entry = readdir(directory)) != NULL)
    {
        if (strcmp(entry->d_name, ".") !=0 &&
            strcmp(entry->d_name, "..") != 0)
        {
            sprintf(proc_file,"/proc/%s/stat", entry->d_name);
            // stat 파일이 존재하는지 확인하고 확인하고
            if (access(proc_file, F_OK) != 0)
            {
                continue;
            }

            // 디렉토리 이름이 숫자인지 확인한다. 
            // 디렉토리 이름이 숫자라면 이를 파싱한다.  
            // 또한 stat파일의 소유 UID를 이용해서 유저이름도 얻어온다. 
            if (IsDigit(entry->d_name)) // 숫자이면 true
            {
                struct passwd *upasswd;
                struct tm timeTm;
                
                stat(proc_file,&lstat);
                lprocess_info.process  = ProcParser(proc_file);

                upasswd = getpwuid(lstat.st_uid);
                localtime_r(&lstat.st_atime, &timeTm);
                char s[20];

                sprintf(s, "%02d.%02d %02d:%02d ",
                            timeTm.tm_mon+1, timeTm.tm_mday,
                            timeTm.tm_hour, timeTm.tm_min
                );
               
                strncpy(lprocess_info.username, upasswd->pw_name, 32);
                
                strncpy(lprocess_info.processTime, s, 20);
                if(atoi(lprocess_info.process[0].c_str()) == atoi(entry->d_name))
                {
                    ProcList.push_back(lprocess_info);
                }
            }
            else
            {
            }
        }
    }

}

/*
 * MakeProcInfo를 통해만들어진 프로세스정보 리스트를 되돌려준다.
 */
Plist Proc::GetProcList()
{
    return ProcList;
}

/* 
 * 프로세스의 갯수를 되돌려준다.  
 * 갯수는 프로세스자료구조(vector)의 원소의 크기가 된다. 
 */
int Proc::ProcNum()
{
    return ProcList.size();
}

/*
 * stat 파일을 열어서 " "문자를 기준으로 파싱해서 
 * 배열(vector)에 집어넣는다.   
 */
vector<string> Proc::ProcParser(char *proc_file)
{
    FILE *fp;
    char buf[512] = {0x00,};
    char field[80];
    int index = 0;
    unsigned int i, j = 0;
    vector<string> vproc;

    fp = fopen(proc_file, "r");
    if (fp == NULL)
    {
        perror("error : ");
        exit(0);
    }

    memset(field, 0x00, 80);
    fgets(buf, 511, fp);
    for(i = 0; i < strlen(buf); i++)
    {
        if (buf[i] != ' ' && i != (strlen(buf) -1))
        {
            field[j++] = buf[i];
        }
        else
        {
            if (index == 1)
            {
                field[strlen(field)-1] = 0x00;
                vproc.push_back(field+1);
            }
            else
                vproc.push_back(field);
            memset(field, 0x00, 80);
            j = 0;
            index++;
        }
    }
    fclose(fp);
    return vproc;
}   

/*
 * 파일이름이 숫자인지 확인한다. 
 */ 
int Proc::IsDigit(char *str)
{
    int i;
    for (i = 0; i < strlen(str); i++)
    {
        if (isdigit(str[i])==0) // 숫자가 아니면
            return 0;
    }
    return 1;
}

int main(int argc, char **argv)
{
        viewProc();
}
