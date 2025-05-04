#include "source.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * FLIGHT MANAGER - Core Data Operations
 *
 * This file contains the core data management functions for the flight system.
 * It handles all memory allocation and data structure management
 * The user interface functions are in flight_functions.c.
 *
 * DATA STRUCTURES:
 * - FlightDepartureList: Doubly-linked list of flights for bidirectional traversal
 * - FlightNode: Individual flight with links to prev/next flights and its passengers
 * - PassengerNode: Singly-linked list of passengers for each flight, sorted by seat number
 * - All memory freed should be added to free_flight_departure_list
 */

/**
 * Creates a new passenger node with the given details
 * Allocates memory for the node and the passenger name string
 *
 * @param seatNumber - Seat number for the passenger
 * @param name - Passenger name (will be copied)
 * @param age - Passenger age
 * @return Pointer to newly created passenger node
 */
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

/**
 * Creates a new flight node with the given details
 * Allocates memory for the node and string fields (flightId, destination)
 *
 * @param flightId - Unique identifier for the flight
 * @param destination - Flight destination name
 * @param seats - Total number of seats on the flight
 * @param departureTime - Departure time (e.g., 1430 can be converted to 2:30 PM at some point)
 * @return Pointer to newly created flight node
 */
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

/**
 * Initializes a new flight departure list
 * @return Pointer to newly created flight departure list
 */
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

/**
 * Adds a passenger to a flight, maintaining seat number order
 * Validates seat number and checks if seat is already taken
 *
 * @param flight - The flight to add passenger to
 * @param seatNumber - Seat number for the passenger
 * @param name - Passenger name
 * @param age - Passenger age
 */
void add_passenger(FlightNode *flight, int seatNumber, const char *name, int age)
{
    if (seatNumber <= 0 || seatNumber > flight->seats)
    {
        fprintf(stderr, "Invalid seat number %d for flight %s\n", seatNumber, flight->flightId);
        return;
    }

    // Check if seat is already taken to avoid duplicates
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

    // If no passengers or new passenger has lower seat number than head for sorting by seat number
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

    newPassenger->next = current->next;
    current->next = newPassenger;
}

/**
 * Adds a flight to the departure list at the end
 * Validates flight ID to ensure it's unique
 *
 * @param list - The flight departure list to add to
 * @param flightId - Unique identifier for the flight
 * @param destination - Flight destination
 * @param seats - Number of seats on the flight
 * @param departureTime - Departure time
 * @return 0 for success, 1 if flight ID already exists, 2 for other errors
 */
int add_flight(FlightDepartureList *list, const char *flightId, const char *destination,
               int seats, int departureTime)
{
    FlightNode *current = list->head;
    while (current != NULL)
    {
        if (strcmp(current->flightId, flightId) == 0)
        {
            fprintf(stderr, "Flight with ID %s already exists\n", flightId);
            return 1;
        }
        current = current->next;
    }

    FlightNode *newFlight = create_flight(flightId, destination, seats, departureTime);
    if (newFlight == NULL)
    {
        return 2;
    }

    // Populate the list
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

    return 0;
}

/**
 * Finds a flight by its ID in the departure list
 *
 * @param list - The flight departure list to search in
 * @param flightId - The flight ID to search for
 * @return Pointer to the flight node if found, NULL otherwise
 */
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

    return NULL;
}

/**
 * Finds a flight by its position in the list (1-based indexing)
 *
 * @param list - The flight departure list to search in
 * @param position - The position in the list (1 = first flight)
 * @return Pointer to the flight node if position is valid, NULL otherwise
 */
FlightNode *find_flight_by_position(FlightDepartureList *list, int position)
{
    if (position <= 0 || position > list->count)
    {
        return NULL;
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

/**
 * Removes a flight from the departure list
 * Handles list links, frees passenger list and flight node memory
 *
 * @param list - The flight departure list to remove from
 * @param flightId - ID of the flight to remove
 */
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

    // Freeing the passengers booked on the flight
    PassengerNode *currentPassenger = flight->passengers;
    PassengerNode *nextPassenger;

    while (currentPassenger != NULL)
    {
        nextPassenger = currentPassenger->next;
        free(currentPassenger->name);
        free(currentPassenger);
        currentPassenger = nextPassenger;
    }

    free(flight->flightId);
    free(flight->destination);
    free(flight);

    list->count--;
}

/**
 * Changes a passenger's seat number on a flight
 * Maintains the sorted order of the passenger list by seat number
 *
 * @param flight - The flight containing the passenger
 * @param oldSeatNumber - Current seat number of the passenger
 * @param newSeatNumber - New seat number to assign
 * @return 0 for success, or error code: 1=flight null, 2=invalid seat,
 *         3=seat taken, 4=passenger not found
 */
int change_passenger_seat(FlightNode *flight, int oldSeatNumber, int newSeatNumber)
{
    // Check if the flight exists
    if (flight == NULL)
    {
        fprintf(stderr, "Flight does not exist\n");
        return 1;
    }

    // Check if the new seat number is valid
    if (newSeatNumber <= 0 || newSeatNumber > flight->seats)
    {
        fprintf(stderr, "Invalid new seat number %d for flight %s\n", newSeatNumber, flight->flightId);
        return 2;
    }

    // Check if new seat is already taken
    PassengerNode *current = flight->passengers;
    while (current != NULL)
    {
        if (current->seatNumber == newSeatNumber)
        {
            fprintf(stderr, "Seat %d is already taken on flight %s\n", newSeatNumber, flight->flightId);
            return 3;
        }
        current = current->next;
    }

    // Find the passenger with the old seat number
    current = flight->passengers;
    PassengerNode *target = NULL;

    while (current != NULL)
    {
        if (current->seatNumber == oldSeatNumber)
        {
            target = current;
            break;
        }
        current = current->next;
    }

    if (target == NULL)
    {
        fprintf(stderr, "No passenger found in seat %d on flight %s\n", oldSeatNumber, flight->flightId);
        return 4;
    }

    // Remove the passenger from the linked list
    if (target == flight->passengers)
    {
        flight->passengers = target->next;
    }
    else
    {
        current = flight->passengers;
        while (current != NULL && current->next != target)
        {
            current = current->next;
        }

        if (current != NULL)
        {
            current->next = target->next;
        }
    }

    target->seatNumber = newSeatNumber;
    target->next = NULL;

    // Reinsert the passenger in the correct position (maintaining sort order)
    if (flight->passengers == NULL || newSeatNumber < flight->passengers->seatNumber)
    {
        target->next = flight->passengers;
        flight->passengers = target;
        return 0;
    }

    current = flight->passengers;
    while (current->next != NULL && current->next->seatNumber < newSeatNumber)
    {
        current = current->next;
    }

    target->next = current->next;
    current->next = target;

    return 0;
}

/**
 * Frees all memory for a passenger list
 * Helper function used by free_flight_departure_list
 *
 * @param head - Head of the passenger list to free
 */
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

/**
 * Frees all memory associated with the flight departure list
 * Includes all flights, passengers, and dynamically allocated strings
 *
 * @param list - The flight departure list to free
 */
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