/*
 * simple program used to get a snapshot of the memory of a Apex Destiny 6100 Alarm panel
 * uses TestSerialPort and a bunch of "system" calls. preatty messy and slow, but it works
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


char* get_wrapped_command_string( const char* command_string );

char* get_location_content_command_string( int byte_count, int offset );

int store_mem_contents_to_file( const char* filename );



int main( int argc, const char *argv[] )
{
	char file_before[256] = "snapshots/apex_mem_before_";
	char file_after[256] = "snapshots/apex_mem_after_";
	char descr[128];

	printf( "Type a description for the test: " );
	scanf( "%s", descr );

	strcat( file_before, descr );
	strcat( file_after, descr );

	store_mem_contents_to_file( file_before );
	
	printf( "\n\nMake the changes to the alarm panel configuration \
			and press Enter to get the \"after\" snapshot\n\n" );
	getchar();

	store_mem_contents_to_file( file_after );

	char command[256];
	sprintf( command, "vimdiff %s %s", file_before, file_after );
	system( command );
	
	return 0;
}

int store_mem_contents_to_file( const char* filename )
{
	char* command_start = "/usr/pluto/bin/TestSerialPort -p /dev/ttyUSB0 -P N81 -b 1200 -t \"";
	char command_end[30] = "\\r\\n\" -i 1.1\0";

	char command[256];
	char util[64];
	
	sprintf( command, "rm -f %s", filename );
	if ( system( command ) == -1 )
		return -1;
	

	int max_offset = 0x1B80;
	int max_byte_count = 0x20;

	int max_read_count = max_offset / max_byte_count;


	int read_count = 0;
	while ( read_count != max_read_count )
	{
		char* serial_command_string = get_location_content_command_string( max_byte_count,
				                                        read_count * max_byte_count );
		sprintf( util, "%.4x", read_count * max_byte_count );
		printf( "%5d: %s\n", read_count, serial_command_string );
		sprintf( command, "%s%s%s | awk '{print \"%s\t\" substr($0,7,68)}' >> %s", 
				command_start, 
				serial_command_string,
				command_end,
				util,
				filename );

		// printf( "%s\n", command );

		if ( system( command ) )
			return -1;
		
		read_count++;
	}
	
	return 0;
}

char* get_location_content_command_string( int byte_count, int offset )
{
	if ( byte_count > 0x20 )
		printf( "Cannot read more then %d bytes at one time", 0x20 );
	else if ( offset > 0x1B80 )
		printf( "The maximum offset is %d", 0x1B80 );

	char* outString;
	// TODO: maybe make it a parameter
	outString = calloc( sizeof( char ), 261 );
	sprintf( outString, "lr%.2x%.4x", byte_count, offset );

	return get_wrapped_command_string( outString );	
}

char* get_wrapped_command_string( const char* command_string )
{
	char buffer[3] ;
	char* outString;
	unsigned char checkSum;
	int i;

	// TODO: maybe make it a parameter
	outString = calloc( sizeof( char ), 261 );
	
	checkSum = 0;
	sprintf(outString,"%02X",strlen(command_string)+ 6);
	strcat(outString, command_string);
	strcat(outString, "00");
	i = 0;
	while (outString[i])
	{
		checkSum += outString[i++];
	}
	checkSum = ~(checkSum) + 1;
	sprintf(buffer,"%02X",checkSum);
	strcat(outString,buffer);
	
	return outString;
}
