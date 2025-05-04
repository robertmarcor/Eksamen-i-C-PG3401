#include "colors.h"
#include "source.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    FlightNode *flight = find_flight_by_id(list, flightId);
    if (flight == NULL)
    {
        printf("Flight %s not found.\n", flightId);
        return;
    }

    printf("Enter passenger name: ");
    scanf("%s", passengerName);

    printf("Enter seat number (1-%d): ", flight->seats);
    scanf("%d", &seatNumber);

    printf("Enter passenger age: ");
    scanf("%d", &age);

    add_passenger(flight, seatNumber, passengerName, age);
    printf("Passenger %s added to flight %s.\n", passengerName, flightId);
}

void display_flight_details_menu(FlightDepartureList *list)
{
    char flightId[20];

    printf("\nEnter flight ID: ");
    scanf("%s", flightId);

    FlightNode *flight = find_flight_by_id(list, flightId);
    display_flight_details(flight);
}

void display_flight_by_pos(FlightDepartureList *list)
{
    int n;

    printf("\nEnter a number: ");
    if (scanf("%d", &n) != 1)
    {
        printf("Invalid input. Please enter a number.\n");
        return;
    }

    FlightNode *flight = find_flight_by_position(list, n);
    if (flight == NULL)
    {
        printf("No flight found at position %d.\n", n);
        return;
    }

    display_flight_details(flight);
}

void display_passengers_menu(FlightDepartureList *list)
{
    char flightId[20];

    printf("\nEnter flight ID: ");
    scanf("%s", flightId);

    FlightNode *flight = find_flight_by_id(list, flightId);
    display_passengers_on_flight(flight);
}

void remove_flight_menu(FlightDepartureList *list)
{
    char flightId[20];

    printf("\n===== REMOVE FLIGHT =====\n");
    printf("Enter flight ID to remove: ");
    scanf("%s", flightId);

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

    FlightNode *flight = find_flight_by_id(list, flightId);
    if (flight == NULL)
    {
        printf("Flight %s not found.\n", flightId);
        return;
    }

    printf("Enter seat number to remove: ");
    scanf("%d", &seatNumber);

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