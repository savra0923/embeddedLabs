#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stddef.h>
#include <fcntl.h>
#include <sysmon_ipc.h>
#include "proc_sensor.h"

int read_pid_count(void) 
{
    DIR *proc = opendir(PROC_INFO);
    if (proc == NULL) 
    {
        perror("opendir /proc");
        return -1;
    }

    size_t pid_count = 0;
    struct dirent *entry;

    while ((entry = readdir(proc)) != NULL) 
    {
        // Check if the directory name contains only digits
        int is_pid = 1;

        for (int i = 0; entry->d_name[i] != '\0'; i++) 
        {
            if (!isdigit((unsigned char)entry->d_name[i])) 
            {
                is_pid = 0;
                break;
            }
        }

        if (is_pid && entry->d_name[0] != '\0')
            pid_count++;
    }

    closedir(proc);
    return pid_count;
}

int main(void)
{
    int fd = open(PROC_FIFO_PATH, O_WRONLY);

    if (fd == -1)
    {
        perror("open PROC FIFO");
    }

    while (1)
    {
        ProcData data;

        data.total_procs = read_pid_count();

        if (write(fd, &data, sizeof(data)) != sizeof(data))
        {
            perror("write PROC FIFO");
            break;
        }

        sleep(INTERVAL+20);
    }
    
	return 0;
}
