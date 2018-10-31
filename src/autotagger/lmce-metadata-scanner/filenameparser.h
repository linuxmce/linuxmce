#pragma once

#include <QObject>
#include <QJsonObject>
#include <QStringList>

class FilenameParser : public QObject
{
    Q_OBJECT
public:
    explicit FilenameParser(QObject *parent = 0);

signals:
    void mediaFileNameParsed(QJsonObject metaData);

public slots:
    void parseFileName(QString fileName);
    void parseFileNameWithData(QString name, int mediaType=-1, int mediaSubtype=-1);


private slots:
    bool setupFilters();

private:
    QStringList m_videoFileNames;
    QStringList m_televisionStrings;
    QStringList m_movieStrings;
    QStringList m_specialStrings;
};


