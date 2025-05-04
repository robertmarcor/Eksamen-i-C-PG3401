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

    printf("\n===== FLIGHT DEPARTURE LIST =====\n");
    printf("Total flights: %d\n\n", list->count);

    FlightNode *current = list->head;
    int flightNum = 1;

    while (current != NULL)
    {
        printf("%d. Flight: %s to %s, Departure: %04d, Seats: %d\n",
               flightNum++, current->flightId, current->destination,
               current->departureTime, current->seats);
        current = current->next;
    }
    printf("===============================\n\n");
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

    printf("\n===== REMOVE FLIGHT =====\n");
    printf("Enter flight ID to remove: ");
    scanf("%s", flightId);
    clear_input_buffer();

    remove_flight(list, flightId);
    printf("Flight %s removed successfully.\n", flightId);
}

void remove_passenger_menu(FlightDepartureList *list)
{
    char flightId[20];
    int seatNumber;

    printf("\n===== REMOVE PASSENGER =====\n");
    printf("Enter flight ID: ");
    scanf("%s", flightId);
    clear_input_buffer();

    FlightNode *flight = find_flight_by_id(list, flightId);
    if (flight == NULL)
    {
        printf("Flight %s not found.\n", flightId);
        return;
    }

    printf("Enter seat number to remove: ");
    scanf("%d", &seatNumber);
    clear_input_buffer();

    remove_passenger(flight, seatNumber);
    printf("Passenger with seat number %d removed from flight %s.\n", seatNumber, flightId);
}

void add_sample_data(FlightDepartureList *list)
{
    // Add sample flights
    add_flight(list, "BA-42", "Bergen", 100, 1200);
    add_flight(list, "AA-12", "Oslo", 200, 1500);
    add_flight(list, "CH-99", "Hong Kong", 300, 2130);

    // Find the flights
    FlightNode *bergenFlight = find_flight_by_id(list, "BA-42");
    FlightNode *osloFlight = find_flight_by_id(list, "AA-12");
    FlightNode *hongKongFlight = find_flight_by_id(list, "CH-99");

    add_passenger(bergenFlight, 12, "Ole Hansen", 35);
    add_passenger(bergenFlight, 5, "Ingrid Larsen", 42);
    add_passenger(bergenFlight, 23, "Magnus Olsen", 28);

    add_passenger(osloFlight, 15, "Sigrid Johansen", 31);
    add_passenger(osloFlight, 42, "Anders Pedersen", 48);

    add_passenger(hongKongFlight, 7, "Wei Zhang", 27);
    add_passenger(hongKongFlight, 19, "Li Chen", 39);
    add_passenger(hongKongFlight, 3, "Mei Wong", 24);

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