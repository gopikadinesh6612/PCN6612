#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

int main()
{
  int s = socket(AF_INET,SOCK_DGRAM,0);
  struct sockaddr_in server={0},client;
  socklen_t len = sizeof(client);
  char domain[100];
  server.sin_family=AF_INET;
  server.sin_port=htons(5003);
  server.sin_addr.s_addr=INADDR_ANY;
  bind(s,(struct sockaddr *)&server,sizeof(server));
  printf("DNS SERVER RUNNING :......\n");
  while(1)
  {
    int n = recvfrom(s,domain,sizeof(domain)-1,0,(struct sockaddr *)&client,&len);
    if(n<=0)
    {
      continue;
    }
    domain[n]='\0';
    char *ip=NULL;
    if(strcmp(domain,"www.example.com")==0)
    {
      ip="10.0.0.1";
    }
    else if(strcmp(domain,"www.test.com")==0)
    {
      ip="10.0.0.2";

    }
    else if(strcmp(domain,"www.demo.com")==0)
    {
      ip="10.0.0.3";
    }
    if(ip != NULL)
    {
      sendto(s,ip,strlen(ip),0,(struct sockaddr *)&client,len);

    }
    else{
      sendto(s,"NOT_FOUND",9,0,(struct sockaddr *)&client,len);

    }
    printf("QUERY : %s -> %s \n",domain,ip? ip : "NOT FOUND");
  }
  close(s);
  return 0;
}