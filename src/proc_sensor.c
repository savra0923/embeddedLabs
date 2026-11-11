#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
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

int write_pid_count(int count) 
{
    FILE *fw = fopen(OUTPUT_FILE, "w");
    if (fw == NULL) 
    {
        perror("fopen proc.dat");
        return -1;
    }

    fprintf(fw, "Number of running processes: %d\n", count);

    fclose(fw);
    return 0;
}

int main(void)
{
    write_pid_count(read_pid_count());
	return 0;
}
