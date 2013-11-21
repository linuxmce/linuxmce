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
#include <databaseFunctions.h>
#include <pilot.h>
#include <film.h>
#include <QImage>



databaseFunctions::databaseFunctions()
{
    databaseName="pluto_media";
    pass ="";
    user="root";
}

//this function checks attributes and deals with insertions and associations.
QString databaseFunctions::checkAttribute(QString attribute, QString fileNo, int AttributeType)                                    //this should be set to return 1 or 0 so that it
{
    if(attribute=="")
    {
        cout << "Attribute Type: " << AttributeType << "Blank Attribute" << endl;
        return "no match";
    }
    else
    {												     //can be used to determine the next options: insert or skip
        QSqlQuery chkAttribExist;
        chkAttribExist.prepare("SELECT * FROM Attribute WHERE Name = (:attrib) AND FK_AttributeType = (:m_type) LIMIT 1");
        QVariant attrib_txt = attribute;
        chkAttribExist.bindValue("(:attrib)", attrib_txt);
        chkAttribExist.bindValue("(:m_type)", AttributeType);                                                    //take file number for processing and comparison and the attribute.
        chkAttribExist.exec();
        //cout << "Query Start for Attribute: " << qPrintable(attribute) << "...";
        QSqlRecord chkRec = chkAttribExist.record();
        QVariant chkVal;
        int attribNum = chkRec.indexOf("PK_Attribute");
        int verify = chkRec.indexOf("FK_AttributeType");

        if(chkRec.count() != 0)
        {
            /*
 Ok this is ugly. To prevent adding extra attributes into the db, we want to reuse existing ones if possible and associate them with the file, assuming their AttributeType
 matches. This involves:
 a. look up the attribute and get attribute type and id
 b. if no match, create and retrieve id and insert into file_attribute. If the attribute didnt exist to begin with, then it cannot be already associated
 c. check file_attribute for corresponding attribute id. if not there, insert
*/
            //  cout << "...got response...";
            if(chkAttribExist.next())                                                                        //loop iterates through results
            {
                if (chkAttribExist.value(verify) == AttributeType)                                      //if the attribute is in the db with matching attribtype, associate it
                {
                    chkVal = chkAttribExist.value(attribNum).toString();
                    //   cout << "...Found: " << qPrintable(attribute) << "....";
                    //sql insert into file_Attribute pk_attribute fk_fileno, but check to see if its there 1st

                    QString  attribNo =chkVal.toString();
                    QSqlQuery chkFile;
                    chkFile.prepare("SELECT * FROM File_Attribute WHERE FK_Attribute = (:attribNo) AND FK_File = (:fileID) LIMIT 1" );
                    chkFile.bindValue(":attribNo", attribNo);
                    chkFile.bindValue(":fileID", fileNo);
                    chkFile.exec();

                    QSqlRecord chkFileRec = chkFile.record();
                    QVariant fileRec = chkAttribExist.result();

                    int fAttrib = chkFileRec.indexOf("FK_Attribute");
                    // cout << qPrintable(chkVal.toString()) <<" ||..";
                    if(chkFile.next())
                    {
                        //cout << qPrintable(chkFile.value(fAttrib).toString()) << "|-" ;
                        if(chkFile.value(fAttrib) == attribNo)
                        {
                            //cout << "....Attribute Already Attatched!...";
                        }
                    }
                    else
                    {
                        //cout << "need to attatch...";
                        if (associateFile(fileNo, attribNo) == 1)                                                                 //sql insert into file_Attribute pk_attribute fk_fileno
                        {
                            //cout << "Success!" << endl;
                        }
                        else
                        {
                            //cout << "Insert Error!" << endl;
                        }

                    }
                }

                //cout << "....association complete" << endl;
            }
            else //this else block catches the not in database attributes
            {
                //cout << "Attribute-" << qPrintable(attribute) << "::" << AttributeType << ": is not in the Database, inserting...";

                QString insertID=insertAttribute(attribute, QString::number(AttributeType));                              //insert into attribute name , fk_attributeType
                chkVal = insertID;
                if (insertID.isNull())
                {
                    //cout <<"Cannot Insert!" << endl;
                }
                else
                {
                    cout << "Done! New AttributeID = " << qPrintable(insertID) << "|| Now for file...";
                    if (associateFile(fileNo, insertID) == 1)                                                                 //sql insert into file_Attribute pk_attribute fk_fileno
                    {
                        //cout << "Success!" << endl;
                    }
                    else
                    {
                        //cout << "Insert Error!" << endl;
                    }
                }
            }

            //cout << "Atatchment  phase complete" << endl;
            return chkVal.toString();
        }
        else
        {
            cout << "Mysql Error" << qPrintable(chkAttribExist.lastError().text()) << endl;
            return chkVal.toString();
        }
    }
}

QString databaseFunctions::searchAttribute(QString file ,int attributeType)
{

    QSqlQuery chkFile;
    chkFile.prepare("SELECT File_Attribute.FK_File, File_Attribute.FK_Attribute, Attribute.PK_Attribute, Attribute.Name, Attribute.FK_AttributeType FROM File_Attribute, Attribute WHERE File_Attribute.FK_Attribute = Attribute.PK_Attribute AND Attribute.FK_AttributeType = (:attribType) AND File_Attribute.FK_File = (:fileID) LIMIT 1 " );
    chkFile.bindValue(":attribType", attributeType);
    chkFile.bindValue(":fileID", file);
    chkFile.exec();

    QString result="";
    int at=0;
    QSqlRecord chkFileRec = chkFile.record();

    if(chkFile.next()){
        at = chkFileRec.indexOf("Name");
        result= chkFile.value(at).toString();
    }

    return result;
}

int databaseFunctions::findMediaType(QString file)
{

    QSqlQuery mediaTypeSearch;
    mediaTypeSearch.prepare("SELECT * FROM File WHERE PK_File= (:fileno)");
    mediaTypeSearch.bindValue("(:fileno)", file);
    mediaTypeSearch.exec();
    QSqlRecord chkFileRec = mediaTypeSearch.record();
    qDebug() << mediaTypeSearch.size();
    if (mediaTypeSearch.next()){
        QString rec = mediaTypeSearch.value(chkFileRec.indexOf("FK_MediaSubType")).toString();
        qDebug()<< rec;
        if(rec=="NULL")
            return -1;
        else
            return rec.toInt();
    }
    else
        return -1;



}

//inserts a new attribute into the database and returns the inserted id for later use in associations
QString databaseFunctions::insertAttribute(QString ins_attribute, QString inc_attribType)
{
    QSqlQuery n_attribute;
    n_attribute.prepare("INSERT INTO Attribute (PK_Attribute, FK_AttributeType, Name) VALUES (:incr, :fk_AttribType, :attrib)");
    n_attribute.bindValue("(:incr)", "");
    n_attribute.bindValue("(:fk_AttribType)", inc_attribType);
    n_attribute.bindValue("(:attrib)", ins_attribute);
    n_attribute.exec();
    QString lastInsert = n_attribute.lastInsertId().toString();
    return lastInsert;
}

//associates attributes with files in the case that the attribute exists. This function is called from another that makes this determination
int databaseFunctions::associateFile(QString fkFile, QString fkAttributeNo)
{
    QSqlQuery a_attribute;
    a_attribute.prepare("INSERT INTO File_Attribute (FK_File, FK_Attribute) VALUES (:fileNo, :attribNo)");
    a_attribute.bindValue("(:fileNo)", fkFile);
    a_attribute.bindValue("(:attribNo)", fkAttributeNo);
    a_attribute.exec();

    if (a_attribute.lastInsertId().isNull())
    {
        return 8;
    }
    else
    {
        return 1;
    }
}


QString databaseFunctions::findFileNo(QString fullFilePath, QString fullFileName)                                   //finds the file number for further processing. this function
{	                                                                                                           //this would be made irrelevant later by updateMedia integration

    QSqlQuery query("SELECT PK_File FROM File WHERE Filename = '"+fullFileName+"' && Path = '"+fullFilePath+" '");
    QSqlRecord rec= query.record();
    int pkFile = rec.indexOf("PK_File");
    QVariant fl;
    QString fileNo;
    while
            (query.next())
    {
        fl = query.value(pkFile).toString();
    }
    return fl.toString();
}

int databaseFunctions::connectDB()
{
    lmceDB = QSqlDatabase::addDatabase("QMYSQL");
    lmceDB.setHostName("dcerouter");
    lmceDB.setDatabaseName("pluto_media");
    lmceDB.setUserName("root");
    lmceDB.setPassword("");


    if (!lmceDB.open())
    {
        cout << "No Connection" << endl;
        return 0;
        lmceDB.close();
    }
    else
    {
        cout << "connection made!" << endl;
        return 1;

    }
}

void databaseFunctions::closeDB()
{
    lmceDB.close();
    cout << "DB Closed!" << endl;
}

//this function updates the synopsis in the long_attribute table. we update to ensure the latest information is grabbed but not duplicated
void databaseFunctions::updateSynopsis(QString synop, QString FileID, int attributeEnum)
{

    QSqlQuery chkSynop;

    chkSynop.prepare("SELECT * FROM Attribute WHERE FK_AttributeType = 37 AND Name = (:fileNum) ");
    chkSynop.bindValue("(:data)", synop);
    chkSynop.exec();

    QSqlRecord synRec = chkSynop.record();
    QString synopResp = synRec.field("PK_Attribute").value().toString();
    // qDebug () << chkSynop.boundValues ();

    if(!chkSynop.next ())
    {
        //cout << "No Synopsis, Adding";
        QSqlQuery insert_synopsis;
        insert_synopsis.prepare("INSERT INTO Attribute (PK_Attribute, FK_AttributeType, Name) VALUES (:attributeIncr, :attribType ,  :data)");
        insert_synopsis.bindValue("(:attributeIncr)", "");
        insert_synopsis.bindValue("(:attribType)", "37");
        insert_synopsis.bindValue("(:data)", synop);
        insert_synopsis.exec();
        //qDebug () << "insert call" << insert_synopsis.boundValues ();
        if (insert_synopsis.lastInsertId ().isNull ())
        {
            cout << "Synopsis Insertion Error!";
        }
        else
        {
            cout << "synopsis created";
        }
    }
    else
    {
        //cout << "Has Synopsis, Updating!";
        QSqlQuery update_synopsis;
        update_synopsis.prepare("UPDATE Attribute WHERE PK_Attribute = (:existingAttrib) AND FK_AttributeType=37 SET Text = (:data)");
        update_synopsis.bindValue("(:existingAttrib)", synopResp);
        update_synopsis.bindValue("(:data)", synop);
        update_synopsis.exec();

        if (update_synopsis.numRowsAffected () == 0)
        {
            cout << "Couldnt update synopsis!..";
        }
        else
        {
            cout << "synopsis updated";
        }
    }
    cout << "....done!" << endl;
}

//this function save pictures for files in the picture_file table.
int databaseFunctions::savePic(QString picUrl, QString pic_Attribute, QString file_num)
{
    QSqlQuery pic_chk;
    pic_chk.prepare("SELECT * FROM Picture_File WHERE FK_File = (:fk_fileNo)  LIMIT 1");
    pic_chk.bindValue("(:fk_File)", file_num);
    qDebug() << pic_chk.boundValues() << endl;
    pic_chk.exec();
    pic_chk.next();
    QSqlRecord pic_chk_resp = pic_chk.record();
    QString picChkVal = pic_chk_resp.field("FK_File").value().toString();
    qDebug() << picChkVal << endl;
    if ( picChkVal== "" || picChkVal=="0" )
    {
        QFile mediaPic;
        mediaPic.setFileName("/var/tmp/"+pic_Attribute+".jpg");
        //---network download
        QNetworkAccessManager *picManager =  new QNetworkAccessManager(this);
        QUrl p_url(picUrl);
        qDebug () << p_url;
        QNetworkRequest p_req(p_url);
        //this->connect(rep, SIGNAL(finished()), this, SLOT(replyFinished()));
        cout << "Getting Pic for file" << endl;
        QNetworkReply *p_rep = picManager->get(p_req);
        QEventLoop p_eventLoop;                                                                  // execute an event loop to process the request (nearly-synchronous)
        // this loop MUST be utilized as you will have no way of making your current file loop wait
        this->connect(picManager, SIGNAL(finished(QNetworkReply *)), &p_eventLoop, SLOT(quit()));   //for data to
        p_eventLoop.exec();                                                                         //continue. also dispose the event loop after the reply has arrived
        QByteArray picData = p_rep->readAll();

        cout<< "file picture processing" << endl;
        if (!mediaPic.open(QFile::ReadWrite))
        {
            cout << "Could Not open temp image file" << endl;
            return 8;
        }
        else
        {
            mediaPic.write(picData);
            QImage validator(mediaPic.fileName ());
            mediaPic.close();
            if (validator.isNull ())
            {
                cout << "No Image here, stopping pre-insert!" << endl;
                return 8;
            }
            else
            {
                cout << "Valid Image downloaded" << endl;
            }
            cout << "wrote file picture...";

            QSqlQuery picture_table;
            picture_table.prepare("INSERT INTO Picture (PK_Picture, Extension, URL) VALUES (:pk_picture, :exten, :url)");
            picture_table.bindValue("(:pk_picture)", "");
            picture_table.bindValue("(:exten)", "jpg");
            picture_table.bindValue("(:url)", picUrl);
            picture_table.exec();
            QString n_picID = picture_table.lastInsertId().toString();
            // cout << "inserted into picture_table..insert id:" << qPrintable(n_picID) ;

            mediaPic.remove("/var/www/lmce-admin/mediapics/"+n_picID+".jpg");
            mediaPic.remove("/var/www/lmce-admin/mediapics/"+n_picID+"_tn.jpg");

            if (mediaPic.copy("/var/www/lmce-admin/mediapics/"+n_picID+".jpg") ==1)
            {
                //cout << "Copied to /mediapics..." ;
                mediaPic.remove(mediaPic.fileName());
                QImage m_Pic("/var/www/lmce-admin/mediapics/"+n_picID+".jpg");
                QImage tn_Pic("/var/www/lmce-admin/mediapics/"+n_picID+"_tn.jpg");

                tn_Pic = m_Pic;
                tn_Pic.scaledToWidth (256).save("/var/www/lmce-admin/mediapics/"+n_picID+"_tn.jpg");
                if (!tn_Pic.isNull())
                {
                    cout << "Thumb created..";
                    QSqlQuery pfile_attribute;
                    pfile_attribute.prepare("INSERT INTO Picture_File (FK_Picture, FK_File) VALUES (:fk_picture, :fk_fileNo)");
                    pfile_attribute.bindValue("(:fk_picture)", n_picID);
                    pfile_attribute.bindValue("(:fk_File)", file_num);
                    pfile_attribute.exec();
                    cout << "Inserted new record into picture_file..";
                }
                else
                {
                    cout << "File Thumb Creation Failed .Aborting process" << endl;
                    return 8;
                }
            }
            else
            {
                cout << "Error Copying File Picture..Nothing Inserted" <<endl;
                return 8;
            }
        }
        return 1;
    }
    else
    {

        return 2;
    }
}

void databaseFunctions::setMediaTypes(QString file_format, QString sub_Type, QString File_Num)
{
    QSqlQuery mediaSubTypeSet;
    mediaSubTypeSet.prepare("UPDATE File SET FK_MediaSubType = (:m_SubType), FK_FileFormat = (:fk_file_format) WHERE PK_File = (:fileNo)");
    mediaSubTypeSet.bindValue("(:m_SubType)", sub_Type);
    mediaSubTypeSet.bindValue("(:fk_file_format)", file_format);
    mediaSubTypeSet.bindValue("(:fileNo)", File_Num);
    mediaSubTypeSet.exec();
}

//this function save pictures that are atatched to attributes and not file numbers
int databaseFunctions::saveAttributePic(QString picUrl, QString attribute)                                        //this specifically saves a picture for in the picture_attribute table
{                                                                                                                 //this is done to associate it with certain attribute such as a season or


    QSqlQuery pic_chk;                                                                                            //other global attribute.
    pic_chk.prepare("SELECT * FROM Picture_Attribute WHERE FK_Attribute = (:fk_attrib)  LIMIT 1");                //also used for saving backdrops which are connected to the imdb attribute
    pic_chk.bindValue("(:fk_attrib)", attribute);                                                                 //if attribute is null, then file ID is utilized
    pic_chk.exec();
    pic_chk.next();
    QSqlRecord pic_chk_resp = pic_chk.record();
    QString picChkVal = pic_chk_resp.field("FK_Attribute").value().toString();

    if ( picChkVal== "" || picChkVal=="0" )
    {
        cout << "Picture for Attribute will be saved. Downloading image...";
        QFile mediaPic;
        mediaPic.setFileName("/var/tmp/"+attribute+".jpg");
        //---network download
        QNetworkAccessManager *picManager =  new QNetworkAccessManager(this);
        QUrl p_url(picUrl);
        qDebug () << p_url;
        QNetworkRequest p_req(p_url);
        //this->connect(rep, SIGNAL(finished()), this, SLOT(replyFinished()));

        QNetworkReply *p_rep = picManager->get(p_req);
        QEventLoop p_eventLoop;                                                                  // execute an event loop to process the request (nearly-synchronous)
        // this loop MUST be utilized as you will have no way of making your current file loop wait
        this->connect(picManager, SIGNAL(finished(QNetworkReply *)), &p_eventLoop, SLOT(quit()));   //for data to
        p_eventLoop.exec();                                                                         //continue. also dispose the event loop after the reply has arrived
        QByteArray picData = p_rep->readAll();

        cout << "---picture processing" << endl;
        if (!mediaPic.open(QFile::ReadWrite))
        {
            //  cout << "Could Not open temp image file" << endl;
            return 8;
        }
        else
        {
            mediaPic.write(picData);
            mediaPic.close();
            cout << "wrote picture file data...";

            QSqlQuery picture_table;
            picture_table.prepare("INSERT INTO Picture (PK_Picture, Extension, URL) VALUES (:pk_picture, :exten, :url)");
            picture_table.bindValue("(:pk_picture)", "");
            picture_table.bindValue("(:exten)", "jpg");
            picture_table.bindValue("(:url)", picUrl);
            picture_table.exec();
            QString n_picID = picture_table.lastInsertId().toString();
            cout << "Inserted new record into picture_table.for arrtibute picture. Insert id:" << qPrintable(n_picID) <<endl;

            mediaPic.remove("/var/www/lmce-admin/mediapics/"+n_picID+".jpg");
            mediaPic.remove("/var/www/lmce-admin/mediapics/"+n_picID+"_tn.jpg");

            if (mediaPic.copy("/var/www/lmce-admin/mediapics/"+n_picID+".jpg") ==1)
            {
                cout << "Copied to /mediapics..."<<qPrintable(n_picID)<<".jpg" << endl;
                mediaPic.remove(mediaPic.fileName());
                QImage m_Pic("/var/www/lmce-admin/mediapics/"+n_picID+".jpg");
                QImage tn_Pic("/var/www/lmce-admin/mediapics/"+n_picID+"_tn.jpg");
                tn_Pic = m_Pic;
                tn_Pic.scaledToWidth (256).save("/var/www/lmce-admin/mediapics/"+n_picID+"_tn.jpg");
                if (!tn_Pic.isNull())
                {
                    cout << "Thumb created.." << endl;
                    QSqlQuery pfile_attribute;
                    pfile_attribute.prepare("INSERT INTO Picture_Attribute (FK_Picture, FK_Attribute) VALUES (:fk_picture, :fk_Attribute)");
                    pfile_attribute.bindValue("(:fk_picture)", n_picID);
                    pfile_attribute.bindValue("(:fk_Attribute)", attribute);
                    pfile_attribute.exec();
                    cout << "Created attribute picture!.." << endl;

                }
                else
                {
                    cout << "Thumb Creation Failed for Attribute..";
                    return 8;
                }
            }
            else
            {
                //cout << "Error Copying Attribute Picture..";
                return 8;
            }
        }
        //inserted new attribute
        return 1;
    }
    else
    {
        //attribute has picture
        return 2;
    }
}

//find the root directory of a given path
QString databaseFunctions::findHome(QString arg)
{

    QSqlQuery findVideos;
    findVideos.prepare("SELECT * FROM File WHERE PATH = (:argument) ");
    findVideos.bindValue("(:argument)",arg);
    findVideos.exec();
    findVideos.first();
    int num_Rows = findVideos.size ();
    int rowInc = 0;
    QSqlRecord find_video_rec;

    for (rowInc = 0; rowInc < findVideos.size (); rowInc++)
    {
        find_video_rec = findVideos.record ();

        if (find_video_rec.field("Path").isNull())
        {
            //return "Not Found";
            return "null";
        }
        else if (find_video_rec.field("isDirectory").value ()==0)
        {
            cout << "Single File Detected --";
            int filePos = findVideos.record().indexOf("Filename");
            int idPos = findVideos.record().indexOf("PK_File");
            int subDir = findVideos.record().indexOf("isDirectory");
            int db_mediaType = findVideos.record().indexOf("EK_MediaType");
            cout << qPrintable(findVideos.record ().field (filePos).value ().toString ()) << "**"  <<endl;
            if (findVideos.record().field(db_mediaType).value().toString() =="5" || "3" && findVideos.record().field(subDir).value().toString() =="0")
            {
                directoryFiles.append(findVideos.record().field(filePos).value().toString());
                directoryFiles_ID.append(findVideos.record().field(idPos).value().toString());
            }
        }
        else
        {
            return find_video_rec.field("Path").value().toString();
        }
        findVideos.next();
    }
    return "ready";
}


//this function provides a list with the filename to parse in the regexp functions and a file number as a reference for adding attributes
QString databaseFunctions::feedMe(QString directoryPath)
{
    //cout << qPrintable(directoryPath) << endl;
    QSqlQuery find_files_in_path;
    find_files_in_path.prepare("SELECT * FROM File WHERE PATH = (:argument) ");
    find_files_in_path.bindValue("(:argument)", directoryPath);
    find_files_in_path.exec();
    //find_files_in_path.nextResult ();
    //QSqlRecord file_path_info = find_files_in_path.record();

    while (find_files_in_path.next())
    {
        int filePos = find_files_in_path.record().indexOf("Filename");
        int idPos = find_files_in_path.record().indexOf("PK_File");
        int subDir = find_files_in_path.record().indexOf("isDirectory");
        int db_mediaType = find_files_in_path.record().indexOf("EK_MediaType");

        if (find_files_in_path.record().field(subDir).value().toString() =="1")
        {
            subDirectoryFiles.append(find_files_in_path.record().field(filePos).value().toString());
            subDirectoryFiles_ID.append(find_files_in_path.record().field(idPos).value().toString());
        }
        else if (find_files_in_path.record().field(db_mediaType).value().toString() =="5" || "3" && find_files_in_path.record().field(subDir).value().toString() =="0")
        {
            directoryFiles.append(find_files_in_path.record().field(filePos).value().toString());
            directoryFiles_ID.append(find_files_in_path.record().field(idPos).value().toString());
            //return "single file";
        }
    }
    if (subDirectoryFiles.size() == 0)
    {
        cout << "No Subdirectories" << endl;
        return "Ready";
    }
    else
    {
        return "sub";
    }
}

//this function is called when we turn a path into a file number, or a file number is passed to us from the start. It has a catch for single files and not directories
QString databaseFunctions::findHome_int(QString arg)
{
    QSqlQuery find_files_in_path;
    find_files_in_path.prepare("SELECT * FROM File WHERE PK_file = (:argument) ");
    find_files_in_path.bindValue("(:argument)", arg);
    find_files_in_path.exec();
    QString path;

    while (find_files_in_path.next())
    {
        int idPos = find_files_in_path.record().indexOf("PK_File");
        int pathPos = find_files_in_path.record().indexOf("Path");
        int filePos = find_files_in_path.record().indexOf("Filename");
        int dirPos = find_files_in_path.record ().indexOf ("isDirectory");
        int db_mediaType = find_files_in_path.record().indexOf("EK_MediaType");

        isDir = find_files_in_path.record().field(dirPos).value().toInt ();
        if (isDir == 1)
        {
            path = find_files_in_path.record().field(pathPos).value().toString()+"/"+find_files_in_path.record().field(filePos).value().toString();
            return path;
        }
        else if (isDir == 0 && find_files_in_path.record().field(db_mediaType).value().toString() =="5" || "3"  )
        {
            directoryFiles.append(find_files_in_path.record().field(filePos).value().toString());
            directoryFiles_ID.append(find_files_in_path.record().field(idPos).value().toString());
            return "single file";
        }
    }return "Ready";
}
