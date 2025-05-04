#include "source.h"
#include "colors.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void display_menu(void)
{
    printf(CYAN "========== Flight List Information  ==========\n" RESET);
    printf("Total flights in the system: 0\n");
    printf(CYAN "========== Flight Management System ==========\n" RESET);
    printf(YELLOW "1" RESET ". Add a new flight\n");
    printf(YELLOW "2" RESET ". Add a passenger to a flight\n");
    printf(YELLOW "3" RESET ". Display all flights\n");
    printf(YELLOW "4" RESET ". Display flight details by id\n");
    printf(YELLOW "5" RESET ". Find flight by destination\n");
    printf(YELLOW "6" RESET ". Passanger Menu\n");
    printf(YELLOW "7" RESET ". Remove flight\n");
    printf(YELLOW "8" RESET ". Remove passanger\n");
    printf(YELLOW "9" RESET ". Add Dummy data\n");
    printf(YELLOW "0" RESET ". Exit\n");

    printf(CYAN "============================================\n" RESET);
}

int get_menu_choice(void)
{
    char inp[3];

    while (1)
    {
        display_menu();
        printf("Enter Selection: " YELLOW);
        if (fgets(inp, sizeof(inp), stdin) == NULL)
        {
            printf(RED "Input error. Exiting.\n" RESET);
            return -1; // Signal error
        }

        inp[strcspn(inp, "\n")] = 0;

        if (strspn(inp, "0123456789") != strlen(inp))
        {
            printf(RED "Invalid input. Please enter a number.\n" RESET);
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

int get_quick_choice(int current_option)
{
    char quick_inp[3];

    printf("\n");
    switch (current_option)
    {
    case 1:
        printf("Would you like to add another flight? (y/n, q to quit): \n");
        break;
    case 2:
        printf("Would you like to add another passenger? (y/n, q to quit): \n");
        break;
    case 3:
        printf("Would you like to display flights again? (y/n, q to quit): \n");
        break;
    case 4:
        printf("Would you like to view another flight's details? (y/n, q to quit): \n");
        break;
    case 5:
        printf("Would you like to find another flight by destination? (y/n, q to quit): \n");
        break;
    case 6:
        printf("Would you like to view passengers for another flight? (y/n, q to quit): \n");
        break;
    case 7:
        printf("Would you like to remove another flight? (y/n, q to quit): \n");
        break;
    case 8:
        printf("Would you like to remove another passenger? (y/n, q to quit): \n");
        break;
    default:
        printf("Continue with the same operation? (y/n, q to quit): \n");
        break;
    }

    if (fgets(quick_inp, sizeof(quick_inp), stdin) == NULL)
    {
        return 0;
    }

    quick_inp[strcspn(quick_inp, "\n")] = 0;

    if (quick_inp[0] == 'y' || quick_inp[0] == 'Y')
    {
        return current_option;
    }
    else if (quick_inp[0] == 'q' || quick_inp[0] == 'Q')
    {
        return -1; // Quit
    }
    else
    {
        return 0; // Default to main menu (for 'n' or any other input)
    }
}