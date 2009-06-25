#ifndef FDIO_H
#define FDIO_H

enum Encoding_enum { Encoding_RAW, Encoding_ENC, Encoding_HEX };
enum BufferType_enum { BufferType_Stream, BufferType_Line };

struct SettingStorage
{
	enum Encoding_enum Encoding;
	enum BufferType_enum BufferType;
	unsigned char EolSeparator;
	int MaxBytes;
	int Fd;
	char *WriteData;
};

extern struct SettingStorage Settings;
extern void UsageExit(char *Program);

void Decode(const unsigned char *EncodedString, int EncodedLength, unsigned char **DecodedString, int *DecodedLength);
void Encode(const unsigned char *RawString, int RawLength, unsigned char **EncodedString, int *EncodedLength);

void ParseParms(int argc, char *argv[]);

#endif /* FDIO_H */
