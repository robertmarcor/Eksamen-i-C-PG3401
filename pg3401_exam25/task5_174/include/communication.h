#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include "ewpdef.h"

// Initialize a server accept message
void ewp_init_server_accept(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERACCEPT *msg, const char *server_id);

// Initialize a response to HELO
void ewp_init_helo_response(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERHELO *response, const char *client_id);

// Initialize a response to MAIL FROM
void ewp_init_mail_from_response(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY *response);

// Initialize a response to RCPT TO
void ewp_init_rcpt_to_response(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY *response);

// Initialize a DATA command response
void ewp_init_data_response(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY *response, int is_valid_filename, const char *filename);

// Initialize a QUIT response
void ewp_init_quit_response(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY *response, const char *server_id);

// Parse received message to determine command type
int ewp_parse_message(const void *message, size_t message_size);

// Send an EWP protocol message
int ewp_send_message(int sock_fd, const void *message, size_t message_size);

#endif /* COMMUNICATION */