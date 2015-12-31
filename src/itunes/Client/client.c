
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>

#include "../ServerLib/ServerConsts.h"

extern int errno;

struct sockaddr_in dest_addr;
int sockfd;

int receive(	char *const buffer,
				const unsigned long dataSize) {

	struct timeval time;
	time.tv_sec = 20;
	time.tv_usec = 0;
    
	fd_set rFd, eFd;
	FD_ZERO(&rFd);
	FD_ZERO(&eFd);

	unsigned long currentSize;
	for (currentSize = 0; currentSize < dataSize; ) {

		FD_SET(sockfd, &rFd);
		FD_SET(sockfd, &eFd);

		int rc = select(sockfd + 1, &rFd, NULL, &eFd, &time);
		if (	rc == -1
			||	FD_ISSET(sockfd, &eFd)
			||	!FD_ISSET(sockfd, &rFd)) {
			printf("Unknown network error - %d (select error).\n", errno);
			return 0;
		} else if (rc == 0) {
			printf("Network timeout.\n");
			return 0;
		}

		rc = recv(sockfd, buffer + currentSize, dataSize - currentSize, 0);
		if (rc == -1) {
			printf("Unknown network error - %d (socket error).\n", errno);
			return 0;
		} else if (rc == 0) {
			printf("Unknown network error (protocol error).\n", errno);
			return 0;
		}

		currentSize += rc;

	}

	return 1;

}


int sendBuff(const char *const buffer, const unsigned long size) {
	const int sent_size = send(sockfd, buffer, size, 0);
	if (sent_size != size) {
		printf("Unknown network error.\n");
		return 0;
	}
	return 1;
}


int sendCmd(int cmd) {
	return sendBuff((char*)&cmd, sizeof(cmd));
}


int getAllFiles(const char* const save_path) {
	if (!sendCmd(CMD_GET_FILES_ALL)) {
		return 0;
	}
	struct TrackFileInfoPacket file_info;
	while (receive((char*)&file_info, sizeof(file_info))) {
		if (!file_info.size) {
			return 1;
		} else if (!receiveFile(&file_info, save_path)) {
			return 0;
		}
	}
	return 0;
}


int getFiles(	const unsigned int ids_count,
				const unsigned long* const track_ids,
				const char* const save_path) {
	if (!sendCmd(CMD_GET_FILES)) {
		return 0;
	}
	unsigned int i;
	for (i = 0; i < ids_count; ++i) {
		if (!getFile(track_ids[i], save_path)) {
			return 0;
		}
	}
	return 1;
}


int getFile(const unsigned long trackId, const char* const save_path) {
	struct TrackFileInfoPacket file_info;
	if (	sendBuff((char*)&trackId, sizeof trackId)
		&&	receive((char*)&file_info, sizeof(file_info))) {
		if (!file_info.size) {
			printf("Unknown file # %d or file has been removed.\n", trackId);
			return 1;
		} else {
			return receiveFile(&file_info, save_path);
		}
	}
	return 0;
}


int receiveFile(	const struct TrackFileInfoPacket*const file_info,
					const char* const save_path) {
	int result = 0;
	char *const buffer = (char*)malloc(file_info->size);
	const size_t save_path_len = strlen(save_path);
	char *const file = (char*)malloc(save_path_len + strlen(file_info->name) + 1);
	strcpy(file, save_path);
	strcpy(file + save_path_len, file_info->name);
	if (receive(buffer, file_info->size)) {
		FILE *f = fopen(file, "w");
		if (f) {
			fwrite(buffer, sizeof(char), file_info->size, f);
			fclose(f);
			printf("File has been successfully saved: %s.\n", file);
			result = 1;
		} else {
			printf("Can't save file: \"%s\".\n", file);
		}
	}
	free(file);
	free(buffer);
	return result;
}


int getPlaylist() {
	unsigned long count;
	if (	!sendCmd(CMD_GET_PLAYLIST)
		||	!receive((char*)&count, sizeof count)) {
		return 0;
	} else if (!count) {
		return 1;
	} 
	struct TrackInfoPacket *tracks = (struct TrackInfoPacket*)malloc(count * sizeof(struct TrackInfoPacket));
	const int rc = receive((char*)tracks, count * sizeof(struct TrackInfoPacket));
	if (rc) {
		int i;
		for (i = 0; i < count; ++i) {
			const float size = ((float)(tracks[i].size / 1024)) / (float)1024;
			printf(
				"%d\t%s - %s - %s (%s, %.2f Mb)\n",
				tracks[i].id,
				tracks[i].artist,
				tracks[i].album,
				tracks[i].file,
				tracks[i].kind,
				size);
		}
	}
	free(tracks);
	return rc;

}

void showHelp() {
	printf("\n");
	printf("USAGE:\n");
	printf("\n");
	printf("  itunes_connector \"host\" [ [get_files_all \"path\" ] |\n");
	printf("            [ get_files \"IDs\" \"path\" ] | get_playlist ]\n");
	printf("\n");
	printf("EXEMPLES:\n");
	printf("\n");
	printf("  itunes_connector 192.168.117.1 get_files_all /home/palchukovsky/music\n");
	printf("  itunes_connector broadcaster.com get_files 2564 789 /home/palchukovsky/music\n");
	printf("\n");
	printf("COMMAND-LINE OPTIONS:\n");
	printf("\n");
	printf("  host:          server hostname or IP-address;\n");
	printf("\n");
	printf("  get_files_all: to download all files from playlist;\n");
	printf("\n");
	printf("  get_files:     to download files with passed IDs\n");
	printf("                 (IDs from playlist, separated by whitespaces);\n");
	printf("\n");
	printf("  get_playlist:  to show server playlist\n");
	printf("\n");

}


int getFilesWithIdsFromCmdLine(	const int argc,
								char *const argv[],
								const char *const save_path) {
	const unsigned int ids_count = argc - 4;
	unsigned long *const ids = (long *)malloc(sizeof(unsigned long) * ids_count);
	int i;
	for (i = 0; i < ids_count; ++i) {
		ids[i] = atol(argv[i + 3]);
	}
	const int result = getFiles(ids_count, ids, save_path);
	free(ids);
	return result;
}


int main(const int argc, char *const argv[]) {

	int cmd = CMD_QUIT;

	if (argc >= 3) {
		if (!strcmp(argv[2], "get_files_all")) {
			if (argc == 4) {
				cmd = CMD_GET_FILES_ALL;
			}
		} else if (!strcmp(argv[2], "get_files")) {
			if (argc > 4) {
				cmd = CMD_GET_FILES;
			}
		} else if (!strcmp(argv[2], "get_playlist")) {
			cmd = CMD_GET_PLAYLIST;
		}
	}
	if (cmd == CMD_QUIT) {
		showHelp();
		return EXIT_SUCCESS;
	}

	const char* const host = argv[1];
	struct hostent *he = gethostbyname(host);
	if (he == NULL) {
		printf("Can't resolve host \"%s\".\n", host);
		return EXIT_FAILURE;
	}

	int save_path_len;
	char* save_path_cmd_ptr = NULL;
	char* save_path = NULL;
	if (cmd == CMD_GET_FILES_ALL || cmd == CMD_GET_FILES) {
		save_path_cmd_ptr = argv[argc - 1];
		save_path_len = strlen(save_path_cmd_ptr);
		if (save_path_cmd_ptr[save_path_len - 1] != '/') {
			save_path = (char*)malloc(save_path_len + 2);
			strcpy(save_path, save_path_cmd_ptr);
			save_path[save_path_len] = '/';
			save_path[save_path_len + 1] = 0;
		} else {
			save_path = save_path_cmd_ptr;
		}
	}

	sockfd = socket(PF_INET, SOCK_STREAM, 0);

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(SERVER_PORT);
	dest_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)he->h_addr)));
	memset(&(dest_addr.sin_zero), 0, 8);

	int result = !(0 > connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr)));

	if (!result) {
		printf("Can't connect to the host \"%s\".\n", host);
	} else {
		switch (cmd) {
			case CMD_GET_FILES_ALL:
				result = getAllFiles(save_path);
				break;
			case CMD_GET_FILES:
				result = getFilesWithIdsFromCmdLine(argc, argv, save_path);
				break;
			case CMD_GET_PLAYLIST:
				result = getPlaylist();
				break;
		}
		sendCmd(CMD_QUIT);
	}

	close(sockfd);

	if (save_path && save_path_cmd_ptr[save_path_len - 1] != '/') {
		free(save_path);
	}

	return result ? EXIT_SUCCESS : EXIT_FAILURE;

}

