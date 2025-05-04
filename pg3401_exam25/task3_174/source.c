#include "colors.h"
#include "source.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int quick_option = 0;
int menu_option = 0;

void quick_menu(int selection)
{
    quick_option = get_quick_choice(selection);
    if (selection == -1)
    {
        printf("Exiting.\n");
        exit(0);
    }
}

int main()
{
    FlightDepartureList *departureList = init_flight_departure_list();

    while (1)
    {
        // Only show the full menu if we're not repeating an operation
        if (quick_option == 0)
        {
            menu_option = get_menu_choice();
        }
        else
        {
            menu_option = quick_option;
        }

        // Reset the quick choice
        quick_option = 0;
        switch (menu_option)
        {
        case 1:
            add_new_flight(departureList);
            quick_menu(1);
            break;

        case 2:
            add_new_passenger(departureList);
            quick_menu(2);
            break;

        case 3:
            display_all_flights(departureList);
            quick_menu(3);
            break;

        case 4:
            display_flight_details_menu(departureList);
            quick_menu(4);
            break;
        case 5:
            printf("Find by dest\n");
            quick_menu(5);
            break;

        case 6:
            display_passengers_menu(departureList);
            quick_menu(6);
            break;

        case 7:
            remove_flight_menu(departureList);
            quick_menu(7);
            break;

        case 8:
            remove_passenger_menu(departureList);
            quick_menu(8);
            break;

        case 9:
            add_sample_data(departureList);
            break;
        case -1: // Quick menu exit
            printf("Exiting.\n");
            return 0;
        case 0:
            printf("Exiting.\n");
            return 0;

        default:
            printf("Invalid choice. Please try again.\n");
        }
    }
}