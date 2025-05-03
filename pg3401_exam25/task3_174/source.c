#include "colors.h"
#include "source.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int menu_option;
    char user_input[5];
    while (1)
    {
        display_menu();
        printf("Enter your choice (1-8): ");

        // Exit early
        if (fgets(user_input, sizeof(user_input), stdin) == NULL)
        {
            printf("Input error. Exiting.\n");
            break;
        }

        user_input[strcspn(user_input, "\n")] = 0;

        // input validation
        if (strspn(user_input, "0123456789") != strlen(user_input))
        {
            printf(RED "Invalid input. Please enter a number.\n" RESET);
            continue;
        }

        menu_option = atoi(user_input);

        switch (menu_option)
        {
        case 1:
            printf("Selected 1\n");
            break;
        case 2:
            printf("Selected 2\n");
        case 0:
            printf("Exiting.\n");
            return 0;
        default:
            printf("Invalid option. Please select 1, 2, or 0 to exit.\n");
            break;
        }
    }
}