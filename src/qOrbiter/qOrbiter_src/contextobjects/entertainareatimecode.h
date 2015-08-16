#ifndef ENTERTAINAREATIMECODE_H
#define ENTERTAINAREATIMECODE_H

#include <QObject>

class EntertainAreaTimeCode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentTimeCode READ currentTimeCode  NOTIFY currentTimeCodeChanged)
    Q_PROPERTY(QString totalTimeCode READ totalTimeCode NOTIFY totalTimeCodeChanged)

public:
    EntertainAreaTimeCode();
    EntertainAreaTimeCode(QString time, QString totalTime);
    EntertainAreaTimeCode(int ea, QString eaName);

    virtual ~EntertainAreaTimeCode() {}

    int eaId() const;
    void setEaId(int value);

    QString eaName() const;
    void setEaName(const QString &eaName);

signals:
    void totalTimeCodeChanged();
    void currentTimeCodeChanged();
    void eaIdChanged();
    void eaNameChanged();

public slots:
    QString currentTimeCode() const;
    void setCurrentTimeCode(const QString &currentTimeCode);

    QString totalTimeCode() const;
    void setTotalTimeCode(const QString &totalTimeCode);

private:
    QString m_currentTimeCode;
    QString m_totalTimeCode;
    int i_eaId;
    QString m_eaName;
};

#endif // ENTERTAINAREATIMECODE_H
