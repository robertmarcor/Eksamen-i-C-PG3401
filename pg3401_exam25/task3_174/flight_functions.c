#include "colors.h"
#include "source.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void clear_input_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

// Display Functions

// Function to display all flights
void display_all_flights(FlightDepartureList *list)
{
    if (list->head == NULL)
    {
        printf("No flights in the system.\n");
        return;
    }

    printf(MAGENTA "\n========== FLIGHT DEPARTURE LIST ==========\n" RESET);
    printf("Total flights: %d\n\n", list->count);

    FlightNode *current = list->head;
    int flightNum = 1;

    while (current != NULL)
    {
        printf("%d. Flight: %s to %s, Departure: %04d\n",
               flightNum++, current->flightId, current->destination,
               current->departureTime);
        current = current->next;
    }
}

// Function to display details of a specific flight
void display_flight_details(FlightNode *flight)
{
    if (flight == NULL)
    {
        printf("Flight not found.\n");
        return;
    }

    printf("\n===== FLIGHT DETAILS =====\n");
    printf("Flight ID: %s\n", flight->flightId);
    printf("Destination: %s\n", flight->destination);
    printf("Departure Time: %04d\n", flight->departureTime);
    printf("Total Seats: %d\n", flight->seats);

    // Count passengers
    int passengerCount = 0;
    PassengerNode *current = flight->passengers;
    while (current != NULL)
    {
        passengerCount++;
        current = current->next;
    }

    printf("Passengers: %d\n", passengerCount);
    printf("=========================\n\n");
}

// Function to display all passengers on a flight
void display_passengers_on_flight(FlightNode *flight)
{
    if (flight == NULL)
    {
        printf("Flight not found.\n");
        return;
    }

    if (flight->passengers == NULL)
    {
        printf("No passengers on flight %s to %s.\n", flight->flightId, flight->destination);
        return;
    }

    printf("\n===== PASSENGERS ON FLIGHT %s =====\n", flight->flightId);
    printf("Destination: %s, Departure: %04d\n\n", flight->destination, flight->departureTime);

    PassengerNode *current = flight->passengers;
    int count = 1;

    while (current != NULL)
    {
        printf("%d. Seat %d: %s, Age: %d\n",
               count++, current->seatNumber, current->name, current->age);
        current = current->next;
    }

    printf("==================================\n\n");
}

// Search for a passenger by name across all flights
void find_passenger_by_name(FlightDepartureList *list)
{
    char passengerName[100];
    int found = 0;

    printf("\n===== FIND PASSENGER BY NAME =====\n");
    printf("Enter passenger name: ");
    scanf("%s", passengerName);
    clear_input_buffer();

    // Convert input name to lowercase for case-insensitive comparison
    char name_lower[100];
    for (int i = 0; passengerName[i]; i++)
    {
        name_lower[i] = tolower(passengerName[i]);
    }
    name_lower[strlen(passengerName)] = '\0';

    // Iterate through all flights
    FlightNode *currentFlight = list->head;
    int flightPos = 0;

    printf("\nSearching for passenger: %s\n", passengerName);

    while (currentFlight != NULL)
    {
        flightPos++;

        // Iterate through passengers of current flight
        PassengerNode *currentPassenger = currentFlight->passengers;
        while (currentPassenger != NULL)
        {
            // Convert passenger name to lowercase for comparison
            char passenger_name_lower[50];
            for (int i = 0; currentPassenger->name[i]; i++)
            {
                passenger_name_lower[i] = tolower(currentPassenger->name[i]);
            }
            passenger_name_lower[strlen(currentPassenger->name)] = '\0';

            // Compare names (case-insensitive)
            if (strstr(passenger_name_lower, name_lower) != NULL)
            {
                if (!found)
                {
                    printf("\nPassenger '%s' found on the following flights:\n", passengerName);
                }

                printf("--------------------------------\n");
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
        printf("No passenger with name '%s' found on any flight.\n", passengerName);
    }
}

// Find passengers booked on multiple flights
void find_passengers_on_multiple_flights(FlightDepartureList *list)
{
    if (list->head == NULL)
    {
        printf("No flights in the system.\n");
        return;
    }

    printf("\n===== PASSENGERS BOOKED ON MULTIPLE FLIGHTS =====\n");
    int foundAny = 0;

    // For each flight
    FlightNode *flight1 = list->head;
    while (flight1 != NULL)
    {
        // Get each passenger on this flight
        PassengerNode *passenger = flight1->passengers;
        while (passenger != NULL)
        {
            int count = 0;

            // Count occurrences on all flights
            FlightNode *flight2 = list->head;
            while (flight2 != NULL)
            {
                PassengerNode *check = flight2->passengers;
                while (check != NULL)
                {
                    // Match if same name and age
                    if (strcmp(passenger->name, check->name) == 0 &&
                        passenger->age == check->age)
                    {
                        count++;
                    }
                    check = check->next;
                }
                flight2 = flight2->next;
            }

            // If found on multiple flights and not printed yet
            if (count > 1)
            {
                // Check if we've already printed this passenger
                // We'll only print the first occurrence to avoid duplicates
                int alreadyPrinted = 0;
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
                    printf("Passenger: %s (Age: %d) is booked on %d flights\n",
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
        printf("No passengers are booked on multiple flights.\n");
    }
}
// UI Interaction Functions

void add_new_flight(FlightDepartureList *list)
{
    char flightId[6];
    char destination[50];
    int seats, departureTime;

    printf("\n===== ADD NEW FLIGHT =====\n");
    printf("Enter flight ID (e.g., BA-42): ");
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
        printf("Flight %s added successfully.\n", flightId);
    }
    else if (result == 1)
    {
        printf("Failed to add flight: Flight ID %s already exists.\n", flightId);
    }
    else
    {
        printf("Failed to add flight: Unknown error occurred.\n");
    }
}

void add_new_passenger(FlightDepartureList *list)
{
    char flightId[20];
    char passengerName[50];
    int seatNumber, age;

    printf("\n===== ADD PASSENGER =====\n");
    printf("Enter flight ID: ");
    scanf("%s", flightId);
    clear_input_buffer();

    FlightNode *flight = find_flight_by_id(list, flightId);
    if (flight == NULL)
    {
        printf("Flight %s not found.\n", flightId);
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
    printf("Passenger %s added to flight %s.\n", passengerName, flightId);
}

// Retrieves flight by position and displays all data including passenger list
void display_flight_by_pos(FlightDepartureList *list)
{
    int n;

    printf("\n===== RETRIEVE FLIGHT BY POSITION =====\n");
    printf("Enter position (1-%d): ", list->count);
    if (scanf("%d", &n) != 1)
    {
        printf("Invalid input. Please enter a number.\n");
        clear_input_buffer();
        return;
    }

    clear_input_buffer();

    FlightNode *flight = find_flight_by_position(list, n);
    if (flight == NULL)
    {
        printf("No flight found at position %d.\n", n);
        return;
    }

    printf("\n===== FLIGHT AT POSITION %d =====\n", n);
    printf("Flight ID: %s\n", flight->flightId);
    printf("Destination: %s\n", flight->destination);
    printf("Departure Time: %04d\n", flight->departureTime);
    printf("Total Seats: %d\n", flight->seats);

    // Count and display passengers
    int passengerCount = 0;
    PassengerNode *current = flight->passengers;
    while (current != NULL)
    {
        passengerCount++;
        current = current->next;
    }
    printf("Number of Passengers: %d\n", passengerCount);

    // Display passenger list if any
    if (passengerCount > 0)
    {
        printf("\n===== PASSENGER LIST =====\n");
        current = flight->passengers;
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
        printf("\nNo passengers on this flight.\n");
    }
}

void remove_flight_menu(FlightDepartureList *list)
{
    char flightId[20];

    printf(RED "\n===== REMOVE FLIGHT =====\n" RESET);
    printf("Enter flight ID to " RED "remove " RESET "eg,.BA-42\n");
    printf("ID: ");
    scanf("%s", flightId);
    clear_input_buffer();

    remove_flight(list, flightId);
    printf("Flight %s removed successfully.\n", flightId);
}

void add_sample_data(FlightDepartureList *list)
{
    // Add sample flights
    add_flight(list, "BA-42", "Bergen", 100, 1200);
    add_flight(list, "AA-12", "Oslo", 200, 1500);
    add_flight(list, "CH-99", "Hong Kong", 300, 2130);

    // Find the flights
    FlightNode *flightExample1 = find_flight_by_id(list, "BA-42");
    FlightNode *flightExample2 = find_flight_by_id(list, "AA-12");
    FlightNode *flightExample3 = find_flight_by_id(list, "CH-99");

    // Flight 1
    add_passenger(flightExample1, 12, "Ole", 35);
    add_passenger(flightExample1, 5, "Ingrid", 42);
    add_passenger(flightExample1, 23, "Magnus", 28);

    // Flight 2
    add_passenger(flightExample2, 15, "Sigrid", 31);
    add_passenger(flightExample2, 42, "Anders", 48);
    add_passenger(flightExample2, 12, "Ole", 35); // on both flights

    // Flight 3
    add_passenger(flightExample3, 7, "Wei", 27);
    add_passenger(flightExample3, 19, "Li", 39);
    add_passenger(flightExample3, 3, "Mei", 24);

    printf("Sample data generated.\n");
}

// Finds flight by destination and returns its position in the list
void find_flight_by_destination(FlightDepartureList *list)
{
    char destination[50];
    int found = 0;

    printf("\n===== FIND FLIGHT BY DESTINATION =====\n");
    printf("Enter destination: ");
    scanf("%s", destination);
    clear_input_buffer();

    FlightNode *current = list->head;
    int position = 0;

    while (current != NULL)
    {
        position++;
        // Use case-insensitive string comparison
        // Convert both strings to lowercase for comparison
        char dest_lower[50];
        char current_dest_lower[50];
        int i;

        // Convert input destination to lowercase
        for (i = 0; destination[i]; i++)
        {
            dest_lower[i] = tolower(destination[i]);
        }
        dest_lower[i] = '\0';

        // Convert current flight destination to lowercase
        for (i = 0; current->destination[i]; i++)
        {
            current_dest_lower[i] = tolower(current->destination[i]);
        }
        current_dest_lower[i] = '\0';

        if (strcmp(current_dest_lower, dest_lower) == 0)
        {
            printf("\nFlight to %s found at position %d (Flight ID: %s)\n",
                   current->destination, position, current->flightId);
            found = 1;

            // Optional: Display more details
            printf("Departure Time: %04d, Seats: %d\n",
                   current->departureTime, current->seats);
        }
        current = current->next;
    }

    if (!found)
    {
        printf("No flights found to destination: %s\n", destination);
    }
}