#include "colors.h"
#include "source.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int pause()
{
    sleep(1);
}

int get_user_selection()
{
    char inp[3];
    int menu_option = 0;
    int validInput = 0;

    do
    {
        printf("Enter Selection: " YELLOW);

        // early exit
        if (fgets(inp, sizeof(inp), stdin) == NULL)
        {
            printf(RED "Input error. Exiting.\n" RESET);
            return -1;
        }

        inp[strcspn(inp, "\n")] = 0;

        if (strlen(inp) == 0)
        {
            printf(RED "\nNo input detected. Please enter a number.\n" RESET);
            continue;
        }

        if (strspn(inp, "0123456789") != strlen(inp))
        {
            printf(RED "\nInvalid input. Please enter a number.\n" RESET);
            continue;
        }

        menu_option = atoi(inp);
        validInput = 1; // Input is valid, exit the loop

    } while (validInput == 0);
    printf(RESET);
    return menu_option;
}

int main()
{
    FlightDepartureList *departureList = init_flight_departure_list();
    int choice;

    do
    {
        printf("\n \n");
        printf(CYAN "========== Flight List Information  ==========\n" RESET);
        printf("Total flights in the system: " BLUE "%d\n" RESET, departureList->count);
        display_all_flights(departureList);
        printf(CYAN "========== Flight Management Menu ==========\n" RESET);
        printf(YELLOW "1" RESET ". Add a new flight to list\n");
        printf(YELLOW "2" RESET ". Add a passenger to a flight-departure\n");
        printf(YELLOW "3" RESET ". Retrieve flight by position in list\n");
        printf(YELLOW "4" RESET ". Find flight by destination\n");
        printf(YELLOW "5" RESET ". Delete flight and it's Passengers\n");
        printf(YELLOW "6" RESET ". Change seat of passenger\n");
        printf(YELLOW "7" RESET ". Find flight by Passenger name\n");
        printf(YELLOW "8" RESET ". Find Passenger with multiple flights\n");
        printf(YELLOW "9" RESET ". Generate Dummy data\n");
        printf(YELLOW "0" RESET ". Exit Program\n");
        printf(CYAN "============================================\n" RESET);

        choice = get_user_selection();

        switch (choice)
        {
        case 1:
            printf("\nYou selected Option 1\n");
            add_new_flight(departureList);
            pause();
            break;

        case 2:
            printf("\nYou selected Option 2\n");
            add_new_passenger_to_flight(departureList);
            pause();
            break;

        case 3:
            printf("\nYou selected Option 3\n");
            display_flight_by_pos(departureList);
            pause();
            break;

        case 4:
            printf("\nYou selected Option 4\n");
            find_flight_by_destination(departureList);
            pause();
            break;

        case 5:
            printf("\nYou selected Option 5\n");
            remove_flight_menu(departureList);
            pause();
            break;

        case 6:
            printf("\nYou selected Option 6\n");
            change_passenger_seat_menu(departureList);
            pause();
            break;

        case 7:
            printf("\nYou selected Option 7\n");
            find_passenger_by_name(departureList);
            pause();
            break;

        case 8:
            printf("\nYou selected Option 8\n");
            find_passengers_on_multiple_flights(departureList);

            pause();
            break;

        case 9:
            printf("\nYou selected Option 9\n");
            add_sample_data(departureList);
            pause();
            break;

        case 0:
            printf("\nExiting program.\n");
            free_flight_departure_list(departureList);
            break;

        default:
            printf("\nInvalid choice! Please enter a number between 0 and 9.\n");
            free_flight_departure_list(departureList);
            pause();
        }

    } while (choice != 0);
}