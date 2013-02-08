#include "Whisperer.h"
#include "ShellInterface.h"

void DisplaySyntaxMessage(const char *ProgName)
{
	fprintf(stderr, "Syntax: %s -d <Device_Number> -r <DCE_Router_Address> [-l <Logfile>] [-e <ExitCodeFile>]\n", ProgName);
}

int main(int argc, char *argv[])
{
	TheShell;

	if (argc == 1)
	{
		DisplaySyntaxMessage(argv[0]);
		return 1;
	}

	int DeviceNo = 0;
	char *RouterAddress = NULL;
	char *LogFile = NULL;
	char *ExitCodeFile = NULL;

	for (int i = 1; i < argc; ++i)
	{
		if (strcmp("-d", argv[i]) == 0)
		{
			if (++i < argc)
				DeviceNo = atoi(argv[i]);
		}
		else if (strcmp("-r", argv[i]) == 0)
		{
			if (++i < argc)
				RouterAddress = argv[i];
		}
		else if (strcmp("-l", argv[i]) == 0)
		{
			if (++i < argc)
				LogFile = argv[i];
		}
		else if (strcmp("-e", argv[i]) == 0)
		{
			if (++i < argc)
				ExitCodeFile = argv[i];
		}
		else
		{
			DisplaySyntaxMessage(argv[0]);
			return 1;
		}
	}

	if (DeviceNo <= 0 || RouterAddress == NULL)
	{
		DisplaySyntaxMessage(argv[0]);
		return 1;
	}

	if (LogFile != NULL)
		LoggerWrapper::SetType(LT_LOGGER_FILE, LogFile);

	Whisperer::Init(DeviceNo, RouterAddress);
	if (TheWhisperer->m_RequestHandlerThread)
	{
		pthread_join(TheWhisperer->m_RequestHandlerThread, NULL);  // This function will return when the device is shutting down
	}

	TheShell->Quit();

	int ExitCode = 0;
	if (TheWhisperer->m_bReload)
		ExitCode = 2;

	if (ExitCodeFile != NULL)
	{
		FILE *f = fopen(ExitCodeFile, "w");
		if (f != NULL)
		{
			fprintf(f, "%d\n", ExitCode);
			fclose(f);
		}
	}

	return ExitCode;
}
