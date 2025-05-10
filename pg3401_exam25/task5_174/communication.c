#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "communication.h"
#include "ewpdef.h"
#include <time.h>
#include "logging.h"

// Constants for server and header info
#define SERVER_IP "127.0.0.1"
#define PROTOCOL_NAME "SMTP"
#define ASCII_HARD_SPACE 0x20
#define HEADER_SIZE "0064"
#define DELIMETER '|'

// Constants for protocol commands
#define CMD_HELO "HELO"
#define CMD_MAIL_FROM "MAIL FROM"
#define CMD_RCPT_TO "RCPT TO"
#define CMD_DATA "DATA"
#define CMD_QUIT "QUIT"
/*
 * EWP Protocol Implementation
 *
 * This file implements the communication protocol defined in ewpdef.h
 *
 * - Connection establishment with server identification (220)
 * - Client greeting with HELO command (250)
 * - Mail sender specification with MAIL FROM command (250)
 * - Mail recipient specification with RCPT TO command (250)
 * - Data transfer with DATA command, including filename validation (354/501)
 * - Connection termination with QUIT command (221)
 *
 * Each message follows the EWP format with a header containing:
 * - Magic number "EWP"
 * - Data size (4 digits)
 * - Delimiter '|'
 *
 * The implementation provides functions to initialize protocol messages,
 * parse incoming commands, and send formatted messages.
 */

void set_ewp_header(struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER *header, const char *size)
{
    memcpy(header->acMagicNumber, EWA_EXAM25_TASK5_PROTOCOL_MAGIC, 3);
    memcpy(header->acDataSize, size, 4);
    header->acDelimeter[0] = DELIMETER;
}

void get_timestamp(char *buffer, size_t size)
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%d %b %Y, %H:%M:%S", tm_info);
}

void ewp_init_server_accept(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERACCEPT *msg, const char *server_id)
{
    PRINT_PROTOCOL("Initializing SERVER ACCEPT message");
    set_ewp_header(&msg->stHead, HEADER_SIZE);

    // RESPONSE
    strcpy(msg->acStatusCode, "220");
    msg->acHardSpace[0] = ASCII_HARD_SPACE;

    // Get current timestamp
    char time_str[30];
    get_timestamp(time_str, sizeof(time_str));

    // Format the string : SERVERIP PROTOCOL SERVERNAME TIMESTAMP
    snprintf(msg->acFormattedString, sizeof(msg->acFormattedString),
             "%s %s %s %s", SERVER_IP, PROTOCOL_NAME, server_id, time_str);
    msg->acHardZero[0] = '\0';

    PRINT_PROTOCOL("SERVER ACCEPT message ready with status code 220");
    PRINT_SEPARATOR();
}

void ewp_init_helo_response(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERHELO *msg, const char *client_id)
{
    PRINT_PROTOCOL("Initializing HELO response");
    set_ewp_header(&msg->stHead, HEADER_SIZE);

    // RESPONSE
    strcpy(msg->acStatusCode, EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_OK);
    msg->acHardSpace[0] = ASCII_HARD_SPACE;

    // Extract username and client IP from client_id which is in format "USERNAME.CLIENTIP"
    char username[50] = {0};
    char clientip[50] = {0};

    // Find the dot separator
    const char *period = strchr(client_id, '.');
    if (period != NULL)
    {
        // Extract username (everything before the dot)
        int username_len = period - client_id;
        if (username_len < sizeof(username))
        {
            strncpy(username, client_id, username_len);
            username[username_len] = '\0';

            // Extract client IP (everything after the dot)
            strcpy(clientip, period + 1);
        }
    }
    else
    {
        // If no dot found, use the entire string as username
        strcpy(username, client_id);
        strcpy(clientip, "unknown");
    }

    // Format string according to the protocol definition: "CLIENTIP Hello USERNAME"
    snprintf(msg->acFormattedString, sizeof(msg->acFormattedString),
             "%s Greetings %s!", clientip, username);
    msg->acHardZero[0] = '\0';

    PRINT_PROTOCOL("HELO response ready with status code 250");
}

void ewp_init_mail_from_response(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY *msg)
{
    PRINT_PROTOCOL("Initializing MAIL FROM response");
    set_ewp_header(&msg->stHead, HEADER_SIZE);

    // RESPONSE
    strcpy(msg->acStatusCode, EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_OK);
    msg->acHardSpace[0] = ASCII_HARD_SPACE;

    // Format string according to the protocol definition
    strcpy(msg->acFormattedString, "Sender OK");
    msg->acHardZero[0] = '\0';

    PRINT_PROTOCOL("MAIL FROM response ready with status code 250");
}

void ewp_init_rcpt_to_response(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY *msg)
{
    PRINT_PROTOCOL("Initializing RCPT TO response");
    set_ewp_header(&msg->stHead, HEADER_SIZE);

    // RESPONSE
    strcpy(msg->acStatusCode, EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_OK);
    msg->acHardSpace[0] = ASCII_HARD_SPACE;

    // Format string according to the protocol definition
    strcpy(msg->acFormattedString, "Recipient OK");
    msg->acHardZero[0] = '\0';

    PRINT_PROTOCOL("RCPT TO response ready with status code 250");
}

void ewp_init_data_response(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY *msg, int is_valid_filename, const char *filename)
{
    PRINT_PROTOCOL("Initializing DATA response");
    set_ewp_header(&msg->stHead, HEADER_SIZE);

    if (is_valid_filename)
    {
        // RESPONSE
        strcpy(msg->acStatusCode, EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_READY);
        msg->acHardSpace[0] = ASCII_HARD_SPACE;

        // Include filename in the response as specified in the protocol
        snprintf(msg->acFormattedString, sizeof(msg->acFormattedString),
                 "%s", filename);

        PRINT_PROTOCOL("DATA response ready with status code 354 (ready for data)");
    }
    else
    {
        // RESPONSE for error
        strcpy(msg->acStatusCode, "501");
        msg->acHardSpace[0] = ASCII_HARD_SPACE;

        // Format string according to the protocol definition
        snprintf(msg->acFormattedString, sizeof(msg->acFormattedString),
                 "ERROR Invalid filename: %s", filename);

        PRINT_PROTOCOL("DATA response ready with status code 501 (invalid filename)");
    }
    msg->acHardZero[0] = '\0';
}

void ewp_init_quit_response(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY *msg, const char *server_id)
{
    PRINT_PROTOCOL("Initializing QUIT response");
    set_ewp_header(&msg->stHead, HEADER_SIZE);

    // RESPONSE
    strcpy(msg->acStatusCode, EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY_CLOSED);
    msg->acHardSpace[0] = ASCII_HARD_SPACE;

    // Format string as goodbye message
    snprintf(msg->acFormattedString, sizeof(msg->acFormattedString),
             "%s Service closing, Goodbye!", server_id);
    msg->acHardZero[0] = '\0';

    PRINT_PROTOCOL("QUIT response ready with status code 221");
}

// Parse received message to determine command type
int ewp_parse_message(const void *message, size_t message_size)
{
    // For simplicity, we'll return command type as an integer
    // 1: HELO, 2: MAIL FROM, 3: RCPT TO, 4: DATA, 5: QUIT, 0: Unknown

    // Skip the protocol header (8 bytes: 3 for magic, 4 for size, 1 for delimiter)
    const char *msg = (const char *)message + 8;

    if (strncmp(msg, CMD_HELO, strlen(CMD_HELO)) == 0)
    {
        PRINT_PROTOCOL("Command identified: HELO");
        return 1;
    }
    else if (strncmp(msg, CMD_MAIL_FROM, strlen(CMD_MAIL_FROM)) == 0)
    {
        PRINT_PROTOCOL("Command identified: MAIL FROM");
        return 2;
    }
    else if (strncmp(msg, CMD_RCPT_TO, strlen(CMD_RCPT_TO)) == 0)
    {
        PRINT_PROTOCOL("Command identified: RCPT TO");
        return 3;
    }
    else if (strncmp(msg, CMD_DATA, strlen(CMD_DATA)) == 0)
    {
        PRINT_PROTOCOL("Command identified: DATA");
        return 4;
    }
    else if (strncmp(msg, CMD_QUIT, strlen(CMD_QUIT)) == 0)
    {
        PRINT_PROTOCOL("Command identified: QUIT");
        return 5;
    }

    PRINT_ERROR("Unknown command received");
    return 0; // Unknown command
}

int ewp_send_message(int sock_fd, const void *message, size_t message_size)
{
    int result = send(sock_fd, message, message_size, 0);
    if (result < 0)
    {
        PRINT_ERROR("Failed to send message: %d", result);
    }
    return result;
}