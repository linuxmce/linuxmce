#ifndef GWEATHERMODEL_H
#define GWEATHERMODEL_H

#include <QAbstractListModel>
#include <QUrl>
#include <QtNetwork/QtNetwork>
#include <QtXml/QDomDocument>
#if   (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
//FIXME: Another unnecessary hdr?
#else
#include <QtDeclarative/QDeclarativeItem>
#endif

class gWeatherItem: public QObject
{
    Q_OBJECT
public:
    enum Roles {
        DayRole = Qt::UserRole+1,
        LowRole =Qt::UserRole+2,
        HighRole= Qt::DisplayRole+3,
        IconRole = Qt::DisplayRole+4,
        ConditionRole = Qt::UserRole+5
    };
    gWeatherItem() {}
    explicit gWeatherItem(QString &day, int low, int high, QString icon, QString condition):
        forecastDay(day), forecastLow(low), forecastHigh(high), forecastIcon(icon), forecastCondition(condition) {
    }

    QString forecastDay;
    int forecastLow;
    int forecastHigh;
    QString forecastIcon;
    QString forecastCondition;

    inline QString getDay() const {return forecastDay;}
    inline int getLow() const {return forecastLow; }
    inline int getHigh() const {return forecastHigh; }
    inline QUrl getIcon() const {return QUrl(forecastIcon);}
    inline QString getCondition() const {return forecastCondition;}

    QHash<int, QByteArray> roleNames() const
    {
        QHash<int, QByteArray> names;
        names[DayRole] = "day";
        names[LowRole] = "lowTemp";
        names[HighRole] = "highTemp";
        names[IconRole] = "icon";
        names[ConditionRole] = "condition";
        return names;
    }

    QVariant data(int role) const
    {
        switch(role) {
        case DayRole:
            return getDay();
        case LowRole:
            return getLow();
        case HighRole:
            return getHigh();
        case IconRole:
            return getIcon();
        case ConditionRole:
            return getCondition();
        default:
            return QVariant();
        }
    }



};

class gWeatherModel : public QAbstractListModel
{

    Q_OBJECT
    Q_PROPERTY (QString statusMessage READ getStatus WRITE setStatus NOTIFY statusChanged)
    // Q_PROPERTY (QString unitsSetup READ getUnits WRITE setUnits NOTIFY unitsChanged)
    Q_PROPERTY(QString cityData READ getCityData WRITE setCityData NOTIFY cityDataChanged)
    Q_PROPERTY(QString postalCode READ getPostalCode WRITE setPostalCode NOTIFY postalCodeChanged)
    Q_PROPERTY(QString dayOfWeek READ getDay WRITE setDay NOTIFY dayChanged)
    Q_PROPERTY(QString windCondition READ getWind WRITE setWind NOTIFY windChanged)
    Q_PROPERTY(QString currentConditions READ getCondition WRITE setCondition NOTIFY conditionsChanged)
    Q_PROPERTY(QString currentIcon READ getIcon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(int currentTemp READ getTemp WRITE setTemp NOTIFY currentTempChanged)

public:
    //subclass specific functions from QAbstractListModel
    gWeatherModel() {}
    explicit gWeatherModel(gWeatherItem* prototype, QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent) const;

    bool appendRow(gWeatherItem * item);
    //end subclass related

    QString unitsSetup;
    QString getUnits() {return unitsSetup;}
    void setUnits(QString u) {unitsSetup = u; emit unitsChanged();}

    int currentTemp;
    int getTemp() {return currentTemp;}
    void setTemp(int t) {currentTemp = t; emit currentTempChanged();}

    QUrl googleDataSource;

    QNetworkAccessManager * dataManager;
    QNetworkReply *reply;
    QDomDocument* xmlData;

    QString cityData;
    QString getCityData() {return cityData;}
    void setCityData(QString c) { cityData = c; emit cityDataChanged();}

    QString postalCode;
    QString getPostalCode() { return postalCode;}
    void setPostalCode(QString p) {postalCode = p; emit postalCodeChanged();}

    QString currentTime;
    QString getCurrentTime() {return currentTime;}
    void setCurrentTime(QString t) {currentTime = t; emit currentTimeChanged();}

    QString dayOfWeek;
    QString getDay() {return dayOfWeek;}
    void setDay (QString day) {dayOfWeek = day; emit dayChanged();}

    QString windCondition;
    QString getWind() {return windCondition;}
    void setWind (QString l ) {windCondition = l; emit windChanged();}

    QString currentConditions;
    QString getCondition() {return currentConditions;}
    void setCondition(QString t) { currentConditions = t; emit conditionsChanged();}

    int tempC;
    int tempF;

    QString humidity;
    QString getHumidity() {return humidity;}
    void setHumidity(QString h) { humidity = h; emit humidityChanged();}

    QString currentIcon;
    QString getIcon() {return currentIcon;}
    void setIcon(QString ic) {currentIcon = "http://www.google.com"+ic; emit iconChanged();}

    //internal properties
    QString statusMessage;


    //functions for getting our
signals:
    void statusChanged();
    void cityDataChanged();
    void dayChanged();
    void iconChanged();
    void conditionsChanged();
    void windChanged();
    void postalCodeChanged();
    void currentTimeChanged();
    void unitsChanged();
    void currentTempChanged();
    void humidityChanged();
    void close();

public slots:
    void getWeather(const QString& location);
    void recieveReply(QNetworkReply* );
    void exitApp() {emit close();}

private:
    QList <gWeatherItem*> m_list;
    void processXML();
    void processCurrent();
    void processModelItems();
    QString getStatus() {return statusMessage;}
    void setStatus(QString s) {statusMessage = s;  emit statusChanged();}
    gWeatherItem * m_prototype;
};

#endif // GWEATHERMODEL_H
