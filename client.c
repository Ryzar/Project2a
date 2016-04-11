#include <time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MULTICAST_ADDR "239.10.5.10"

int main(int argc, char *argv[]) {
   int socketfd, n;
   struct sockaddr_in beacon_address, client_address;
   struct ip_mreq group;
   int nbytes, flags, reuse;
   socklen_t addrlen;
   ssize_t size;
   char messagebuf[1024];
   int listeningPortNum, groupNum;
   struct timespec time;

   if(argc != 2) {
      fprintf(stderr, "Usage: %s <group-number>\n", argv[0]);
      return -1;
   }
   groupNum = atoi(argv[1]);
   listeningPortNum = 22200 + groupNum;

   socketfd = socket(AF_INET, SOCK_DGRAM, 0);
   if(socketfd < 0) {
      perror("Could not open socket");
      return -1;
   }

   reuse = 1;

   if(setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0) {
      perror("Could not set socket address reuse");
      return -1;
   }

   client_address.sin_family = AF_INET;
   client_address.sin_port = htons(listeningPortNum);
   client_address.sin_addr.s_addr = htonl(INADDR_ANY);
   if(bind(socketfd, (struct sockaddr *)&client_address, sizeof(client_address))) {
      perror("Could not bind socket");
      return -1;
   }

   addrlen = sizeof(client_address);

   group.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
   group.imr_interface.s_addr = htonl(INADDR_ANY);
   if(setsockopt(socketfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&group, (socklen_t) sizeof(struct ip_mreq))) {
      perror("Group joined failed with error");
      return -1;
   }
   
   for(;;) {//infinite loop for reading broadcast messages
      if(recvfrom(socketfd, messagebuf, sizeof(messagebuf), 0, (struct sockaddr *)&client_address, &addrlen) < 0) {
         perror("Reading datagram message error");
         close(socketfd);
         return -1;
      }
      //get current time
      n = clock_gettime(CLOCK_REALTIME, &time);
      //print current time and beacon message
      printf("%lu.%010lu: %s\n", (long)time.tv_sec, (long)time.tv_nsec, messagebuf);
   }
   return 0;
}
