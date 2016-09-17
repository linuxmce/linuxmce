/*
        LinuxMCE autotagger
        Langston Ball golgoj4@gmail.com
        GPLv2 Licensed
        LinuxMCE auto-metadata lookup
        Utilizes www.thetvdb.com and
        www.themoviedb.com to find
        relevant information and populate the
        linuxmce Database
*/

#include <autotagger.h>
#include <QTextStream>
void filterFile(QString nFile);
void sendData();
int isLogging;

int main(int argc, char *argv[])        //main loop
{

   // bool loggerPresent = freopen ("/var/log/pluto/linuxmcetag.log","a+",stdout); //open the log


    QTime scanStart = QTime::currentTime ();
    using namespace std;
    QCoreApplication a(argc, argv);
    QString startingDirectory;
    QString status;
    databaseFunctions metaDataDB;      //connecting database right away

    if (!metaDataDB.connectDB())
    {
        cout << "Database Connection error!" << endl;
        return 1;
    }

    metaDataDB.appPath=argv[0];

    cout << "Media Autotagger v1.6 - Scan Start" << qPrintable(scanStart.toString ()) << endl;
    if(argc == 1)                                                                //no arguments, setting to hardcoded default and looking up file id.
    {
        startingDirectory = metaDataDB.findHome("/home/public/data/videos");

        if ( startingDirectory == "Not Found")
        {
            cout << " '/home/public/data/videos' - Was Not Found in DB" << endl;
            return 1;
        }
        else
        {
            cout << "This is default scan directory: " << qPrintable(startingDirectory) << endl;

            QString status = metaDataDB.feedMe(startingDirectory);
            if (status.isNull())
            {
                cout << "Could Not get filelist, internal error" << endl;
                return 1;
            }
            else
            {
                cout << "Got File List for Directory, Going to Work" << endl;
            }
        }
    }
    else if
            (argc > 1 )                                                                    //arguments passed. now we have to figure out if its a number or string
    {
        QString cmp =argv[1];
        int split  = cmp.indexOf("home");                                                  //in this case home will more than likely be in the base path for linuxmce related media, even shares

        if (split > 0)
        {
            QString userPath = argv[1];                                                    //the purpose of this is to check for a slash at the end

            if (userPath.endsWith ("/"))                                                   //think that im checking for an non character ie "/"
            {
                userPath.chop(1);
            }

            startingDirectory = metaDataDB.findHome(userPath);
            cout << "User Path Entered: " << qPrintable(userPath) << endl;

            if ( startingDirectory == "Not Found")
            {
                cout << qPrintable(userPath) <<" - Was Not Found in DB" << endl;

                return 1;
            }
            else if (startingDirectory =="single file")
            {
                cout << "Processed File, Starting" << endl;

            }
            else
            {
                cout << "Directory Submitted" << endl;
                status = metaDataDB.feedMe(startingDirectory);
                // cout << "Status: " << qPrintable(status) << endl;
                if (status.isNull())                                              //errors
                {
                    cout << "Could Not get filelist, internal error getting directory number from mysql" << endl;
                    return 1;
                }
                else if (status == "sub")                                                      //detected subdirectory
                {
                    cout << "Subdirectories Found" << endl;
                    cout << "Processing Subdirectories, this may impact system load" << endl;
                    int subSize = metaDataDB.subDirectoryFiles.size();

                    for (int sub_list_length = 0; sub_list_length < subSize ; sub_list_length++)
                    {
                        QString subDirPath = startingDirectory+"/"+metaDataDB.subDirectoryFiles.at(sub_list_length);
                        QString chk = metaDataDB.feedMe(subDirPath);
                        if (chk == "Ready")
                        {
                            cout << qPrintable(subDirPath) << " Added to list" << endl;
                        }
                    }
                }
                else if (status == "Ready")                                                      //no subdirectories starting to tag
                {
                    cout << "Got File List for Directory, Going to Work" << endl;

                }
                else
                {
                    qDebug () << status ;
                    return 1;
                }
            }
        }

        else                                                                               //didnt find home, assuming its a filenumber
        {
            QString userPath = argv[1];

            startingDirectory = metaDataDB.findHome_int(userPath);
            if (!startingDirectory.isNull())
            {
                cout << "User Path Entered: " << qPrintable(startingDirectory) << endl;

                status = metaDataDB.feedMe(startingDirectory);
                cout << "Status: " << qPrintable(status) << endl;
                if (status.isNull())                                              //errors
                {
                    cout << "Could Not get filelist, internal error getting directory number from mysql" << endl;
                    return 1;
                }
                else if (status == "sub")                                                      //detected subdirectory
                {
                    cout << "Subdirectories Found" << endl;
                    cout << "Processing Subdirectories, this may impact system load" << endl;
                    int subSize = metaDataDB.subDirectoryFiles.size();

                    for (int sub_list_length = 0; sub_list_length < subSize ; sub_list_length++)
                    {
                        QString subDirPath = startingDirectory+"/"+metaDataDB.subDirectoryFiles.at(sub_list_length);
                        QString chk = metaDataDB.feedMe(subDirPath);
                        if (chk == "Ready")
                        {
                            cout << qPrintable(subDirPath) << " Added to list" << endl;
                        }
                    }
                }
                else if (status == "Ready")                                                      //no subdirectories starting to tag
                {
                    cout << "Got File List for Directory, Going to Work" << endl;

                }
                else
                {
                    qDebug () << "Find Home Int status:" << status ;
                    return 1;
                }

            }
            else if (startingDirectory =="single file")
            {
                cout << "Processed Single File, Starting" << endl;
                status = metaDataDB.feedMe (startingDirectory);
            }
            else
            {
                cout << "Invalid Directory Number, exiting!" ;                                       //could be modified to see if its a file and just take that
                return 1;
            };
            cout << "Checking Status" << endl;



        }
        cout << "Found " << metaDataDB.directoryFiles.size() << " File(s) and " << metaDataDB.subDirectoryFiles.size() << " SubDirectories" << endl;
        //	    logString = "Found " + QString::number (metaDataDB.directoryFiles.size()) + " File(s) and " + QString::number (metaDataDB.subDirectoryFiles.size()) + " SubDirectories";
    };
    //via updatemedia at some point, this is a development workaround.
    //getting list of files from directory to iterate over
    for (int fileIterator = 0; fileIterator < metaDataDB.directoryFiles.size(); fileIterator++)           //begin a per file loop
    {

        QString currScan = metaDataDB.directoryFiles.at(fileIterator);
        QString db_File_ID = metaDataDB.directoryFiles_ID.at(fileIterator);

        cout << "============================| New File |==============================" << endl;
        cout << "*************Scanned:"<< qPrintable(currScan) << endl;
        cout << "*************PK_File: " << qPrintable(db_File_ID) << endl;


        videoMedia currentFile;                 //creating new class

        //currentFile.fileNumber=1;               //bs variable initialization, will be taken from parameters passed to it
        currentFile.incfileName = currScan;       //by updateMedia later.

        int mType;
        int mediaSubType = metaDataDB.findMediaType(db_File_ID);
        if(mediaSubType > 0)
        {
            mType = mediaSubType;
            cout << "Found Overriding Media Sub Type::"<< mType << endl;
        }
        else{
            cout << "No Override found, using hueristics." << endl;
            mType= currentFile.videoMediaType(currScan);
        }


        if (mType == 2)
        {
            cout << "-----------Open MovieDB.com Filter--------------" << endl;
            currentFile.mediaType="film";
            film workingPrint;
            workingPrint.setIdent(currentFile.incfileName);
            workingPrint.searchMovie();

            if (workingPrint.mTitle.isNull())
            {
                cout << "------------No MovieDB Match----------"<< endl;
            }
            else
            {
                if(!workingPrint.dlData()){
                    exit(9);
                }

                //begins sql portion
                cout << "-----------------------------Mysql Interaction-------------------" << endl;

                QString mfileID = db_File_ID;   //find file id in system for initial processing in db
                //cout << "Found File ID :" << qPrintable(mfileID)  << endl;

                /*this block checks for the metadate in the db, then checks to see if its part of the file
          if its in the db, its associated to the file, otherwise its inserted AND associated.
           this process could be done more elegantly with a list pair setup maybe
           but im new here and this works just fine. so far.*/
                /* QVariant tmTitle = workingPrint.mTitle.toLatin1();
           int lenG = workingPrint.mTitle.count();
           cout << lenG << endl;
           QString tf = tmTitle.toString();*/


                QString m_Attribute = metaDataDB.checkAttribute(workingPrint.mTitle , mfileID, TITLE);
                QString imdb_attribute=metaDataDB.checkAttribute(workingPrint.mIMDB, mfileID, IMDB);

                metaDataDB.checkAttribute(workingPrint.rating, mfileID, RATING);
                metaDataDB.checkAttribute(workingPrint.releaseYear, mfileID, RELEASEDATE);
                metaDataDB.updateSynopsis(workingPrint.synopsis, mfileID, SYNOPSIS);

                cout << qPrintable(workingPrint.mTitle) << ":("<< qPrintable(workingPrint.releaseYear) << "), IMDB:" << qPrintable(workingPrint.mIMDB);
                cout << "Identified resolution:" << qPrintable(workingPrint.m_rez) << endl;

                // mutiple values in a string. seperated into an array and loop through each

                QStringList directorList;
                directorList = workingPrint.director.split("|");
                for (int dCnt = 0 ; dCnt < directorList.size(); dCnt++ )
                {
                    metaDataDB.checkAttribute(directorList.at(dCnt), mfileID, DIRECTOR);
                }

                QStringList actorList;
                actorList = workingPrint.actor.split("|");
                for (int aCnt = 0; aCnt < actorList.size(); aCnt++)
                {
                    metaDataDB.checkAttribute(actorList.at(aCnt), mfileID, PERFORMER);
                }

                QStringList genreList;
                genreList = workingPrint.genre.split("|");
                for (int gCnt = 0; gCnt < genreList.size(); gCnt++)
                {
                    metaDataDB.checkAttribute(genreList.at(gCnt), mfileID, GENRE);
                }

                QStringList mStudios;
                mStudios = workingPrint.studio.split("|");
                for (int sCnt = 0; sCnt < mStudios.size(); sCnt++)
                {
                    metaDataDB.checkAttribute(mStudios.at(sCnt), mfileID, STUDIO);
                }


                if(workingPrint.picUrl.count () == 0)
                {
                    cout << "no backdrops availible!" << endl;
                }
                else
                {
                    cout << "Checking File Picture" << endl;

                    if (metaDataDB.savePic(workingPrint.picUrl.at (0), m_Attribute, mfileID) == 1)
                    {
                        cout << "---------------File Picture Done with Sucess!" << endl;
                    }
                    else
                    {
                        cout << "!! ------------File Picture was not saved" << endl;
                    }
                }

                if (workingPrint.bgUrl.count () == 0)
                {
                    cout << "no backdrops availible!" << endl;
                }
                else
                {
                    cout << "checking backdrop" << endl;
                    if (metaDataDB.saveAttributePic(workingPrint.bgUrl.at (0), imdb_attribute) == 1)
                    {
                        cout << "--------------BG Saved Pic!" << endl;
                    }
                    else
                    {
                        cout << "!! -----------Background Pic not saved" << endl;
                    }
                }

                metaDataDB.setMediaTypes(workingPrint.m_rez, "2", mfileID);                             //set mediatypes for our movies
                cout << "Media Type Movie Set, Resolution set to:" <<  qPrintable(workingPrint.m_rez) << endl;
            }
        }
        else if (mType == 1)
        {
            cout << "------------------TVDB.com Filter--------------" << endl;
            currentFile.mediaType="tv";

            tvshow pilot;
            //tvShow determination. We set the object variables as we recieve them.
            QString tVar = currentFile.incfileName;

            QString seriesChecker = metaDataDB.searchAttribute(db_File_ID, PROGRAM );
            if(seriesChecker.isEmpty()){
                pilot.showTitle = pilot.setShow(tVar);
            }
            else{
                pilot.showTitle = seriesChecker;
            }

            QString cSeason = metaDataDB.searchAttribute(db_File_ID, SEASON_NUMBER);
            if(cSeason.isEmpty()){
                pilot.season = pilot.setSeason(tVar);
            }
            else
            {
                pilot.season = cSeason;
            }

            cout << "Guessed TV Show:" << qPrintable(pilot.showTitle) << endl;
            pilot.episodeNum=pilot.setEpisode(tVar);
            cout << "Guessed Season: " << qPrintable(pilot.season) <<"..."<<endl;
            cout << "Guessed Episode Number: " << qPrintable(pilot.episodeNum) << endl;
            pilot.createRequest();                                                         //initial thetvdb.com request

            QString fileID = db_File_ID;
            cout << "File ID :" << qPrintable(fileID) << endl;
            if (pilot.progID.isNull())
            {
                cout << "No Match, Moving On" << endl;
            }
            else if
                    (fileID.isNull())
            {
                cout << "Cannot Locate File ID:" << endl;
            }
            else
            {
                //begins sql portion
                cout << "-----------------------------Mysql Interaction-------------------" << endl;
                /*this block checks for the episode title  in the db, then checks to see if its part of the file
           if its in the db, its associated to the file, otherwise its inserted AND associated.
        this process could be done more elegantly with a list pair setup maybe
        but im new here and this works just fine. so far.*/

                QString titlePic = metaDataDB.checkAttribute(pilot.episodeTitle, fileID, EPISODE);
                cout << "Guessed Episode: " << qPrintable(pilot.episodeTitle) << "..." << endl;
                QString cleanEpNum = pilot.episodeNum.remove("");
                metaDataDB.checkAttribute(cleanEpNum, fileID,EPISODE_NUMBER);
                metaDataDB.checkAttribute(pilot.season, fileID, SEASON_NUMBER);
                QString title_Attribute_p = metaDataDB.checkAttribute(pilot.showTitle, fileID, PROGRAM);
                QString title_Attribute = metaDataDB.checkAttribute(pilot.showTitle, fileID, TITLE);

                QString imdb_attribute = metaDataDB.checkAttribute(pilot.IMDB, fileID, IMDB);
                metaDataDB.checkAttribute(pilot.tvNetwork, fileID, CHANNEL);
                metaDataDB.checkAttribute(pilot.tvRating, fileID, RATING);
                metaDataDB.checkAttribute(pilot.airdate, fileID, RELEASEDATE);
                QString attribute_progID = metaDataDB.checkAttribute(pilot.progID, fileID, TVPROGRAMID);              //using this as global for entire series -will be phased out when datagrids change
                QString attribute_seriesID = metaDataDB.checkAttribute(pilot.seriesID, fileID, TVSERIESID);             //using this for seried identifier to prepare for datagrid change.

                cout << "Identified:"<< "Show:" << qPrintable(pilot.showTitle) << ", Season: " << qPrintable(pilot.season) << "Episode: (" << qPrintable(pilot.episodeNum) << ") " << qPrintable(pilot.episodeTitle) << endl;

                metaDataDB.updateSynopsis(pilot.synopsis, fileID, SYNOPSIS);
                /*
         ------DO NOT UN COMMENT UNLESS
         You have added seasonID to AttributeType in pluto_media with PK_AttributeType 52*/
                QString attribute_seasonID = metaDataDB.checkAttribute(pilot.seasonID, fileID, TVSEASONID);         //using for specific seaon identifier


                // with multiple values in a string, break into array and loop through
                QStringList tvDirectorList;
                tvDirectorList = pilot.tvDirector.split("|");
                for (int tv_dCnt = 0; tv_dCnt < tvDirectorList.size(); tv_dCnt++)
                {
                    metaDataDB.checkAttribute(tvDirectorList.at(tv_dCnt), fileID, DIRECTOR);
                }

                QStringList tvActorList;
                tvActorList = pilot.actor.split("|");
                for (int tv_aCnt = 0; tv_aCnt < tvActorList.size() ; tv_aCnt++)
                {
                    metaDataDB.checkAttribute(tvActorList.at(tv_aCnt), fileID, PERFORMER);
                }

                QStringList tv_genreList;
                tv_genreList = pilot.genre.split("|");
                for (int tv_gCnt=0; tv_gCnt < tv_genreList.size(); tv_gCnt++)
                {
                    metaDataDB.checkAttribute(tv_genreList.at(tv_gCnt), fileID, GENRE);
                }

                QStringList tv_guestStars;
                tv_guestStars = pilot.guestStars.split("|");
                for (int gStars = 0 ; gStars < tv_guestStars.size(); gStars++)
                {
                    metaDataDB.checkAttribute(tv_guestStars.at(gStars), fileID, PERFORMER);
                }
                //------Picture Processing-------------------------------------------------------------------------------

                //episode picture atatches to pk_fike
                if (pilot.epImage.isNull ())
                {
                    cout << "No Episode Pic Availible!" << endl;

                }
                else
                {
                    cout << "Checking File Picture-------" << endl;
                    int epImgChk = metaDataDB.savePic(pilot.epImage, attribute_progID, fileID);
                    if (epImgChk == 1)
                    {
                        cout << "------------Saved Episode Pic!" << endl;
                    }
                    else if (epImgChk == 2)
                    {
                        cout << "!----------Episode Has Pic,skipping" << endl;
                    }
                    else
                    {
                        cout << "!!----------Episode Pic not saved" << endl;
                    }
                }
                //series image atatches to series id
                if (pilot.seriesImg.isEmpty ())
                {
                    cout << "No Series Pic Availible!" << endl;
                }
                else
                {
                    cout << "Checking Series Picture" << endl;
                    QString best_series_img;

                    best_series_img = pilot.seriesImg.at (0);

                    int seriesImgChk = metaDataDB.saveAttributePic(best_series_img, title_Attribute);
                    if (seriesImgChk == 1)
                    {
                        cout << "------------Saved Series Pic!" << endl;
                    }
                    else if (seriesImgChk == 2)
                    {
                        cout << "!----------Series Has Picture, skipping" << endl;
                    }
                    else
                    {
                        cout << "!!----------Series Pic not saved" << endl;
                    }
                }
                // adding viginettes to imdb
                if (pilot.viginettes.isEmpty ())
                {
                    cout << "No banners Availible!" << endl;
                }
                else
                {
                    cout << "Checking imdb  - banner" <<endl;
                    int viginettesImgChk = metaDataDB.saveAttributePic(pilot.viginettes.at(0), imdb_attribute);
                    if (viginettesImgChk == 1)
                    {
                        cout <<"------------Saved banner!" << endl;
                    }
                    else if (viginettesImgChk == 2)
                    {
                        cout << "!!----------Banner exist(s), skipping" << endl;
                    }
                    else
                    {
                        cout << "!!----------banner not saved" << endl;
                    }
                }

                // adding season img to season id
                if (pilot.seasonImg.isEmpty ())
                {
                    cout << "No season image Availible!" << endl;
                }
                else
                {
                    cout << "Checking season image poster" << endl;
                    int seasonImgChk = metaDataDB.saveAttributePic(pilot.seasonImg.at(0), attribute_seasonID);
                    if (seasonImgChk == 1)
                    {
                        cout << "------------Saved season image!" << endl;
                    }
                    else if (seasonImgChk == 2)
                    {
                        cout << "!----------season image exists, skipping" << endl;
                    }
                    else
                    {
                        cout << "!!----------season image not saved" << endl;
                    }
                }

                if (pilot.seriesImg.isEmpty ())
                {
                    cout << "No Series ID  image Availible!" << endl;
                }
                else
                {
                    cout << "Checking Series ID Viginette" << endl;
                    int seriesIDmgChk = metaDataDB.saveAttributePic(pilot.seriesGraphic.at(0), attribute_seriesID);
                    if (seriesIDmgChk == 1)
                    {
                        cout << "------------Series ID Image Saved!" << endl;
                    }
                    else if (seriesIDmgChk == 2)
                    {
                        cout << "!----------Series ID Image exists, skipping." << endl;
                    }
                    else
                    {
                        cout << "!!----------Series ID Image not saved!" << endl;
                    }
                }
                if (pilot.seriesImg.isEmpty ())
                {
                    cout << "No Program image Availible!" << endl;
                }
                else
                {
                    cout << "Checking Program  Viginette" << endl;
                    int seriesIDmgChk = metaDataDB.saveAttributePic(pilot.seriesImg.at(0), title_Attribute_p);
                    if (seriesIDmgChk == 1)
                    {
                        cout << "------------No Program image Saved!" << endl;
                    }
                    else if (seriesIDmgChk == 2)
                    {
                        cout << "!----------Program image Exists, skipping" << endl;
                    }
                    else
                    {
                        cout << "!!----------Program image Not saved!!" << endl;
                    }
                }


                metaDataDB.setMediaTypes(pilot.t_rez,"1", fileID);                                     //set media type and media subtype
                cout << "TV Shows media type set, Resolution set to:" << qPrintable(pilot.t_rez) << endl;

                cout << "***************************************************************" << endl;
                cout << "***************Database Updated, file complete*****************" << endl;
                cout << "***************************************************************" << endl;
            }
        } else{
            cout << "***************No scanner for this file type, file complete*****************" << endl;
        }
        cout << "**************************End File*****************************" << endl;
    };

    metaDataDB.closeDB();
    cout << "                    Finished with batch" << endl;
 //  fclose (stdout); //close the log

    //return a.exec(); //maintains loop
    return 0;           //exits script
}
