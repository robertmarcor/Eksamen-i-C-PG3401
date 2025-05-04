#ifndef SOURCE_H
#define SOURCE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct PassengerNode
{
    int seatNumber;
    char *name;
    int age;
    struct PassengerNode *next;
} PassengerNode;

typedef struct FlightNode
{
    char *flightId;
    char *destination;
    int seats;
    int departureTime;

    PassengerNode *passengers;

    struct FlightNode *prev;
    struct FlightNode *next;
} FlightNode;

typedef struct
{
    FlightNode *head;
    FlightNode *tail;
    int count;
} FlightDepartureList;

// Helper functions
void clear_input_buffer(void);

// ===== DATA MANAGEMENT FUNCTIONS =====
FlightDepartureList *init_flight_departure_list();
// Flight management operations
FlightNode *create_flight(const char *flightId, const char *destination, int seats, int departureTime);

int add_flight(FlightDepartureList *list, const char *flightId, const char *destination, int seats, int departureTime);
void remove_flight(FlightDepartureList *list, const char *flightId);

FlightNode *find_flight_by_id(FlightDepartureList *list, const char *flightId);
FlightNode *find_flight_by_position(FlightDepartureList *list, int position);

void free_flight_departure_list(FlightDepartureList *list);

// Passenger management operations
PassengerNode *create_passenger(int seatNumber, const char *name, int age);
void add_passenger(FlightNode *flight, int seatNumber, const char *name, int age);
void remove_passenger(FlightNode *flight, int seatNumber);
void free_passengers(PassengerNode *head);

// ===== DISPLAY FUNCTIONS =====
// UI display functions
void display_all_flights(FlightDepartureList *list);
void display_flight_details(FlightNode *flight);
void display_passengers_on_flight(FlightNode *flight);

// Menu interaction functions
void add_new_flight(FlightDepartureList *list);
void add_new_passenger(FlightDepartureList *list);
void display_flight_details_menu(FlightDepartureList *list);
void display_flight_by_pos(FlightDepartureList *list);
void find_flight_position_by_id(FlightDepartureList *list);
void display_passengers_menu(FlightDepartureList *list);
void remove_flight_menu(FlightDepartureList *list);
void remove_passenger_menu(FlightDepartureList *list);
void add_sample_data(FlightDepartureList *list);
void find_flight_by_destination(FlightDepartureList *list);

#endif // SOURCE_H
