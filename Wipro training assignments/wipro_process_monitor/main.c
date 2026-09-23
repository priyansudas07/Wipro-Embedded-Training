#include <stdio.h>
#include <stdlib.h>
#include "process_monitor.h"

int main(int argc, char *argv[]) {
    // Non-interactive mode for automated testing / log capturing
    if (argc > 1) {
        int arg = atoi(argv[1]);
        print_proc_banner();
        if (arg == 1) list_all_processes();
        else if (arg == 2 && argc > 2) search_process(argv[2]);
        else show_process_stats();
        return 0;
    }

    int choice = 0;
    char query[128];

    while (1) {
        print_proc_banner();
        printf("  1. Show Process Details (PID, Name, State, PPID)\n");
        printf("  2. Search Process by Name or PID\n");
        printf("  3. View Process Statistics Summary\n");
        printf("  4. Exit Program\n");
        printf("----------------------------------------------------------------------\n");
        printf("Select an option (1-4): ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("\nInvalid choice! Please enter a number between 1 and 4.\n\n");
            continue;
        }

        switch (choice) {
            case 1:
                printf("\n");
                list_all_processes();
                break;
            case 2:
                printf("\nEnter process name or PID to search: ");
                scanf("%127s", query);
                search_process(query);
                break;
            case 3:
                show_process_stats();
                break;
            case 4:
                printf("\nExiting Linux Process Monitor. Goodbye!\n");
                return 0;
            default:
                printf("\nInvalid choice! Please select an option between 1 and 4.\n");
                break;
        }

        printf("\nPress ENTER to return to main menu...");
        getchar();
        getchar();
        printf("\n");
    }

    return 0;
}
