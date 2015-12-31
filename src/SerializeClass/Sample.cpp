#include "SerializeClass.h"
#include "PlutoUtils/FileUtils.h"

#include <stdio.h>

#include <string>
using namespace std;

class SerializeObject : public SerializeClass
{
	public:
		int aInt;
		string aString;

		SerializeObject();
		virtual ~SerializeObject();

		virtual bool Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData=NULL);
		virtual string SerializeClassClassName() { return "SerializeObject"; }
		virtual void SetupSerialization(int /*iSC_Version*/);
};

SerializeObject::SerializeObject()
{
}

SerializeObject::~SerializeObject()
{
}

/*virtual*/ bool SerializeObject::Serialize(bool bWriting, char *&pcDataBlock, unsigned long &dwAllocatedSize, char *&pcCurrentPosition, void *pExtraSerializationData/*=NULL*/)
{
	printf("*** Serialize\n");
	return SerializeClass::Serialize(bWriting, pcDataBlock, dwAllocatedSize, pcCurrentPosition, pExtraSerializationData);
}

/*virtual*/ void SerializeObject::SetupSerialization(int /*iSC_Version*/)
{
	StartSerializeList() + aInt + aString;
}

int main()
{
	SerializeObject s;
	const string filename = "/tmp/SerializeObject.serobj";

	if (FileUtils::FileExists(filename))
	{
		s.SerializeRead(filename);
		printf("Loaded from file: aInt=0x%x aString=%s\n", s.aInt, s.aString.c_str());
	}

	s.aInt = 0xdeadbeef;
	time_t nowTime = time(NULL);
	s.aString = ctime(&nowTime);

	s.SerializeWrite(filename);
	return 0;
}
