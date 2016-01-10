/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#include <fstream>
#include <iostream>
#include <string.h>
using namespace std;

int main(int argc, char *argv[]){
	if(argc<1){
		cout<<"Usage: "<< argv[0]
				<<" fileWhatToRemove"<<endl;
		return 1;
	}
	char del[16384],str[16384];
	ifstream f( argv[1] );
	if(!f)
		return 1;
	f.getline( del, sizeof(del) );
//	cerr<<del<<endl;
	int i=0;
	cin.getline( str, sizeof(str) );
	while( !cin.eof() ){
		i=strncmp(str, del, 12);
		if(i>0){
			if(f.getline( del, sizeof(del) )){
//				cerr<<del<<endl;
				continue;
			}else{
				cout<<str<<endl;
				break;
			}
		}else if( i!=0 ){
			cout<<str<<endl;
		}
		cin.getline( str, sizeof(str) );
	}
	while( cin.getline( str, sizeof(str) ) ){
		cout<<str<<endl;
	}
	return 0;
}
