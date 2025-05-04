#ifndef SOURCE_H
#define SOURCE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure for passenger reservation (single-linked list)
typedef struct PassengerNode
{
    int seatNumber;             // SEAT NUMBER
    char *name;                 // NAME of the passenger
    int age;                    // AGE of the passenger
    struct PassengerNode *next; // Pointer to the next passenger
} PassengerNode;

// Structure for flight information (double-linked list)
typedef struct FlightNode
{
    char *flightId;    // FLIGHTID (e.g., BA-42)
    char *destination; // DESTINATION
    int seats;         // Total number of SEATS
    int departureTime; // TIME for departure

    PassengerNode *passengers; // Head of the passenger list (sorted by seat number)

    struct FlightNode *prev; // Pointer to the previous flight
    struct FlightNode *next; // Pointer to the next flight
} FlightNode;

// Structure to manage the entire flight departure list
typedef struct
{
    FlightNode *head; // Pointer to the first flight in the list
    FlightNode *tail; // Pointer to the last flight in the list
    int count;        // Number of flights in the list
} FlightDepartureList;

// User interface
void display_menu();
int get_menu_choice(void);
int get_quick_choice(int current_option);

// Function prototypes
FlightDepartureList *initFlightDepartureList();
void addFlight(FlightDepartureList *list, const char *flightId, const char *destination, int seats, int departureTime);
void addPassenger(FlightNode *flight, int seatNumber, const char *name, int age);
FlightNode *findFlightById(FlightDepartureList *list, const char *flightId);
void removeFlight(FlightDepartureList *list, const char *flightId);
void removePassenger(FlightNode *flight, int seatNumber);
void displayAllFlights(FlightDepartureList *list);
void displayFlightDetails(FlightNode *flight);
void displayPassengersOnFlight(FlightNode *flight);
void freeFlightDepartureList(FlightDepartureList *list);

// Add missing function prototypes from flight_functions.c
void addNewFlight(FlightDepartureList *list);
void addNewPassenger(FlightDepartureList *list);
void displayFlightDetailsMenu(FlightDepartureList *list);
void displayPassengersMenu(FlightDepartureList *list);
void removeFlightMenu(FlightDepartureList *list);
void removePassengerMenu(FlightDepartureList *list);
void addSampleData(FlightDepartureList *list);

#endif // SOURCE_H
