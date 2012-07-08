#include <plugins/GoogleWeather/googleweather.h>
#ifdef debug
#include "QDebug"
#endif
#include <QtXml/QDomDocument>

gWeatherModel::gWeatherModel(gWeatherItem *prototype, QObject *parent) :
    QAbstractListModel(parent), m_prototype(prototype)
{
    setRoleNames(m_prototype->roleNames());
    qRegisterMetaType<QModelIndex>("QModelIndex");
    googleDataSource.setEncodedUrl("http://www.google.com/ig/api?weather=");
    setStatus("Starting");
    m_list.clear();

}


QVariant gWeatherModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= m_list.size())
        return QVariant();
    return m_list.at(index.row())->data(role);
}

QVariant gWeatherModel::headerData(int section, Qt::Orientation orientation, int role) const
{
}

int gWeatherModel::rowCount(const QModelIndex &parent) const
{
    return m_list.count();
}

void gWeatherModel::getWeather(const QString &location)
{
    if (!m_list.isEmpty())
        m_list.clear();

    dataManager = new QNetworkAccessManager();
    QNetworkRequest weatherRequest(googleDataSource.toString()+location);
    QObject::connect(dataManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(recieveReply(QNetworkReply* )));
    reply = dataManager->get(weatherRequest);
}

void gWeatherModel::recieveReply(QNetworkReply* )
{
    xmlData = new QDomDocument();
    xmlData->setContent(reply->readAll());

    setStatus("Got Reply");
    processXML();
}

void gWeatherModel::processXML()
{
    QDomElement forecastData = xmlData->firstChildElement("xml_api_reply").firstChild().firstChildElement("forecast_information");
    if(forecastData.isElement())
    {
        setStatus("Processing");

        setCityData(forecastData.firstChildElement("city").attribute("data"));
        setPostalCode(forecastData.firstChildElement("postal_code").attribute("data"));
        setCurrentTime(forecastData.firstChildElement("current_date_time").attribute("data"));
        if(forecastData.firstChildElement("unit_system").attribute("data").contains("US"))
        {setUnits("e");}
        else
        {setUnits("m");}

         //setUnits(forecastData.firstChildElement("unit_system").attribute("data"));
        processCurrent();
    }
    else
    {
#ifdef debug
        qDebug() << xmlData->firstChildElement("weather").nodeName();
#endif
        setStatus("Invalid Weather. Did you a enter real location?");
    }
}

void gWeatherModel::processCurrent()
{
    QDomElement currentData = xmlData->firstChildElement("xml_api_reply").firstChild().firstChildElement("current_conditions");
  setIcon(currentData.firstChildElement("icon").attribute("data"));
    setHumidity(currentData.firstChildElement("humidity").attribute("data"));
   setCondition(currentData.firstChildElement("condition").attribute("data"));
   setWind(currentData.firstChildElement("wind_condition").attribute("data"));
    if(unitsSetup == "e")
    {
       setTemp(currentData.firstChildElement("temp_f").attribute("data").toInt());
    }
    else
    {
        setTemp(currentData.firstChildElement("temp_c").attribute("data").toInt());
    }
    processModelItems();
}

void gWeatherModel::processModelItems()
{

    QDomNodeList dailyForecasts = xmlData->elementsByTagName("forecast_conditions");

    for(int i = 0; i < dailyForecasts.count() ; i++)
    {
       QString day = dailyForecasts.at(i).firstChildElement("day_of_week").attribute("data");
       int low = dailyForecasts.at(i).firstChildElement("low").attribute("data").toInt();
       int high = dailyForecasts.at(i).firstChildElement("high").attribute("data").toInt();
       QString iconUrl = "http://www.google.com"+ dailyForecasts.at(i).firstChildElement("icon").attribute("data");
       QString condition =  dailyForecasts.at(i).firstChildElement("condition").attribute("data");

       appendRow(new gWeatherItem(day,  low,  high,  iconUrl,  condition) );
    }

}



bool gWeatherModel::appendRow(gWeatherItem *item)
{
    beginInsertRows(QModelIndex(), 0, m_list.count());
    m_list.append(item);
    endInsertRows();
}

