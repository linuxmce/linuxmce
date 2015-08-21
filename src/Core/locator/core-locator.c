#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

char *detectCoreIpAddress(void) {
//        printf("Searching for dcerouter's ip address...");
        struct sockaddr_in si_other;
        int s, slen=sizeof(si_other);
        int on=1;
        char buf[3]="xxx";
        char *ret = "";

        s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        fcntl(s, F_SETFL, O_NONBLOCK);
        setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
        memset((char *) &si_other, sizeof(si_other), 0);
        si_other.sin_family = AF_INET;
        si_other.sin_port = htons(33333);

        inet_aton("255.255.255.255", &si_other.sin_addr);
        sendto(s, buf, 3, 0, (struct sockaddr *) &si_other, slen);

        time_t start_time = time(0);
        while ((time(0) - start_time) < 1) {
                recvfrom(s, buf, 3, 0, (struct sockaddr *) &si_other, &slen);
        }
        close(s);

        ret = inet_ntoa(si_other.sin_addr);
	if ( strncmp(ret, "255.255.255.255",16) ) {
//                printf("   %s\n",ret);
                return ret;
        } else {
//                printf("   not found\n");
                return NULL;
        }
}

int main()
{
	char *sIP = detectCoreIpAddress();

	if ( sIP == NULL )
		return 1;

	printf("%s\n", sIP);
	return 0;
}
