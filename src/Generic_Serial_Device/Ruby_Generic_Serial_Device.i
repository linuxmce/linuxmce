%module Ruby_Generic_Serial_Device

%{
	#include "RubyDeviceWrapper.h"
	#include "RubyCommandWrapper.h"
	#include "RubySerialWrapper.h"
	#include "RubySerialIOWrapper.h"
	#include "RubySerialIOConnectionWrapper.h"

	using namespace DCE;
%}

%apply (char *STRING, int LENGTH) { (const char* buff, unsigned int size) };
%apply (char *STRING, int LENGTH) { (const char* delimbuff, unsigned int delimsize) };


%typemap(in) (char* buff, unsigned int size) {
   $2 = NUM2INT($input);
#ifdef __cplusplus
   $1 = ($1_ltype) new char[$2+1];
#else
   $1 = ($1_ltype) malloc($2+1);
#endif
}

%typemap(argout) (char* buff, unsigned int size) {
   if(result <= 0) {
		$result = rb_str_new2("");
   } else {
		$result = rb_str_new(arg2, result);
   }
#ifdef __cplusplus
		delete [] arg2;
		arg2 = NULL;
#else
		free(arg2);
		arg2 = NULL;
#endif
}

%include "std_map.i"
%include "std_string.i"

namespace std
{
	%template(RDWMap) map<int, DCE::RubyDeviceWrapper>;	
	%template(IParamMap) map<int, std::string>;
	%template(LParamMap) map<long, std::string>;
	%template(SParamMap) map<std::string, int>;
}


%typemap(out) std::string {
    $result = rb_str_new($1.data(), $1.length());
}

%include "RubyDeviceWrapper.h"
%include "RubyCommandWrapper.h"
%include "RubySerialIOConnectionWrapper.h"
%include "RubySerialWrapper.h"
%include "RubySerialIOWrapper.h"




