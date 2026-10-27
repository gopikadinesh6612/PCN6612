
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int check_crc(char data[], char gen[])
{
    char temp[512];
    int n = strlen(data), g = strlen(gen);

    strcpy(temp, data);

    for (int i = 0; i <= n - g; i++)
    {
        if (temp[i] == '1')
        {
            for (int j = 0; j < g; j++)
                temp[i + j] =
                    (temp[i + j] == gen[j]) ? '0' : '1';
        }
    }

    for (int i = n - g + 1; i < n; i++)
        if (temp[i] == '1')
            return 0;

    return 1;
}

int main()
{
    int s = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server = {0}, client;
    socklen_t len = sizeof(client);
    char buffer[512];

    server.sin_family = AF_INET;
    server.sin_port = htons(5003);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind");
        close(s);
        return 1;
    }

    printf("CRC Server waiting...\n");

    int n = recvfrom(s, buffer, sizeof(buffer) - 1, 0,
                     (struct sockaddr *)&client, &len);

    if (n > 0)
    {
        buffer[n] = '\0';

        char *sep = strchr(buffer, '|');

        if (sep != NULL)
        {
            *sep = '\0';
            char *gen = buffer;
            char *codeword = sep + 1;

            printf("Received codeword: %s\n", codeword);

            if (check_crc(codeword, gen))
                printf("Result: No error detected.\n");
            else
                printf("Result: Error detected!\n");
        }
        else
        {
            printf("Invalid data received.\n");
        }
    }

    close(s);
    return 0;
}
