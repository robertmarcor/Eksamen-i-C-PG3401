#include "colors.h"
#include "source.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * FLIGHT MANAGEMENT SYSTEM SUMMARY
 *
 * DATA STRUCTURE:
 * - Flights are stored in a doubly linked list (FlightDepartureList) allowing bidirectional traversal
 * - Each flight node contains a singly linked list of passengers sorted by seat number
 * - Flight operations (add, find, remove) traverse the list using next/prev pointers
 *
 * FUNCTION ORGANIZATION:
 * 1. Helper Functions - Utility functions used across multiple operations
 * 2. Display Functions - Functions that show information to users without modifying data
 * 3. Search Functions - Functions for finding specific flights or passengers
 * 4. UI Functions - Menu-based functions that handle user interaction
 *
 * IMPLEMENTATION DETAILS:
 * - The UI functions in this file handle user interaction
 * - The underlying data operations are performed by functions in flight_manager.c
 * - All memory is dynamically allocated and freed in flight_manager.c free_flight_departure_list
 * - Passengers are sorted by seat number within each flight's passenger list
 */

/* Helper Functions */
void clear_input_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void str_to_lowercase(char *dest, const char *src, size_t dest_size)
{
    size_t i;
    for (i = 0; src[i] && i < dest_size - 1; i++)
    {
        dest[i] = tolower(src[i]);
    }
    dest[i] = '\0';
}

int count_passengers(FlightNode *flight)
{
    if (flight == NULL)
        return 0;

    int count = 0;
    PassengerNode *current = flight->passengers;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    return count;
}

/* Display Functions */
void display_all_flights(FlightDepartureList *list)
{
    if (list->head == NULL)
    {
        printf(RED "No flights in the system.\n" RESET);
        return;
    }

    printf(MAGENTA "\n========== FLIGHT DEPARTURE LIST ==========\n" RESET);
    printf("Total flights: %d\n\n", list->count);

    FlightNode *current = list->head;
    int flightNum = 1;

    while (current != NULL)
    {
        printf(MAGENTA "%d. Flight: %s to %s, Departure: %04d\n" RESET,
               flightNum++, current->flightId, current->destination,
               current->departureTime);
        current = current->next;
    }
    printf("\n");
}

void display_flight_details(FlightNode *flight)
{
    if (flight == NULL)
    {
        printf(RED "Flight not found.\n" RESET);
        return;
    }

    printf(BLUE "\n===== FLIGHT DETAILS =====\n" RESET);
    printf("Flight ID: %s\n", flight->flightId);
    printf("Destination: %s\n", flight->destination);
    printf("Departure Time: %04d\n", flight->departureTime);
    printf("Total Seats: %d\n", flight->seats);
    printf("Passengers: %d\n", count_passengers(flight));
    printf(BLUE "=========================\n\n" RESET);
}

void display_passengers_on_flight(FlightNode *flight)
{
    if (flight == NULL)
    {
        printf(RED "Flight not found.\n" RESET);
        return;
    }

    if (flight->passengers == NULL)
    {
        printf(YELLOW "No passengers on flight %s to %s.\n" RESET, flight->flightId, flight->destination);
        return;
    }

    printf(GREEN "\n===== PASSENGERS ON FLIGHT %s =====\n" RESET, flight->flightId);
    printf("Destination: %s, Departure: %04d\n\n", flight->destination, flight->departureTime);

    PassengerNode *current = flight->passengers;
    int count = 1;

    while (current != NULL)
    {
        printf("%d. Seat %d: %s, Age: %d\n",
               count++, current->seatNumber, current->name, current->age);
        current = current->next;
    }

    printf(GREEN "==================================\n\n" RESET);
}

/* Search Functions */

/**
 * Searches for passengers by name across all flights
 * Performs case-insensitive partial matching on names
 */
void find_passenger_by_name(FlightDepartureList *list)
{
    if (list->head == NULL)
    {
        printf(RED "No flights in the system.\n" RESET);
        return;
    }

    char passengerName[100];
    int found = 0;

    printf(BLUE "\n===== FIND PASSENGER BY NAME =====\n" RESET);
    printf("Enter passenger name: ");
    scanf("%s", passengerName);
    clear_input_buffer();

    char name_lower[100];
    str_to_lowercase(name_lower, passengerName, sizeof(name_lower));

    FlightNode *currentFlight = list->head;
    int flightPos = 0;

    printf(YELLOW "\nSearching for passenger: %s\n" RESET, passengerName);

    while (currentFlight != NULL)
    {
        flightPos++;
        PassengerNode *currentPassenger = currentFlight->passengers;

        while (currentPassenger != NULL)
        {
            char passenger_name_lower[100];
            str_to_lowercase(passenger_name_lower, currentPassenger->name, sizeof(passenger_name_lower));

            if (strstr(passenger_name_lower, name_lower) != NULL)
            {
                if (!found)
                {
                    printf(GREEN "\nPassenger '%s' found on the following flights:\n" RESET, passengerName);
                }

                printf(BLUE "--------------------------------\n" RESET);
                printf("Flight: %s (position %d in list)\n", currentFlight->flightId, flightPos);
                printf("Destination: %s\n", currentFlight->destination);
                printf("Departure Time: %04d\n", currentFlight->departureTime);
                printf("Seat Number: %d\n", currentPassenger->seatNumber);
                printf("Passenger Name: %s\n", currentPassenger->name);
                printf("Passenger Age: %d\n", currentPassenger->age);

                found = 1;
            }

            currentPassenger = currentPassenger->next;
        }

        currentFlight = currentFlight->next;
    }

    if (!found)
    {
        printf(RED "No passenger with name '%s' found on any flight.\n" RESET, passengerName);
    }
}

/**
 * Identifies and displays passengers who are booked on multiple flights
 * Uses name and age to match passengers across different flights
 */
void find_passengers_on_multiple_flights(FlightDepartureList *list)
{
    if (list->head == NULL)
    {
        printf(RED "No flights in the system.\n" RESET);
        return;
    }

    printf(MAGENTA "\n===== PASSENGERS BOOKED ON MULTIPLE FLIGHTS =====\n" RESET);
    int foundAny = 0;

    FlightNode *flight1 = list->head;
    while (flight1 != NULL)
    {
        PassengerNode *passenger = flight1->passengers;
        while (passenger != NULL)
        {
            int count = 0;
            int alreadyPrinted = 0;

            // Check if we've already printed this passenger (from an earlier flight)
            FlightNode *prevFlight = list->head;
            while (prevFlight != flight1)
            {
                PassengerNode *prevPassenger = prevFlight->passengers;
                while (prevPassenger != NULL)
                {
                    if (strcmp(passenger->name, prevPassenger->name) == 0 &&
                        passenger->age == prevPassenger->age)
                    {
                        alreadyPrinted = 1;
                        break;
                    }
                    prevPassenger = prevPassenger->next;
                }
                if (alreadyPrinted)
                    break;
                prevFlight = prevFlight->next;
            }

            if (!alreadyPrinted)
            {
                // Count occurrences on all flights
                FlightNode *flight2 = list->head;
                while (flight2 != NULL)
                {
                    PassengerNode *check = flight2->passengers;
                    while (check != NULL)
                    {
                        if (strcmp(passenger->name, check->name) == 0 &&
                            passenger->age == check->age)
                        {
                            count++;
                        }
                        check = check->next;
                    }
                    flight2 = flight2->next;
                }

                if (count > 1)
                {
                    printf(GREEN "Passenger: %s (Age: %d) is booked on %d flights\n" RESET,
                           passenger->name, passenger->age, count);
                    foundAny = 1;
                }
            }

            passenger = passenger->next;
        }

        flight1 = flight1->next;
    }

    if (!foundAny)
    {
        printf(YELLOW "No passengers are booked on multiple flights.\n" RESET);
    }
}

/**
 * Searches for flights by destination
 * Performs case-insensitive exact matching on destination names
 */
void find_flight_by_destination(FlightDepartureList *list)
{
    if (list->head == NULL)
    {
        printf(RED "No flights in the system.\n" RESET);
        return;
    }

    char destination[50];
    int found = 0;

    printf(BLUE "\n===== FIND FLIGHT BY DESTINATION =====\n" RESET);
    printf("Enter destination: ");
    scanf("%s", destination);
    clear_input_buffer();

    char dest_lower[50];
    str_to_lowercase(dest_lower, destination, sizeof(dest_lower));

    FlightNode *current = list->head;
    int position = 0;

    while (current != NULL)
    {
        position++;

        char current_dest_lower[50];
        str_to_lowercase(current_dest_lower, current->destination, sizeof(current_dest_lower));

        if (strcmp(current_dest_lower, dest_lower) == 0)
        {
            printf(GREEN "\nFlight to %s found at position %d (Flight ID: %s)\n" RESET,
                   current->destination, position, current->flightId);
            printf("Departure Time: %04d, Seats: %d\n",
                   current->departureTime, current->seats);
            found = 1;
        }
        current = current->next;
    }

    if (!found)
    {
        printf(RED "No flights found to destination: %s\n" RESET, destination);
    }
}

/* UI Functions */
void add_new_flight(FlightDepartureList *list)
{
    char flightId[20]; // Increased size for safety
    char destination[50];
    int seats, departureTime;

    printf(BLUE "\n===== ADD NEW FLIGHT =====\n" RESET);
    printf("Enter flight ID (e.g., BA-42 Case-Sensetive): ");
    scanf("%s", flightId);

    printf("Enter destination: ");
    scanf("%s", destination);

    printf("Enter number of seats: ");
    scanf("%d", &seats);

    printf("Enter departure time (24-hour format, e.g., 1430 for 2:30 PM): ");
    scanf("%d", &departureTime);

    int result = add_flight(list, flightId, destination, seats, departureTime);

    if (result == 0)
    {
        printf(GREEN "Flight %s added successfully.\n" RESET, flightId);
    }
    else if (result == 1)
    {
        printf(RED "Failed to add flight: Flight ID %s already exists.\n" RESET, flightId);
    }
    else
    {
        printf(RED "Failed to add flight: Unknown error occurred.\n" RESET);
    }
}

void add_new_passenger_to_flight(FlightDepartureList *list)
{
    char flightId[20];
    char passengerName[100];
    int seatNumber, age;

    printf(BLUE "\n===== ADD PASSENGER =====\n" RESET);
    printf("Enter flight ID (Case-Sensetive): ");
    scanf("%s", flightId);
    clear_input_buffer();

    FlightNode *flight = find_flight_by_id(list, flightId);
    if (flight == NULL)
    {
        printf(RED "Flight %s not found.\n" RESET, flightId);
        return;
    }

    printf("Enter passenger name: ");
    scanf("%s", passengerName);
    clear_input_buffer();

    printf("Enter seat number (1-%d): ", flight->seats);
    scanf("%d", &seatNumber);
    clear_input_buffer();

    printf("Enter passenger age: ");
    scanf("%d", &age);
    clear_input_buffer();

    add_passenger(flight, seatNumber, passengerName, age);
    printf(GREEN "Passenger %s added to flight %s.\n" RESET, passengerName, flightId);
}

/**
 * Retrieves and displays a flight by its position in the list starting from 1
 * Shows all flight details including passenger information
 */
void display_flight_by_pos(FlightDepartureList *list)
{
    int n;

    printf(BLUE "\n===== RETRIEVE FLIGHT BY POSITION =====\n" RESET);
    printf("Enter position (1-%d): ", list->count);
    if (scanf("%d", &n) != 1)
    {
        printf(RED "Invalid input. Please enter a number.\n" RESET);
        clear_input_buffer();
        return;
    }

    clear_input_buffer();

    FlightNode *flight = find_flight_by_position(list, n);
    if (flight == NULL)
    {
        printf(RED "No flight found at position %d.\n" RESET, n);
        return;
    }

    printf(BLUE "\n===== FLIGHT AT POSITION %d =====\n" RESET, n);
    printf("Flight ID: %s\n", flight->flightId);
    printf("Destination: %s\n", flight->destination);
    printf("Departure Time: %04d\n", flight->departureTime);
    printf("Total Seats: %d\n", flight->seats);

    int passengerCount = count_passengers(flight);
    printf("Number of Passengers: %d\n", passengerCount);

    if (passengerCount > 0)
    {
        printf(GREEN "\n===== PASSENGER LIST =====\n" RESET);
        PassengerNode *current = flight->passengers;
        int count = 1;
        while (current != NULL)
        {
            printf("%d. Seat %d: %s, Age: %d\n",
                   count++, current->seatNumber, current->name, current->age);
            current = current->next;
        }
    }
    else
    {
        printf(YELLOW "\nNo passengers on this flight.\n" RESET);
    }
}

/**
 * Removes a flight from the system based on flight ID
 */
void remove_flight_menu(FlightDepartureList *list)
{
    char flightId[20];

    printf(RED "\n===== REMOVE FLIGHT =====\n" RESET);
    printf("Enter flight ID to " RED "remove " RESET "eg,.BA-42\n");
    printf("ID: ");
    scanf("%s", flightId);
    clear_input_buffer();

    FlightNode *flight = find_flight_by_id(list, flightId);
    if (flight == NULL)
    {
        printf(RED "Flight %s not found.\n" RESET, flightId);
        return;
    }

    remove_flight(list, flightId);
    printf(GREEN "Flight %s removed successfully.\n" RESET, flightId);
}

void add_sample_data(FlightDepartureList *list)
{
    add_flight(list, "BA-42", "Bergen", 100, 1200);
    add_flight(list, "AA-12", "Oslo", 200, 1500);
    add_flight(list, "CH-99", "Hong Kong", 300, 2130);

    FlightNode *flightExample1 = find_flight_by_id(list, "BA-42");
    FlightNode *flightExample2 = find_flight_by_id(list, "AA-12");
    FlightNode *flightExample3 = find_flight_by_id(list, "CH-99");

    add_passenger(flightExample1, 12, "Ole", 35); // on all flights for testing
    add_passenger(flightExample1, 5, "Ingrid", 42);
    add_passenger(flightExample1, 23, "Magnus", 28);

    add_passenger(flightExample2, 15, "Sigrid", 31);
    add_passenger(flightExample2, 42, "Anders", 48);
    add_passenger(flightExample2, 12, "Ole", 35); // on all flights for testing

    add_passenger(flightExample3, 7, "Wei", 27);
    add_passenger(flightExample3, 19, "Li", 39);
    add_passenger(flightExample3, 3, "Mei", 24);
    add_passenger(flightExample3, 43, "Ole", 35); // on all flights for testing

    printf(GREEN "Sample data generated.\n" RESET);
}

/**
 * Changes a passenger's seat on a specified flight
 * Displays available seats and handles validation(seat number, flight id etc)
 */
void change_passenger_seat_menu(FlightDepartureList *list)
{
    char flightId[20];
    int oldSeat, newSeat;

    printf(BLUE "\n===== CHANGE PASSENGER SEAT =====\n" RESET);
    printf("Enter flight ID: ");
    scanf("%s", flightId);
    clear_input_buffer();

    FlightNode *flight = find_flight_by_id(list, flightId);
    if (flight == NULL)
    {
        printf(RED "Flight %s not found.\n" RESET, flightId);
        return;
    }

    display_passengers_on_flight(flight);

    printf("Enter current seat number: ");
    scanf("%d", &oldSeat);
    clear_input_buffer();

    printf("Enter new seat number (1-%d): ", flight->seats);
    scanf("%d", &newSeat);
    clear_input_buffer();

    int result = change_passenger_seat(flight, oldSeat, newSeat);

    switch (result)
    {
    case 0:
        printf(GREEN "Passenger seat successfully changed from %d to %d.\n" RESET, oldSeat, newSeat);
        break;
    case 1:
        printf(RED "Error: Flight does not exist.\n" RESET);
        break;
    case 2:
        printf(RED "Error: Invalid new seat number %d (valid range: 1-%d).\n" RESET, newSeat, flight->seats);
        break;
    case 3:
        printf(RED "Error: Seat %d is already taken.\n" RESET, newSeat);
        break;
    case 4:
        printf(RED "Error: No passenger found in seat %d.\n" RESET, oldSeat);
        break;
    default:
        printf(RED "Unknown error occurred.\n" RESET);
    }
}