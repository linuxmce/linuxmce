#include <stdio.h>

void write_to_emulator(char data)
{
	FILE *fp;
	char buf[2];
	int len;
	fp = fopen("/run/Game_Player/send","wb");
	buf[0]=data;
	len = fwrite(&buf,1,1,fp);
	printf("write_to_emulator: wrote 0x%0x to send pipe. %d bytes\n",data,len);
	fclose(fp);
}

int main(int argc, char* argv[])
{
	int i;
	for (i=0;i<112;i++)
	{
		write_to_emulator(0x24);
		usleep(250000);
		write_to_emulator(0x00);
		usleep(250000);
	}
	return 0;
}
