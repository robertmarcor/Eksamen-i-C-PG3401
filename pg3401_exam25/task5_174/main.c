#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "ewpdef.h"
#include "communication.h"

#define DEFAULT_PORT 25
#define DEFAULT_ID "SmtpTest"

int take_args(int argc, char *argv[], int *port, char **id)
{
    if (argc != 5)
    {
        printf("Program takes 5 arguments\nUsage:\n");
        printf("%s -port <port> -id <id>\n", argv[0]);
        printf("Example: %s -port %d -id %s\n", argv[0], DEFAULT_PORT, DEFAULT_ID);
        return 1;
    }

    *port = atoi(argv[2]);
    *id = argv[4];
    return 0;
}

int create_server(int port)
{
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0)
    {
        perror("ERROR opening socket");
        return -1;
    }
    struct sockaddr_in saAddr = {0};
    saAddr.sin_family = AF_INET;
    saAddr.sin_port = htons(port);
    saAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockFd, (struct sockaddr *)&saAddr, sizeof(saAddr)) < 0)
    {
        perror("ERROR on binding");
        close(sockFd);
        return -1;
    }

    // Listen for connections
    if (listen(sockFd, 5) < 0)
    {
        perror("ERROR on listen");
        close(sockFd);
        return -1;
    }
    printf("Socket created successfully,\nbound to port %d and listening for connections...\n", port);
    return sockFd;
}

int accept_connection(int sockFd)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int new_sockfd = accept(sockFd, (struct sockaddr *)&client_addr, &client_len);
    if (new_sockfd < 0)
    {
        perror("ERROR on accept");
        return -1;
    }
    printf("Connection accepted\n");
    return new_sockfd;
}

int main(int argc, char *argv[])
{
    int port;
    char *id;

    // Guard for invalid arguments
    if (take_args(argc, argv, &port, &id) != 0)
    {
        return 1;
    }
    // This server implements a custom SMTP-like binary protocol EWP that mimics the SMTP protocol:
    // - Handles client connections and communication
    // - Validates commands (HELO, MAIL FROM, RCPT TO, DATA, QUIT)
    // - Sends appropriate response codes (220, 250, 354, 221)
    // - Saves email content to files based on client requests

    printf("Server started with id:'%s':%d\n", id, port);

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
    // Send server accept message to client
    struct EWA_EXAM25_TASK5_PROTOCOL_SERVERACCEPT server_msg;
    ewp_init_server_accept(&server_msg, id);
    ewp_send_message(clientFd, &server_msg, sizeof(server_msg));
    printf("Sent server accept message to client\n");

    while (1)
    {
        // Read client request
        char buffer[1024];
        ssize_t bytesRead = read(clientFd, buffer, sizeof(buffer));
        if (bytesRead <= 0)
            break; // Connection closed or error

        printf("Received message from client: %s\n", buffer);

        // Parse the command type
        int cmd_type = ewp_parse_message(buffer, bytesRead);

        switch (cmd_type)
        {
        case 1: // HELO
        {
            struct EWA_EXAM25_TASK5_PROTOCOL_SERVERHELO response;
            ewp_init_helo_response(&response, "client"); // Replace with actual client info
            ewp_send_message(clientFd, &response, sizeof(response));
            printf("Sent HELO response\n");
            break;
        }

        case 2: // MAIL FROM
        {
            struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY response;
            ewp_init_mail_from_response(&response);
            ewp_send_message(clientFd, &response, sizeof(response));
            printf("Sent MAIL FROM response\n");
            break;
        }

        case 3: // RCPT TO
        {
            struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY response;
            ewp_init_rcpt_to_response(&response);
            ewp_send_message(clientFd, &response, sizeof(response));
            printf("Sent RCPT TO response\n");
            break;
        }

        case 4: // DATA
        {
            // TODO: Extract filename and check if valid
            struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY response;
            ewp_init_data_response(&response, 1); // Assuming valid filename for now
            ewp_send_message(clientFd, &response, sizeof(response));
            printf("Sent DATA response\n");
            break;
        }

        case 5: // QUIT
        {
            struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY response;
            ewp_init_quit_response(&response, id);
            ewp_send_message(clientFd, &response, sizeof(response));
            printf("Sent QUIT response\n");
            goto cleanup; // Exit the loop to close connection
        }
        default:
            printf("Unknown command received, exiting\n");
            goto cleanup;
        }
    }

// Cleanup connection
cleanup:
    close(clientFd);
    close(sockFd);
    return 0;
}
