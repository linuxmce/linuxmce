/* 
  mjproxy.c

  Build: gcc -o mjproxy md5.c mjproxy.c

*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include "md5.h"

//#define DEBUG

const char hexdigits_upcase[] = "0123456789ABCDEF";

char *bin2hex(char *p, const char *cp, int count)
{
	while (count) {
		unsigned char c = *cp++;
		/* put lowercase hex digits */
		*p++ = 0x20 | hexdigits_upcase[c >> 4];
		*p++ = 0x20 | hexdigits_upcase[c & 0xf];
		count--;
	}
	*p++ = 0;
	return p;
}

char *md5(char *res, char *value) {
	// Calculate MD5
#ifdef DEBUG
	printf("Calculating MD5 of '%s' (%d)\n", value, strlen(value));
#endif
	MD5_CTX context;
	unsigned char digest[MD5_DIGEST_CHARS];
	MD5Init(&context);
	MD5Update(&context, value, strlen(value));
	MD5Final(digest, &context);
	return bin2hex(res, digest, MD5_DIGEST_CHARS);
}

int main(int argc, char *argv[]) {

	struct sockaddr_in a;
	struct sockaddr_in sa;
	struct sockaddr_in da;
	int os = 0;

	if (argc < 6) {
		printf("Usage: %s listen-ip|0.0.0.0 listen-port proxy-address proxy-port password\n", argv[0]);
		printf("Usage: %s TEST nonce callid user password\n", argv[0]);
		exit(1);
	}

	int test = (strcmp(argv[1], "TEST") == 0);

	if (!test) {
		if (fork() != 0) exit(1);
		setsid();

		if ((os = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0) {
			printf("Can't open socket\n");
			exit(1);
		}

		a.sin_family = AF_INET;
		a.sin_addr.s_addr = inet_addr(argv[1]);
		a.sin_port = htons(atoi(argv[2]));
		if (bind(os, (struct sockaddr *)&a, sizeof(a)) == -1) {
			printf("Can't bind listen address (%s:%s)\n", argv[1], argv[2]);
			exit(1);
		}

		a.sin_addr.s_addr = inet_addr(argv[3]);
		if (a.sin_addr.s_addr == -1) {
			// try to resolve it as a hostname
			struct hostent *host = gethostbyname(argv[3]);
			if (host != NULL) {
				memset(&a, 0, sizeof(a));
				memcpy(&a.sin_addr, host->h_addr, sizeof(a.sin_addr));
			}
			else {
				printf("Invalid network address (%s:%s)\n", argv[3], argv[4]);
				exit(1);
			}
		}
		a.sin_port = htons(atoi(argv[4]));

		da.sin_addr.s_addr = 0;
	}

	while (1) {
		char buf[65535];
		char *cp;
		char *c;
		int i, n = 0;

		if (!test) {
			int sn = sizeof(sa);
			n = recvfrom(os, buf, sizeof(buf), 0, (struct sockaddr *)&sa, &sn);
			if (n <= 0)
				continue;
		}

		/* 
		 * Process and modify incoming packets here
		 * We need to look for and modify the following headers:
		 *
		 * CSeq: 2 REGISTER
		 * Call-ID: 02DBE29427B1429EAC3FAE4F36DAA8F60xc0a80104
		 * Authorization: Digest username="EXXXXXXXXXX01",realm="stratus.com",nonce="6cb3f218_09370",uri="sip:talk4free.com",response="ae7e04d8fcf44fdbf820783a9d1d89a6",algorithm=MD5
		 */

		cp = NULL;
		if (test || (cp = strstr(buf, "Authorization: Digest"))) {
			char *pw = argv[5];
			char *callid;

			if (test)
				callid = argv[3];
			else {
				callid = strstr(buf, "Call-ID: ");
				if (callid)
					callid += 9;
			}

			char method[34];
			memset(&method, 0x00, sizeof(method));
			if (test) {
				strcpy(method, "REGISTER");
			} else {
				if ((c = strstr(buf, "CSeq:"))) {
					c += 6;
					while (*c < 'A') c++;
					char *b = strstr(c, "\r\n");
					strncpy(method, c, b - c);
				}
			}

			char uname[15];
			memset(&uname, 0x00, sizeof(uname));
			if (test) {
				strcpy(uname, argv[4]);
			} else {
				if ((c = strstr(cp, "username="))) {
					strncpy(uname, c + 10, 13);
				}
			}

			char uri[64];
			memset(&uri, 0x00, sizeof(uri));
			if (test)
				strcpy(uri, "sip:talk4free.com");
			else {
				if ((c = strstr(cp, "uri="))) {
					char *b = strstr(c + 5, "\"");
					strncpy(uri, c + 5, b - (c + 5));
				}
			}

			char realm[64];
			memset(&realm, 0x00, sizeof(realm));
			if (test)
				strcpy(realm, "stratus.com");
			else {
				if ((c = strstr(cp, "realm="))) {
					char *b = strstr(c + 7, "\"");
					strncpy(realm, c + 7, b - (c + 7));
				}
			}

			char nonce[64];
			memset(&nonce, 0x00, sizeof(nonce));
			if (test)
				strcpy(nonce, argv[2]);
			else {
				if ((c = strstr(cp, "nonce="))) {
					char *b = strstr(c + 7, "\"");
					strncpy(nonce, c + 7, b - (c + 7));
				}
			}

#ifdef DEBUG
			printf("%s: user='%s',nonce='%s',realm='%s',uri='%s',pw='%s'\n",
				method, uname, nonce, realm, uri, pw);
#endif

			/*
			 * Call-ID is used as a lookup table to append the nonce value
			 * 75E16D8104254DB68CFE... callid
			 * 0123456789abcdef....... index
			 * First an underscore is appended to the nonce
			 * Now take the first hex character of the nonce which is 5 so get the callid character at index 5
			 * This is a D since the index is zero based
			 * Append a D to the nonce and so on
			 * The final nonce = 5437837f0_06998_D6110116 in this example
			 * The next block of code does the trick
			 */
			if (callid && nonce) {
				char response[40];
				memset(&response, 0x00, sizeof(response));

				// Calculate new nonce
				strcat(nonce, "_");
				c = nonce + strlen(nonce);
				char hex[2];
				hex[1] = 0;
				for (i = 0; i < 8; i++) {
					hex[0] = nonce[i];
					int x = strtol(hex, NULL, 16);
					*c++ = callid[x];
				}
				*c++ = 0;

#ifdef DEBUG			
				printf("new nonce: %s\n", nonce);
#endif

				// Calculate new MD5 response
				char a1[128], a2[128], a3[128];

				sprintf(a1, "%s:%s:%s", uname, realm, pw);
				md5(a1, a1);

				sprintf(a2, "%s:%s", method, uri);
				md5(a2, a2);

				sprintf(a3, "%s:%s:%s", a1, nonce, a2);
				md5(response, a3);

#ifdef DEBUG
				printf("new responce: %s\n", response);
#endif

				// copy the new response field back to the SIP header
				if (!test && (c = strstr(cp, "response="))) {
					c += 10;
					strncpy(c, response, 32);
				}
			}
		}

		if (test) {
			exit(0);
		}

		if (sa.sin_addr.s_addr == a.sin_addr.s_addr &&
			sa.sin_port == a.sin_port) {
			if(da.sin_addr.s_addr) {
				sendto(os, buf, n, 0, (struct sockaddr *)&da, sizeof(da));
			}
		} else {
			sendto(os, buf, n, 0, (struct sockaddr *)&a, sizeof(a));
			da = sa;
		}
	}
}
