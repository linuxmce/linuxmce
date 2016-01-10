#include "fdio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

const unsigned char IntToHex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
const unsigned char HexToInt[256] =
{
	/*00-0F*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*10-1F*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*20-2F*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*30-3F*/ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,       // 0-9
	/*40-4F*/ 0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, // A-F
	/*50-5F*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*60-6F*/ 0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, // a-f
	/*70-7F*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*80-8F*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*90-9F*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*A0-AF*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*B0-BF*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*C0-CF*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*D0-DF*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*E0-EF*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	/*F0-FF*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

struct SettingStorage Settings;

/* Decoder functions */

void Decode_ENC(const unsigned char *EncodedString, int EncodedLength, unsigned char **DecodedString, int *DecodedLength)
{
	*DecodedString = malloc(EncodedLength); // the output is up to the same length as the input
	*DecodedLength = 0;

	const unsigned char *Ptr = EncodedString;
	unsigned char *DecPtr = *DecodedString;
	unsigned char Chr = 0;

	int i = 0;
	while (i < EncodedLength)
	{
		if (*Ptr != '%')
		{
			Chr = *Ptr;
		}
		else
		{
			++Ptr;
			++i;
			if (*Ptr == '%')
				Chr = '%';
			else
			{
				Chr = (HexToInt[*Ptr] << 4) | HexToInt[*(Ptr + 1)];
				++Ptr;
				++i;
			}
		}

		*DecPtr = Chr;
		++*DecodedLength;
		++DecPtr;
		++Ptr;
		++i;
	}
}

void Decode_HEX(const unsigned char *EncodedString, int EncodedLength, unsigned char **DecodedString, int *DecodedLength)
{
	*DecodedString = malloc(EncodedLength / 2); // the output is always half the length of the input
	*DecodedLength = 0;

	const unsigned char *Ptr = EncodedString;
	unsigned char *DecPtr = *DecodedString;

	int i = 0;
	while (i < EncodedLength)
	{
		*DecPtr = (HexToInt[*Ptr] << 4) | HexToInt[*(Ptr + 1)];
		++Ptr;
		++Ptr;
		++DecPtr;
		++i;
		++i;
	}
	*DecodedLength = EncodedLength / 2;
}

void Decode(const unsigned char *EncodedString, int EncodedLength, unsigned char **DecodedString, int *DecodedLength)
{
	switch (Settings.Encoding)
	{
		case Encoding_RAW:
			*DecodedString = malloc(EncodedLength);
			memcpy(*DecodedString, EncodedString, EncodedLength);
			*DecodedLength = EncodedLength;
			break;
		case Encoding_ENC:
			Decode_ENC(EncodedString, EncodedLength, DecodedString, DecodedLength);
			break;
		case Encoding_HEX:
			Decode_HEX(EncodedString, EncodedLength, DecodedString, DecodedLength);
			break;
	}
}

/* Encoder functions */

void Encode_ENC(const unsigned char *RawString, int RawLength, unsigned char **EncodedString, int *EncodedLength)
{
	*EncodedString = malloc(RawLength * 3); // the encoded result can be three times the length of the input
	*EncodedLength = 0;

	const unsigned char *Ptr = RawString;
	unsigned char *EncPtr = *EncodedString;
	for (int i = 0; i < RawLength; ++i)
	{
		if (*Ptr < 32 || *Ptr > 127)
		{
			*EncPtr = '%';
			++EncPtr;
			*EncPtr = IntToHex[(*Ptr & 0xf0) >> 4];
			++EncPtr;
			*EncPtr = IntToHex[(*Ptr & 0x0f)];
			++EncPtr;
			*EncodedLength += 3;
		}
		else if (*Ptr == '%')
		{
			*EncPtr = '%';
			++EncPtr;
			*EncPtr = '%';
			++EncPtr;
			*EncodedLength += 2;
		}
		else
		{
			*EncPtr = *Ptr;
			++EncPtr;
			++*EncodedLength;
		}
		++Ptr;
	}
}

void Encode_HEX(const unsigned char *RawString, int RawLength, unsigned char **EncodedString, int *EncodedLength)
{
	*EncodedString = malloc(RawLength * 2); // the encoded result is always twice the length of the input
	*EncodedLength = 0;

	const unsigned char *Ptr = RawString;
	unsigned char *EncPtr = *EncodedString;
	for (int i = 0; i < RawLength; ++i)
	{
		*EncPtr = IntToHex[(*Ptr & 0xf0) >> 4];
		++EncPtr;
		*EncPtr = IntToHex[(*Ptr & 0x0f)];
		++EncPtr;
		++Ptr;
	}
	*EncodedLength = RawLength * 2;
}

void Encode(const unsigned char *RawString, int RawLength, unsigned char **EncodedString, int *EncodedLength)
{
	switch (Settings.Encoding)
	{
		case Encoding_RAW:
			*EncodedString = malloc(RawLength);
			memcpy(*EncodedString, RawString, RawLength);
			*EncodedLength = RawLength;
			break;
		case Encoding_ENC:
			Encode_ENC(RawString, RawLength, EncodedString, EncodedLength);
			break;
		case Encoding_HEX:
			Encode_HEX(RawString, RawLength, EncodedString, EncodedLength);
			break;
	}
}

/* Parse the parameters passed to the program */

void ParseParms(int argc, char *argv[])
{
	int i;

	Settings.Encoding = Encoding_RAW;
	Settings.BufferType = BufferType_Stream;
	Settings.EolSeparator = '\n';
	Settings.MaxBytes = 1024;
	Settings.Fd = -1;
	Settings.WriteData = NULL;

	for (i = 1; i < argc; i++)
	{
		int next_i = i + 1;
		if (strcmp(argv[i], "--raw") == 0)
			Settings.Encoding = Encoding_RAW;
		else if (strcmp(argv[i], "--enc") == 0)
			Settings.Encoding = Encoding_ENC;
		else if (strcmp(argv[i], "--hex") == 0)
			Settings.Encoding = Encoding_HEX;
		else if (strcmp(argv[i], "--line") == 0)
			Settings.BufferType = BufferType_Line;
		else if (strcmp(argv[i], "--eol") == 0 && (next_i < argc))
		{
			unsigned char *DecodedString;
			int DecodedLength;

			Decode((unsigned char *) argv[next_i], Settings.BufferType, &DecodedString, &DecodedLength);
			Settings.EolSeparator = DecodedString[0];
			++i;
		}
		else if (strcmp(argv[i], "--stream") == 0)
			Settings.BufferType = BufferType_Stream;
		else if (strcmp(argv[i], "--count") == 0 && (next_i < argc))
		{
			Settings.MaxBytes = atoi(argv[next_i]);
			++i;
		}
		else if (strcmp(argv[i], "--fd") == 0 && (next_i < argc))
		{
			Settings.Fd = atoi(argv[next_i]);
			++i;
		}
		else if (strcmp(argv[i], "--data") == 0 && (next_i < argc))
		{
			Settings.WriteData = argv[next_i];
			++i;
		}
		else
			UsageExit(argv[0]);
	}

	if (Settings.Fd < 0)
		UsageExit(argv[0]);
}
