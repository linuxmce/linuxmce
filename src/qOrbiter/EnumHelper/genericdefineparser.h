#ifndef GENERICDEFINEPARSER_H
#define GENERICDEFINEPARSER_H

#include <QObject>
#include <QFile>
#include <QStringList>
#include <QMap>

class GenericDefineParser : public QObject
{
    Q_OBJECT
    typedef QMap<QString, QString> ReplacementMap;
public:
    explicit GenericDefineParser( QString startingFile, int index=-1, QObject *parent = 0);
    bool parserReady(){return m_isReady;}
    int idx;
    QString name() {return m_dgSourceFile;}
signals:

public slots:
    bool setDefinePattern(QString dgDefinePattern) {m_definePattern =dgDefinePattern; return true;}
    bool setSourceFile(QString dgFile){m_dgSourceFile = dgFile; return true;}
    bool setOutFile(QString outFile){m_dgOutFile = outFile; return true;}
    bool setUsingTags(bool useTags){m_usingTags = useTags; return true;}

    bool setStripList(QStringList strip ){m_stripList = strip; return true;}
    bool setReplacements(ReplacementMap replacements){m_replaceList = replacements; return true;}
    bool setTagList(QStringList tags){m_tagList = tags; return true;}

    void startParsing();


signals:
    void finishedParsing(int index, bool success);

private:

    QString m_definePattern;
    QString m_dgSourceFile;
    QString m_dgOutFile;
    bool m_usingTags;
    QStringList m_tagList;
    ReplacementMap m_replaceList;
    QStringList m_stripList;

    QString outString;
    QFile *m_logFile;

    bool m_isReady;

    bool setupLogFile();
};

#endif // GENERICDEFINEPARSER_H
