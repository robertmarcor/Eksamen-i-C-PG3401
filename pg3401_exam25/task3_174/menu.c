#include "source.h"
#include "colors.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void display_menu(void)
{
    printf(CYAN "========== Flight List Information  ==========\n" RESET);
    printf("Total flights in the system: " BLUE "%d\n" RESET, get_flight_list());
    printf(CYAN "========== Flight Management System ==========\n" RESET);
    printf(YELLOW "1" RESET ". Add a new flight\n");
    printf(YELLOW "2" RESET ". Add a passenger to a flight\n");
    printf(YELLOW "3" RESET ". Display flight details by index\n");
    printf(YELLOW "4" RESET ". Find flight by destination\n");
    printf(YELLOW "5" RESET ". Delete a flight\n");
    printf(YELLOW "6" RESET ". Change passenger seat\n");
    printf(YELLOW "7" RESET ". Search for a passenger by name\n");
    printf(YELLOW "8" RESET ". Find passengers with multiple bookings\n");
    printf(YELLOW "0" RESET ". Exit\n");
    printf(CYAN "============================================\n" RESET);
}

int get_menu_choice(void)
{
    char user_input[5];

    while (1)
    {
        display_menu();
        printf("Enter Selection: " YELLOW);
        if (fgets(user_input, sizeof(user_input), stdin) == NULL)
        {
            printf(RED "Input error. Exiting.\n" RESET);
            return -1; // Signal error
        }

        user_input[strcspn(user_input, "\n")] = 0;

        if (strspn(user_input, "0123456789") != strlen(user_input))
        {
            printf(RED "Invalid input. Please enter a number.\n" RESET);
            continue;
        }

        int menu_option = atoi(user_input);

        if (menu_option < 0 || menu_option > 8)
        {
            printf(RED "Invalid option. Please select 0-8.\n" RESET);
            continue;
        }
        printf(RESET);
        return menu_option;
    }
}

int get_quick_choice(int current_option)
{
    char choice[3];

    printf("\n");
    switch (current_option)
    {
    case 1:
        printf("Would you like to add another flight? (y/n, q to quit): ");
        break;
    case 3:
        printf("Would you like to view another flight? (y/n, q to quit): ");
        break;
    case 5:
        printf("Would you like to delete another flight? (y/n, q to quit): ");
        break;
    default:
        printf("Continue with the same operation? (y/n, q to quit): ");
        break;
    }

    if (fgets(choice, sizeof(choice), stdin) == NULL)
    {
        return 0; // Return to main menu on error
    }

    // Remove newline if present
    choice[strcspn(choice, "\n")] = 0;

    // Check user's choice
    if (choice[0] == 'y' || choice[0] == 'Y')
    {
        return current_option; // Repeat the same operation
    }
    else if (choice[0] == 'q' || choice[0] == 'Q')
    {
        return -1; // Quit
    }
    else
    {
        return 0; // Default to main menu (for 'n' or any other input)
    }
}