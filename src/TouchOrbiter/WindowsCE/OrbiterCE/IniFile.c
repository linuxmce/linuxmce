#include "IniFile.h"
#include <malloc.h>
#include <string.h>

#define MAXSIZE 4096

struct IniNode
{
	struct IniData Data;
	struct IniNode *Next;
};

static void StripEol(char *str)
{
	if (strlen(str) == 0)
		return;

	// stript the EOL character
	char *LastChar = &str[strlen(str) - 1];
	while (LastChar != str && (*LastChar == '\r' || *LastChar == '\n'))
	{
		*LastChar = 0;
		--LastChar;
	}
}

BOOL InitIniFile(struct IniStore *IniStore)
{
	InitializeCriticalSection(&IniStore->CriticalSection);
	return TRUE;
}

BOOL ReadIniFile(LPCWSTR Filename, struct IniStore *IniStore)
{
	EnterCriticalSection(&IniStore->CriticalSection);
	FILE *f = _wfopen(Filename, L"rt");
	if (f == NULL)
	{
		IniStore->Count = 0;
		IniStore->Data = NULL;
		LeaveCriticalSection(&IniStore->CriticalSection);
		return FALSE;
	}

	char buffer[MAXSIZE];
	char Section[MAXSIZE];
	char Key[MAXSIZE];

	struct IniNode *IniNodeHead = NULL;
	struct IniNode *IniNodeCurrent = NULL;
	int IniNodeCount = 0;
	
	// read the file into a single linked list
	// reason: avoids continuously expanding an array
	// alternative idea: could do a two pass, first time just taking a count, second time getting the data
	while (fgets(buffer, MAXSIZE, f) != NULL)
	{
		StripEol(buffer);
		// skip empty lines
		if (strlen(buffer) == 0)
			continue;

		const char *LastChar = &buffer[strlen(buffer) - 1];
		if (buffer[0] == '[' && *LastChar == ']')
		{
			// get the string between the brackets
			memset(Section, 0, sizeof(Section));
			strncpy(Section, buffer + 1, strlen(buffer) - 2);
		}
		else
		{
			char *EqualSign = strchr(buffer, '=');
			// skip malformed lines
			if (EqualSign == NULL)
				continue;
			*EqualSign = 0;

			memset(Key, 0, MAXSIZE);
			snprintf(Key, MAXSIZE - 1, "%s.%s", Section, buffer);
			if (IniNodeCurrent == NULL)
			{
				IniNodeHead = IniNodeCurrent = (struct IniNode*) malloc(sizeof(struct IniNode));
			}
			else
			{
				IniNodeCurrent->Next = (struct IniNode*) malloc(sizeof(struct IniNode));
				IniNodeCurrent = IniNodeCurrent->Next;
			}
			IniNodeCount++;

			IniNodeCurrent->Data.Key = strdup(Key);
			IniNodeCurrent->Data.Value = strdup(EqualSign + 1);
			IniNodeCurrent->Next = NULL;
		}
	}

	// copy the linked list into an array
	// reason: easier to loop through
	// enhancement idea: could be a tree, for lookup speed
	IniStore->Data = (struct IniData *) malloc(IniNodeCount * sizeof(struct IniData));
	IniStore->Count = IniNodeCount;

	int i = 0;
	IniNodeCurrent = IniNodeHead;
	while (IniNodeCurrent != NULL)
	{
		IniStore->Data[i].Key = IniNodeCurrent->Data.Key;
		IniStore->Data[i].Value = IniNodeCurrent->Data.Value;

		// free the transcribed IniNode memory
		// its Key and Data pointers have been assigned to an array element
		IniNodeCurrent = IniNodeCurrent->Next;
		free(IniNodeHead);
		IniNodeHead = IniNodeCurrent;

		i++;
	}

	LeaveCriticalSection(&IniStore->CriticalSection);
	return TRUE;
}

const char * GetIniData(struct IniStore *IniStore, const char *Key)
{
	int i;

	EnterCriticalSection(&IniStore->CriticalSection);
	for (i = 0; i < IniStore->Count; i++)
	{
		if (strcmp(IniStore->Data[i].Key, Key) == 0)
		{
			const char *Return = strdup(IniStore->Data[i].Value);
			LeaveCriticalSection(&IniStore->CriticalSection);
			return Return;
		}
	}

	LeaveCriticalSection(&IniStore->CriticalSection);
	return NULL;
}
