#include "source.h"
#include "colors.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void display_menu(int count)
{
    printf(CYAN "========== Flight List Information  ==========\n" RESET);
    printf("Total flights in the system: %d\n", count);
    printf(CYAN "========== Flight Management System ==========\n" RESET);
    printf(YELLOW "1" RESET ". Add a new flight to list\n");
    printf(YELLOW "2" RESET ". Add a passenger to a flight-departure\n");
    printf(YELLOW "3" RESET ". Retrieve flight by position in list\n");
    printf(YELLOW "9" RESET ". Generate Dummy data\n");

    printf(CYAN "============================================\n" RESET);
}

int get_menu_choice(int count)
{
    char inp[3];

    while (1)
    {
        display_menu(count);
        printf("Enter Selection: " YELLOW);
        if (fgets(inp, sizeof(inp), stdin) == NULL)
        {
            printf(RED "Input error. Exiting.\n" RESET);
            return -1; // Signal error
        }

        inp[strcspn(inp, "\n")] = 0;

        if (strspn(inp, "0123456789") != strlen(inp))
        {
            printf(RED "\nInvalid input. Please enter a number.\n" RESET);
            continue;
        }

        int menu_option = atoi(inp);

        if (menu_option < 0 || menu_option > 9)
        {
            printf(RED "Invalid option. Please select 0-9.\n" RESET);
            continue;
        }
        printf(RESET);
        return menu_option;
    }
}