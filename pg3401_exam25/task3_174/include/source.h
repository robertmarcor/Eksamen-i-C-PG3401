#ifndef SOURCE_H
#define SOURCE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct passenger
{
    int seat_number;
    char name[100];
    int age;
} Passenger;

typedef struct flight Flight;

struct flight
{
    int flight_id;
    char destination[100];
    int seats;
    int time_of_departure;
    Passenger passengers[100];
    Flight *next;
    Flight *prev;
};

// User interface
int get_menu_choice(void);

// Flight Operations
Flight *print_flight_by_number(Flight *head, int n);
Flight *add_flight(Flight *head, Flight *new_flight);
Flight *remove_flight(Flight *head, int flight_id);
int find_flight_by_destination(Flight *head, const char *destination);
void find_flights_by_same_passanger();

// Passanger Operations
void find_passanger_flights_by_name(Flight *head, const char *passenger_name);
int add_passenger_to_flight(Flight *flight, Passenger *passenger, int seat_number);
int change_passenger_seat(Flight *flight, const char *passenger_name, int new_seat_number);

#endif // SOURCE_H
