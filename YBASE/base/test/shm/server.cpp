#include "comm.h"

int main()
{
    int shmid = CreatShmid(4097);
    if(shmid > 0)
    {
        int i = 0;
        char* addr = shmat(shmid, NULL, 0);
        while(i < 20)
        {
            addr[i++] = 'A';
            addr[i] = '\0';
            sleep(1);
        }
        if(shmdt(addr) == -1)
        {
            perror("shmat");
            return -3;
        }
    }
    else
    {
        perror("CreateShmid");
        return -1;
    }
    if(Destory(shmid) < 0)
    {
        perror("Destory");
        return -2;
    }
    return 0;
}