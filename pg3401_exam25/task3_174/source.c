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
    FlightDepartureList *departureList = initFlightDepartureList();

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
            addNewFlight(departureList);
            quick_menu(1);
            break;

        case 2:
            addNewPassenger(departureList);
            quick_menu(2);
            break;

        case 3:
            displayAllFlights(departureList);
            quick_menu(3);
            break;

        case 4:
            displayFlightDetailsMenu(departureList);
            quick_menu(4);
            break;
        case 5:
            printf("Find by dest\n");
            quick_menu(5);
            break;

        case 6:
            displayPassengersMenu(departureList);
            quick_menu(6);
            break;

        case 7:
            removeFlightMenu(departureList);
            quick_menu(7);
            break;

        case 8:
            removePassengerMenu(departureList);
            quick_menu(8);
            break;

        case 9:
            addSampleData(departureList);
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