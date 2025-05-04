#include "colors.h"
#include "source.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int menu_option = 99;

int main()
{
    FlightDepartureList *departureList = init_flight_departure_list();
    int count = departureList->count;

    while (1)
    {
        if (menu_option == 99)
        {
            menu_option = get_menu_choice(count);
        }
        else
        {
            char inp[3];
            printf("\n(y)Display Menu again or (n)Quit Program\n");
            printf("Y/N: " YELLOW);
            scanf("%s", inp);
            printf(RESET);
            if (inp[0] == 'n' || inp[0] == 'N')
            {
                menu_option = 0;
            }
            else if (inp[0] == 'y' || inp[0] == 'Y')
            {
                menu_option = get_menu_choice(count);
            }
        }

        switch (menu_option)
        {
        case 1:
            add_new_flight(departureList);
            break;

        case 2:
            add_new_passenger(departureList);
            break;

        case 3:
            display_flight_by_pos(departureList);
            break;

        case 4:
            display_flight_details_menu(departureList);
            break;
        case 5:
            printf("Find by dest\n");
            break;

        case 6:
            display_passengers_menu(departureList);
            break;

        case 7:
            remove_flight_menu(departureList);
            break;

        case 8:
            remove_passenger_menu(departureList);
            break;

        case 9:
            add_sample_data(departureList);
            break;
        case 0:
            printf("Exiting.\n");
            return 0;

        default:
            printf("Invalid choice. Please try again.\n");
        }
    }
}