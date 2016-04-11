#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

#define GROUP_ADDRESS "239.10.5.10"

int main(int argc, char *argv[]) {
   int targetPort;
   int socketfd;
   int ttl;
   int cnt;
   int group;
   char messageBuffer[64];
   struct sockaddr_in multicast_address;

   ttl = 10;
   //Read command line
   if(argc != 2) {
      fprintf(stderr, "Usage: %s <group-number>\n", argv[0]);
      return -1;
   } else {
      group = atoi(argv[1]);
      targetPort = 22200 + group;
   }
   
   //create socket
   socketfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   if(socketfd < 0) {
      perror("Could not open socket");
      return -1;
   }

   if(setsockopt(socketfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl))) {
      perror("Could not set TTL for multicast");
   }

   multicast_address.sin_family = AF_INET;
   multicast_address.sin_addr.s_addr = inet_addr(GROUP_ADDRESS);
   multicast_address.sin_port = htons(targetPort);

   cnt = 1;
   for(;;) {
      sprintf(messageBuffer, "This is message %u from the group %u beacon\n", cnt++, group);
      //display "This is message <N> from the group <X> beacon\n"
      if(sendto(socketfd, messageBuffer, sizeof(messageBuffer), 0, (struct sockaddr *)&multicast_address, sizeof(multicast_address)) < 0) {
         perror("Could not send multicast message");
         close(socketfd);
         return -1;
      }
      //sleep for 1 second
      sleep(1);
   }   
   close(socketfd);
   return 0;
}
