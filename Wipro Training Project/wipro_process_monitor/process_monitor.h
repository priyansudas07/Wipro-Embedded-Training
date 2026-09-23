#ifndef PROCESS_MONITOR_H
#define PROCESS_MONITOR_H

typedef struct {
    int pid;
    char name[256];
    char state[32];
    int ppid;
} ProcessInfo;

typedef struct {
    int total;
    int running;
    int sleeping;
    int zombie;
    int stopped;
} ProcessStats;

void print_proc_banner(void);
int read_process_info(int pid, ProcessInfo *info);
void list_all_processes(void);
void search_process(const char *query);
void show_process_stats(void);

#endif // PROCESS_MONITOR_H
