#include "source.h"
#include <stdio.h>
#include <stdlib.h>

// Function to create a new passenger node
PassengerNode *create_passenger(int seatNumber, const char *name, int age)
{
    PassengerNode *newPassenger = (PassengerNode *)malloc(sizeof(PassengerNode));
    if (newPassenger == NULL)
    {
        fprintf(stderr, "Memory allocation failed for passenger\n");
        exit(EXIT_FAILURE);
    }

    newPassenger->seatNumber = seatNumber;

    newPassenger->name = (char *)malloc(strlen(name) + 1);
    if (newPassenger->name == NULL)
    {
        fprintf(stderr, "Memory allocation failed for passenger name\n");
        free(newPassenger);
        exit(EXIT_FAILURE);
    }
    strcpy(newPassenger->name, name);

    newPassenger->age = age;
    newPassenger->next = NULL;

    return newPassenger;
}

// Function to create a new flight node
FlightNode *create_flight(const char *flightId, const char *destination, int seats, int departureTime)
{
    FlightNode *newFlight = (FlightNode *)malloc(sizeof(FlightNode));
    if (newFlight == NULL)
    {
        fprintf(stderr, "Memory allocation failed for flight\n");
        exit(EXIT_FAILURE);
    }

    newFlight->flightId = (char *)malloc(strlen(flightId) + 1);
    if (newFlight->flightId == NULL)
    {
        fprintf(stderr, "Memory allocation failed for flight ID\n");
        free(newFlight);
        exit(EXIT_FAILURE);
    }
    strcpy(newFlight->flightId, flightId);

    newFlight->destination = (char *)malloc(strlen(destination) + 1);
    if (newFlight->destination == NULL)
    {
        fprintf(stderr, "Memory allocation failed for destination\n");
        free(newFlight->flightId);
        free(newFlight);
        exit(EXIT_FAILURE);
    }
    strcpy(newFlight->destination, destination);

    newFlight->seats = seats;
    newFlight->departureTime = departureTime;
    newFlight->passengers = NULL;
    newFlight->prev = NULL;
    newFlight->next = NULL;

    return newFlight;
}

// Initialize a new flight departure list
FlightDepartureList *init_flight_departure_list()
{
    FlightDepartureList *list = (FlightDepartureList *)malloc(sizeof(FlightDepartureList));
    if (list == NULL)
    {
        fprintf(stderr, "Memory allocation failed for flight departure list\n");
        exit(EXIT_FAILURE);
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;

    return list;
}

// Function to add a passenger to a flight (maintaining sorted order by seat number)
void add_passenger(FlightNode *flight, int seatNumber, const char *name, int age)
{
    // Check if the seat number is valid
    if (seatNumber <= 0 || seatNumber > flight->seats)
    {
        fprintf(stderr, "Invalid seat number %d for flight %s\n", seatNumber, flight->flightId);
        return;
    }

    // Check if seat is already taken
    PassengerNode *current = flight->passengers;
    while (current != NULL)
    {
        if (current->seatNumber == seatNumber)
        {
            fprintf(stderr, "Seat %d is already taken on flight %s\n", seatNumber, flight->flightId);
            return;
        }
        current = current->next;
    }

    PassengerNode *newPassenger = create_passenger(seatNumber, name, age);

    // If no passengers or new passenger has lower seat number than head
    if (flight->passengers == NULL || seatNumber < flight->passengers->seatNumber)
    {
        newPassenger->next = flight->passengers;
        flight->passengers = newPassenger;
        return;
    }

    // Find the correct position to insert (keeping the list sorted by seat number)
    current = flight->passengers;
    while (current->next != NULL && current->next->seatNumber < seatNumber)
    {
        current = current->next;
    }

    // Insert after current
    newPassenger->next = current->next;
    current->next = newPassenger;
}

// Function to add a flight to the departure list
// Return codes: 0 = success, 1 = already exists, 2 = other error
int add_flight(FlightDepartureList *list, const char *flightId, const char *destination,
               int seats, int departureTime)
{
    // Check if flight ID already exists
    FlightNode *current = list->head;
    while (current != NULL)
    {
        if (strcmp(current->flightId, flightId) == 0)
        {
            fprintf(stderr, "Flight with ID %s already exists\n", flightId);
            return 1; // Return code for "already exists"
        }
        current = current->next;
    }

    FlightNode *newFlight = create_flight(flightId, destination, seats, departureTime);
    if (newFlight == NULL)
    {
        return 2; // Return code for other errors
    }

    // Add to list...
    if (list->head == NULL)
    {
        list->head = newFlight;
        list->tail = newFlight;
    }
    else
    {
        list->tail->next = newFlight;
        newFlight->prev = list->tail;
        list->tail = newFlight;
    }
    list->count++;

    return 0; // Success
}

// Function to find a flight by ID
FlightNode *find_flight_by_id(FlightDepartureList *list, const char *flightId)
{
    FlightNode *current = list->head;

    while (current != NULL)
    {
        if (strcmp(current->flightId, flightId) == 0)
        {
            return current;
        }
        current = current->next;
    }

    return NULL; // Flight not found
}

FlightNode *find_flight_by_position(FlightDepartureList *list, int position)
{
    if (position <= 0 || position > list->count)
    {
        return NULL; // Invalid position
    }

    FlightNode *current = list->head;
    int current_pos = 1;

    while (current != NULL && current_pos < position)
    {
        current = current->next;
        current_pos++;
    }

    return current;
}

// Function to remove a flight from the list
void remove_flight(FlightDepartureList *list, const char *flightId)
{
    FlightNode *flight = find_flight_by_id(list, flightId);

    if (flight == NULL)
    {
        fprintf(stderr, "Flight with ID %s not found\n", flightId);
        return;
    }

    // Update links for adjacent nodes
    if (flight->prev != NULL)
    {
        flight->prev->next = flight->next;
    }
    else
    {
        list->head = flight->next; // Removing the head
    }

    if (flight->next != NULL)
    {
        flight->next->prev = flight->prev;
    }
    else
    {
        list->tail = flight->prev; // Removing the tail
    }

    // Free passenger list
    PassengerNode *currentPassenger = flight->passengers;
    PassengerNode *nextPassenger;

    while (currentPassenger != NULL)
    {
        nextPassenger = currentPassenger->next;
        free(currentPassenger->name);
        free(currentPassenger);
        currentPassenger = nextPassenger;
    }

    // Free flight node
    free(flight->flightId);
    free(flight->destination);
    free(flight);

    list->count--;
}

// Function to remove a passenger from a flight
void remove_passenger(FlightNode *flight, int seatNumber)
{
    if (flight->passengers == NULL)
    {
        fprintf(stderr, "No passengers on flight %s\n", flight->flightId);
        return;
    }

    // If passenger is at the head of the list
    if (flight->passengers->seatNumber == seatNumber)
    {
        PassengerNode *temp = flight->passengers;
        flight->passengers = flight->passengers->next;
        free(temp->name);
        free(temp);
        return;
    }

    // Search for the passenger in the list
    PassengerNode *current = flight->passengers;
    while (current->next != NULL && current->next->seatNumber != seatNumber)
    {
        current = current->next;
    }

    // If passenger found, remove it
    if (current->next != NULL)
    {
        PassengerNode *temp = current->next;
        current->next = temp->next;
        free(temp->name);
        free(temp);
    }
    else
    {
        fprintf(stderr, "Passenger with seat number %d not found on flight %s\n",
                seatNumber, flight->flightId);
    }
}

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

// Function to free the passenger list
void free_passengers(PassengerNode *head)
{
    PassengerNode *current = head;
    PassengerNode *next;

    while (current != NULL)
    {
        next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
}

// Function to free the entire flight departure list
void free_flight_departure_list(FlightDepartureList *list)
{
    FlightNode *current = list->head;
    FlightNode *next;

    while (current != NULL)
    {
        next = current->next;

        // Free the passenger list first
        free_passengers(current->passengers);

        // Free the flight node
        free(current->flightId);
        free(current->destination);
        free(current);

        current = next;
    }

    free(list);
}