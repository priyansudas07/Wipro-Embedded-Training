#include <stdio.h>
#include <stdlib.h>
#include "sys_info.h"

int main(int argc, char *argv[]) {
    // Non-interactive mode when option passed as argument
    if (argc > 1) {
        int arg = atoi(argv[1]);
        print_banner();
        if (arg == 1) show_system_info();
        else if (arg == 2) show_cpu_info();
        else if (arg == 3) show_memory_info();
        else show_all_info();
        return 0;
    }

    int choice = 0;

    while (1) {
        print_banner();
        printf("  1. System Information (OS, Kernel, Uptime, Hostname)\n");
        printf("  2. CPU Information (Model, Cores, Frequency)\n");
        printf("  3. Memory Information (Total, Used, Free RAM & Swap)\n");
        printf("  4. Display Full System Summary\n");
        printf("  5. Exit Program\n");
        printf("----------------------------------------------------------\n");
        printf("Select an option (1-5): ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("\nInvalid input! Please enter a number between 1 and 5.\n\n");
            continue;
        }

        switch (choice) {
            case 1:
                show_system_info();
                break;
            case 2:
                show_cpu_info();
                break;
            case 3:
                show_memory_info();
                break;
            case 4:
                show_all_info();
                break;
            case 5:
                printf("\nExiting Linux System Information Tool. Goodbye!\n");
                return 0;
            default:
                printf("\nInvalid choice! Please select an option from 1 to 5.\n");
                break;
        }

        printf("\nPress ENTER to return to main menu...");
        getchar();
        getchar();
        printf("\n");
    }

    return 0;
}
