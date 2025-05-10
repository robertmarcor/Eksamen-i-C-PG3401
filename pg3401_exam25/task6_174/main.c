#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <ctype.h>
#include "main.h"

#define BUFFER_SIZE 1024
#define KEY_LIMIT 256

int is_likely_valid_text(const unsigned char *data, size_t size)
{
    int count = 0;
    size_t check_size = size < 32 ? size : 32;

    for (size_t i = 0; i < check_size; i++)
    {
        if (isprint(data[i]) || isspace(data[i]))
            count++;
    }

    return (count >= check_size * 0.75);
}

size_t remove_pkcs5_padding(unsigned char *data, size_t data_len)
{
    if (data_len == 0)
        return 0;

    unsigned char padding_value = data[data_len - 1];

    if (padding_value == 0 || padding_value > 8)
        return data_len;

    for (int i = 1; i <= padding_value; i++)
    {
        if (data_len - i < 0 || data[data_len - i] != padding_value)
        {
            return data_len;
        }
    }

    return data_len - padding_value;
}

/*
 * Program that creates a client, connects to EWA server
 * recieved a file encrypted with TEA and padded with PKSCH#5
 * decrypts the file, by using David Wheeler and Roger Needham TEA decipher.
 *
 * Encryption KEY bruteforced and used to decrypt message.
 * Decrypted message is saved as plain text.
 */
int main(int argc, char *argv[])
{
    char *server_ip = NULL;
    int port = 0;
    int socket_fd;
    struct sockaddr_in server_addr;
    unsigned char buffer[BUFFER_SIZE];
    unsigned char *file_data = NULL;
    size_t total_bytes = 0;

    // Argument parsing
    for (int i = 1; i < argc; i += 2)
    {
        if (i + 1 >= argc)
        {
            fprintf(stderr, "Missing argument for %s\n", argv[i]);
            return 1;
        }

        if (strcmp(argv[i], "-server") == 0)
        {
            server_ip = argv[i + 1];
        }
        else if (strcmp(argv[i], "-port") == 0)
        {
            port = atoi(argv[i + 1]);
        }
        else
        {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            return 1;
        }
    }

    if (!server_ip || port <= 0)
    {
        fprintf(stderr, "Usage: %s -server 127.0.0.1 -port <port>\n", argv[0]);
        return 1;
    }

    /* SOCKET CONNECTION SETUP */
    printf("=== ESTABLISHING CONNECTION ===\n");
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address");
        close(socket_fd);
        return 1;
    }

    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(socket_fd);
        return 1;
    }

    printf("Connected to server %s:%d\n", server_ip, port);

    printf("\n=== RECEIVING ENCRYPTED FILE ===\n");
    ssize_t bytes_read;
    while ((bytes_read = recv(socket_fd, buffer, BUFFER_SIZE, 0)) > 0)
    {
        unsigned char *new_data = realloc(file_data, total_bytes + bytes_read);
        if (!new_data)
        {
            perror("Memory allocation failed");
            free(file_data);
            close(socket_fd);
            return 1;
        }

        file_data = new_data;
        memcpy(file_data + total_bytes, buffer, bytes_read);
        total_bytes += bytes_read;
    }

    close(socket_fd);

    if (total_bytes == 0)
    {
        fprintf(stderr, "No data received from server\n");
        free(file_data);
        return 1;
    }

    printf("Received %zu bytes of encrypted data\n", total_bytes);

    /* HTTP HEADER HANDLING */
    char *body_start = NULL;
    size_t body_size = total_bytes;

    if (total_bytes > 4 && strncmp((char *)file_data, "HTTP", 4) == 0)
    {
        printf("\n=== HTTP RESPONSE ===\n");
        body_start = strstr((char *)file_data, "\r\n\r\n");
        if (body_start)
        {
            // Print HTTP headers
            printf("HTTP Headers:\n");
            size_t header_size = body_start - (char *)file_data;
            char *headers = malloc(header_size + 1);
            if (headers)
            {
                memcpy(headers, file_data, header_size);
                headers[header_size] = '\0';

                // Split and print each header line
                char *line = strtok(headers, "\r\n");
                while (line)
                {
                    printf("%s\n", line);
                    line = strtok(NULL, "\r\n");
                }
                free(headers);
            }

            body_start += 4; // Skip past the \r\n\r\n
            body_size = total_bytes - (body_start - (char *)file_data);
            printf("\nHTTP body size: %zu bytes\n", body_size);

            file_data = (unsigned char *)body_start;
            total_bytes = body_size;
        }
    }

    /* guards before TEA decryption */
    size_t aligned_size = (total_bytes / 8) * 8;
    if (total_bytes % 8 != 0)
    {
        printf("Warning: File size %zu is not a multiple of 8. Using first %zu bytes for decryption.\n",
               total_bytes, aligned_size);
    }

    if (aligned_size == 0)
    {
        fprintf(stderr, "Not enough data to decrypt\n");
        if (body_start == NULL)
            free(file_data);
        return 1;
    }

    unsigned char *decrypted = malloc(aligned_size);
    if (!decrypted)
    {
        perror("Memory allocation failed");
        if (body_start == NULL)
            free(file_data);
        return 1;
    }

    /* BRUTEFORCE TEA KEY */
    printf("\n=== BRUTEFORCING TEA KEY ===\n");
    printf("Trying all 256 possible single-byte keys...\n");
    printf("Trying keys:");

    int found_key = 0;

    for (int key_byte = 0; key_byte < KEY_LIMIT; key_byte++)
    {
        // Create a TEA key where all bytes have the same value
        unsigned int key[4];
        for (int i = 0; i < 4; i++)
        {
            key[i] = (key_byte << 24) | (key_byte << 16) | (key_byte << 8) | key_byte;
        }

        printf("0x%02X,", key_byte);

        // Decrypt file in 8-byte blocks
        for (size_t i = 0; i < aligned_size; i += 8)
        {
            unsigned int v[2], w[2];
            unsigned int *temp_v = (unsigned int *)(file_data + i);
            unsigned int *temp_w = (unsigned int *)(decrypted + i);

            // Copy with proper alignment
            v[0] = temp_v[0];
            v[1] = temp_v[1];

            // Decrypt this block
            decipher(v, w, key);

            // Store back the decrypted block
            temp_w[0] = w[0];
            temp_w[1] = w[1];
        }

        // Validate decryption result
        if (is_likely_valid_text(decrypted, aligned_size))
        {
            printf("\n=== SUCCESSFUL DECRYPTION ===\n");
            printf("Found key: 0x%02X\n", key_byte);

            // Remove PKCS#5 padding
            size_t actual_size = remove_pkcs5_padding(decrypted, aligned_size);

            // Display the decrypted content
            if (actual_size < aligned_size)
            {
                decrypted[actual_size] = '\0';
                printf("\nFull decrypted content:\n%s\n", decrypted);

                // Save to file
                FILE *fp = fopen("decrypted.txt", "wb");
                if (fp)
                {
                    fwrite(decrypted, 1, actual_size, fp);
                    fclose(fp);
                    printf("\nDecrypted content saved to decrypted.txt\n");
                }

                found_key = 1;
                break;
            }
            else
            {
                printf("Could not remove padding, showing raw decrypted data:\n");
                decrypted[aligned_size] = '\0';
                printf("%s\n", decrypted);
            }
        }
    }

    if (!found_key)
    {
        printf("\n=== DECRYPTION FAILED ===\n");
        printf("Could not find a valid decryption key.\n");
    }

    /* CLEANUP */
    if (body_start == NULL)
        free(file_data);
    free(decrypted);
    return 0;
}
