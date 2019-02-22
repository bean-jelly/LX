#ifndef LX_BASE_PROCESSINFO_H
#define LX_BASE_PROCESSINFO_H

#include <LX/base/YStringPiece.h>
#include <LX/base/YTypes.h>
#include <LX/base/YTimestamp.h>
#include <vector>
#include <sys/types.h>

namespace LX
{

    namespace ProcessInfo
    {
        pid_t pid();
        string pidString();
        uid_t uid();
        string username();
        uid_t euid();
        Timestamp startTime();
        int clockTicksPerSecond();
        int pageSize();
        bool isDebugBuild();  // constexpr

        string hostname();
        string procname();
        StringPiece procname(const string& stat);

        /// read /proc/self/status
        string procStatus();

        /// read /proc/self/stat
        string procStat();

        /// read /proc/self/task/tid/stat
        string threadStat();

        /// readlink /proc/self/exe
        string exePath();

        int openedFiles();
        int maxOpenFiles();

        struct CpuTime
        {
            double userSeconds;
            double systemSeconds;

            CpuTime() : userSeconds(0.0), systemSeconds(0.0) { }
        };
        CpuTime cpuTime();

        int numThreads();
        std::vector<pid_t> threads();
    }
}

#endif  // MUDUO_BASE_PROCESSINFO_H