#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include "sys_info.h"

// ANSI Color Codes for terminal UI
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_RED     "\033[31m"

void print_banner(void) {
    printf("%s%s", COLOR_CYAN, COLOR_BOLD);
    printf("==========================================================\n");
    printf("        LINUX SYSTEM INFORMATION TOOL (Wipro COE)         \n");
    printf("==========================================================\n");
    printf("%s", COLOR_RESET);
}

void print_section_header(const char *title) {
    printf("\n%s%s=== %s ===%s\n", COLOR_YELLOW, COLOR_BOLD, title, COLOR_RESET);
}

void show_system_info(void) {
    print_section_header("1. SYSTEM INFORMATION");
    
    // 1. Kernel and Hostname via utsname
    struct utsname sys_uts;
    if (uname(&sys_uts) == 0) {
        printf("  %s%-20s%s : %s\n", COLOR_BOLD, "OS Name", COLOR_RESET, sys_uts.sysname);
        printf("  %s%-20s%s : %s\n", COLOR_BOLD, "Hostname", COLOR_RESET, sys_uts.nodename);
        printf("  %s%-20s%s : %s\n", COLOR_BOLD, "Kernel Release", COLOR_RESET, sys_uts.release);
        printf("  %s%-20s%s : %s\n", COLOR_BOLD, "Kernel Version", COLOR_RESET, sys_uts.version);
        printf("  %s%-20s%s : %s\n", COLOR_BOLD, "Architecture", COLOR_RESET, sys_uts.machine);
    } else {
        perror("uname failed");
    }

    // 2. OS Distribution from /etc/os-release
    FILE *fp = fopen("/etc/os-release", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "PRETTY_NAME=", 12) == 0) {
                char *name = line + 12;
                if (name[0] == '"') name++;
                size_t len = strlen(name);
                if (len > 0 && name[len-1] == '\n') name[--len] = '\0';
                if (len > 0 && name[len-1] == '"') name[--len] = '\0';
                printf("  %s%-20s%s : %s\n", COLOR_BOLD, "OS Distribution", COLOR_RESET, name);
                break;
            }
        }
        fclose(fp);
    }

    // 3. System Uptime via sysinfo
    struct sysinfo s_info;
    if (sysinfo(&s_info) == 0) {
        long uptime_sec = s_info.uptime;
        long days = uptime_sec / (24 * 3600);
        uptime_sec %= (24 * 3600);
        long hours = uptime_sec / 3600;
        uptime_sec %= 3600;
        long minutes = uptime_sec / 60;
        long seconds = uptime_sec % 60;

        printf("  %s%-20s%s : %ld days, %ld hrs, %ld mins, %ld secs\n", 
               COLOR_BOLD, "System Uptime", COLOR_RESET, days, hours, minutes, seconds);
    }
}

void show_cpu_info(void) {
    print_section_header("2. CPU INFORMATION");

    // 1. Hardware Architecture & Online Cores via sysconf
    long num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    long total_conf_cores = sysconf(_SC_NPROCESSORS_CONF);
    printf("  %s%-20s%s : %ld Active Cores (%ld Configured)\n", 
           COLOR_BOLD, "CPU Cores", COLOR_RESET, num_cores, total_conf_cores);

    // 2. Read /proc/cpuinfo for Model Name, Clock Speed & Cache Size
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (fp) {
        char line[256];
        char model_name[256] = "Unknown CPU Model";
        char cpu_mhz[64] = "Unknown";
        char cache_size[64] = "Unknown";
        int found_model = 0;

        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "model name", 10) == 0 && !found_model) {
                char *colon = strchr(line, ':');
                if (colon) {
                    strncpy(model_name, colon + 2, sizeof(model_name) - 1);
                    size_t len = strlen(model_name);
                    if (len > 0 && model_name[len-1] == '\n') model_name[len-1] = '\0';
                    found_model = 1;
                }
            } else if (strncmp(line, "cpu MHz", 7) == 0) {
                char *colon = strchr(line, ':');
                if (colon) {
                    strncpy(cpu_mhz, colon + 2, sizeof(cpu_mhz) - 1);
                    size_t len = strlen(cpu_mhz);
                    if (len > 0 && cpu_mhz[len-1] == '\n') cpu_mhz[len-1] = '\0';
                }
            } else if (strncmp(line, "cache size", 10) == 0) {
                char *colon = strchr(line, ':');
                if (colon) {
                    strncpy(cache_size, colon + 2, sizeof(cache_size) - 1);
                    size_t len = strlen(cache_size);
                    if (len > 0 && cache_size[len-1] == '\n') cache_size[len-1] = '\0';
                }
            }
        }
        fclose(fp);

        printf("  %s%-20s%s : %s\n", COLOR_BOLD, "Model Name", COLOR_RESET, model_name);
        if (strcmp(cpu_mhz, "Unknown") != 0) {
            printf("  %s%-20s%s : %s MHz\n", COLOR_BOLD, "CPU Frequency", COLOR_RESET, cpu_mhz);
        }
        if (strcmp(cache_size, "Unknown") != 0) {
            printf("  %s%-20s%s : %s\n", COLOR_BOLD, "Cache Size", COLOR_RESET, cache_size);
        }
    } else {
        printf("  Could not open /proc/cpuinfo\n");
    }
}

void show_memory_info(void) {
    print_section_header("3. MEMORY INFORMATION");

    struct sysinfo s_info;
    if (sysinfo(&s_info) == 0) {
        double unit_gb = 1024.0 * 1024.0 * 1024.0;
        double total_ram_gb = (double)s_info.totalram * s_info.mem_unit / unit_gb;
        double free_ram_gb  = (double)s_info.freeram  * s_info.mem_unit / unit_gb;
        double buffer_ram_gb = (double)s_info.bufferram * s_info.mem_unit / unit_gb;
        double used_ram_gb  = total_ram_gb - free_ram_gb - buffer_ram_gb;
        double used_percent = (used_ram_gb / total_ram_gb) * 100.0;

        double total_swap_gb = (double)s_info.totalswap * s_info.mem_unit / unit_gb;
        double free_swap_gb  = (double)s_info.freeswap  * s_info.mem_unit / unit_gb;
        double used_swap_gb  = total_swap_gb - free_swap_gb;

        printf("  %s%-20s%s : %.2f GB\n", COLOR_BOLD, "Total RAM", COLOR_RESET, total_ram_gb);
        printf("  %s%-20s%s : %.2f GB (%.1f%%)\n", COLOR_BOLD, "Used RAM", COLOR_RESET, used_ram_gb, used_percent);
        printf("  %s%-20s%s : %.2f GB\n", COLOR_BOLD, "Free RAM", COLOR_RESET, free_ram_gb);
        printf("  %s%-20s%s : %.2f GB\n", COLOR_BOLD, "Buffer / Cache", COLOR_RESET, buffer_ram_gb);
        printf("  %s%-20s%s : %.2f GB (Used: %.2f GB, Free: %.2f GB)\n", 
               COLOR_BOLD, "Swap Memory", COLOR_RESET, total_swap_gb, used_swap_gb, free_swap_gb);
    } else {
        perror("sysinfo failed");
    }
}

void show_all_info(void) {
    show_system_info();
    show_cpu_info();
    show_memory_info();
}
