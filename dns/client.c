#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main()
{
  int s = socket(AF_INET,SOCK_DGRAM,0);
  struct sockaddr_in server={0};
  char domain[100],ip[100];
  server.sin_family=AF_INET;
  server.sin_port=htons(5003);
  inet_pton(AF_INET,"127.0.0.1",&server.sin_addr);
  char cache_domain[10][100];
  char cache_ip[10][100];
  int count =0;
  while(1)
  {
    printf("ENTER THE DOMAIN OR EXIT : ");
    
    if(fgets(domain,sizeof(domain),stdin)==NULL)
    {break;}
    domain[strcspn(domain,"\n")]='\0';
    if(strcmp(domain,"exit")==0)
    {
      break;
    }
    int hit =0;
    for(int i=0;i<count;i++)
    {
      if(strcmp(domain,cache_domain[i])==0)
      {
        strcpy(ip,cache_ip[i]);
        printf("CACHE HIT : %s -> %s\n ",domain,ip);
        hit = 1;
        break;

      }
    }
    if(!hit)
    {
      printf("CACHE MISS : ");
      sendto(s,domain,sizeof(domain),0,(struct sockaddr *)&server,sizeof(server));
      int n = recvfrom(s,ip,sizeof(ip)-1,0,NULL,NULL);
      if(n<=0)
      {
        printf("NO RESPONSE FROM SEVRER ");
      }
      ip[n]='\0';
      if(strcmp(ip,"NOT_FOUND")==0)
      {
        printf("DOMAIN NOT FOUND");
        continue;
      }
      printf("SERVER RESPONSE : %s -> %s",domain,ip);
      if(count < 10)
      {
        strcpy(cache_domain[count],domain);
        strcpy(cache_ip[count],ip);
        count++;
      }
    }

  }
  close(s);
  return 0;
}