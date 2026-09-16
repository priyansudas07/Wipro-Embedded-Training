#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include "process_monitor.h"

// ANSI Color Codes
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RED     "\033[31m"

void print_proc_banner(void) {
    printf("%s%s", COLOR_CYAN, COLOR_BOLD);
    printf("======================================================================\n");
    printf("            LINUX PROCESS MONITOR TOOL (Wipro COE)                   \n");
    printf("======================================================================\n");
    printf("%s", COLOR_RESET);
}

int read_process_info(int pid, ProcessInfo *info) {
    char status_path[256];
    snprintf(status_path, sizeof(status_path), "/proc/%d/status", pid);

    FILE *fp = fopen(status_path, "r");
    if (!fp) return 0;

    info->pid = pid;
    strcpy(info->name, "Unknown");
    strcpy(info->state, "U");
    info->ppid = 0;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "Name:", 5) == 0) {
            char *val = line + 5;
            while (*val == ' ' || *val == '\t') val++;
            size_t len = strlen(val);
            if (len > 0 && val[len-1] == '\n') val[len-1] = '\0';
            strncpy(info->name, val, sizeof(info->name) - 1);
        } else if (strncmp(line, "State:", 6) == 0) {
            char *val = line + 6;
            while (*val == ' ' || *val == '\t') val++;
            // Extract single letter state char e.g., 'S', 'R', 'Z'
            info->state[0] = val[0];
            info->state[1] = '\0';
        } else if (strncmp(line, "PPid:", 5) == 0) {
            char *val = line + 5;
            info->ppid = atoi(val);
        }
    }

    fclose(fp);
    return 1;
}

// Function to sort processes by PID ascending
int compare_processes(const void *a, const void *b) {
    ProcessInfo *p1 = (ProcessInfo *)a;
    ProcessInfo *p2 = (ProcessInfo *)b;
    return p1->pid - p2->pid;
}

void list_all_processes(void) {
    DIR *dir = opendir("/proc");
    if (!dir) {
        perror("Failed to open /proc");
        return;
    }

    ProcessInfo *processes = malloc(sizeof(ProcessInfo) * 2048);
    int count = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Check if directory name is numeric (PID)
        if (entry->d_type == DT_DIR) {
            int is_num = 1;
            for (int i = 0; entry->d_name[i] != '\0'; i++) {
                if (!isdigit(entry->d_name[i])) {
                    is_num = 0;
                    break;
                }
            }
            if (is_num) {
                int pid = atoi(entry->d_name);
                if (read_process_info(pid, &processes[count])) {
                    count++;
                }
            }
        }
    }
    closedir(dir);

    // Sort processes by PID ascending
    qsort(processes, count, sizeof(ProcessInfo), compare_processes);

    // Display formatted table matching user requirement
    printf("%s%-10s %-30s %-10s %-10s%s\n", COLOR_BOLD, "PID", "Process Name", "State", "PPID", COLOR_RESET);
    printf("----------------------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        printf("%-10d %-30s %-10s %-10d\n", 
               processes[i].pid, processes[i].name, processes[i].state, processes[i].ppid);
    }

    printf("----------------------------------------------------------------------\n");
    printf("%sTotal Running Processes Found: %d%s\n", COLOR_GREEN, count, COLOR_RESET);

    free(processes);
}

void search_process(const char *query) {
    DIR *dir = opendir("/proc");
    if (!dir) {
        perror("Failed to open /proc");
        return;
    }

    int query_pid = atoi(query);
    int found_count = 0;

    printf("\n%s%-10s %-30s %-10s %-10s%s\n", COLOR_BOLD, "PID", "Process Name", "State", "PPID", COLOR_RESET);
    printf("----------------------------------------------------------------------\n");

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            int is_num = 1;
            for (int i = 0; entry->d_name[i] != '\0'; i++) {
                if (!isdigit(entry->d_name[i])) {
                    is_num = 0;
                    break;
                }
            }
            if (is_num) {
                int pid = atoi(entry->d_name);
                ProcessInfo info;
                if (read_process_info(pid, &info)) {
                    if ((query_pid > 0 && info.pid == query_pid) || 
                        (strcasestr(info.name, query) != NULL)) {
                        printf("%-10d %-30s %-10s %-10d\n", 
                               info.pid, info.name, info.state, info.ppid);
                        found_count++;
                    }
                }
            }
        }
    }
    closedir(dir);

    printf("----------------------------------------------------------------------\n");
    printf("Found %d matching process(es) for query: '%s'\n", found_count, query);
}

void show_process_stats(void) {
    DIR *dir = opendir("/proc");
    if (!dir) return;

    ProcessStats stats = {0, 0, 0, 0, 0};

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            int is_num = 1;
            for (int i = 0; entry->d_name[i] != '\0'; i++) {
                if (!isdigit(entry->d_name[i])) {
                    is_num = 0;
                    break;
                }
            }
            if (is_num) {
                int pid = atoi(entry->d_name);
                ProcessInfo info;
                if (read_process_info(pid, &info)) {
                    stats.total++;
                    if (info.state[0] == 'R') stats.running++;
                    else if (info.state[0] == 'S' || info.state[0] == 'I') stats.sleeping++;
                    else if (info.state[0] == 'Z') stats.zombie++;
                    else if (info.state[0] == 'T') stats.stopped++;
                }
            }
        }
    }
    closedir(dir);

    printf("\n%s=== PROCESS STATISTICS SUMMARY ===%s\n", COLOR_YELLOW, COLOR_RESET);
    printf("  %sTotal Active Processes%s : %d\n", COLOR_BOLD, COLOR_RESET, stats.total);
    printf("  %sRunning (R)%s            : %d\n", COLOR_BOLD, COLOR_RESET, stats.running);
    printf("  %sSleeping (S/I)%s        : %d\n", COLOR_BOLD, COLOR_RESET, stats.sleeping);
    printf("  %sStopped (T)%s           : %d\n", COLOR_BOLD, COLOR_RESET, stats.stopped);
    printf("  %sZombie (Z)%s            : %d\n", COLOR_BOLD, COLOR_RESET, stats.zombie);
}
