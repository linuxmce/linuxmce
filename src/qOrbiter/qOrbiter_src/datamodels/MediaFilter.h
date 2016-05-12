/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org

    QOrbiter is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MediaFilter_h
#define MediaFilter_h

#include "DCE/Logger.h"
#include <QObject>
#include <QStringList>
#include <QDebug>

namespace DCE
{
/*!
 *\class MediaFilter
 * \ingroup dce_controllers
 *\brief The object represents the media filter used in qOrbiter's media browser.
 *
 *
 */
class MediaFilter : public QObject {

    Q_OBJECT
    Q_PROPERTY(QString genericOptions READ getGenericOptions WRITE setGenericOptions NOTIFY genericOptionsChanged)

private:
    QStringList filterStack;             /*!< List of Comma seperated strings of the previous data grid request */
    QString qs_seek;                /*!< The Seek letter of the data grid request */
    int q_mediaType;            /*!< The media type of the requested data grid. this is at position 1 */
    QString q_subType;              /*!< The media sub type of the request data grid. This is at position 2*/
    QString q_fileFormat;           /*!< The file format of the requested data grid. This is at position 3*/
    QString q_attribute_genres;     /*!< The attribute_genres value for the requested datagrid. This is at position 4*/
    QString q_mediaSources;         /*!< The media sources for the requested datagrid. This is at position 5.*/
    QString q_usersPrivate;         /*!< The private users to include in the requested media grid files. This is at position 6*/
    QString q_attributetype_sort;   /*!< The Attribute that is being used to sort (filter) the requested data grid. This is at position 7*/
    QString q_pk_users;             /*!< The requested public users to include. This is position 8*/
    QString q_last_viewed;          /*!< Unknown Position 9*/
    QString q_pk_attribute;         /*!< The Attribute currently being searched(?) This is position 10*/
    QString videoDefaultSort;       /*!< Comma seperated string of the default sorting for video files */
    QString audioDefaultSort;       /*!< Comma seperated string of the default audio sorting */
    QString photoDefaultSort;       /*!< Comma seperated string of the default photo sorting */
    QString gamesDefaultSort;       /*!< Comma seperated string of the default games sorting */
    QString dataGridId;             /*!< datagrid this filter is controlling */
    QString genericOptions; /*!< The generic options for non media grids */
public:

    MediaFilter() {
        q_mediaType = 0;
        clear();
        // TODO: should be loaded from DCE router or from user preferences
        videoDefaultSort = "13";
        audioDefaultSort = "2";
        photoDefaultSort = "29";
        gamesDefaultSort = "49";
        genericOptions = "";
    }

    int getMediaType() { return q_mediaType; }
    QString getDataGridId() { return dataGridId; }
    void setDataGridId(QString id) { dataGridId = id; }

    void clear() {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaFilter::clear()");

        filterStack.clear();
        //datagrid option variables
        //  QString q_mediaType;           //1
        q_subType="";             //2
        q_fileFormat="";          //3
        q_attribute_genres="";    //4
        q_mediaSources ="1,2";         //5 need comma delineation
        q_usersPrivate = "0";        //6
        q_attributetype_sort="";  //7
        q_pk_users="0";             //8
        q_last_viewed=" 2 ";        //9
        q_pk_attribute="";        //10
        qs_seek ="";
        // 4||||1,2|0|3|0 | 2 |

        // 4||||1,2|0 |2|0|2|

        // 5||||1,2|0|0|0 | 2 |

        /*	if (q_mediaType > 0) {
        // Don't add this to the stack if no media type is set
        QString datagridVariableString ;
        datagridVariableString.append(QString::number(q_mediaType)).append("|").append(q_subType).append("|").append(q_fileFormat).append("|").append(q_attribute_genres).append("|").append(q_mediaSources).append("|").append(q_usersPrivate).append("|").append(q_attributetype_sort).append("|").append(q_pk_users).append("|").append(q_last_viewed).append("|").append(q_pk_attribute);
        filterStack.append(datagridVariableString);
        }*/
    }

    /*
     * Set a custom filter based on the user choosing an attribute directly,
     * such as from the file details screen
     *
     */
    void setFilterFromMediaGrid(int attributeType, int attribute){

        q_subType="";                                     //2
       // q_fileFormat="";                                  //3
        q_attribute_genres="";                            //4
        q_mediaSources ="1,2";                            //5 need comma delineation
        q_usersPrivate = "0";                             //6
        q_attributetype_sort="";                          //7
        q_pk_users="0";                                   //8
        q_last_viewed=" 2 ";                              //9
        q_pk_attribute=QString::number(attribute);        //10
        qs_seek ="";

        updateAttributeToList();
        emit filterChanged(q_mediaType);
        emit filterStringChanged(dataGridId);
        newMediaFilter();
    }

    /*
     * Set a filter parameter
     * Calling this will trigger filterChanged+filterStringChanged signals
     */
    void setStringParam(int paramType, QString param) {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaFilter::setStringParam() type = %d, param = %s", paramType, param.toStdString().c_str());
        /*
    QString q_subType;             //1
    QString q_fileFormat;          //2
    QString q_attribute_genres;    //3
            q_pk_attribute;        //4
    QString q_mediaSources;        //4? needs comma seperator

    QString q_usersPrivate;        //5
    QString q_attributetype_sort;  //6
    QString q_pk_users;            //7
    QString q_last_viewed;         //8
    QString q_pk_attribute;        //9
      */

        // !P = playlist, !D = directory, !F = file, !A = attribute

        switch (paramType)
        {
        case 0: q_mediaType = param.toInt(); break;
        case 1: q_subType = param;           break;
        case 2: q_fileFormat = param;        break;
        case 3: q_attribute_genres = param;  break;
        case 4:
            if (!param.contains("!D")){                                                           //not browsing by directory
                qDebug() << "Browse by file not engaged.";
                if(param.contains("!F") || param.contains("!P"))  {
                    emit itemSelected(param);               // we found a file or playlist marker
                } else  {
                    q_pk_attribute = param.remove("!A");    // we found an attribute marker
                }
            } else{                                                                                                  // 5||||1,2	!D'/home/public/data/videos/1.82 TB (sdd1) ST2000DL003-9VT [51]'|0|0|0 | 2 |
                qDebug() << "Browsing by file";
                int t = param.indexOf("\t");
                QString apnd = param.mid(t);
                q_mediaSources.append(apnd);
                qDebug() << "mediaFilter()::Current Media sources "<< q_mediaSources;
            }
            break;
        case 5: q_usersPrivate = "0,"+param; break;
        case 6: if (param.contains("!P")) { emit itemSelected(param);  break; }
            else {
                q_attributetype_sort = param;
                if(q_attributetype_sort == "0"){
                    q_pk_attribute="";
                    q_subType ="";
                }
                break;
            }
        case 7: q_pk_users = param;          break;
        case 8: q_last_viewed = param;       break;
        case 9:
            if(param.contains("!F") || param.contains("!P")) { emit itemSelected(param); break; }
            else { q_pk_attribute = param.remove("!A");  }
            break;
        default: qDebug() << "No Parameter set for media filter!"; break;
        }

        // Does this new filter parameter cause a change in the attribute to list?
        if(param.contains("!F") || param.contains("!P")) {
            return;
        }
        updateAttributeToList();
        emit filterChanged(q_mediaType);
        emit filterStringChanged(dataGridId);
        newMediaFilter();
    }

    /*
     * Update the attributetype_sort (what attribute to list) based on the previous type.
     * Called to update the string after a change in the browse by attribute
     */
    bool updateAttributeToList() {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaFilter::updateAttributeToList()");

        //video
        if (q_mediaType == 5)
        {
            if (q_attributetype_sort == "")
            {
                q_attributetype_sort = videoDefaultSort;
            }
            else if(q_attributetype_sort == "12" && q_pk_attribute !="") //catches program
            {
                q_attributetype_sort= "52";
            }
            else if(q_attributetype_sort == "1" && q_pk_attribute !="") //catches director
            {
                q_attributetype_sort = "13";
            }
            else if (q_attributetype_sort =="2" && q_pk_attribute !="" ) // performer - sets filter to title
            {
                q_attributetype_sort = "13";
            }
            else if (q_attributetype_sort =="8" && q_pk_attribute !="") // genre?
            {
                q_attributetype_sort = "13";
            }
            else if(q_attributetype_sort =="52" && q_pk_attribute !="")
            {
                q_attributetype_sort = "13";
            }
            else if(q_attributetype_sort =="10" && q_pk_attribute !="")
            {
                q_attributetype_sort = "12";
            }
            else if (q_pk_attribute !=""){
                q_attributetype_sort = "13";
            }
            else if(q_subType == "1" ){
                if(q_attributetype_sort ==""||"13"){
                    q_attributetype_sort = "12";
                }
            }
            else if(q_subType == "2"){
                if(q_attributetype_sort ==""){
                    q_attributetype_sort ="13";
                }
            }
        }

        //audio
        if (q_mediaType == 4)
        {
            if (q_attributetype_sort == "" )
            {
                q_attributetype_sort = audioDefaultSort;
            }
            else if(q_attributetype_sort == "2" && q_pk_attribute !="" ) //catches performer
            {
                q_attributetype_sort= "3";
            }
            else if (q_attributetype_sort =="8"  && q_pk_attribute !="") //catches genre
            {
                q_attributetype_sort = "2";
            }
            else if(q_attributetype_sort =="3" && q_pk_attribute !="") //album
            {
                q_attributetype_sort = "13";
            } else if(q_attributetype_sort == "" && q_pk_attribute !="" ) {
                q_attributetype_sort = "3";
            }
        }
        //photos
        if (q_mediaType == 7)
        {
            if (q_attributetype_sort == "" )
            {
                qDebug() << "Browsing by PhotoDefaultSort ;"<<photoDefaultSort;
                q_attributetype_sort = photoDefaultSort;
            } else if (q_attributetype_sort != "" && q_pk_attribute !="") {
                q_attributetype_sort = 13 ;
            }
        }
        //radio (streaming)
        if (q_mediaType == 43)
        {
            if (q_attributetype_sort == "" )
            {
                q_attributetype_sort = "8"; // first genre
            }
            else if(q_attributetype_sort == "8" && q_pk_attribute !="" ) // genre -> channel
            {
                q_attributetype_sort= "10";
            }
        }

        //playlists


        //games

        if(q_attributetype_sort=="0"){
            q_usersPrivate = "0";
        }


        QString s = getFilterString();
        // Don't add any filter with mediaType == 0
        if(q_mediaType > 0 && (filterStack.count() == 0 || !filterStack.at(filterStack.count()-1).contains(s))){
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaFilter::updateAttributeToList() new filter string: %s", s.toStdString().c_str());
            filterStack<< s;
            emit newMediaFilter();
            return true;
        }
        return false;
    }

    QString getFilterString() {
        //       qDebug() << "Current Media type is " << q_mediaType;
        QStringList paramList;
        paramList << QString::number(q_mediaType) << q_subType << q_fileFormat << q_attribute_genres << q_mediaSources << q_usersPrivate << q_attributetype_sort << q_pk_users << q_last_viewed << q_pk_attribute;
        //qDebug() <<"Desired Output::" << paramList.join("|");
        QString params = QString::number(q_mediaType)+"|"+q_subType +"|"+q_fileFormat+"|"+q_attribute_genres+"|"+q_mediaSources+"|"+q_usersPrivate +"|"+q_attributetype_sort+"|"+q_pk_users+"|"+q_last_viewed+"|"+q_pk_attribute+"";
        //     qDebug() << "Current Output::" << params;
        return params;
    }

public slots:

    void setGenericOptions(QString opt) {if(genericOptions != opt) { genericOptions=opt; emit genericOptionsChanged(dataGridId);} }
    QString getGenericOptions(){return genericOptions;}

    void setMediaType(int mediaType) {
        if (mediaType != q_mediaType)
        {
            clear();

            q_mediaType = mediaType;
            qDebug() << "!!!!!!!!!!!!!!! MODEL CLEAR";
            currentMediaTypeChanged(QString::number(q_mediaType));
            updateAttributeToList();
        }
    }

    /*
     * Signals the media filter to go back to the previous filter setting
     */
    bool goBack() {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaFilter::goBack() stack count = %d", filterStack.count());
        // always keep one filter on stack, this is the initial filter
        if(filterStack.count() > 1)
        {
            // remove current filter
            filterStack.removeLast();
            // load previous filter
            int back = filterStack.count()-1;
            QStringList reverseParams = filterStack.at(back).split("|", QString::KeepEmptyParts);

            //	    if(!filterStack.empty()){
            q_mediaType = reverseParams.first().toInt();
            q_subType = reverseParams.at(1);
            q_fileFormat = reverseParams.at(2);
            q_attribute_genres = reverseParams.at(3);
            q_mediaSources = reverseParams.at(4);
            q_usersPrivate = reverseParams.at(5);
            q_attributetype_sort = reverseParams.at(6);
            q_pk_users = reverseParams.at(7);
            q_last_viewed = reverseParams.at(8);
            q_pk_attribute = reverseParams.at(9);
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaFilter::goBack() new filter string: %s", getFilterString().toStdString().c_str());
            emit filterChanged(q_mediaType);
            emit filterStringChanged(dataGridId);

            newMediaFilter();
            return true;
            //}
        }
        return false;
    }

    /*
     * signal that no media was found, and allow the filter to adjust
     */
    void noMedia() {
        LoggerWrapper::GetInstance()->Write(LV_STATUS, "MediaFilter::noMedia()");

        if(q_attributetype_sort=="52" /* TODO: only call this method when going deeper in the media browser && backwards==false */)
        {
            q_attributetype_sort=="11";
            filterStack.removeLast();

            // backwards = false;
            emit filterChanged(q_mediaType);
            emit filterStringChanged(dataGridId);
            newMediaFilter();
        }
    }

    void setPrivateUser(int u){
        qDebug() << Q_FUNC_INFO  << u;
        setStringParam(5, QString::number(u));
    }

signals:
    void filterStringChanged(QString dataGridId);
    void filterChanged(int PK_MediaType);
    void itemSelected(QString id);
    void currentMediaTypeChanged(QString m);
    void newMediaFilter();
    void genericOptionsChanged(QString dataGridId);
    void backIndexChanged(int idx);
};
}
#endif
