#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCEConfig.h"
#include "Logger.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#else
#include <sys/wait.h>
#endif

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_RepositorySource.h"
#include "pluto_main/Table_RepositoryType.h"
#include "pluto_main/Table_RepositorySource_URL.h"
#include "pluto_main/Table_Installation_RepositorySource_URL.h"
#include "pluto_main/Table_Package.h"
#include "pluto_main/Table_Package_Compat.h"
#include "pluto_main/Table_Distro.h"
#include "pluto_main/Table_Version.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Table_Directory.h"
#include "pluto_main/Table_Package_Source_Compat.h"
#include "pluto_main/Table_Package_Source.h"
#include "pluto_main/Table_Package_Package.h"
#include "pluto_main/Table_Package_Directory.h"
#include "pluto_main/Table_Package_Directory_File.h"

#include "PlutoUtils/StringUtils.h"

#include "../version.h"

using namespace std;
using namespace DCE;

map<string,bool> g_DebianPackages;



// When we need to move files, we will use a list of this structure
class FileInfo
{
public:
	FileInfo(string sSource,string sDestination,Row_Package_Directory *pRow_Package_Directory)
	{
		m_sSource=sSource;
		m_sDestination=sDestination;
		m_pRow_Package_Directory=pRow_Package_Directory;
	}

	string m_sSource;
	string m_sDestination;
	Row_Package_Directory *m_pRow_Package_Directory;
};

string g_sPackages, g_sPackages_Exclude, g_sManufacturer, g_sSourcecodePrefix, g_sNonSourcecodePrefix, g_sCompile_Date, g_sBaseVersion, g_sReplacePluto, g_sOutputPath;
string g_sPK_RepositorySource;
string g_sSourceRevision;
int g_iPK_Distro=0;
int g_iCoresToUse=1;
bool g_bBuildSource = true, g_bCreatePackage = true, g_bCreateSourcePackage = false, g_bInteractive = false, g_bSimulate = false, g_bSupressPrompts = false, g_bDontTouchDB = false, g_bSetVersion = true, g_bOnlyCompileIfNotFound = false;
bool g_bStripAll = false;
Database_pluto_main *g_pDatabase_pluto_main;
Row_Version *g_pRow_Version;
Row_Distro *g_pRow_Distro;
DCEConfig dceConfig;
DBHelper *g_pDBHelper_pluto_builder=NULL;

// Int is the package ID, bool is true/false if it's been built.  This will be prepopulated with all the packages and 'false'
map<int,bool> g_mapPackagesToBuild,g_mapPackagesCompileSourceInOrder;

// The source code needs to be compiled in a certain order.  So we call CompileSourceInOrder first
// which is recursive and will call itself for all it's dependencies first.  It will then call
// CompileSource to do the actual compilation.  g_mapPackagesToBuild ensures we don't build the 
// same source code twice.
bool CompileSourceInOrder(Row_Package *pRow_Package);
bool CompileSource(Row_Package *pRow_Package);

// This will create the repository sources that we specified on the command line
bool CreateSources(Row_Package *pRow_Package);
bool CreateSource(Row_Package_Source *pRow_Package_Source,list<FileInfo *> &listFileInfo);

// These are the specific output functions that need to be implemented and expanded upon
bool CreateSource_PlutoDebian(Row_Package_Source *pRow_Package_Source,list<FileInfo *> &listFileInfo);
bool CreateSource_PlutoUbuntu(Row_Package_Source *pRow_Package_Source,list<FileInfo *> &listFileInfo);
bool CreateSource_FTPHTTP(Row_Package_Source *pRow_Package_Source,list<FileInfo *> &listFileInfo);
bool CreateSource_SourceForgeCVS(Row_Package_Source *pRow_Package_Source,list<FileInfo *> &listFileInfo);

// This will figure out what files need to be moved into the output
bool GetSourceFilesToMove(Row_Package *pRow_Package,list<FileInfo *> &listFileInfo);
bool GetNonSourceFilesToMove(Row_Package *pRow_Package,list<FileInfo *> &listFileInfo);

// Create archives
bool TarFiles(string sArchiveFileName);
bool ZipFiles(string sArchiveFileName);

// isDriverPackage
// hardcoded list of packages
bool isDriverPackage(int iPK_Package)
{
	static const int DriverPkgs[] = { 339, 338, 337, 336, 516, 515, 543, 542, 545, 544, 568, 569, 589, 590, -1 };
	for (int i = 0; DriverPkgs[i] != -1; i++)
	{
		if (iPK_Package == DriverPkgs[i])
			return true;
	}
	return false;
}

// isStrippablePackage
// hardcoded lists of packages
bool isStrippablePackage(int iPK_Package)
{
	static const int StripPkgs[] = { 237, -1 };
	// -y allows us to strip all packages.
	if (g_bStripAll) 
	{
		return true;
	}
	for (int i = 0; StripPkgs[i] != -1; i++)
	{
		if (iPK_Package == StripPkgs[i])
			return true;
	}
	return false;
}

// Misc
bool PackageIsCompatible(Row_Package *pRow_Package);
bool CopySourceFile(string sInput,string sOutput)
{
	if( g_sReplacePluto.size() && sInput.find("MakeRelease.cpp")==string::npos )
	{
		StringUtils::Replace(sInput,"/tmp/MakeRelease.tmp", "Pluto", g_sReplacePluto );
		sInput = "/tmp/MakeRelease.tmp";
	}
	return FileUtils::PUCopyFile(sInput,sOutput);
}

fstream fstr_compile,fstr_make_release;

int main(int argc, char *argv[])
{
	int iVersion=-1;
	bool bError=false,bUseFileLog=false;
	string sDefines;
	char c;
	for(int optnum=1;optnum<argc;++optnum)
	{
		c=argv[optnum][1];
		switch (c)
		{
		case 'h':
			dceConfig.m_sDBHost = argv[++optnum];
			break;
		case 'u':
			dceConfig.m_sDBUser = argv[++optnum];
			break;
		case 'p':
			dceConfig.m_sDBPassword = argv[++optnum];
			break;
		case 'D':
			dceConfig.m_sDBName = argv[++optnum];
			break;
		case 'P':
			dceConfig.m_iDBPort = atoi(argv[++optnum]);
			break;
		case 'g':
			bUseFileLog=true;
			break;
		case 'k':
			g_sPackages = argv[++optnum];
			break;
		case 'X':
			g_bOnlyCompileIfNotFound = true;
			break;
		case 'K':
			g_sPackages_Exclude = argv[++optnum];
			break;
		case 'm':
			g_sManufacturer = argv[++optnum];
			break;
		case 'x':
			g_sReplacePluto = argv[++optnum];
			break;
		case 'o':
			g_iPK_Distro = atoi(argv[++optnum]);
			break;
		case 'r':
			g_sPK_RepositorySource = argv[++optnum];
			break;
		case 'R':
			g_sSourceRevision = argv[++optnum];
			break;
		case 'b':
			g_bBuildSource = false;
			break;
		case 'c':
			g_bCreatePackage = false;
			break;
		case 'l':
			g_bCreateSourcePackage = true;
			break;
		case 'i':
			g_bInteractive = true;
			break;
		case 's':
			g_sSourcecodePrefix = argv[++optnum];
			break;
		case 'n':
			g_sNonSourcecodePrefix = argv[++optnum];
			break;
		case 'a':
			g_bSupressPrompts = true;
			break;
		case 'S':
			g_bSimulate = true;
			break;
		case 'v':
			iVersion = atoi(argv[++optnum]);
			break;
		case 'f':
			sDefines = argv[++optnum];
			break;
		case 'O':
			g_sOutputPath = argv[++optnum];
			break;
		case 'd':
			g_bDontTouchDB = true;
			break;
		case 'V':
			g_bSetVersion = false;
			break;
                case 'j': 
			// if (!verify_next(argv, optnum, argc, "-j")) { bError=true; break; } 
			g_iCoresToUse = atoi(argv[++optnum]); 
			break; 
		case 'y':
			g_bStripAll = true;
			break;
		default:
			cout << "Unknown: " << argv[optnum] << endl;
			bError=true;
			break;
		};
	}

	if ( bError)
	{
		cout << "MakeRelease, v." << VERSION << endl
			<< "Usage: MakeRelease [-h hostname] [-u username] [-p password] [-D database] [-P mysql port] [-j] [-k Packages] [-K Exclude Packages] [-m Manufacturers] [-o Distro] [-a] [-f Defines]" << endl 
			<< "                   [-d] [-r PK repository source] [-v version] [-V] [-O outputpath]" << endl
			<< "                   [-g] [-b] [-c] [-i] [-s source code prefix] [-S] [-j number of cores to use] " << endl
			<< "                   [-n non-source code prefix]  [-r PK repository source] [-v version] [-V]" << endl
			<< "hostname    -- address or DNS of database host, default is `dce_router`" << endl
			<< "username    -- username for database connection" << endl
			<< "password    -- password for database connection, default is `` (empty)" << endl
			<< "database    -- database name.  default is pluto_main" << endl
			<< "port        -- port for database connection, default is 3306" << endl
			<< "input path  -- Where to find the template files.  Default is . then ../MakeRelease" << endl
			<< "-a(abort)   -- Abort on error without prompting" << endl
			<< "-g          -- Use file log" << endl
			<< "-b          -- Don't build source package" << endl
			<< "-c          -- Don't build bin package" << endl
			<< "-i          -- Interactive" << endl
			<< "-s prefix   -- Prefix for source code" << endl
			<< "-n prefix   -- Prefix for non-source code" << endl
			<< "-S          -- Simulate" << endl
			<< "-O path     -- Where to put the output files. Default is ../[database name]" << endl
			<< "-V		-- Don't set version" << endl 
			<< "-d          -- Don't make changes to the database" << endl
			<< "-X          -- Simulate" << endl
			<< "-x          -- Replace pluto" << endl
			<< "-y          -- DH_STRIP all packages" << endl;

		exit(1);
	}

	string sWhere;


	struct tm *newtime;
	time_t aclock;
	time( &aclock );                 /* Get time in seconds */
	newtime = localtime( &aclock );  /* Convert time to struct */
	g_sCompile_Date = asctime( newtime );
	g_sCompile_Date = g_sCompile_Date.substr( 0, g_sCompile_Date.length()-1 );   // Strip off the trailing \n

	if( g_sPackages.length() )
		sWhere = "PK_Package IN (" + g_sPackages + ")";

	if( g_sPackages_Exclude.length() )
	{
		if( sWhere.length() )
			sWhere += " AND ";
		sWhere += "PK_Package NOT IN (" + g_sPackages_Exclude + ")";
	}

	if( g_sManufacturer.length() )
	{
		if( sWhere.length() )
			sWhere += " AND ";
		sWhere += "FK_Manufacturer IN (" + g_sManufacturer + ")";
	}

	if( g_sSourcecodePrefix.length() && g_sSourcecodePrefix[ g_sSourcecodePrefix.length()-1 ]!='/' )
		g_sSourcecodePrefix += "/";
	if( g_sNonSourcecodePrefix.length() && g_sNonSourcecodePrefix[ g_sNonSourcecodePrefix.length()-1 ]!='/' )
		g_sNonSourcecodePrefix += "/";

	fstr_compile.open("Compile.script",fstream::out);  // A log of all the commands we executed to do the compile
	fstr_make_release.open("MakeRelease.script",fstream::out);  // A log of all the commands we executed to make the release

	g_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
	if(!g_pDatabase_pluto_main->Connect(&dceConfig))
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot connect to database!");
		return 0;
	}

	if( bUseFileLog )
	{
		g_pDBHelper_pluto_builder = new DBHelper();
		if( g_pDBHelper_pluto_builder->DBConnect(dceConfig.m_sDBHost,dceConfig.m_sDBUser,dceConfig.m_sDBPassword,"pluto_builder",dceConfig.m_iDBPort)==false )
		{
			delete g_pDBHelper_pluto_builder;
			g_pDBHelper_pluto_builder=NULL;
			cout << "Cannot connect to pluto_builder" << endl;
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot connect to pluto_builder");
		}
	}

	// Get a list of all the packages that we are going to build based on the package and manufacturer data passed in
	vector<Row_Package *> vectPackages_Main;
	g_pDatabase_pluto_main->Package_get()->GetRows( sWhere.length()>0 ? sWhere : "1=1", &vectPackages_Main );
	cout << sWhere << endl;
	cout << "Found the following " << vectPackages_Main.size() << " packages:" << endl;
	for(size_t s=0;s<vectPackages_Main.size();++s)
	{
		g_mapPackagesToBuild[vectPackages_Main[s]->PK_Package_get()]=false;
		cout << vectPackages_Main[s]->PK_Package_get() << " " << vectPackages_Main[s]->Description_get() << " ";
//		if( (s+1) % 3==0 )
			cout << endl;
	}

	// If the user specified the version on the command line, he probably doesn't want any prompts.  It's an automated build
	if( iVersion==-1 )
	{
		vector<Row_Version *> vectRow_Version;
		g_pDatabase_pluto_main->Version_get()->GetRows("1=1 ORDER BY date desc limit 1",&vectRow_Version);
		Row_Version *pRow_Version = vectRow_Version[0];
		iVersion = pRow_Version->PK_Version_get();
		cout << "Determined latest version is " << iVersion << endl;
	}

	g_pRow_Distro = g_pDatabase_pluto_main->Distro_get()->GetRow(g_iPK_Distro);
	if( !g_pRow_Distro )
	{
		cout << "Cannot find that distro" << endl;
		return 1;
	}

	if( iVersion != -1 )
		g_pRow_Version = g_pDatabase_pluto_main->Version_get()->GetRow(iVersion);

	if( !g_pRow_Version )
	{
		cout << "What is the PK_Version?";
		string sPK_Version = StringUtils::GetStringFromConsole();
		g_pRow_Version = g_pDatabase_pluto_main->Version_get()->GetRow( atoi(sPK_Version.c_str()) );
		if( !g_pRow_Version )
		{
			cout << "Cannot find that version" << endl;
			return 1;
		}
	}

	if( g_sOutputPath.empty() )
		g_sOutputPath = "/home/builds/" + g_pRow_Version->VersionName_get();
	g_sBaseVersion = g_pRow_Version->VersionName_get();
	cout << "Version is " << g_sBaseVersion << " path is: " << g_sOutputPath << endl;

	string::size_type pos=0;
	int iDotCount;
	for(iDotCount=0;iDotCount<4 && pos!=string::npos;iDotCount++)
		pos = g_sBaseVersion.find('.',pos+1);
	if( iDotCount==4 && pos!=string::npos )
		g_sBaseVersion = g_sBaseVersion.substr(0,pos);

#pragma warning("had to use tiny text because medium text fails due to char[] in commit with field asSQL");
	
	cout << "Building version: " << g_pRow_Version->Description_get() << endl;  

	for(size_t s=0;s<vectPackages_Main.size();++s)
	{
		Row_Package *pRow_Package = vectPackages_Main[s];

		vector<Row_Package_Source *> vectRow_Package_Source_All;
		string::size_type pos=0;
		string sPK_RepositorySource;
		cout << g_sPK_RepositorySource << endl;	
		while( (sPK_RepositorySource=StringUtils::Tokenize(g_sPK_RepositorySource,",",pos)).length() )
		{
			vector<Row_Package_Source *> vectRow_Package_Source;
			g_pDatabase_pluto_main->Package_Source_get()->GetRows( 
				"FK_Package=" + StringUtils::itos(pRow_Package->PK_Package_get()) + " AND " +
				"FK_RepositorySource=" + sPK_RepositorySource,&vectRow_Package_Source);
			cout << "sources: " << vectRow_Package_Source.size() << endl;	
			for(size_t s=0;s<vectRow_Package_Source.size();++s)
			{
				Row_Package_Source *pRow_Package_Source = vectRow_Package_Source[s];
				// Update the version record
				cout << "Setting version for package " << pRow_Package_Source->FK_Package_get() << " " << pRow_Package_Source->FK_Package_getrow()->Description_get() << 
					" Source " << pRow_Package_Source->PK_Package_Source_get() << " " << pRow_Package_Source->FK_RepositorySource_getrow()->Description_get() <<
					": " << g_pRow_Version->VersionName_get() << endl;

				if( g_bSetVersion )
				{
//					if( pRow_Package_Source->FK_RepositorySource_get()==REPOSITORYSOURCE_Pluto_CVS_CONST || pRow_Package_Source->FK_RepositorySource_get()==REPOSITORYSOURCE_Pluto_SVN_CONST )
//						pRow_Package_Source->Version_set(g_pRow_Version->VersionName_get());
//					else
						pRow_Package_Source->Version_set(g_pRow_Version->VersionName_get());
				}
			}
		}
	}

	if( !g_bDontTouchDB )
	{
		cout << "Committing database changes" << endl;
		g_pDatabase_pluto_main->Package_Source_get()->Commit();
	}
	else
	{
		cout << "Not committing database changes, as told" << endl;
	}

	cout << "********************************************************************" << endl
		<< "COMPILATION PHASE" << endl
		<< "********************************************************************" << endl;
	if( g_bBuildSource )
	{
		for(size_t s=0;s<vectPackages_Main.size();++s)
		{
			Row_Package *pRow_Package = vectPackages_Main[s];
			if( !PackageIsCompatible(pRow_Package) )
			{
				cout << "Skipping: " << pRow_Package->Description_get() << " because it is not compatible" << endl;
				continue;
			}

			if( g_bInteractive && pRow_Package->FK_Package_Sourcecode_isNull() && pRow_Package->IsSource_get()==0 )
			{
				cout << "There is no source code for package: " << pRow_Package->Description_get() << endl;
				if( !AskYNQuestion("Continue without building it?",false)  )
					return 1;
			}
			if( !pRow_Package->FK_Package_Sourcecode_isNull() )
			{
				if( !CompileSourceInOrder(pRow_Package) )
				{
					cout << "Aborting!" << endl;
					exit(1);
				}
			}
		}
	}

	cout << "********************************************************************" << endl
		<< "PACKAGE CREATION PHASE" << endl
		<< "********************************************************************" << endl;

	if( g_bCreatePackage )
	{
		for(size_t s=0;s<vectPackages_Main.size();++s)
		{
			Row_Package *pRow_Package = vectPackages_Main[s];

			if( !PackageIsCompatible(pRow_Package) )
			{
				cout << "Skipping: " << pRow_Package->Description_get() << " because it is not compatible" << endl;
				continue;
			}

			if( pRow_Package->IsSource_get() && !g_bCreateSourcePackage )
			{
				cout << "Skipping: " << pRow_Package->Description_get() << " because source packaging is disabled" << endl;
				continue;
			}

			if( !CreateSources(pRow_Package) )
			{
				cout << "Aborting!" << endl;
				return 1;
			}
		}
	}

	if( g_iPK_Distro==DISTRO_Debian_Sarge_CONST )
	{
		cout << "About to write debian-packages.tmp" << endl;
		map<string,bool>::iterator it;
		string sDebPkg = "";
		for (it = g_DebianPackages.begin(); it != g_DebianPackages.end(); it++)
		{
			sDebPkg += it->first + " ";
		}
		
		FILE * f = fopen((g_sOutputPath + "/debian-packages.tmp").c_str(), "wb");
		if(f)
		{
			cout << "Writing debian-packages.tmp" << endl;
			fprintf(f, "%s\n", sDebPkg.c_str());
			fclose(f);
		}
	}
	cout << "Done!" << endl << endl;
	delete g_pDBHelper_pluto_builder;
	g_pDBHelper_pluto_builder=NULL;
}

bool PackageIsCompatible(Row_Package *pRow_Package)
{
	vector<Row_Package_Compat *> vectRow_Package_Compat;
	pRow_Package->Table_Package_get()->Database_pluto_main_get()->Package_Compat_get()->GetRows(
		"(FK_Distro IS NULL OR FK_Distro=" + StringUtils::itos(g_pRow_Distro->PK_Distro_get()) +") AND " +
		"(FK_OperatingSystem IS NULL OR FK_OperatingSystem=" + StringUtils::itos(g_pRow_Distro->FK_OperatingSystem_get()) + ") AND "+
		"FK_Package=" + StringUtils::itos(pRow_Package->PK_Package_get()),
		&vectRow_Package_Compat);
	return vectRow_Package_Compat.size()!=0;
	
}

bool CreateSources(Row_Package *pRow_Package)
{
	cout << "====================================================" << endl;
	cout << "Creating sources for : " << pRow_Package->Description_get() << "#: " << pRow_Package->PK_Package_get() << " Source: #" << pRow_Package->FK_Package_Sourcecode_get() << endl;
	cout << "====================================================" << endl;

	vector<Row_Package_Source *> vectRow_Package_Source_All;
	string::size_type pos=0;
	string sPK_RepositorySource;
cout << g_sPK_RepositorySource << endl;	
	while( (sPK_RepositorySource=StringUtils::Tokenize(g_sPK_RepositorySource,",",pos)).length() )
	{
		vector<Row_Package_Source *> vectRow_Package_Source;
		g_pDatabase_pluto_main->Package_Source_get()->GetRows( 
			"FK_Package=" + StringUtils::itos(pRow_Package->PK_Package_get()) + " AND " +
			"FK_RepositorySource=" + sPK_RepositorySource,&vectRow_Package_Source);
		if( vectRow_Package_Source.size()==0 )
		{
			cout << "***WARNING*** There is no info for this package and repository source " << sPK_RepositorySource << endl;
			if( g_bInteractive && !AskYNQuestion("Is this okay?",false) )
				return false;
			continue;
		}

cout << "sources: " << vectRow_Package_Source.size() << endl;	
		for(size_t s=0;s<vectRow_Package_Source.size();++s)
		{
			Row_Package_Source *pRow_Package_Source = vectRow_Package_Source[s];
			vectRow_Package_Source_All.push_back(pRow_Package_Source);
		}
	}

	cout << "Sources:" << endl;
	for(size_t s=0;s<vectRow_Package_Source_All.size();++s)
	{
		Row_Package_Source *pRow_Package_Source = vectRow_Package_Source_All[s];
		cout << pRow_Package_Source->FK_RepositorySource_getrow()->Description_get() << ":" << pRow_Package_Source->Repository_get()<<"  ";
		if( (s+1) % 3==0 )
			cout << endl;
	}
	cout << endl;

	// Now go through and figure out what files we need to move over.  The process is different if this is a source package versus a binary package
	cout << "Scanning for files..." << endl;
	list<FileInfo *> listFileInfo;
	if( pRow_Package->IsSource_get() )
	{
		if( !GetSourceFilesToMove(pRow_Package,listFileInfo) )
		{
			cout << "GetSourceFilesToMove failed" << endl;
			return false;
		}
	}
	else if( !GetNonSourceFilesToMove(pRow_Package,listFileInfo) )
	{
		cout << "GetNonSourceFilesToMove failed" << endl;
		return false;
	}

	cout << "Found: " << listFileInfo.size() << " files" << endl;

	if( listFileInfo.size()==0 )
	{
		cout << "***ERROR*** No files found for this package!" << endl;
		if( g_bInteractive )
			return AskYNQuestion("Continue anyway?",false);
		return true;
	}

	// Now go through and figure out what executible files we need to move over.
	if( g_bInteractive )
	{
		if( AskYNQuestion("View the list of files?",false) )
		{
			for(list<FileInfo *>::iterator it=listFileInfo.begin();it!=listFileInfo.end();++it)
			{
				FileInfo *pFileInfo = *it;
				cout << pFileInfo->m_sSource << "  -->  " << pFileInfo->m_sDestination << endl;
			}

		if( !AskYNQuestion("Continue?",false) )
			return false;
		}
	}

	cout << "Ready to create: " << vectRow_Package_Source_All.size() << " packages" << endl;

	for(size_t s=0;s<vectRow_Package_Source_All.size();++s)
	{
		Row_Package_Source *pRow_Package_Source = vectRow_Package_Source_All[s];
		if( !g_bSimulate && !CreateSource(pRow_Package_Source,listFileInfo) )
		{
			cout << "***ABORTING***" << endl;
			return false;
		}
	}
	return true;
}

bool CreateSource(Row_Package_Source *pRow_Package_Source,list<FileInfo *> &listFileInfo)
{
	cout << "\tCreating source: " << pRow_Package_Source->FK_RepositorySource_getrow()->Description_get() << endl;
	cout << "\t--------------------------" << endl;

	if( pRow_Package_Source->FK_RepositorySource_getrow()->FK_RepositoryType_get()==REPOSITORYTYPE_FTP_or_HTTP_Download_CONST )
	{
		CreateSource_FTPHTTP(pRow_Package_Source,listFileInfo);
	}
	else
	{
		switch(pRow_Package_Source->FK_RepositorySource_get())
		{
		case REPOSITORYSOURCE_Pluto_Debian_CONST:
			return CreateSource_PlutoDebian(pRow_Package_Source,listFileInfo);
			break;
		case REPOSITORYSOURCE_Ubuntu_Pluto_Addons_CONST:
			//return CreateSource_PlutoUbuntu(pRow_Package_Source,listFileInfo);
			return CreateSource_PlutoDebian(pRow_Package_Source,listFileInfo);
			break;
		case REPOSITORYSOURCE_Ubuntu_LinuxMCE_Addons_CONST:
			return CreateSource_PlutoDebian(pRow_Package_Source,listFileInfo);
			break;
		case REPOSITORYSOURCE_Raspbian_LinuxMCE_addons_CONST:
			return CreateSource_PlutoDebian(pRow_Package_Source,listFileInfo);
			break;
		case REPOSITORYSOURCE_SourceForge_CVS_CONST:
			return CreateSource_SourceForgeCVS(pRow_Package_Source,listFileInfo);
			break;
		default:
			cout << "***ERROR*** Don't know how to create this source. " ;
			return false;
		}
	}

	return true;
}

bool GetSourceFilesToMove(Row_Package *pRow_Package,list<FileInfo *> &listFileInfo)
{
	vector<Row_Package_Directory *> vectRow_Package_Directory;
	cout <<"\nSearch for package number: " << pRow_Package->PK_Package_get() << endl;
	pRow_Package->Package_Directory_FK_Package_getrows(&vectRow_Package_Directory);
	for(size_t s=0;s<vectRow_Package_Directory.size();++s)
	{
		Row_Package_Directory *pRow_Package_Directory = vectRow_Package_Directory[s];
		if( pRow_Package_Directory->FK_Directory_get()==DIRECTORY_Compiled_Output_CONST )
			continue; // We ignore these

		string sDirectory;
		if( pRow_Package_Directory->InputPath_get()!="" )
		{
			if( pRow_Package_Directory->InputPath_get()[0]=='/' )
				sDirectory = pRow_Package_Directory->InputPath_get();
			else
				sDirectory += g_sSourcecodePrefix + "/" + pRow_Package_Directory->InputPath_get();
		}
		else
		{
			if( pRow_Package_Directory->Path_get()[0]=='/' )
				sDirectory = pRow_Package_Directory->Path_get();
			else
				sDirectory += g_sSourcecodePrefix + "/" + pRow_Package_Directory->Path_get();
		}

		vector<Row_Package_Directory_File *> vectPackage_Directory_File;
		pRow_Package_Directory->Package_Directory_File_FK_Package_Directory_getrows(&vectPackage_Directory_File);
		for(size_t s2=0;s2<vectPackage_Directory_File.size();++s2)
{
			Row_Package_Directory_File *pRow_Package_Directory_File = vectPackage_Directory_File[s2];
			string File = pRow_Package_Directory_File->File_get();
			if( File.find('*')!=string::npos || File.find('?')!=string::npos )
			{
				list<string> listFiles;
				FileUtils::FindFiles(listFiles,sDirectory,File,true);
				if( g_bInteractive )
					cout << "Found: " << listFiles.size() << " files in " << sDirectory << endl;
				for(list<string>::iterator it=listFiles.begin();it!=listFiles.end();++it)
				{
					//cout<<"\ncreate source sD: "<<sDirectory<<' '<<*it;
					FileInfo *pFileInfo = new FileInfo(sDirectory + "/" + *it,
						pRow_Package_Directory->Path_get() + "/" + *it,
						pRow_Package_Directory);
					listFileInfo.push_back(pFileInfo);
				}
			}
			else
			{
				if( !FileUtils::FileExists(sDirectory + "/" + File) )
				{
					cout << "***WARNING*** " << sDirectory << "/" << File << " not found" <<  endl;
					if( g_bSupressPrompts || !AskYNQuestion("Continue?",false) )
						return false;
				}
					//cout<<"\ncreate source SD: "<<sDirectory<<' '<<File;
				FileInfo *pFileInfo = new FileInfo(sDirectory + "/" + File,
					pRow_Package_Directory->Path_get() + "/" + File,
					pRow_Package_Directory);
				listFileInfo.push_back(pFileInfo);
			}
		}
	}
	return true;
}

string MakeCommandMods(string in) 
{
	string out = in; 
	if (g_iCoresToUse > 1) 
	{
		out = StringUtils::Replace(out, "make clean bin", "make clean ; make bin"); 
		out = StringUtils::Replace(out, "make clean lib", "make clean ; make lib"); 
		stringstream sstr; 
		sstr << g_iCoresToUse; 
		if (out.find("qmake") == string::npos) {
			out = StringUtils::Replace(out, "make", "make -j" + sstr.str()); 
		} else {
			out = StringUtils::Replace(out, " make", " make -j" + sstr.str()); 
			out = StringUtils::Replace(out, ";make", "; make -j" + sstr.str()); 
		}
	}
	return out; 
}

bool GetNonSourceFilesToMove(Row_Package *pRow_Package,list<FileInfo *> &listFileInfo)
{
	/*
		For non-source files, the following directory structure is assumed:
		The path is always the output path on the target machine.  It is assumed
		to be in the input path on the build machine too unless an alternate
		input path is specified.

		The input path will be prefixed with the -direcoty n unless
		directory type is Source Implementation, in which case we'll look in -s.
		If FlipSource is 1, that behavior is inverted.

		If a directory entry exists for 'compiled output', then for all binary
		executibles and binary libraries the prefix is assumed to be the -s
		source + the compiled output directory.
	*/

	// First is there a directory specified for finding the compiled output?
	// If so, we'll grab the binaries from there, since the build process put them in that directory
	Row_Package_Directory *pRow_Package_Directory_CompiledOutput=NULL;
	vector<Row_Package_Directory *> vectRow_Package_Directory;
	g_pDatabase_pluto_main->Package_Directory_get()->GetRows( 
		"FK_Package=" + StringUtils::itos(pRow_Package->PK_Package_get()) + " AND FK_Directory=" + StringUtils::itos(DIRECTORY_Compiled_Output_CONST),
		&vectRow_Package_Directory);
	if( vectRow_Package_Directory.size() )
		pRow_Package_Directory_CompiledOutput = vectRow_Package_Directory[0];

	// What are the files for this?
	pRow_Package->Package_Directory_FK_Package_getrows(&vectRow_Package_Directory);
	for(size_t s=0;s<vectRow_Package_Directory.size();++s)
	{
		Row_Package_Directory *pRow_Package_Directory = vectRow_Package_Directory[s];
		if( (!pRow_Package_Directory->FK_Distro_isNull() && pRow_Package_Directory->FK_Distro_get()!=g_pRow_Distro->PK_Distro_get()) ||
				(!pRow_Package_Directory->FK_OperatingSystem_isNull() && pRow_Package_Directory->FK_OperatingSystem_get()!=g_pRow_Distro->FK_OperatingSystem_get()) )
			continue;
		string sInputPath;

		// If there's a compiled output directory, use that unless the user specifically gave an input path
		if( pRow_Package_Directory_CompiledOutput && pRow_Package_Directory->InputPath_get()=="" &&
			(pRow_Package_Directory->FK_Directory_get()==DIRECTORY_Binary_Executables_CONST ||
			pRow_Package_Directory->FK_Directory_get()==DIRECTORY_Binary_Library_CONST) )
		{
			sInputPath = g_sSourcecodePrefix + pRow_Package_Directory_CompiledOutput->Path_get();
		}
		else 
		{
			if( (pRow_Package_Directory->FK_Directory_get()==DIRECTORY_Source_Implementation_CONST && pRow_Package_Directory->FlipSource_get()==0) ||
					(pRow_Package_Directory->FK_Directory_get()!=DIRECTORY_Source_Implementation_CONST && pRow_Package_Directory->FlipSource_get()==1) )
				sInputPath = g_sSourcecodePrefix;
			else
				sInputPath = g_sNonSourcecodePrefix;

			if( pRow_Package_Directory->InputPath_get()!="" )
				sInputPath += pRow_Package_Directory->InputPath_get();
			else
				sInputPath += pRow_Package_Directory->Path_get();
		}

		if( sInputPath.length()==0 )
		{
			if( g_bSupressPrompts || !AskYNQuestion("***WARNING*** Directory is empty.  Continue?",false) )
				return false;

			sInputPath = g_sNonSourcecodePrefix;
		}
//		else if( sDirectory[0]!='/' && sDirectory.substr(1,2) != ":\\" && sDirectory.substr(1,2) != ":/")
// put the prefix above			sDirectory =  + sDirectory;
	
		vector<Row_Package_Directory_File *> vectPackage_Directory_File;
		pRow_Package_Directory->Package_Directory_File_FK_Package_Directory_getrows(&vectPackage_Directory_File);
		for(size_t s2=0;s2<vectPackage_Directory_File.size();++s2)
		{
			Row_Package_Directory_File *pRow_Package_Directory_File = vectPackage_Directory_File[s2];
			if( (!pRow_Package_Directory_File->FK_Distro_isNull() && pRow_Package_Directory_File->FK_Distro_get()!=g_pRow_Distro->PK_Distro_get()) ||
					(!pRow_Package_Directory_File->FK_OperatingSystem_isNull() && pRow_Package_Directory_File->FK_OperatingSystem_get()!=g_pRow_Distro->FK_OperatingSystem_get()) )
				continue;

			string File = pRow_Package_Directory_File->File_get();
			if( pRow_Package_Directory_File->MakeCommand_get()!="" )
			{
				string tmp = pRow_Package_Directory_File->MakeCommand_get(); 
				if( g_bInteractive && !AskYNQuestion("About to execute non-source make: " + tmp + " Continue?",false) ) 
					return false;
#ifndef WIN32
				system(("mkdir -p " + sInputPath).c_str());
#endif
				chdir(sInputPath.c_str());
				tmp = MakeCommandMods(tmp); 
				cout << "Package: " << pRow_Package->PK_Package_get() << " Executing: " << tmp << " from dir: " << sInputPath << endl; 
				if( g_bOnlyCompileIfNotFound && FileUtils::FileExists(sInputPath + "/" + File) )
					cout << "Skipping compilation of package: " << pRow_Package->PK_Package_get() << endl;
				else if( !g_bSimulate && system(tmp.c_str()) ) 
				{
					cout << "Description: " << pRow_Package_Directory_File->FK_Package_Directory_getrow()->FK_Package_getrow()->Description_get() << endl;
					cout << "Path: " << pRow_Package_Directory_File->FK_Package_Directory_getrow()->Path_get() << endl;
					cout << tmp << " ***FAILED***" << endl; 
					cout << "Error: " << tmp << " failed!" << endl; 
					if( g_bSupressPrompts || !AskYNQuestion("Continue anyway?",false) )
						return false;
				}
			}
			if( File.find('*')!=string::npos || File.find('?')!=string::npos )
			{
				list<string> listFiles;
				
				cout << "Scanning: " << sInputPath << " for " << File << endl;
				FileUtils::FindFiles(listFiles,sInputPath,File,true);
				if( g_bInteractive )
					cout << "Found: " << listFiles.size() << " files" << endl;
				
				if( listFiles.size()==0 )
				{
					cout << "***WARNING*** No files found in " << sInputPath << " for " << File << endl;
					if( g_bInteractive )
					{
						if (!AskYNQuestion("Continue anyway?",false))
							return false;
					}
					continue;
				}
				for(list<string>::iterator it=listFiles.begin();it!=listFiles.end();++it)
				{
					//cout<<"\ncreate source : "<<sInputPath<<' '<<*it;
					FileInfo *pFileInfo = new FileInfo(sInputPath +"/" + *it,
						pRow_Package_Directory->Path_get() + "/" + *it,
						pRow_Package_Directory);
					listFileInfo.push_back(pFileInfo);
				}
			}
			else
			{
				if( !FileUtils::FileExists(sInputPath + "/" + File) )
				{
					cout << "***WARNING*** " << sInputPath << "/" << File << " not found" <<  endl;
					if( g_bSupressPrompts || !AskYNQuestion("Continue?",false) )
						return false;
				}
				//cout<<"\ncreate source : "<<sInputPath<<' '<<File;
				FileInfo *pFileInfo = new FileInfo(sInputPath + "/" + File,
					pRow_Package_Directory->Path_get() + "/" + File,
					pRow_Package_Directory);
				listFileInfo.push_back(pFileInfo);
			}
		}
	}
	return true;
}

bool CompileSourceInOrder(Row_Package *pRow_Package)
{
	if( g_mapPackagesToBuild.find(pRow_Package->PK_Package_get())==g_mapPackagesToBuild.end() )
		return true; // It's okay--this isn't one of the packages in our list anyway

	g_mapPackagesCompileSourceInOrder[pRow_Package->PK_Package_get()]=true;

	// Get all the dependencies of this package and build them first
	vector<Row_Package_Package *> vectRow_Package_Package;
	pRow_Package->Package_Package_FK_Package_getrows(&vectRow_Package_Package);
	for(size_t s=0;s<vectRow_Package_Package.size();++s)
	{
		Row_Package_Package *pRow_Package_Package = vectRow_Package_Package[s];
		Row_Package *pRow_Package_Package_DependsOn = pRow_Package_Package->FK_Package_DependsOn_getrow();
		if( !pRow_Package_Package_DependsOn )
		{
			cerr << "Package: " << pRow_Package_Package->FK_Package_get() << " depends on non-existant package: " << pRow_Package_Package->FK_Package_DependsOn_get();
			if( !AskYNQuestion("Continue?",false) )
				return false;
			else
				return true;
		}

		if( !PackageIsCompatible(pRow_Package_Package_DependsOn) )
		{
			cout << "Skipping: " << pRow_Package_Package_DependsOn->Description_get() << " because it is not compatible" << endl;
			continue;
		}

		// Don't allow an infinite recursion
		if( g_mapPackagesCompileSourceInOrder[pRow_Package_Package_DependsOn->PK_Package_get()]==false &&
			!CompileSourceInOrder(pRow_Package_Package_DependsOn) )
				return false;
	}

	if( !pRow_Package->FK_Package_Sourcecode_isNull() && !CompileSourceInOrder(pRow_Package->FK_Package_Sourcecode_getrow()) )
	{
		cout << "BuildSource failed for: " << pRow_Package->FK_Package_Sourcecode_getrow()->PK_Package_get() << " " << pRow_Package->FK_Package_Sourcecode_getrow()->Description_get() << endl;
		return false;
	}

	// We know this package is in the map.  If it was built already, we don't need to do it again
	if( g_mapPackagesToBuild[pRow_Package->PK_Package_get()]==true )
		return true;

	if( !CompileSource(pRow_Package) )
	{
		cout << "BuildSource failed for: " << pRow_Package->PK_Package_get() << " " << pRow_Package->Description_get() << endl;
		return false;
	}
	return true;
}

bool CompileSource(Row_Package *pRow_Package)
{
	if( pRow_Package->FK_Package_Sourcecode_isNull() )
		return true; // It's okay.  Nothing to do here

	if( g_mapPackagesToBuild.find(pRow_Package->FK_Package_Sourcecode_getrow()->PK_Package_get())==g_mapPackagesToBuild.end() )
	{
		cout << "***ERROR***: The package: " << pRow_Package->Description_get() << " needs to be built" << endl 
			<< "But it's source code: " << pRow_Package->FK_Package_Sourcecode_getrow()->Description_get() << " isn't in the include list!" << endl;
		return false;
	}

	if( pRow_Package->FK_Package_Sourcecode_getrow()->IsSource_get()==0 )
	{
		cout << "***ERROR***: The package: " << pRow_Package->Description_get() << " indicated its source is in" << endl 
			<< pRow_Package->FK_Package_Sourcecode_getrow()->Description_get() << " but that isn't marked as a source package" << endl;
		return false;
	}

	cout << "====================================================" << endl;
	cout << "Compiling source package: " << pRow_Package->FK_Package_Sourcecode_get() << " " << pRow_Package->FK_Package_Sourcecode_getrow()->Description_get() << endl;
	cout << "====================================================" << endl;

	vector<Row_Package_Source *> vectRow_Package_Source;
	g_pDatabase_pluto_main->Package_Source_get()->GetRows(
		"JOIN Package_Source_Compat ON PK_Package_Source=FK_Package_Source WHERE FK_Package=" + StringUtils::itos(pRow_Package->FK_Package_Sourcecode_get()) +
		" AND FK_RepositorySource=" + StringUtils::itos(REPOSITORYSOURCE_Pluto_SVN_CONST) +
		" AND ((FK_Distro=" + StringUtils::itos(g_pRow_Distro->PK_Distro_get()) + " OR FK_Distro IS NULL) AND (FK_OperatingSystem IS NULL OR FK_OperatingSystem=" + StringUtils::itos(g_pRow_Distro->FK_OperatingSystem_get()) + "))",
		&vectRow_Package_Source);
	if( vectRow_Package_Source.size()>1 )
	{
		cout << "Package: " << pRow_Package->Description_get() << " has multiple SVN compatibilities for this distro!" << endl;
		return false;
	}

	// Find out where this is in our SVN repository
	Row_Package_Source *pRow_Package_Source_SVN=NULL;
	if( vectRow_Package_Source.size()==0 )
	{
		cout << "Package: " << pRow_Package->Description_get() << " does not have an entry in Package_Source table for SVN." << endl;
		if( !g_bSupressPrompts && AskYNQuestion("Create one?",true) )
		{
			pRow_Package_Source_SVN = g_pDatabase_pluto_main->Package_Source_get()->AddRow();
			pRow_Package_Source_SVN->FK_RepositorySource_set(REPOSITORYSOURCE_Pluto_SVN_CONST);
			pRow_Package_Source_SVN->FK_Package_set(pRow_Package->FK_Package_Sourcecode_get());
AsksSourceQuests:
			cout << "What is the name?";
			string s = StringUtils::GetStringFromConsole();
			pRow_Package_Source_SVN->Name_set(s);

			cout << "What is the repository?"; 
			s = StringUtils::GetStringFromConsole();
			pRow_Package_Source_SVN->Repository_set(s);

			cout << "Are there any parms?";
			s = StringUtils::GetStringFromConsole();

			pRow_Package_Source_SVN->Parms_set(s);

			char c = AskMCQuestion("Is this okay? (Yes/No/Abort)","yna");
			if( c=='a' )
			{
				return false;
			}
			if( c=='n' )
				goto AsksSourceQuests;
		}
		else
		{
			return true;  // Not necessarily a critical error.  Keep going
		}

		if( g_bSetVersion )
			pRow_Package_Source_SVN->Version_set(g_pRow_Version->VersionName_get());
		if( !g_bDontTouchDB )
			g_pDatabase_pluto_main->Package_Source_get()->Commit();
	}
	else
		pRow_Package_Source_SVN = vectRow_Package_Source[0];

	// What directories contain the source code?
	vector<Row_Package_Directory *> vectRow_Package_Directory;
	g_pDatabase_pluto_main->Package_Directory_get()->GetRows( 
		"FK_Package=" + StringUtils::itos(pRow_Package->FK_Package_Sourcecode_get()) + " AND FK_Directory=" + StringUtils::itos(DIRECTORY_Compiled_Output_CONST),
		&vectRow_Package_Directory);
	if( vectRow_Package_Directory.size()==0 )
	{
		cout << "There is no directory specified for the compiled output." << endl;
		char c = AskMCQuestion("Specify one now? (Yes/No/Abort)","yna");
		if( c=='a' )
		{
			return false;
		}
		if( c=='y' )
		{
			Row_Package_Directory *pRow_Package_Directory = g_pDatabase_pluto_main->Package_Directory_get()->AddRow();
			pRow_Package_Directory->FK_Package_set(pRow_Package->FK_Package_Sourcecode_get());
			pRow_Package_Directory->FK_Directory_set(DIRECTORY_Compiled_Output_CONST);
			pRow_Package_Directory->FK_Distro_set(g_iPK_Distro);

			cout << "What is the name?"; 
			string s = StringUtils::GetStringFromConsole();
			pRow_Package_Directory->Path_set(s);
			if( !g_bDontTouchDB )
				g_pDatabase_pluto_main->Package_Directory_get()->Commit();
			vectRow_Package_Directory.push_back(pRow_Package_Directory);
		}
	}


	for(size_t s=0;s<vectRow_Package_Directory.size();++s)
	{
		Row_Package_Directory *pRow_Package_Directory = vectRow_Package_Directory[s];
		if( (!pRow_Package_Directory->FK_Distro_isNull() && pRow_Package_Directory->FK_Distro_get()!=g_pRow_Distro->PK_Distro_get()) ||
				(!pRow_Package_Directory->FK_OperatingSystem_isNull() && pRow_Package_Directory->FK_OperatingSystem_get()!=g_pRow_Distro->FK_OperatingSystem_get()) )
			continue;

		string sCompiledOutput = pRow_Package_Directory->Path_get();
		string sSource;
		if( pRow_Package_Directory->InputPath_get()!="" )
			sSource=pRow_Package_Directory->InputPath_get();
		else
		{
			cout << "***WARNING*** There is no separate input directory.  Using the output directory.  This isn't normal" << endl;
			sSource = sCompiledOutput;
		}
		
		cout << "\tChecking directory: " << pRow_Package_Directory->Path_get() << endl;
		cout << "\t------------------------" << endl;

		// We now the source code is in pRow_Package_Directory and the binary goes in pRow_Package_Directory_Binary
		if( pRow_Package_Directory->Path_get().length()==0 )
		{
			cout << "Error: The directory for PackageDirectory: " << pRow_Package_Directory->PK_Package_Directory_get() << " is empty" << endl;
			return false;
		}

		// If the directory starts with a /, it is considered absolute.  Otherwise it's relative
		string sSourceDirectory = g_sSourcecodePrefix;
		if( sSource[0]=='/' )
			sSourceDirectory = sSource;
		else
			sSourceDirectory += "/" + sSource;

		if( !FileUtils::DirExists(sSourceDirectory) )
		{
			cout << "Error: The directory " << sSourceDirectory << " does not exist" << endl;
			return false;
		}

       if( sCompiledOutput[0]!='/' )
		   sCompiledOutput = g_sSourcecodePrefix + sCompiledOutput;

	   if( !FileUtils::DirExists(sCompiledOutput) )
		{
			cout << "Warning: Compiled Output directory: " << sCompiledOutput << " doesn't exist" << endl;
#ifndef WIN32
            system(("mkdir -p " + sCompiledOutput).c_str());
#endif			
		}
	   

		
		if( !g_bSimulate )
		{
			chdir(sSourceDirectory.c_str());
		}
		fstr_compile << "cd " << sSourceDirectory << endl;

		// Find the make command for pRow_Package_Source_SVN in the _Compat table.  First look for a match on this distro
		Row_Package_Source_Compat *pRow_Package_Source_Compat=NULL;

		vector<Row_Package_Source_Compat *> vectRow_Package_Source_Compat;
		g_pDatabase_pluto_main->Package_Source_Compat_get()->GetRows(
				" FK_Package_Source=" + StringUtils::itos(pRow_Package_Source_SVN->PK_Package_Source_get()) + 
				" AND (FK_Distro IS NULL OR FK_Distro=" + StringUtils::itos(g_pRow_Distro->PK_Distro_get()) + ")" + 
				" AND (FK_OperatingSystem IS NULL OR FK_OperatingSystem=" + StringUtils::itos(g_pRow_Distro->FK_OperatingSystem_get()) + ")", &vectRow_Package_Source_Compat);

		cout << "NUMBER OF COMPATS: " << StringUtils::itos(vectRow_Package_Source_Compat.size()) << endl;

		if( vectRow_Package_Source_Compat.size()==0 )
		{
			if( !g_bInteractive )
			{
				cout << "***WARNING*** Source code is not compatible with this distro" << endl;
				return true; // It's okay
			}

			cout << "I can't find a corresponding row in Package_Source_Compat." << endl
				<< "This is normal if this package cannot be built with this distro." << endl;
				
			if( g_bSupressPrompts || !AskYNQuestion("Add a record Package_Source_Compat and try to build it?",false) )
				return true;

			pRow_Package_Source_Compat = g_pDatabase_pluto_main->Package_Source_Compat_get()->AddRow();
			pRow_Package_Source_Compat->FK_Package_Source_set(pRow_Package_Source_SVN->PK_Package_Source_get());
			pRow_Package_Source_Compat->FK_Distro_set( g_pRow_Distro->PK_Distro_get() );
			if( !g_bDontTouchDB )
				g_pDatabase_pluto_main->Package_Source_Compat_get()->Commit();
		}
		else
			pRow_Package_Source_Compat = vectRow_Package_Source_Compat[0];

		// See what files this should be outputing
		vector<Row_Package_Directory_File *> vectRow_Package_Directory_File;
		g_pDatabase_pluto_main->Package_Directory_File_get()->GetRows(
			"FK_Package_Directory=" + StringUtils::itos(pRow_Package_Directory->PK_Package_Directory_get()) +
			" AND (FK_OperatingSystem IS NULL OR FK_OperatingSystem=" + StringUtils::itos(g_pRow_Distro->FK_OperatingSystem_get()) +") " +
			" AND (FK_Distro IS NULL OR FK_Distro=" + StringUtils::itos(g_pRow_Distro->PK_Distro_get()) +")",
			&vectRow_Package_Directory_File);

		if( vectRow_Package_Directory_File.size()==0 )
		{
			if( !g_bInteractive )
			{
				cout << "Nothing to build in this directory" << endl;
				continue; // We just assume there is nothing to build here
			}

			cout << "No output file is specified.  You need one to compile this directory." << endl;
			if( g_bSupressPrompts || !AskYNQuestion("Specify one?",false) )
				continue;

			cout << "What is the file?" << endl;
			string s = StringUtils::GetStringFromConsole();
			Row_Package_Directory_File *pRow_Package_Directory_File = g_pDatabase_pluto_main->Package_Directory_File_get()->AddRow();
			pRow_Package_Directory_File->FK_Package_Directory_set(pRow_Package_Directory->PK_Package_Directory_get());
			pRow_Package_Directory_File->FK_Distro_set(g_pRow_Distro->PK_Distro_get());
			pRow_Package_Directory_File->File_set(s);
			if( !g_bDontTouchDB )
				g_pDatabase_pluto_main->Package_Directory_File_get()->Commit();
			vectRow_Package_Directory_File.push_back(pRow_Package_Directory_File);
		}

		// First remove any output files that we should be building from source
		for(size_t s=0;s<vectRow_Package_Directory_File.size();++s)
		{
			Row_Package_Directory_File *pRow_Package_Directory_File = vectRow_Package_Directory_File[s];
			remove(pRow_Package_Directory_File->File_get().c_str());
		}

		// Now we've got to run the make file
		for(size_t s=0;s<vectRow_Package_Directory_File.size();++s)
		{
			Row_Package_Directory_File *pRow_Package_Directory_File = vectRow_Package_Directory_File[s];
			if( (!pRow_Package_Directory_File->FK_Distro_isNull() && pRow_Package_Directory_File->FK_Distro_get()!=g_pRow_Distro->PK_Distro_get()) ||
					(!pRow_Package_Directory_File->FK_OperatingSystem_isNull() && pRow_Package_Directory_File->FK_OperatingSystem_get()!=g_pRow_Distro->FK_OperatingSystem_get()) )
				continue;

			string tmp = pRow_Package_Directory_File->MakeCommand_get(); 
			if( g_bInteractive )
			{
				cout << "About to execute make: " << tmp << endl 
					<< "In directory: " << sSourceDirectory << endl;
				if( !AskYNQuestion("Execute command?",false) )
					return false;
			}

			tmp = MakeCommandMods(tmp); 
			fstr_compile << tmp << endl; 
			cout << "Package: " << pRow_Package->FK_Package_Sourcecode_get() << " Executing: " << tmp << endl; 
			if( g_bOnlyCompileIfNotFound && FileUtils::FileExists(sCompiledOutput + "/" + pRow_Package_Directory_File->File_get()) )
				cout << "Skipping compilation of package: " << pRow_Package->FK_Package_Sourcecode_get() << endl;
			else if( !g_bSimulate && system(tmp.c_str()) ) 
			{
				cout << tmp << " ***FAILED***" << endl; 
				cout << "Error: " << tmp << " failed!" << endl; 
				if( g_bSupressPrompts || !AskYNQuestion("Continue anyway?",false) )
					return false;
			}
			cout << tmp << " succeeded" << endl; 

			if( !FileUtils::FileExists(sCompiledOutput + "/" + pRow_Package_Directory_File->File_get()) ) 
			{
				cout << "***ERROR*** The file: " << sCompiledOutput << "/" << pRow_Package_Directory_File->File_get() << " was not created.";
				if( g_bSupressPrompts || !AskYNQuestion("Continue anyway?",false) )
					return false;
			}
			cout << sCompiledOutput << "/" << pRow_Package_Directory_File->File_get() << " exists" << endl;
		}

		// We're done with this directory!!!
	}
	g_mapPackagesToBuild[pRow_Package->PK_Package_get()]=true;
	g_mapPackagesToBuild[pRow_Package->FK_Package_Sourcecode_getrow()->PK_Package_get()]=true;
	return true;
}

bool CreateSource_SourceForgeCVS(Row_Package_Source *pRow_Package_Source,list<FileInfo *> &listFileInfo)
{
	// 1.	Create a temporary directory
	// 2.   chdir to the directory and do a cvs co .
	// 3.   copy the files over one at a time
	// 4.	Do a cvs add for each sub-directory
	// 5.   Do a cvs add for each file
	// 6.   Call FileUtils::GetDirectory(vector<strings)), and see what files are there, that are not in listFileInfo and do a delete
	// 7.   Do a cvs ci
	string MyPath, WorkPath;
	FILE *fp;
	string cmd, cmd2, basepath;
	bool flag;
	list<FileInfo *>::iterator iFileInfo;
	string::size_type pos;
	string::size_type length;
	list <string> MyList;
	list <string>::iterator iMyList;
	char direct[255];

	fp = fopen("flist.tmp","wt");
	for (iFileInfo = listFileInfo.begin();iFileInfo != listFileInfo.end(); iFileInfo++)
	{
		FileInfo *pFileInfo = (*iFileInfo);
		cmd = FileUtils::BasePath(pFileInfo->m_sSource);
		basepath = cmd;
		cmd2 = FileUtils::FilenameWithoutPath(pFileInfo->m_sSource);
		fprintf(fp,"%s/%s\n",cmd.c_str(),cmd2.c_str());
	}
	fclose(fp);
	fp = fopen("npkg.tmp","wt");
	cmd = pRow_Package_Source->Name_get();
	fprintf(fp,"%s\n",cmd.c_str());
	fprintf(fp,"%s\n",basepath.c_str());
	fclose(fp);
	system("/usr/sbin/sync-SF.pl");
return true;
//------------------------------------------------------------------------------------

	cout << "\nCreting temporary directory\n";
	MyPath = "cvs_temp";
	//Building Temporary Directory
#ifdef WIN32
	mkdir(MyPath.c_str());
#else
	mkdir(MyPath.c_str(), 0777);
#endif
	//ChDir to Temporary Directory
	chdir(MyPath.c_str());

cout << "Making CVS Checkout to temporary\n";
	//Checking Version From SourceForge
	cmd = " cvs -d:ext:plutoinc@cvs.sourceforge.net:/cvsroot/ checkout " + pRow_Package_Source->Name_get();
	system(cmd.c_str());

	cmd = pRow_Package_Source->Name_get();
	chdir(cmd.c_str());

cout << "Reading files from temporary ";
	getcwd(direct, 255);
	FileUtils::FindFiles(MyList, direct, "*.*", true, "");
cout << " [Done]\n";

	//reading actual directory list
//	cout<<"\n\n SourceForgeCVS : "<<pRow_Package_Source->FK_Package_getrow()->Description_get();
//	cout<<"\n Nr of files : "<<listFileInfo.size();
	
cout << "Copying Files\n";
/////////////-------------------- We copy the files from the project to the temporary
	flag = false;

	for (iFileInfo = listFileInfo.begin();iFileInfo != listFileInfo.end(); iFileInfo++)
	{
		FileInfo *pFileInfo = (*iFileInfo);
		for (iMyList = MyList.begin();iMyList != MyList.end(); iMyList++)
		{
			cmd = FileUtils::BasePath(pFileInfo->m_sSource);
			pos = cmd.rfind("/");
			length = cmd.length();
			cmd = cmd.substr(pos+1,length-pos-1);
			if(cmd.compare(pRow_Package_Source->Name_get().c_str()) == 0) {
				cmd = FileUtils::FilenameWithoutPath(pFileInfo->m_sSource);
			} else {
				cmd = cmd + "/" + FileUtils::FilenameWithoutPath(pFileInfo->m_sSource);
			}
			
			cmd2 = FileUtils::BasePath(*iMyList);
			pos = cmd2.rfind("/");
			length = cmd2.length();
			cmd2 = cmd2.substr(pos+1,length-pos-1);
			if(cmd2 != "") {
				cmd2 = cmd2 + "/" + FileUtils::FilenameWithoutPath(*iMyList);
			} else {
				cmd2 = FileUtils::FilenameWithoutPath(*iMyList);
			}

			if(cmd.compare (cmd2) == 0) {
				//if the file exist we overwrite it
				cout << "Updated File Detected <-\n";
				cmd = FileUtils::BasePath(pFileInfo->m_sSource);
				pos = cmd.rfind("/");
				length = cmd.length();
				cmd = cmd.substr(pos+1,length-pos-1);
				if(cmd.compare(pRow_Package_Source->Name_get().c_str()) == 0) {
					cmd = "";
				}
				if(FileUtils::DirExists(cmd) != true) {
					if(cmd == "") {
						cmd = "cp -f " + pFileInfo->m_sSource + " " + FileUtils::FilenameWithoutPath(pFileInfo->m_sSource);
						cmd2 = "cvs add " + FileUtils::FilenameWithoutPath(pFileInfo->m_sSource);
					} else {
						cmd = "cp -f " + pFileInfo->m_sSource + " " + cmd + "/" + FileUtils::FilenameWithoutPath(pFileInfo->m_sSource);
						cmd2 = "cvs add " + cmd + "/" + FileUtils::FilenameWithoutPath(pFileInfo->m_sSource);
					}
					system(cmd.c_str());
					system(cmd2.c_str());
				}
				flag = true;
				break;
			}
		}
		if(flag != true) {
				//if it is a new file we copy it
				cout << "New file detected <-\n";
				cmd=FileUtils::BasePath(pFileInfo->m_sSource);
				string::size_type pos = cmd.rfind("/");
				string::size_type marime = cmd.length();
				cmd = cmd.substr(pos+1,marime-pos-1);
				//if we are on the root dir we just copy it as a file
				if(cmd.compare(pRow_Package_Source->Name_get()) == 0)
				{
					cmd = "cp -f " + pFileInfo->m_sSource + " " + FileUtils::FilenameWithoutPath(pFileInfo->m_sSource);
					system(cmd.c_str());
					cmd = "cvs add " + FileUtils::FilenameWithoutPath(pFileInfo->m_sSource);
					system(cmd.c_str());
				}
				else {
					//if we are not in ther root
					if(FileUtils::DirExists(cmd) != true) {
#ifdef WIN32
						mkdir(cmd.c_str());
#else
						mkdir(cmd.c_str(), 0777);
#endif
						cmd2 = "cvs add " + cmd;
						system(cmd2.c_str());
						cmd2 = "cp " + pFileInfo->m_sSource + " " + cmd + "/" + FileUtils::FilenameWithoutPath(pFileInfo->m_sSource);
						system(cmd2.c_str());
						cmd2 = "cvs add " + cmd + "/" + FileUtils::FilenameWithoutPath(pFileInfo->m_sSource);
						system(cmd2.c_str());
					}
					else {
						cmd2 = "cp -f " + pFileInfo->m_sSource + " " + cmd + "/" + FileUtils::FilenameWithoutPath(pFileInfo->m_sSource);
						system(cmd2.c_str());
						cmd2 = "cvs add " +	cmd + "/" + FileUtils::FilenameWithoutPath(pFileInfo->m_sSource);
						system(cmd2.c_str());
						cout << cmd2.c_str() << endl;
					}
				}
		}
		flag = false;
	}
///////////////////////////////////////////////////////////
////////---------------- Finding old files and delete them
	flag = false;
	cout << "Removing older files\n";
	for (iMyList = MyList.begin();iMyList != MyList.end(); iMyList++)
	{
		cmd2 = FileUtils::BasePath(*iMyList);
		pos = cmd2.rfind("/");
		length = cmd2.length();
		cmd2 = cmd2.substr(pos+1,length-pos-1);
		if(cmd2 == "") {
			cmd2 = FileUtils::FilenameWithoutPath(*iMyList);
		} else if(cmd2 == "CVS" || cmd2 == "CVSROOT") {
			cmd2 = "";
		} else {
			cmd2 = cmd2 + "/" + FileUtils::FilenameWithoutPath(*iMyList);
		}
		for (iFileInfo = listFileInfo.begin();iFileInfo != listFileInfo.end(); iFileInfo++)
		{
			FileInfo *pFileInfo = (*iFileInfo);

			cmd = FileUtils::BasePath(pFileInfo->m_sSource);
			pos = cmd.rfind("/");
			length = cmd.length();
			cmd = cmd.substr(pos+1,length-pos-1);
			if(cmd.compare(pRow_Package_Source->Name_get()) == 0) {
				cmd = FileUtils::FilenameWithoutPath(pFileInfo->m_sSource);
			} else {
				cmd = cmd + "/" + FileUtils::FilenameWithoutPath(pFileInfo->m_sSource);
			}
			
			if(cmd.compare (cmd2) == 0) {
				flag = true;
				break;
			}
		}
		if (flag != true) {
			if(cmd2 != "") {
				cmd = "rm -r -f " + cmd2;
				system(cmd.c_str());
				cout << cmd << endl;
				cmd = "cvs remove " + cmd2;
				system(cmd.c_str());
				cout << cmd << endl;
			}
		}
		flag = false;
	}

	cout<<"\n\nMaking commit ";
	//Updating files from the server to the sourceforge
	cmd = "cvs -d:ext:plutoinc@cvs.sourceforge.net:/cvsroot/ commit -m 'Automatic Update'";
	system(cmd.c_str());
	cout << "[Done]\n";

	//at the end we delete the temporary directory
	cout << "Clearing Temporary ";
	cmd = "../../";
	chdir(cmd.c_str());
	cmd = "rm cvs_temp -r";
	system(cmd.c_str());
	cout << "[Done]\n";
	return true;
}

/* CreateSource_Pluto<type> Create specified type of package using given files */

// list<FileInfo *> listFileInfo: files that go into the package
// assumptions for the Pluto Debian repository:
// - the .deb file will go in dists/<pluto2 (repository name - var in package source)>/
//   intended to be run from a directory with this dists directory under it
// - the target destination will be /usr/pluto/<relative path>

// listFileInfo is input not output!!! (we should get used to using const when a reference is for input)


bool CreateSource_PlutoUbuntu(Row_Package_Source *pRow_Package_Source,list<FileInfo *> &listFileInfo)
{
	string Version("");
	if (!pRow_Package_Source->Version_isNull())
	{
		Version = pRow_Package_Source->Version_get();
	}
	if (Version == "")
	{
		Version = "2.0.0.0";
	}
	
	string Package_Name = StringUtils::ToLower(pRow_Package_Source->Name_get());
	string Dir(g_sOutputPath + "/tmp/" + Package_Name + "-" + Version);
	string Prefix("/usr/");
	if (pRow_Package_Source->FK_Package_getrow()->IsSource_get())
	{
		Prefix = "/usr/pluto/sources/";
	}


#ifndef WIN32
	FILE *f;
	system(("rm -rf " + Dir).c_str());
	system(("mkdir -p " + Dir + "/root").c_str());
	char CurrentDir[1024];
	getcwd(CurrentDir, 1024);
	chdir(Dir.c_str());
	cout << "chdir " << Dir << endl;

string Makefile = "none:\n"
"\t\n"
"\n"
"clean:\n"
"\t\n"
"\n"
"install:\n"
"\tcp -a root/* $(DESTDIR) || true\n"; 		// */

	f = fopen((Dir + "/Makefile").c_str(), "w");
	if (!f)
	{
		cout << "Error: cannot open Makefile:" << Dir << "/Makefile" << endl;
		return false;
	}
	fprintf(f, "%s", Makefile.c_str());
	fclose(f);
	system("echo | DEBFULLNAME='LinuxMCE Developers' dh_make -c gpl -s -n -e 'developers@linuxmce.org'");
#endif

	list<FileInfo *>::iterator iFileInfo;
	for (iFileInfo = listFileInfo.begin(); iFileInfo != listFileInfo.end(); iFileInfo++)
	{
		string sSource = (*iFileInfo)->m_sSource;

		// TODO: Error checks
		if (sSource.find("mkr_postinst") != string::npos) // NOTE: not right; should check at begining of file name
		{
		}
		else if (sSource.find("mkr_preinst") != string::npos) // NOTE: not right; should check at begining of file name
		{
		}
		else if (sSource.find("mkr_prerm") != string::npos) // NOTE: not right; should check at begining of file name
		{
		}
		else if (sSource.find("mkr_postrm") != string::npos) // NOTE: not right; should check at begining of file name
		{
		}
		else
		{
			// no prefix for absolute paths (the ones that start with a slash)
			string thePrefix(Prefix);
			if ((*iFileInfo)->m_sDestination[0] == '/')
				thePrefix = "";
		
			string sDestination;
			if ((*iFileInfo)->m_sDestination.find("/debian") != string::npos) {
				sSource = (*iFileInfo)->m_sSource;
				if (sSource.find("init.d") != string::npos) {
					sDestination = Dir + "/root/etc/init.d/" + Package_Name;
				} else {
					sDestination = Dir + thePrefix + (*iFileInfo)->m_sDestination;
				}
			} else {
				sDestination = Dir + "/root/" + thePrefix + (*iFileInfo)->m_sDestination;
			}
		
		
			if( g_pDBHelper_pluto_builder )
			{
				Row_Package *pRow_Package_SourceCode = pRow_Package_Source->FK_Package_getrow()->FK_Package_Sourcecode_getrow();
				string sSQL = "INSERT INTO File(Filename,FK_Package,FK_Package_Source) VALUES('" + StringUtils::SQLEscape(sSource) + "',"
					+ StringUtils::itos(pRow_Package_Source->FK_Package_get()) + "," + (pRow_Package_SourceCode ? StringUtils::itos(pRow_Package_SourceCode->PK_Package_get()) : "NULL") + ")";
				g_pDBHelper_pluto_builder->threaded_db_wrapper_query(sSQL);
			}
			cout << "COPY: " << sSource << " --> " + sDestination << endl;

			if( !g_bSimulate )
				CopySourceFile(sSource, sDestination);
		}
	}

	// Get a list of all the other packages which we depend on, and which have Debian sources.  We are going to add them to the .deb as dependencies
	vector<Row_Package_Source *> vect_pRow_Package_Source_Dependencies;
	pRow_Package_Source->Table_Package_Source_get()->GetRows("JOIN Package_Package ON Package_Package.FK_Package_DependsOn=Package_Source.FK_Package AND Package_Package.FK_Package=" + 
		StringUtils::itos(pRow_Package_Source->FK_Package_get()) + " WHERE FK_RepositorySource IN ("
		+ StringUtils::itos(REPOSITORYSOURCE_Ubuntu_CONST)
		+ "," + StringUtils::itos(REPOSITORYSOURCE_Ubuntu_Pluto_Addons_CONST)
		+ "," + StringUtils::itos(REPOSITORYSOURCE_Debian_CONST)
		+ "," + StringUtils::itos(REPOSITORYSOURCE_MythTV_CONST)
		+ "," + StringUtils::itos(REPOSITORYSOURCE_Ubuntu_Mirrors_CONST)
		+ "," + StringUtils::itos(REPOSITORYSOURCE_Ubuntu_Security_CONST)
		+ "," + StringUtils::itos(REPOSITORYSOURCE_Ubuntu_LinuxMCE_Addons_CONST)
		+ "," + StringUtils::itos(REPOSITORYSOURCE_Slimdevices_CONST)
		+ ")",
		&vect_pRow_Package_Source_Dependencies);

	string sDepends,sPreDepends;

//	if( pRow_Package_Source->FK_Package_getrow()->FK_Manufacturer_get()==1
//			&& !isDriverPackage(pRow_Package_Source->FK_Package_get()) ) // Pluto && ! kernel_module
//	{
//	}
	for (size_t s=0;s<vect_pRow_Package_Source_Dependencies.size();++s)
	{
		Row_Package_Source *pRow_Package_Source_Dependency = vect_pRow_Package_Source_Dependencies[s];
		Row_Package_Package *pRow_Package_Package = g_pDatabase_pluto_main->Package_Package_get()->GetRow(pRow_Package_Source->FK_Package_get(),pRow_Package_Source_Dependency->FK_Package_get());
		string sPkgName = pRow_Package_Source_Dependency->Name_get();
		string sPkgVersion = pRow_Package_Source_Dependency->Version_get();
		int count = 0;
		for (size_t i = 0; i < sPkgVersion.length(); i++)
		{
			if (sPkgVersion[i] == '.')
				count++;
		}

		string sPkgVerBase = sPkgVersion;
		if (count == 4)
		{
			string::size_type iLastDot = sPkgVersion.rfind(".", sPkgVersion.length());
			sPkgVerBase = iLastDot == string::npos ? "" : sPkgVersion.substr(0, iLastDot);
		}
		int iPkgManufacturer = pRow_Package_Source_Dependency->FK_Package_getrow()->FK_Manufacturer_get();
		if (iPkgManufacturer == 1 || iPkgManufacturer == 1108 || iPkgManufacturer == 1176 ) /* HARDCODED: 1 = Pluto, 1108 = Pluto Closed Source, 1176 = LinuxMCE MakeRelease */
		{
			if( pRow_Package_Package->PreDependency_get()==1 )
				sPreDepends += ", " + sPkgName;
			else
				sDepends += ", " + sPkgName;
			if( sPkgVerBase != "" )
			{
				string::size_type iLastDotNext = sPkgVerBase.rfind(".", sPkgVerBase.length());
				string sPkgVerBaseMajor = sPkgVerBase.substr(0, iLastDotNext + 1);
				string sPkgVerBaseMinor = sPkgVerBase.substr(iLastDotNext + 1);
				int iPkgVerBaseNext = atoi(sPkgVerBaseMinor.c_str()) + 1;
				if( pRow_Package_Package->PreDependency_get()==1 )
					sPreDepends += string("")+ " (>= " + sPkgVerBase + ")" + ", " + sPkgName + " (<< " + sPkgVerBaseMajor + StringUtils::itos(iPkgVerBaseNext) + ")";
				else
					sDepends += string("")+ " (>= " + sPkgVerBase + ")" + ", " + sPkgName + " (<< " + sPkgVerBaseMajor + StringUtils::itos(iPkgVerBaseNext) + ")";
			}
		}
		else
			sPreDepends += ", " + sPkgName;
		g_DebianPackages[sPkgName] = true;
	}
	g_DebianPackages[Package_Name] = true;

	if (sPreDepends != "")
		sDepends += ", " + sPreDepends;
	sPreDepends = "";
	cout << "Depends list: " << sDepends << endl;
	cout << "PreDepends list: " << sPreDepends << endl;
	cout << "Replaces: " << pRow_Package_Source->Replaces_get() << endl;  // This is a comma-delimited list
	cout << "Conflicts: " << pRow_Package_Source->Conflicts_get() << endl;  // This is a comma-delimited list
	cout << "Provides: " << pRow_Package_Source->Provides_get() << endl;  // This is a comma-delimited list

#ifndef WIN32
	string sed_predepends = "";
	if (sPreDepends != "")
	{
		sed_predepends = "\\nPre-Depends: ${shlibs:Depends}, ${misc:Depends}" + sPreDepends;
	}
	string sed_cmd = "s/^Depends:.*$/Depends: ${shlibs:Depends}, ${misc:Depends}" + sDepends + sed_predepends + "/";
	string replaces = pRow_Package_Source->Replaces_get();
	string conflicts = pRow_Package_Source->Conflicts_get();
	string provides = pRow_Package_Source->Provides_get();
	if (replaces != "")
	{
		sed_cmd += "; /^Description: / { x; s/^.*$/Replaces: " + replaces + "/; p; x; }";
	}
	if (conflicts != "")
	{
		sed_cmd += "; /^Description: / { x; s/^.*$/Conflicts: " + conflicts + "/; p; x; }";
	}
	if (provides != "")
	{
		sed_cmd += "; /^Description: / { x; s/^.*$/Provides: " + provides + "/; p; x; }";
	}
	string cmd = string("sed -i '" + sed_cmd + "' " + Dir + "/debian/control");
	cout << cmd << endl;
	system(cmd.c_str());

//	// handle dh_strip - the old dh_make way
//	if (isStrippablePackage(pRow_Package_Source->FK_Package_get()))
//		sed_cmd = "s/^.*dh_strip.*$/\tdh_strip/g";
//	else
//		sed_cmd = "s/^.*dh_strip.*$/\t# dh_strip/g";
//	cmd = string("sed -i '" + sed_cmd + "' " + Dir + "/debian/rules");
//	cout << cmd << endl;
//	system(cmd.c_str());

	// handle dh_strip - the new dh_make way
	if (!isStrippablePackage(pRow_Package_Source->FK_Package_get()))
	{
		sed_cmd = "$ a \\\n\\noverride_dh_strip:\\n";
		cmd = string("sed -i '" + sed_cmd + "' " + Dir + "/debian/rules");
		cout << cmd << endl;
		system(cmd.c_str());
	}

	cout << string(("dpkg-buildpackage -b -rfakeroot -us -uc")) << endl;
	if (!g_bSimulate)
	{
		// in C++, the headers do a "*(int *) &(status)" - where C uses the "status" parameter verbatim
		int status = system("dpkg-buildpackage -b -rfakeroot -us -uc");
		printf("dpkg-buildpackage returned %d\n", WEXITSTATUS(status));
		if (WEXITSTATUS(status))
		{
			cout << "dpkg-buildpackage -b failed.  Aborting." << endl;
			return false;
		}
	}
#endif
	
#ifndef WIN32
	chdir(CurrentDir);
	cout << "chdir " << Dir << endl;
	if( !g_bSimulate )
		system(("rm -rf " + Dir).c_str());
#endif
	
	return true;
}

bool CreateSource_PlutoDebian(Row_Package_Source *pRow_Package_Source,list<FileInfo *> &listFileInfo)
{
	string Version("");
	if (!pRow_Package_Source->Version_isNull())
	{
		Version = pRow_Package_Source->Version_get();
	}
	if (Version == "")
	{
		Version = "2.0.0.0";
	}

/*	string VCopy(Version);
	string sV[4];
	string::size_type pos = 0;
	for (int i = 0; i < 4; i++)
	{
		sV[i] = StringUtils::Tokenize(VCopy, ".", pos);
	}
	int x;
	sscanf(sV[3].c_str(), "%d", &x);
	Version = sV[0] + "." + sV[1] + "." + sV[2] + "." + StringUtils::itos(x + 1);
	pRow_Package_Source->Version_set(Version);
*/	
	string Package_Name = StringUtils::ToLower(pRow_Package_Source->Name_get());
	string Dir(g_sOutputPath + "/tmp/" + Package_Name + "-" + Version);
	string Prefix("/usr/");
	if (pRow_Package_Source->FK_Package_getrow()->IsSource_get())
	{
		Prefix = "/usr/pluto/sources/";
	}

//	FILE * f;

#ifndef WIN32
	FILE *f;
	system(("rm -rf " + Dir).c_str());
	system(("mkdir -p " + Dir + "/root").c_str());
	char CurrentDir[1024];
	getcwd(CurrentDir, 1024);
	chdir(Dir.c_str());
	cout << "chdir " << Dir << endl;

string Makefile = "none:\n"
"\t\n"
"\n"
"clean:\n"
"\t\n"
"\n"
"install:\n"
"\tcp -a root/* $(DESTDIR) || true\n";		// */

	f = fopen((Dir + "/Makefile").c_str(), "w");
	if (!f)
	{
		cout << "Error: cannot open Makefile:" << Dir << "/Makefile" << endl;
		return false;
	}
	fprintf(f, "%s", Makefile.c_str());
	fclose(f);
	system("echo | DEBFULLNAME='LinuxMCE Developers' dh_make -c gpl -s -n -e 'developers@linuxmce.org'");
//	mkdir("DEBIAN", 0666);
#endif

	list<FileInfo *>::iterator iFileInfo;
	for (iFileInfo = listFileInfo.begin(); iFileInfo != listFileInfo.end(); iFileInfo++)
	{
		string sSource = (*iFileInfo)->m_sSource;

		// TODO: Error checks
		if (sSource.find("mkr_postinst") != string::npos) // NOTE: not right; should check at begining of file name
		{
#ifndef WIN32
			cout << "POSTINST: " << sSource << endl;
			// put mkr_postinst scripts into control section instead of package contents
			FILE * g;
			f = fopen("debian/postinst.ex", "r");
			g = fopen("debian/postinst", "w");
			fchmod(fileno(g), 0644);
			fprintf(g,"#!/bin/bash\n");
			char buffer[1024];
			while (fgets(buffer, 1024, f))
			{
				fprintf(g, "%s", buffer);
				if (strstr(buffer, "configure)") != NULL && strstr(buffer, "deconfigure)") == NULL)
				{
					fprintf(g, "%s", ("# " + sSource + "\n").c_str());
					FILE * g2 = fopen(sSource.c_str(), "r");

					if(NULL == g2)
					{
						cout << "Aborting! Can't open file: " << sSource << endl;
						exit(1);
					}

					while (fgets(buffer, 1024, g2))
					{
						fprintf(g, "%s", buffer);
					}
					fclose(g2);
				}
			}
			fclose(g);
			fclose(f);
			FileUtils::PUCopyFile("debian/postinst", "debian/postinst.ex");
#endif
		}
		else if (sSource.find("mkr_preinst") != string::npos) // NOTE: not right; should check at begining of file name
		{
#ifndef WIN32
			cout << "PREINST: " << sSource << endl;
			// put mkr_preinst scripts into control section instead of package contents
			FILE * g;
			f = fopen("debian/preinst.ex", "r");
			g = fopen("debian/preinst", "w");
			fchmod(fileno(g), 0644);
			fprintf(g,"#!/bin/bash\n");
			char buffer[1024];
			while (fgets(buffer, 1024, f))
			{
				fprintf(g, "%s", buffer);
				if (strstr(buffer, "install|upgrade)") != NULL)
				{
					fprintf(g, "%s", ("# " + sSource + "\n").c_str());
					FILE * g2 = fopen(sSource.c_str(), "r");
					while (fgets(buffer, 1024, g2))
					{
						fprintf(g, "%s", buffer);
					}
					fclose(g2);
				}
			}
			fclose(g);
			fclose(f);
			FileUtils::PUCopyFile("debian/preinst", "debian/preinst.ex");
#endif
		}
		else if (sSource.find("mkr_prerm") != string::npos) // NOTE: not right; should check at begining of file name
		{
#ifndef WIN32
			cout << "PRERM: " << sSource << endl;
			// put mkr_preinst scripts into control section instead of package contents
			FILE * g;
			f = fopen("debian/prerm.ex", "r");
			g = fopen("debian/prerm", "w");
			fchmod(fileno(g), 0644);
			fprintf(g,"#!/bin/bash\n");
			char buffer[1024];
			while (fgets(buffer, 1024, f))
			{
				fprintf(g, "%s", buffer);
				if (strstr(buffer, "remove|upgrade|deconfigure)") != NULL)
				{
					fprintf(g, "%s", ("# " + sSource + "\n").c_str());
					FILE * g2 = fopen(sSource.c_str(), "r");
					while (fgets(buffer, 1024, g2))
					{
						fprintf(g, "%s", buffer);
					}
					fclose(g2);
				}
			}
			fclose(g);
			fclose(f);
			FileUtils::PUCopyFile("debian/prerm", "debian/prerm.ex");
#endif
		}
		else if (sSource.find("mkr_postrm") != string::npos) // NOTE: not right; should check at begining of file name
		{
#ifndef WIN32
			cout << "POSTRM: " << sSource << endl;
			// put mkr_preinst scripts into control section instead of package contents
			FILE * g;
			f = fopen("debian/postrm.ex", "r");
			g = fopen("debian/postrm", "w");
			fchmod(fileno(g), 0644);
			fprintf(g,"#!/bin/bash\n");
			char buffer[1024];
			while (fgets(buffer, 1024, f))
			{
				fprintf(g, "%s", buffer);
				if (strstr(buffer, "purge|remove|upgrade|failed-upgrade|abort-install|abort-upgrade|disappear)") != NULL)
				{
					fprintf(g, "%s", ("# " + sSource + "\n").c_str());
					FILE * g2 = fopen(sSource.c_str(), "r");
					while (fgets(buffer, 1024, g2))
					{
						fprintf(g, "%s", buffer);
					}
					fclose(g2);
				}
			}
			fclose(g);
			fclose(f);
			FileUtils::PUCopyFile("debian/postrm", "debian/postrm.ex");
#endif
		}
		else
		{
			// no prefix for absolute paths (the ones that start with a slash)
			string thePrefix(Prefix);
			if ((*iFileInfo)->m_sDestination[0] == '/')
				thePrefix = "";
	
			if ((*iFileInfo)->m_sDestination.find("/debian") != string::npos) { continue; }

			string sDestination = Dir + "/root/" + thePrefix + (*iFileInfo)->m_sDestination;
		
			if( g_pDBHelper_pluto_builder )
			{
				Row_Package *pRow_Package_SourceCode = pRow_Package_Source->FK_Package_getrow()->FK_Package_Sourcecode_getrow();
				string sSQL = "INSERT INTO File(Filename,FK_Package,FK_Package_Source) VALUES('" + StringUtils::SQLEscape(sSource) + "',"
					+ StringUtils::itos(pRow_Package_Source->FK_Package_get()) + "," + (pRow_Package_SourceCode ? StringUtils::itos(pRow_Package_SourceCode->PK_Package_get()) : "NULL") + ")";
				g_pDBHelper_pluto_builder->threaded_db_wrapper_query(sSQL);
			}
			cout << "COPY: " << sSource << " --> " + sDestination << endl;
			if( !g_bSimulate )
				CopySourceFile(sSource, sDestination);
		}
	}

	//f = fopen((Dir + "/root/dummy-file").c_str(), "w");
	//if( !f )
	//{
	//	cout << "Error: cannot open dummy-file" << endl;
	//	return false;
	//}
	//fclose(f);

//	// Get a list of all the other packages which we depend on, and which have Debian sources.  We are going to add them to the .deb as dependencies
//	vector<Row_Package_Source *> vect_pRow_Package_Source_Dependencies;
//	if ( pRow_Package_Source->FK_RepositorySource_get() != REPOSITORYSOURCE_Ubuntu_Pluto_Addons_CONST && pRow_Package_Source->FK_RepositorySource_get() != REPOSITORYSOURCE_Ubuntu_LinuxMCE_Addons_CONST ) {
//		pRow_Package_Source->Table_Package_Source_get()->GetRows("JOIN Package_Package ON Package_Package.FK_Package_DependsOn=Package_Source.FK_Package AND Package_Package.FK_Package=" + 
//		StringUtils::itos(pRow_Package_Source->FK_Package_get()) + " WHERE FK_RepositorySource IN ("
//		+ StringUtils::itos(REPOSITORYSOURCE_Ubuntu_CONST)
//		+ "," + StringUtils::itos(REPOSITORYSOURCE_Ubuntu_Pluto_Addons_CONST)
//		+ "," + StringUtils::itos(REPOSITORYSOURCE_Debian_CONST)
//		+ "," + StringUtils::itos(REPOSITORYSOURCE_MythTV_CONST)
//		+ "," + StringUtils::itos(REPOSITORYSOURCE_Ubuntu_Mirrors_CONST)
//		+ "," + StringUtils::itos(REPOSITORYSOURCE_Ubuntu_Security_CONST)
//		+ "," + StringUtils::itos(REPOSITORYSOURCE_Ubuntu_LinuxMCE_Addons_CONST)
//		+ "," + StringUtils::itos(REPOSITORYSOURCE_Slimdevices_CONST)
//		+ ")",
//		&vect_pRow_Package_Source_Dependencies);
//	} else {
//		pRow_Package_Source->Table_Package_Source_get()->GetRows(
//				"JOIN Package_Package ON Package_Package.FK_Package_DependsOn=Package_Source.FK_Package AND Package_Package.FK_Package=" + StringUtils::itos(pRow_Package_Source->FK_Package_get()) +
//				" JOIN Package_Source_Compat ON Package_Source_Compat.FK_Package_Source = Package_Source.PK_Package_Source AND Package_Source_Compat.FK_Distro = " + StringUtils::itos(g_iPK_Distro) +
//				" WHERE FK_RepositorySource IN (" 
//					+ StringUtils::itos(pRow_Package_Source->FK_RepositorySource_get())
//					+ "," + StringUtils::itos(REPOSITORYSOURCE_Ubuntu_CONST)
//					+ "," + StringUtils::itos(REPOSITORYSOURCE_MythTV_CONST)
//					+ "," + StringUtils::itos(REPOSITORYSOURCE_Ubuntu_Mirrors_CONST)
//					+ "," + StringUtils::itos(REPOSITORYSOURCE_Ubuntu_LinuxMCE_Addons_CONST)
//					+ "," + StringUtils::itos(REPOSITORYSOURCE_Slimdevices_CONST)
//					+ ")",
//				&vect_pRow_Package_Source_Dependencies);
//	}

	// Get a list of all the other packages which we depend on, and which are compatible and have compatible 'package' repository sources.  We are going to add them to the .deb as dependencies
	vector<Row_Package_Source *> vect_pRow_Package_Source_Dependencies;
	pRow_Package_Source->Table_Package_Source_get()->GetRows(
		"JOIN Package_Package ON Package_Package.FK_Package_DependsOn=Package_Source.FK_Package AND Package_Package.FK_Package=" + StringUtils::itos(pRow_Package_Source->FK_Package_get()) +
		" JOIN Package_Source_Compat ON Package_Source_Compat.FK_Package_Source = Package_Source.PK_Package_Source JOIN RepositorySource ON RepositorySource.PK_RepositorySource=Package_Source.FK_RepositorySource AND RepositorySource.FK_RepositoryType=" + StringUtils::itos(REPOSITORYTYPE_PACKAGE_CONST)
		+ " JOIN Package_Compat ON Package_Source.FK_Package=Package_Compat.FK_Package WHERE (((Package_Compat.FK_Distro IS NULL OR Package_Compat.FK_Distro = " + StringUtils::itos(g_iPK_Distro)
			+ ") AND (Package_Compat.FK_OperatingSystem IS NULL OR Package_Compat.FK_OperatingSystem = " + StringUtils::itos(g_pRow_Distro->FK_OperatingSystem_get())
			+ ")) AND ((Package_Source_Compat.FK_Distro IS NULL OR Package_Source_Compat.FK_Distro = " + StringUtils::itos(g_iPK_Distro)
			+ ") AND (Package_Source_Compat.FK_OperatingSystem IS NULL OR Package_Source_Compat.FK_OperatingSystem = " + StringUtils::itos(g_pRow_Distro->FK_OperatingSystem_get()) + ")))"
		, &vect_pRow_Package_Source_Dependencies);

	string sDepends,sPreDepends;

//	if( pRow_Package_Source->FK_Package_getrow()->FK_Manufacturer_get()==1
//			&& !isDriverPackage(pRow_Package_Source->FK_Package_get())
//	        && pRow_Package_Source->FK_RepositorySource_get() != REPOSITORYSOURCE_Ubuntu_Pluto_Addons_CONST
//		&& pRow_Package_Source->FK_RepositorySource_get() != REPOSITORYSOURCE_Ubuntu_LinuxMCE_Addons_CONST ) // Pluto && ! kernel_module
//	{
//		sPreDepends = "pluto-kernel-upgrade ";
//	}

	// Interate through all of this package's dependencies for addition to the list
	for (size_t s=0;s<vect_pRow_Package_Source_Dependencies.size();++s)
	{
		Row_Package_Source *pRow_Package_Source_Dependency = vect_pRow_Package_Source_Dependencies[s];
		Row_Package_Package *pRow_Package_Package = g_pDatabase_pluto_main->Package_Package_get()->GetRow(pRow_Package_Source->FK_Package_get(),pRow_Package_Source_Dependency->FK_Package_get());
		string sPkgName = pRow_Package_Source_Dependency->Name_get();
		string sPkgVersion = pRow_Package_Source_Dependency->Version_get();
		int count = 0;
		for (size_t i = 0; i < sPkgVersion.length(); i++)
		{
			if (sPkgVersion[i] == '.')
				count++;
		}

		string sPkgVerBase = sPkgVersion;
		if (count == 4)
		{
			string::size_type iLastDot = sPkgVersion.rfind(".", sPkgVersion.length());
			sPkgVerBase = iLastDot == string::npos ? "" : sPkgVersion.substr(0, iLastDot);
		}
		int iPkgManufacturer = pRow_Package_Source_Dependency->FK_Package_getrow()->FK_Manufacturer_get();
		if (iPkgManufacturer == 1 || iPkgManufacturer == 1108 || iPkgManufacturer == 1176 ) /* HARDCODED: 1 = Pluto, 1108 = Pluto Closed Source, 1176 = LinuxMCE MakeRelease */
		{
			if( pRow_Package_Package->PreDependency_get()==1 )
				sPreDepends += ", " + sPkgName;
			else
				sDepends += ", " + sPkgName;
			if( sPkgVerBase != "" )
			{
				string::size_type iLastDotNext = sPkgVerBase.rfind(".", sPkgVerBase.length());
				string sPkgVerBaseMajor = sPkgVerBase.substr(0, iLastDotNext + 1);
				string sPkgVerBaseMinor = sPkgVerBase.substr(iLastDotNext + 1);
				int iPkgVerBaseNext = atoi(sPkgVerBaseMinor.c_str()) + 1;
				if( pRow_Package_Package->PreDependency_get()==1 )
					sPreDepends += string("")+ " (>= " + sPkgVerBase + ")" + ", " + sPkgName + " (<< " + sPkgVerBaseMajor + StringUtils::itos(iPkgVerBaseNext) + ")";
				else
					sDepends += string("")+ " (>= " + sPkgVerBase + ")" + ", " + sPkgName + " (<< " + sPkgVerBaseMajor + StringUtils::itos(iPkgVerBaseNext) + ")";
			}
		}
		else
			sPreDepends += ", " + sPkgName;
		g_DebianPackages[sPkgName] = true;
	}
	g_DebianPackages[Package_Name] = true;

	if (sPreDepends != "")
		sDepends += ", " + sPreDepends;
	sPreDepends = "";
	cout << "Depends list: " << sDepends << endl;
	cout << "PreDepends list: " << sPreDepends << endl;
	cout << "Replaces: " << pRow_Package_Source->Replaces_get() << endl;  // This is a comma-delimited list
	cout << "Conflicts: " << pRow_Package_Source->Conflicts_get() << endl;  // This is a comma-delimited list
	cout << "Provides: " << pRow_Package_Source->Provides_get() << endl;  // This is a comma-delimited list

#ifndef WIN32
	string sed_predepends = "";
	if (sPreDepends != "")
	{
		sed_predepends = "\\nPre-Depends: ${shlibs:Depends}, ${misc:Depends}" + sPreDepends;
	}
	string sed_cmd = "s/^Depends:.*$/Depends: ${shlibs:Depends}, ${misc:Depends}" + sDepends + sed_predepends + "/";
	string replaces = pRow_Package_Source->Replaces_get();
	string conflicts = pRow_Package_Source->Conflicts_get();
	string provides = pRow_Package_Source->Provides_get();
	if (replaces != "")
	{
		sed_cmd += "; /^Description: / { x; s/^.*$/Replaces: " + replaces + "/; p; x; }";
	}
	if (conflicts != "")
	{
		sed_cmd += "; /^Description: / { x; s/^.*$/Conflicts: " + conflicts + "/; p; x; }";
	}
	if (provides != "")
	{
		sed_cmd += "; /^Description: / { x; s/^.*$/Provides: " + provides + "/; p; x; }";
	}
	string cmd = string("sed -i '" + sed_cmd + "' " + Dir + "/debian/control");
	cout << cmd << endl;
	system(cmd.c_str());

//	// handle dh_strip - the old dh_make way
//	if (isStrippablePackage(pRow_Package_Source->FK_Package_get()))
//		sed_cmd = "s/^.*dh_strip.*$/\tdh_strip/g";
//	else
//		sed_cmd = "s/^.*dh_strip.*$/\t# dh_strip/g";
//	cmd = string("sed -i '" + sed_cmd + "' " + Dir + "/debian/rules");
//	cout << cmd << endl;
//	system(cmd.c_str());

	// handle dh_strip - the new dh_make way
	if (!isStrippablePackage(pRow_Package_Source->FK_Package_get()))
	{
		sed_cmd = "$ a \\\n\\noverride_dh_strip:\\n";
		cmd = string("sed -i '" + sed_cmd + "' " + Dir + "/debian/rules");
		cout << cmd << endl;
		system(cmd.c_str());
	}

	cout << string(("dpkg-buildpackage -b -rfakeroot -us -uc")) << endl;
	if (!g_bSimulate)
	{
		// in C++, the headers do a "*(int *) &(status)" - where C uses the "status" parameter verbatim
		int status = system("dpkg-buildpackage -b -rfakeroot -us -uc");
		printf("dpkg-buildpackage returned %d\n", WEXITSTATUS(status));
		if (WEXITSTATUS(status))
		{
			cout << "dpkg-buildpackage -b failed.  Aborting." << endl;
			return false;
		}
	}
#endif
	
//	cout << "------------DEBIAN PACKAGE OUTPUT" << endl;
//	cout << " rep: " << pRow_Package_Source->Repository_get() << " ver: " << pRow_Package_Source->Version_get() << " parm: " << pRow_Package_Source->Parms_get() << endl;
//	cout << "Press any key to continue..." << endl;
//	char c = getch();
					
#ifndef WIN32
	chdir(CurrentDir);
	cout << "chdir " << Dir << endl;
	if( !g_bSimulate )
		system(("rm -rf " + Dir).c_str());
#endif
	
	return true;
}

bool CreateSource_FTPHTTP(Row_Package_Source *pRow_Package_Source,list<FileInfo *> &listFileInfo)
{
	string ArchiveFileName,Username,Password;
	if( pRow_Package_Source->FK_RepositorySource_get()==REPOSITORYSOURCE_SourceForge_Debian_Sarge_Archi_CONST )
	{
		Username = dceConfig.ReadString("SF_User");
		Password = dceConfig.ReadString("SF_Pass");
		ArchiveFileName = g_sOutputPath + "/sfarch/" + pRow_Package_Source->Repository_get() + "/";
	}
	else
		ArchiveFileName = g_sOutputPath + "/" + pRow_Package_Source->Repository_get() + "/";
	system(("mkdir -p " + ArchiveFileName).c_str());
	
	ArchiveFileName += pRow_Package_Source->Name_get() + "_" + pRow_Package_Source->Version_get() + pRow_Package_Source->Parms_get();
	cout << "Creating Archive: " << ArchiveFileName << endl;
	chdir("/home/tmp");
	
	char templ[12]="mra_XXXXXX";
	string TempDir = mktemp(templ);
#ifndef WIN32
	system(("mkdir -p /home/tmp/" + TempDir).c_str());
#endif
	unlink( ("/home/tmp/" + ArchiveFileName).c_str() );
	chdir(("/home/tmp/" + TempDir).c_str());
	
	for(list<FileInfo *>::iterator it=listFileInfo.begin();it!=listFileInfo.end();++it)
	{
		FileInfo *pFileInfo = *it;
		if( !g_bSimulate && !CopySourceFile(pFileInfo->m_sSource,"/home/tmp/" + TempDir + "/" + pFileInfo->m_sDestination) )
		{
			cout << "***ERROR*** Unable to copy file " <<  pFileInfo->m_sSource << " -> " << TempDir << "/" <<  pFileInfo->m_sDestination << endl;
			return false;
		}
	}
	// See what type it is
	if( pRow_Package_Source->Parms_get()==".tar.gz" )
	{
		if( !g_bSimulate && !TarFiles(ArchiveFileName) )
		{
			cout << "***ERROR*** Tar " << ArchiveFileName << endl;
			return false;
		}
	}
	else if( pRow_Package_Source->Parms_get()==".zip" )
	{
		if( !g_bSimulate && !ZipFiles(ArchiveFileName) )
		{
			cout << "***ERROR*** Zip " << ArchiveFileName << endl;
			return false;
		}
	}
	else
	{
		cout << "***ERROR*** Unknown archive type (" << pRow_Package_Source->PK_Package_Source_get() << "/" << pRow_Package_Source->FK_Package_get() << "): " << pRow_Package_Source->Parms_get() << endl;
		return false;
	}

	if( pRow_Package_Source->FK_RepositorySource_get()==REPOSITORYSOURCE_SourceForge_Debian_Sarge_Archi_CONST || pRow_Package_Source->FK_RepositorySource_get()==REPOSITORYSOURCE_SourceForge_Windows_Archives_CONST || pRow_Package_Source->FK_RepositorySource_get()==REPOSITORYSOURCE_SourceForge_Source_Archives_CONST)
	{
		// ArchiveFileName will be the name of the archive.  Add code here to actually upload it to sourceforge
		// Initiate an ftp upload using Username & Password
		string cmd;
		cmd = "echo 'open -u anonymous,sf@plutohome.com upload.sourceforge.net' > batch";
		system(cmd.c_str());
		cmd = "echo 'cd incoming' >> batch";
		system(cmd.c_str());
		cmd = "echo 'put " + ArchiveFileName + "' >> batch";
		system(cmd.c_str());
		cmd = "echo quit >> batch";
		system(cmd.c_str());
		cmd = "lftp -f batch";
		system(cmd.c_str());
		cmd = "rm -f batch";
		system(cmd.c_str());
	}
	system(("ls -l " + ArchiveFileName + " >> ../dirlist").c_str());
	chdir("/");
#ifndef WIN32
	system(("rm -rf /home/tmp/" + TempDir).c_str());
#endif
	
	return true;
}

bool TarFiles(string sArchiveFileName)
{
	string SystemCall = "tar -zcvf \"" + sArchiveFileName + "\" *";
	system("ls -lR >> ../dirlist");
	cout << "Executing: " << SystemCall << endl;
	if( g_bSimulate )
		return true;
	return system(SystemCall.c_str())==0;
}

bool ZipFiles(string sArchiveFileName)
{
	string SystemCall = "zip -r \"" + sArchiveFileName + "\" *";
	system("ls -lR >> ../dirlist");
	cout << "executing: " << SystemCall << endl;
	if( g_bSimulate )
		return true;
	return system(SystemCall.c_str())==0;
}
