#ifndef ENTERTAINAREATIMECODE_H
#define ENTERTAINAREATIMECODE_H

#include <QObject>

class EntertainAreaTimeCode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentTimeCode READ currentTimeCode  NOTIFY currentTimeCodeChanged)
    Q_PROPERTY(QString totalTimeCode READ totalTimeCode NOTIFY totalTimeCodeChanged)
    Q_PROPERTY(int timecodePosition READ timecodePosition NOTIFY currentTimeCodeChanged)
    Q_PROPERTY(int timecodeLength READ timecodeLength NOTIFY totalTimeCodeChanged)
    Q_PROPERTY(int dragTime READ dragTime WRITE setDragTime NOTIFY dragTimeChanged)
    Q_PROPERTY(QString dragString READ dragString NOTIFY dragStringChanged)

public:
    EntertainAreaTimeCode();
    EntertainAreaTimeCode(QString time, QString totalTime);
    EntertainAreaTimeCode(int ea, QString eaName, int room);

    virtual ~EntertainAreaTimeCode() {}

    int eaId() const;
    void setEaId(int value);

    QString eaName() const;
    void setEaName(const QString &eaName);



    int timecodeLength() const;
    void setTimecodeLength(int timecodeLength);

    int timecodePosition() const;
    void setTimecodePosition(int timecodePosition);

    QString dragString() const;
    void setDragString(const QString &dragString);

    int dragTime() const;
    void setDragTime(int dragTime);

    int getI_roomId() const;
    void setI_roomId(int value);

signals:
    void totalTimeCodeChanged();
    void currentTimeCodeChanged();
    void eaIdChanged();
    void eaNameChanged();
    void seekToTime(QString time);
    void dragStringChanged();
    void dragTimeChanged();

public slots:

    Q_INVOKABLE void finishDragging(int i_dragTime);

    QString currentTimeCode() const;
    void setCurrentTimeCode(const QString &currentTimeCode);

    QString totalTimeCode() const;
    void setTotalTimeCode(const QString &totalTimeCode);

private:
    int m_timecodeLength;
    int m_timecodePosition;
    QString m_currentTimeCode;
    QString m_totalTimeCode;
    int i_eaId;
    int i_roomId;
    QString m_eaName;

    QString m_dragString;
    int m_dragTime;
};

#endif // ENTERTAINAREATIMECODE_H
