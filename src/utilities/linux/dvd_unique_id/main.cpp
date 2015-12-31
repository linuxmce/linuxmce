#define __STDC_CONSTANT_MACROS
#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include <iostream>
#include <dvdread/dvd_reader.h>

using namespace std;

int main(int argc, char *argv[]) {
	
	// we need device name to continue
	if (argc < 2) {
		cout << "Usage: dvd_unique_id device" << endl;
		return 2;
	}

	dvd_reader_t *pDVD = DVDOpen(argv[1]);
	if (!pDVD) {
		cerr << "Failed to open " << argv[1] << endl;
		return 1;
	}

	int iRetCode = 0;

	// generate unique disk ID
	unsigned char uniqueID[32];
	if ( DVDDiscID(pDVD, uniqueID)!=0 ) {
		cerr << "Failed to generated unique ID" << endl;
		iRetCode = 3;
	}
	else {
		cout << hex;
		for (int i=0; i<16; i++)
			cout << (unsigned int) uniqueID[i];
		cout << endl;
	}

	DVDClose(pDVD);

	return iRetCode;
}

