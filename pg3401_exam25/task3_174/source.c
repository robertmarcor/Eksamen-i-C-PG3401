#include <stdlib.h>
#include "source.h"

int main()
{
    int menu_option;
    printf("Flight Management\n");
    while (1)
    {
        display_menu();

        switch (menu_option)
        {
        case 1:
            printf("Selected 1\n");
            break;
        case 2:
            printf("Selected 2\n");
            break;

        default:
            break;
        }
    }
}