// TagTest.cpp : Defines the entry point for the console application.
//

#include "../../include/taglib/tag.h"
#include "../../include/taglib/fileref.h"
using namespace TagLib;

#include <string>
#include <fstream>
using namespace std;

#include <assert.h>

#define CPPUNIT_ASSERT					assert
#define CPPUNIT_ASSERT_EQUAL(a, b)		assert((a) == (b));

void fileRefSave(const string newname)
{
	FileRef *f = new FileRef(newname.c_str());
	CPPUNIT_ASSERT(!f->isNull());
	f->tag()->setArtist("test artist");
	f->tag()->setTitle("test title");
	f->tag()->setGenre("Test!");
	f->tag()->setAlbum("albummmm");
	f->tag()->setTrack(5);
	f->tag()->setYear(2020);
	f->save();
	delete f;

	f = new FileRef(newname.c_str());
	CPPUNIT_ASSERT(!f->isNull());
	CPPUNIT_ASSERT_EQUAL(f->tag()->artist(), String("test artist"));
	CPPUNIT_ASSERT_EQUAL(f->tag()->title(), String("test title"));
	CPPUNIT_ASSERT_EQUAL(f->tag()->genre(), String("Test!"));
	CPPUNIT_ASSERT_EQUAL(f->tag()->album(), String("albummmm"));
	CPPUNIT_ASSERT_EQUAL(f->tag()->track(), TagLib::uint(5));
	CPPUNIT_ASSERT_EQUAL(f->tag()->year(), TagLib::uint(2020));
	f->tag()->setArtist("ttest artist");
	f->tag()->setTitle("ytest title");
	f->tag()->setGenre("uTest!");
	f->tag()->setAlbum("ialbummmm");
	f->tag()->setTrack(7);
	f->tag()->setYear(2080);
	f->save();
	delete f;

	f = new FileRef(newname.c_str());
	CPPUNIT_ASSERT(!f->isNull());
	CPPUNIT_ASSERT_EQUAL(f->tag()->artist(), String("ttest artist"));
	CPPUNIT_ASSERT_EQUAL(f->tag()->title(), String("ytest title"));
	CPPUNIT_ASSERT_EQUAL(f->tag()->genre(), String("uTest!"));
	CPPUNIT_ASSERT_EQUAL(f->tag()->album(), String("ialbummmm"));
	CPPUNIT_ASSERT_EQUAL(f->tag()->track(), TagLib::uint(7));
	CPPUNIT_ASSERT_EQUAL(f->tag()->year(), TagLib::uint(2080));
	delete f;
} 

int main(int argc, char* argv[])
{
	fileRefSave("/home/tests/file.mp3");
	fileRefSave("/home/tests/file.ogg");
	fileRefSave("/home/tests/file.flac");

	return 0;
}

