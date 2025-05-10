#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include "communication.h"
#include "ewpdef.h"
#include <time.h>

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

// Response codes
#define RESP_SERVER_READY "220"
#define RESP_OK "250"
#define RESP_START_MAIL_INPUT "354"
#define RESP_SERVICE_CLOSING "221"
#define RESP_SYNTAX_ERROR "500"

#define CUSTOM_END_OF_FILE "\r\n.\r\n"

// Helper function to set the common header fields
void set_ewp_header(struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER *header, const char *size)
{
    memcpy(header->acMagicNumber, EWA_EXAM25_TASK5_PROTOCOL_MAGIC, 3);
    memcpy(header->acDataSize, size, 4);
    header->acDelimeter[0] = DELIMETER;
}

// Get formatted timestamp string
void get_timestamp(char *buffer, size_t size)
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%d %b %Y, %H:%M:%S", tm_info);
}

// Initialize a server accept message
void ewp_init_server_accept(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERACCEPT *msg, const char *server_id)
{
    // Set Header
    set_ewp_header(&msg->stHead, HEADER_SIZE);

    // RESPONSE
    strcpy(msg->acStatusCode, RESP_SERVER_READY);
    msg->acHardSpace[0] = ASCII_HARD_SPACE;

    // Get current timestamp
    char time_str[30];
    get_timestamp(time_str, sizeof(time_str));

    // Format the string correctly: SERVERIP PROTOCOL SERVERNAME TIMESTAMP
    snprintf(msg->acFormattedString, sizeof(msg->acFormattedString),
             "%s %s %s %s", SERVER_IP, PROTOCOL_NAME, server_id, time_str);
    msg->acHardZero[0] = '\0';
}

// Initialize a response to HELO
void ewp_init_helo_response(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERHELO *msg, const char *client_id)
{
    // Set Header
    set_ewp_header(&msg->stHead, HEADER_SIZE);

    // RESPONSE
    strcpy(msg->acStatusCode, RESP_OK);
    msg->acHardSpace[0] = ASCII_HARD_SPACE;

    // Format string according to the protocol definition
    snprintf(msg->acFormattedString, sizeof(msg->acFormattedString),
             "%s Hello %s", SERVER_IP, client_id);
    msg->acHardZero[0] = '\0';
}

// Initialize a response to MAIL FROM
void ewp_init_mail_from_response(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY *msg)
{
    // Set Header
    set_ewp_header(&msg->stHead, HEADER_SIZE);

    // RESPONSE
    strcpy(msg->acStatusCode, RESP_OK);
    msg->acHardSpace[0] = ASCII_HARD_SPACE;

    // Format string according to the protocol definition
    strcpy(msg->acFormattedString, "Sender OK");
    msg->acHardZero[0] = '\0';
}

// Initialize a response to RCPT TO
void ewp_init_rcpt_to_response(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY *msg)
{
    // Set Header
    set_ewp_header(&msg->stHead, HEADER_SIZE);

    // RESPONSE
    strcpy(msg->acStatusCode, RESP_OK);
    msg->acHardSpace[0] = ASCII_HARD_SPACE;

    // Format string according to the protocol definition
    strcpy(msg->acFormattedString, "Recipient OK");
    msg->acHardZero[0] = '\0';
}

// Initialize a DATA command response
void ewp_init_data_response(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY *msg, int is_valid_filename)
{
    // Set Header
    set_ewp_header(&msg->stHead, HEADER_SIZE);

    if (is_valid_filename)
    {
        // RESPONSE
        strcpy(msg->acStatusCode, RESP_START_MAIL_INPUT);
        msg->acHardSpace[0] = ASCII_HARD_SPACE;

        // Format string according to the protocol definition
        strcpy(msg->acFormattedString, "Start mail input; end with <CRLF>.<CRLF>");
    }
    else
    {
        // RESPONSE for error
        strcpy(msg->acStatusCode, RESP_SYNTAX_ERROR);
        msg->acHardSpace[0] = ASCII_HARD_SPACE;

        // Format string according to the protocol definition
        strcpy(msg->acFormattedString, "Invalid filename");
    }
    msg->acHardZero[0] = '\0';
}

// Initialize a QUIT response
void ewp_init_quit_response(struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY *msg, const char *server_id)
{
    // Set Header
    set_ewp_header(&msg->stHead, HEADER_SIZE);

    // RESPONSE
    strcpy(msg->acStatusCode, RESP_SERVICE_CLOSING);
    msg->acHardSpace[0] = ASCII_HARD_SPACE;

    // Format string according to the protocol definition
    snprintf(msg->acFormattedString, sizeof(msg->acFormattedString),
             "%s Service closing transmission channel", server_id);
    msg->acHardZero[0] = '\0';
}

// Parse received message to determine command type
int ewp_parse_message(const void *message, size_t message_size)
{
    // For simplicity, we'll return command type as an integer
    // 1: HELO, 2: MAIL FROM, 3: RCPT TO, 4: DATA, 5: QUIT, 0: Unknown

    // Skip the protocol header (8 bytes: 3 for magic, 4 for size, 1 for delimiter)
    const char *msg = (const char *)message + 8;

    if (strncmp(msg, CMD_HELO, strlen(CMD_HELO)) == 0)
        return 1;
    else if (strncmp(msg, CMD_MAIL_FROM, strlen(CMD_MAIL_FROM)) == 0)
        return 2;
    else if (strncmp(msg, CMD_RCPT_TO, strlen(CMD_RCPT_TO)) == 0)
        return 3;
    else if (strncmp(msg, CMD_DATA, strlen(CMD_DATA)) == 0)
        return 4;
    else if (strncmp(msg, CMD_QUIT, strlen(CMD_QUIT)) == 0)
        return 5;

    return 0; // Unknown command
}

// Send an EWP protocol message
int ewp_send_message(int sock_fd, const void *message, size_t message_size)
{
    // Simply wrap the send function
    return send(sock_fd, message, message_size, 0);
}