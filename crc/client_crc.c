
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main()
{
    int s = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server = {0};
    char data[100], gen[30], temp[256], codeword[256];
    char packet[512];

    server.sin_family = AF_INET;
    server.sin_port = htons(5003);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    printf("Enter data bits: ");
    scanf("%99s", data);

    printf("Enter generator bits: ");
    scanf("%29s", gen);

    int d = strlen(data), g = strlen(gen);

    if (g < 2 || d + g >= sizeof(temp))
    {
        printf("Invalid input length.\n");
        close(s);
        return 1;
    }

    // Append zeros to data
    strcpy(temp, data);

    for (int i = 0; i < g - 1; i++)
        temp[d + i] = '0';

    temp[d + g - 1] = '\0';

    // CRC division using XOR
    for (int i = 0; i < d; i++)
    {
        if (temp[i] == '1')
        {
            for (int j = 0; j < g; j++)
                temp[i + j] =
                    (temp[i + j] == gen[j]) ? '0' : '1';
        }
    }

    // Append CRC remainder to original data
    strcpy(codeword, data);
    strncat(codeword, temp + d, g - 1);

    printf("CRC codeword: %s\n", codeword);

    int error;
    printf("Introduce error? (1 = Yes, 0 = No): ");
    scanf("%d", &error);

    if (error == 1)
    {
        codeword[0] = (codeword[0] == '0') ? '1' : '0';
        printf("Codeword after error: %s\n", codeword);
    }

    // Send generator and codeword to server
    snprintf(packet, sizeof(packet), "%s|%s", gen, codeword);

    sendto(s, packet, strlen(packet), 0,
           (struct sockaddr *)&server, sizeof(server));

    printf("Codeword sent to server.\n");

    close(s);
    return 0;
}
