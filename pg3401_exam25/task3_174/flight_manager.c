#include "source.h"
#include "colors.h"
#include <stdio.h>

int flight_list = 0;

int get_flight_list()
{
    return flight_list;
}

Flight *add_flight(Flight *head, Flight *new_flight)
{
    // Increment the list size
    flight_list++;

    // If the list is empty, new flight becomes the head
    if (head == NULL)
    {
        new_flight->next = NULL;
        new_flight->prev = NULL;
        return new_flight;
    }

    // Add to the beginning of the list (prepend)
    new_flight->next = head;
    new_flight->prev = NULL;
    head->prev = new_flight;
    printf("Flight with ID %d added successfully\n", new_flight->flight_id);
    printf("Departure Time: %d\n", new_flight->time_of_departure);
    printf("Destination: %s\n", new_flight->destination);
    printf("Total Seats: %d\n", new_flight->seats);
    printf("Next flight ID: %d\n", head->flight_id);
    return new_flight; // New flight is now the head
}

Flight *remove_flight(Flight *head, int flight_id)
{
    Flight *current = head;
    Flight *new_head = head;

    // Find the flight to delete
    while (current != NULL)
    {
        if (current->flight_id == flight_id)
        {
            // Update links
            if (current->prev != NULL)
            {
                current->prev->next = current->next;
            }
            else
            {
                // Deleting the head
                new_head = current->next;
            }

            if (current->next != NULL)
            {
                current->next->prev = current->prev;
            }

            // Free memory
            free(current);
            flight_list--;
            return new_head;
        }
        current = current->next;
    }

    return head; // Flight not found, return original head
}

Flight *print_flight_by_number(Flight *head, int n)
{
    Flight *current = head;
    int count = 1; // Start counting from 1

    // Check for invalid index or empty list
    if (n <= 0 || head == NULL)
    {
        printf("Error: Invalid flight number or empty list.\n");
        return NULL;
    }

    // Traverse the list to the Nth node
    while (current != NULL && count < n)
    {
        current = current->next;
        count++;
    }

    // Check if we found the Nth node
    if (current == NULL)
    {
        printf("Error: Flight number %d not found (list too short).\n", n);
        return NULL;
    }

    // --- Print Flight Details ---
    printf("--- Flight %d ---\n", n);
    printf("Flight ID: %d\n", current->flight_id);
    printf("Destination: %s\n", current->destination);
    printf("Total Seats: %d\n", current->seats);
    printf("Departure Time: %d\n", current->time_of_departure); // Assuming int format

    // --- Print Passenger/Booking Details ---
    printf("Bookings:\n");
    int bookings_found = 0;
    // Assuming bookings[0] is invalid if seat_number is 0 or name is empty
    // A better approach would be to track the actual number of bookings.
    for (int i = 0; i < 100; ++i)
    {
        // Check if this booking slot is actually used
        // THIS IS A GUESS - you need a reliable way to know if a slot is used!
        // Maybe check if seat_number is non-zero?
        if (current->bookings[i].seat_number != 0)
        {
            printf("  - Seat: %d, Name: %s, Age: %d\n",
                   current->bookings[i].seat_number,
                   current->bookings[i].passenger->name,
                   current->bookings[i].passenger->age);
            bookings_found++;
        }
    }

    if (bookings_found == 0)
    {
        printf("  No bookings found for this flight.\n");
    }
    printf("------------------\n");

    return current; // Return pointer to the found flight
}