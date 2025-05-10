#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define PRINT_INFO(format, ...) printf(ANSI_COLOR_CYAN "[INFO] " ANSI_COLOR_RESET format "\n", ##__VA_ARGS__)
#define PRINT_SUCCESS(format, ...) printf(ANSI_COLOR_GREEN "[SUCCESS] " ANSI_COLOR_RESET format "\n", ##__VA_ARGS__)
#define PRINT_ERROR(format, ...) printf(ANSI_COLOR_RED "[ERROR] " ANSI_COLOR_RESET format "\n", ##__VA_ARGS__)
#define PRINT_RECV(format, ...) printf(ANSI_COLOR_YELLOW "[RECEIVED] " ANSI_COLOR_RESET format "\n", ##__VA_ARGS__)
#define PRINT_SEND(format, ...) printf(ANSI_COLOR_BLUE "[SENT] " ANSI_COLOR_RESET format "\n", ##__VA_ARGS__)
#define PRINT_HEADER(text) printf("\n" ANSI_COLOR_MAGENTA "========== %s ==========" ANSI_COLOR_RESET "\n", text)
#define PRINT_PROTOCOL(format, ...) printf(ANSI_COLOR_YELLOW "[PROTOCOL] " ANSI_COLOR_RESET format "\n", ##__VA_ARGS__)
#define PRINT_SEPARATOR() printf(ANSI_COLOR_MAGENTA "====================" ANSI_COLOR_RESET "\n")

#endif /* LOGGING_H */