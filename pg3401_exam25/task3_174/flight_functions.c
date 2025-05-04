#include "colors.h"
#include "source.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void addNewFlight(FlightDepartureList *list)
{
    char flightId[20];
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

    addFlight(list, flightId, destination, seats, departureTime);
    printf("Flight %s added successfully.\n", flightId);
}

void addNewPassenger(FlightDepartureList *list)
{
    char flightId[20];
    char passengerName[50];
    int seatNumber, age;

    printf("\n===== ADD PASSENGER =====\n");
    printf("Enter flight ID: ");
    scanf("%s", flightId);

    FlightNode *flight = findFlightById(list, flightId);
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

    addPassenger(flight, seatNumber, passengerName, age);
    printf("Passenger %s added to flight %s.\n", passengerName, flightId);
}

void displayFlightDetailsMenu(FlightDepartureList *list)
{
    char flightId[20];

    printf("\nEnter flight ID: ");
    scanf("%s", flightId);

    FlightNode *flight = findFlightById(list, flightId);
    displayFlightDetails(flight);
}

void displayPassengersMenu(FlightDepartureList *list)
{
    char flightId[20];

    printf("\nEnter flight ID: ");
    scanf("%s", flightId);

    FlightNode *flight = findFlightById(list, flightId);
    displayPassengersOnFlight(flight);
}

void removeFlightMenu(FlightDepartureList *list)
{
    char flightId[20];

    printf("\n===== REMOVE FLIGHT =====\n");
    printf("Enter flight ID to remove: ");
    scanf("%s", flightId);

    removeFlight(list, flightId);
    printf("Flight %s removed successfully.\n", flightId);
}

void removePassengerMenu(FlightDepartureList *list)
{
    char flightId[20];
    int seatNumber;

    printf("\n===== REMOVE PASSENGER =====\n");
    printf("Enter flight ID: ");
    scanf("%s", flightId);

    FlightNode *flight = findFlightById(list, flightId);
    if (flight == NULL)
    {
        printf("Flight %s not found.\n", flightId);
        return;
    }

    printf("Enter seat number to remove: ");
    scanf("%d", &seatNumber);

    removePassenger(flight, seatNumber);
    printf("Passenger with seat number %d removed from flight %s.\n", seatNumber, flightId);
}

void addSampleData(FlightDepartureList *list)
{
    // Add sample flights
    addFlight(list, "BA-42", "London", 150, 1430);
    addFlight(list, "LH-101", "Berlin", 200, 1545);
    addFlight(list, "AF-202", "Paris", 180, 1620);

    // Find the flights
    FlightNode *londonFlight = findFlightById(list, "BA-42");
    FlightNode *berlinFlight = findFlightById(list, "LH-101");
    FlightNode *parisFlight = findFlightById(list, "AF-202");

    // Add passengers to London flight
    addPassenger(londonFlight, 12, "John_Smith", 35);
    addPassenger(londonFlight, 5, "Emma_Johnson", 42);
    addPassenger(londonFlight, 23, "Michael_Brown", 28);

    // Add passengers to Berlin flight
    addPassenger(berlinFlight, 15, "Sarah_Davis", 31);
    addPassenger(berlinFlight, 42, "Robert_Wilson", 48);

    // Add passengers to Paris flight
    addPassenger(parisFlight, 7, "Jennifer_Lee", 27);
    addPassenger(parisFlight, 19, "David_Clark", 39);
    addPassenger(parisFlight, 3, "Lisa_Anderson", 24);

    printf("Sample data added successfully.\n");
}