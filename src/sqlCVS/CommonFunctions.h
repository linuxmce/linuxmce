/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
/**
 *
 * @file CommonFunctions.h
 * @brief header file for the GlobalConfig, SerializeableStrings classes; it uses the Table, Repository and Database classes
 * @author
 *
 */

#ifndef CommonFunctions_h
#define CommonFunctions_h

#include <iostream>
#include <map>
#include <string>
#include "SerializeClass/SerializeClass.h"
#include "DCE/DCEConfig.h"

#define NULL_TOKEN	"**( NULL )**"

typedef map<string,string> MapStringString;
extern int ValidateUser(string Username,string Password,bool &NoPassword,bool &bSupervisor);

namespace sqlCVS
{
	class Table;
	class Repository;
	class Database;
	class ChangedRow;
	enum TypeOfChange { toc_New=0, toc_Delete=1, toc_Modify=2 };
	typedef list<ChangedRow *> ListChangedRow;

	typedef map<string,Table *> MapTable;
	typedef map<string,Repository *> MapRepository;

	class ValidatedUser
	{
	public:
		bool m_bWithoutPassword,m_bIsSupervisor;
		int m_psc_user;
		ValidatedUser(int psc_user,bool bWithoutPassword,bool bIsSupervisor) { m_psc_user=psc_user; m_bWithoutPassword=bWithoutPassword; m_bIsSupervisor=bIsSupervisor; }
	};

	// This is a change from a Mask file indicating a row a user wants to revert or checkin
	class MaskedChange
	{
	public:
		Table *m_pTable;
		Repository *m_pRepository;
		TypeOfChange m_eTypeOfChange;
		int m_psc_id;
		MaskedChange(Table *pTable,Repository *pRepository,TypeOfChange eTypeOfChange,int psc_id)
		{
			m_pTable=pTable; m_pRepository=pRepository; m_eTypeOfChange=eTypeOfChange; m_psc_id=psc_id;
		}
	};

	/**
	 * @brief  class modelling the global configuration
	 */

	class GlobalConfig
	{
	public:
		DCEConfig dceConfig;
		string m_sTableType;	/**< the table type */

		string m_sDBHost;	/**< MySQL information: the host */
		string m_sDBUser;	/**< MYSQL information: the user */
		string m_sDBPassword;	/**< MYSQL information: the password */
		string m_sDBName;	/**< MySQL information: the name */

		string m_sCommand;      /**< Command line */
		string m_sRepository;   /**< Command line  */
		string m_sTable; 	/**< Command line  */

		string m_sSqlCVSHost;	/**< The host name or IP for the sqlCVS listener */
		string m_sComments; /**< Comments to be included during a checkin  */

		string m_sSkipVerification; /**< Table:Field's to be skipped during a verify integrity */

		string m_sPK_Old,m_sPK_New; /**< Only for the change primary key command */

		// The following are useful when automating the process via a web page
		string m_sOutputFile; /**< Save the output to a file so an outside program can read it*/
		string m_sMaskFile; /**< Limit the transactions to be checked in to the contents of this file */
		map< pair<string,string>, MaskedChange *> m_mapMaskedChanges; // <rep:tablename,where clause>=true for all the changes we want to be committed from a mask file

		vector<int> m_vectRestrictions; /** if Restrictions are specified on the command line, they will be here.  0 matches no restrictions */

		int m_iDBPort;		/**< The port of the database */
		int m_iSqlCVSPort;	/**< The port of the sqlCVS */

		int m_psc_batch;    /**< The batch number the user passed on the command line */
		int m_iScreenWidth;  /**< The width of the screen */

		int m_iVerifyRestrictID; /**< If doing a verify database, orphaned records will be set to this restriction */

		MapRepository m_mapRepository;  /**< The repositories we're currently operating on */

		MapTable m_mapTable;  /**< The tables we're currently operating on */

		string m_sUsers;  /**< The users specified on the command line */

		map<int,MapTable *> m_mapUsersTables; 	/**< This will have all the users who have made changes,
							 * pointing to a list of the tables they modified
							 */
		MapStringString m_mapUsersPasswords;	/**< This will have all the users and their passwords */
		string m_sDefaultUser; /**< If checking in for multiple users, this will be the one who is assigned ownership of all unclaimed new records */

		Database *m_pDatabase;		/**< points to the database */
		Repository *m_pRepository;	/**< points to the Repository */

		map<int,ValidatedUser *> m_mapValidatedUsers; /**< Map of user id's to bool's if the user is a supervisor */

		map<int, ListChangedRow *> m_mapBatch_ChangedRow;  // Records that couldn't be checked in, by the unauthorized batch number (a negative number)

		bool m_bNewDatabase,m_bVerify,m_bVerifyID,m_bAllowUnmetDependencies,m_bCheckinEveryone,m_bNoPrompts,m_bAllowAnonymousLogin;

		int m_iMaxConnectAttemptCount;
		int m_iServerProcessorTimeout;
		/**
		 * @ brief constructor
		 */

		GlobalConfig()
		{
			m_sTableType="InnoDB";
			m_sDBHost=dceConfig.ReadString("sqlCVS_MySqlHost","localhost");
			m_sDBUser=dceConfig.ReadString("sqlCVS_MySqlUser","root");
			m_sDBPassword=dceConfig.ReadString("sqlCVS_MySqlPassword","");
			m_sDBName=dceConfig.ReadString("sqlCVS_MySqlDatabase","pluto_main");
			m_iDBPort=dceConfig.ReadInteger("sqlCVS_MySqlPort",3306);
			m_iSqlCVSPort=dceConfig.ReadInteger("sqlCVS_Port",3485);
			m_sSqlCVSHost=dceConfig.ReadString("sqlCVS_Host","localhost");
			m_sUsers=dceConfig.ReadString("sqlCVS_Users","");
			m_pDatabase=NULL;
			m_pRepository=NULL;
			m_bNewDatabase=false;
			m_psc_batch=0;
			m_bVerify=m_bVerifyID=m_bAllowUnmetDependencies=m_bCheckinEveryone=m_bNoPrompts=false;
			m_iScreenWidth=dceConfig.ReadInteger("ScreenWidth",120);
			m_bAllowAnonymousLogin=false;
			m_iMaxConnectAttemptCount = -1;
			m_iServerProcessorTimeout = 900;
		}

		string GetRestrictionClause(string sTableName,vector<int> *p_vectRestrictions=NULL)
		{
			if( p_vectRestrictions==NULL )
				p_vectRestrictions=&m_vectRestrictions;

			if( p_vectRestrictions->size()==0 )
				return "(`" + sTableName + "`.psc_restrict IS NULL OR `" + sTableName + "`.psc_restrict=0)";

			string sResult;
			bool bIncludeNoRestriction=false;
			for(vector<int>::iterator it=p_vectRestrictions->begin();it!=p_vectRestrictions->end();++it)
			{
				if( (*it) == 0 )
					bIncludeNoRestriction=true;
				else
					sResult += (sResult.size() ? "," : "") + StringUtils::itos(*it);
			}

			if( sResult.size() )
				sResult = "(`" + sTableName + "`.psc_restrict in (" + sResult + ")";

			if( bIncludeNoRestriction && sResult.size() )
				sResult += " OR `" + sTableName + "`.psc_restrict IS NULL OR `" + sTableName + "`.psc_restrict=0)";
			else if( bIncludeNoRestriction )
				sResult += "(`" + sTableName + "`.psc_restrict IS NULL OR `" + sTableName + "`.psc_restrict=0)";
			else
				sResult += ")";

			return sResult;
		}

		/**
		 * @ brief return all the validated user id's in a comma delimited string
		 */
		string csvUserID();

		/**
		 * @ brief returns true if the user has been validated
		 */
		bool UserIsValidated(int psc_user)
		{
			return m_mapValidatedUsers.find(psc_user)!=m_mapValidatedUsers.end();
		}

		/**
		 * @ brief validates all the users who logged in
		 */
		bool ValidateUsers(bool &bSupervisor,bool bExpectAtLeastOne=true,MapStringString *pmapUsersPasswords=NULL)
		{
			// reset the validated users map
			m_mapValidatedUsers.clear();
		
			if( !pmapUsersPasswords )
			{
				pmapUsersPasswords = &m_mapUsersPasswords;
cout << "Map was null, using internal" << endl;
			}
			bSupervisor=false;
			bool bValidatedUser=false;  // Be sure there is at least 1 user that was validated, and the map isn't empty
cout << "Ready to loop.  size is: " << (int) pmapUsersPasswords->size() << endl;
			for(MapStringString::iterator it=pmapUsersPasswords->begin();it!=pmapUsersPasswords->end();++it)
			{
				bool bNoPassword,bSupervisor2;

				int psc_user = 0;

				// if the command option enabled the "anonymous login", then the special user is allowed
				if (m_bAllowAnonymousLogin && ((*it).first=="anonymous") && ((*it).first=="anonymous") )
				{
cout << "validate anonymous user as it is enabled in command prompt" << endl;
					bNoPassword = false;
					bSupervisor2 = true;
				}
				else
				{
cout << "before validate user" << endl;
					psc_user = ValidateUser( (*it).first, (*it).second, bNoPassword, bSupervisor2 );
cout << "validate user:" << psc_user << " bnopass: " << bNoPassword << " is sup: " << bSupervisor2 << endl;
					if( !psc_user )
					{
						return false;
					}
				}

				cout << "Validated user: " << psc_user << " Is sup: " << bSupervisor2 << endl;
				bValidatedUser=true;
				m_mapValidatedUsers[psc_user]=new ValidatedUser(psc_user,bNoPassword,bSupervisor2);
				if( bSupervisor2 && !bNoPassword )
					bSupervisor=true;
			}
cout << "exiting validate user is sup: " << bSupervisor << endl;
			return bExpectAtLeastOne==false || bValidatedUser;
		}
	};

	/**
	 * @brief  class modelling the serializable strings
	 * @todo complete documentation
	 */

	class SerializeableStrings : public SerializeClass
	{
	public:
		vector<string> m_vectString;

		/**
		 * @brief Sets up serialization
		 */

		void SetupSerialization(int iSC_Version) {	StartSerializeList() + m_vectString; 	}
	};

	extern GlobalConfig g_GlobalConfig;

	/**
	 * @brief   brief documentation
	 * @todo complete documentation
	 */

	void NewSection();
}
#include <string>
#include <map>
using namespace std;

string int2string(int i); /**< casts ints to strings */


string getTableFromForeignKey(string field,map<string,class TableInfo_Generator *> *mapTableNames);

#endif
