//
// C++ Interface: ExtensionStatus
//
// Description: 
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EXTENSIONSTATUS_H
#define EXTENSIONSTATUS_H

#include <string>

using namespace std;

/**
The class which will keep all the information related to a telecom extension.

	@author Eugen Constantinescu <eugen.c@plutohome.com>
*/

namespace DCE
{
	class ExtensionStatus
	{
		public:
			
			/***/
			enum Availability { Unregistered=0, Registered, UnknownAvailability };
			
			/***/
			enum Activity { Ready=0, OnThePhone, Ringing, Dialing, UnknownActivity };
			
			/***/
			enum ExtensionStatusFormat { esfType=0, esfExtension, esfState, NUM_FIELDS};
			
			/***/
			enum ExtensionType { SIP=0, IAX, SCCP, OTHER };

			/***/
			static string Availability2String( Availability );
			/***/
			static string Activity2String( Activity );
			/***/
			static string Type2String( ExtensionType );
			/***/
			static Availability String2Availability(const string &);
			/***/
			static Activity String2Activity(const string &);
			/***/
			static ExtensionType String2Type(const string &);
		
			/***/
			ExtensionStatus();
			/***/
			~ExtensionStatus() {}
			
			/***/
			void SetID(const string & id) { sID = id; }
			/***/
			string GetID() const { return sID; }
			
			/***/
			void SetAvailability(Availability av) { availability = av; }
			/***/
			Availability GetAvailability() const { return availability; }
			
			/***/
			void SetActivity(Activity ac) { activity = ac; }
			/***/
			Activity GetActivity() const { return activity; }
			
			/***/
			void SetExtensionType( ExtensionType type ) { extType = type; }
			/***/
			ExtensionType GetExtensionType() const { return extType; }
			
			/***/
			string GetDebugInfo() const;
			
		private:
			
			string sID;
			Availability availability;
			Activity activity;
			ExtensionType extType;
	};
}

#endif
