#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "ewpdef.h"
#include "communication.h"
#include "logging.h"

#define DEFAULT_PORT 25
#define DEFAULT_ID "SmtpTest"

int take_args(int argc, char *argv[], int *port, char **id)
{
    if (argc != 5)
    {
        PRINT_ERROR("Program takes 5 arguments\nUsage:");
        PRINT_INFO("%s -port <port> -id <id>", argv[0]);
        PRINT_INFO("Example: %s -port %d -id %s", argv[0], DEFAULT_PORT, DEFAULT_ID);
        return 1;
    }

    *port = atoi(argv[2]);
    *id = argv[4];
    return 0;
}

int create_server(int port)
{
    PRINT_HEADER("CREATING SERVER");
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0)
    {
        PRINT_ERROR("Failed to open socket");
        return -1;
    }
    PRINT_INFO("Socket created successfully");

    struct sockaddr_in saAddr = {0};
    saAddr.sin_family = AF_INET;
    saAddr.sin_port = htons(port);
    saAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockFd, (struct sockaddr *)&saAddr, sizeof(saAddr)) < 0)
    {
        PRINT_ERROR("Failed to bind to port %d", port);
        close(sockFd);
        return -1;
    }
    PRINT_INFO("Socket bound to port %d", port);

    // Listen for connections
    if (listen(sockFd, 5) < 0)
    {
        PRINT_ERROR("Failed to listen on socket");
        close(sockFd);
        return -1;
    }
    PRINT_SUCCESS("Server initialized and listening for connections on port %d", port);
    return sockFd;
}

int accept_connection(int sockFd)
{
    PRINT_HEADER("WAITING FOR CLIENT");
    PRINT_INFO("Waiting for client connection...");

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int new_sockfd = accept(sockFd, (struct sockaddr *)&client_addr, &client_len);
    if (new_sockfd < 0)
    {
        PRINT_ERROR("Failed to accept connection");
        return -1;
    }

    // Convert IP to string
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

    PRINT_SUCCESS("Connection accepted from %s:%d", client_ip, ntohs(client_addr.sin_port));
    return new_sockfd;
}

// This server implements a protocol EWP that mimics the SMTP protocol:
// - Handles client connections and communication
// - Validates commands (HELO, MAIL FROM, RCPT TO, DATA, QUIT)
// - and executes a switch case based on cmd
// - Sends appropriate response codes (220, 250, 354, 221)
// - Saves email content to files
// - PRINT Macros just for displaying info in the terminal

int main(int argc, char *argv[])
{
    int port;
    char *id;

    // Guard for invalid arguments
    if (take_args(argc, argv, &port, &id) != 0)
    {
        return 1;
    }
    PRINT_HEADER("EWP");
    PRINT_INFO("Starting server initialization...");
    PRINT_INFO("Server ID: '%s', Port: %d", id, port);

    // Create socket and start listening
    int sockFd = create_server(port);
    if (sockFd < 0)
    {
        return 1;
    }

    // Accept client connection
    int clientFd = accept_connection(sockFd);
    if (clientFd < 0)
    {
        close(sockFd);
        return 1;
    }

    // Server accept resposne to client
    PRINT_HEADER("SESSION STARTED");
    struct EWA_EXAM25_TASK5_PROTOCOL_SERVERACCEPT server_msg;
    ewp_init_server_accept(&server_msg, id);
    ewp_send_message(clientFd, &server_msg, sizeof(server_msg));
    PRINT_SEND("220 Server ready: %s", server_msg.acFormattedString);

    PRINT_HEADER("COMMUNICATION");
    while (1)
    {
        // Read client request
        char buffer[1024];
        PRINT_INFO("Waiting for client command...");
        ssize_t bytesRead = read(clientFd, buffer, sizeof(buffer));
        if (bytesRead <= 0)
        {
            PRINT_ERROR("Connection closed or error reading data");
            break; // Error reading data
        }

        // Parse the client command
        int cmd_type = ewp_parse_message(buffer, bytesRead);
        const char *cmd_name = "UNKNOWN";
        char formatted_message[100] = {0};

        // PURELY COSMIETIC FOR DISPLAYING IN TERMINAL
        const char *cmd_start = buffer + 8;
        strncpy(formatted_message, cmd_start, sizeof(formatted_message) - 1);

        switch (cmd_type)
        {
        case 1:
            cmd_name = "HELO";
            break;
        case 2:
            cmd_name = "MAIL FROM";
            break;
        case 3:
            cmd_name = "RCPT TO";
            break;
        case 4:
            cmd_name = "DATA";
            break;
        case 5:
            cmd_name = "QUIT";
            break;
        default:
            cmd_name = "UNKNOWN";
            break;
        }

        PRINT_RECV("%s: %s", cmd_name, formatted_message);
        // END OF DISPLAY BLOCK

        switch (cmd_type)
        {
        case 1: // HELO
        {
            // Extract client ID from the HELO message
            // The client ID is in the acFormattedString field (after HELO + space)
            // Skip the protocol header (8 bytes) + HELO (4 bytes) + space (1 byte)
            const char *client_id = buffer + 8 + 4 + 1;

            struct EWA_EXAM25_TASK5_PROTOCOL_SERVERHELO response;
            ewp_init_helo_response(&response, client_id);
            ewp_send_message(clientFd, &response, sizeof(response));
            PRINT_SEND("250 %s", response.acFormattedString);
            break;
        }

        case 2: // MAIL FROM
        {
            struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY response;
            ewp_init_mail_from_response(&response);
            ewp_send_message(clientFd, &response, sizeof(response));
            PRINT_SEND("250 %s", response.acFormattedString);
            break;
        }

        case 3: // RCPT TO
        {
            struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY response;
            ewp_init_rcpt_to_response(&response);
            ewp_send_message(clientFd, &response, sizeof(response));
            PRINT_SEND("250 %s", response.acFormattedString);
            break;
        }

        case 4: // DATA
        {
            // Extract filename from DATA command
            // Skip the protocol header (8 bytes) + DATA (4 bytes) + space (1 byte)
            const char *filename = buffer + 8 + 4 + 1;

            // Validate filename
            int is_valid_filename = 1; // Assume valid by default

            // Check if filename is empty
            if (strlen(filename) == 0)
            {
                is_valid_filename = 0;
                PRINT_ERROR("Empty filename");
            }

            // Basic validation - check for invalid characters
            for (size_t i = 0; i < strlen(filename); i++)
            {
                if (filename[i] == '/' || filename[i] == '\\' || filename[i] == ':' ||
                    filename[i] == '*' || filename[i] == '?' || filename[i] == '"' ||
                    filename[i] == '<' || filename[i] == '>' || filename[i] == '|')
                {
                    is_valid_filename = 0;
                    PRINT_ERROR("Invalid character in filename");
                    break;
                }
            }

            // Check if filename ends with .eml
            const char *extension = strrchr(filename, '.');
            if (extension == NULL || strcmp(extension, ".eml") != 0)
            {
                is_valid_filename = 0;
                PRINT_ERROR("Filename must end with .eml extension");
            }

            // Send response based on filename validation
            struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY response;
            ewp_init_data_response(&response, is_valid_filename, filename);
            ewp_send_message(clientFd, &response, sizeof(response));

            if (is_valid_filename)
            {
                PRINT_SEND("354 %s", response.acFormattedString);
                PRINT_INFO("Ready to receive email data for file: %s", filename);

                // Create the file
                FILE *email_file = fopen(filename, "wb");
                if (email_file == NULL)
                {
                    PRINT_ERROR("Failed to create email file: %s", filename);
                    break;
                }

                // Read email data from client until end marker (CRLF.CRLF)
                char email_buffer[1024];
                int end_of_data = 0;
                size_t total_bytes = 0;

                PRINT_INFO("Receiving email data...");
                while (!end_of_data)
                {
                    ssize_t bytes_read = read(clientFd, email_buffer, sizeof(email_buffer));
                    if (bytes_read <= 0)
                    {
                        PRINT_ERROR("Connection closed while receiving email data");
                        break; // Connection closed or error
                    }

                    // Parse the message to check if it contains the end of file marker
                    const char *end_marker = "\r\n.\r\n";
                    char *end_pos = strstr(email_buffer, end_marker);

                    if (end_pos != NULL)
                    {
                        // End marker found, write data up to the marker
                        size_t data_size = end_pos - email_buffer;
                        fwrite(email_buffer, 1, data_size, email_file);
                        total_bytes += data_size;
                        end_of_data = 1;
                        PRINT_INFO("End of email data received");
                    }
                    else
                    {
                        // Write the entire buffer
                        fwrite(email_buffer, 1, bytes_read, email_file);
                        total_bytes += bytes_read;
                    }
                }

                // Close the file
                fclose(email_file);

                // Send OK response after receiving the data
                struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY ok_response;
                ewp_init_mail_from_response(&ok_response); // Reuse the mail from response (250 OK)
                strcpy(ok_response.acFormattedString, "Message accepted for delivery");
                ewp_send_message(clientFd, &ok_response, sizeof(ok_response));

                PRINT_SUCCESS("Email data saved to file: %s (%zu bytes)", filename, total_bytes);
                PRINT_SEND("250 %s", ok_response.acFormattedString);
            }
            else
            {
                PRINT_SEND("501 %s", response.acFormattedString);
            }
            break;
        }

        case 5: // QUIT
        {
            struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY response;
            ewp_init_quit_response(&response, id);
            ewp_send_message(clientFd, &response, sizeof(response));
            PRINT_SEND("221 %s", response.acFormattedString);
            PRINT_INFO("Client requested to quit, closing connection");
            goto cleanup; // Exit the loop to close connection
        }
        default:
            PRINT_ERROR("Unknown command received, closing connection");
            goto cleanup;
        }
    }

// Cleanup connection
cleanup:
    PRINT_HEADER("SHUTTING DOWN");
    PRINT_INFO("Closing client connection");
    close(clientFd);
    PRINT_INFO("Closing server socket");
    close(sockFd);
    PRINT_SUCCESS("Server shutdown complete");
    return 0;
}
