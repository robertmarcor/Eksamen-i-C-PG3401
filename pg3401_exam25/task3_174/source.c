#include "colors.h"
#include "source.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Flight *head = NULL;

int main()
{
    int menu_option = 0;
    int quick_choice = 0;

    while (1)
    {
        // Only show the full menu if we're not repeating an operation
        if (quick_choice == 0)
        {
            menu_option = get_menu_choice();
        }
        else
        {
            menu_option = quick_choice;
        }

        // Reset the quick choice
        quick_choice = 0;

        switch (menu_option)
        {
        case 1:
        {
            // Create a new flight
            Flight *new_flight = (Flight *)malloc(sizeof(Flight));
            if (!new_flight)
            {
                printf(RED "Memory allocation failed\n" RESET);
                break;
            }

            // Initialize the flight data
            printf("Enter flight ID: ");
            scanf("%d", &new_flight->flight_id);
            getchar(); // Consume newline

            printf("Enter destination: ");
            fgets(new_flight->destination, sizeof(new_flight->destination), stdin);
            new_flight->destination[strcspn(new_flight->destination, "\n")] = 0; // Remove newline

            printf("Enter total seats: ");
            scanf("%d", &new_flight->seats);

            printf("Enter departure time (24h format): ");
            scanf("%d", &new_flight->time_of_departure);
            getchar(); // Consume newline

            // Initialize bookings array
            memset(new_flight->bookings, 0, sizeof(new_flight->bookings));

            // Add flight to the list
            head = add_flight(head, new_flight);

            printf(GREEN "Flight added successfully\n" RESET);

            // Get quick choice
            quick_choice = get_quick_choice(1);
            if (quick_choice == -1)
            {
                printf("Exiting.\n");
                return 0;
            }
        }
        break;
        case 2:
            printf("Selected 2\n");
            break;
        case 3:
        {
            int a = get_flight_list();
            if (a == 0)
            {
                printf("No flights available %d\n", a);
            }
            else
            {
                int id = 1;
                printf("Enter flight number: ");
                scanf("%d", &id);
                getchar(); // Consume newline
                print_flight_by_number(head, id);
            }

            // Get quick choice
            quick_choice = get_quick_choice(3);
            if (quick_choice == -1)
            {
                printf("Exiting.\n");
                return 0;
            }
        }
        break;
        case 4:
            printf("Selected 4\n");
            break;
        case 5:
            printf("Selected 5\n");
            break;
        case 6:
            printf("Selected 6\n");
            break;
        case 7:
            printf("Selected 7\n");
            break;
        case 8:
            printf("Selected 8\n");
            break;
        case -1: // Quick choice quit
            printf("Exiting.\n");
            return 0;
        case 0:
            printf("Exiting.\n");
            return 0;
        default:
            printf("Invalid option. Please select 1-8, or 0 to exit.\n");
            break;
        }
    }
}