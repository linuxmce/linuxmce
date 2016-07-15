#ifndef FONTSHELPER_H
#define FONTSHELPER_H

#include <QObject>
#include <QFontDatabase>
#include <QStringList>
#include <QVariant>

class SystemFontItem : public QObject
{
    Q_PROPERTY(QString familyName READ familyName NOTIFY familyNameChanged)
    Q_PROPERTY(QStringList styles READ styles  NOTIFY stylesChanged)


    Q_OBJECT
public:
    SystemFontItem() {}
    virtual ~SystemFontItem() {}

    QString familyName() const;
    void setFamilyName(const QString &familyName);

    QStringList styles() const;
    void setStyles(const QStringList &styles);

    QList<int *> sizes() const;
    void setSizes(const QList<int *> &sizes);

   signals:
    void stylesChanged();
    void sizesChanged();
    void familyNameChanged();

private:
    QString m_familyName;
    QStringList m_styles;
    QList<int *> m_sizes;
};

class FontsHelper : public QObject
{
    Q_PROPERTY(QList<QObject*> fontList READ fontList NOTIFY fontListChanged)
    Q_PROPERTY(QString currentFont READ currentFont WRITE setCurrentFont NOTIFY currentFontChanged)
    Q_PROPERTY(QString currentStyle READ currentStyle WRITE setCurrentStyle NOTIFY currentStyleChanged)
    Q_OBJECT
public:
    explicit FontsHelper(QObject *parent = 0);

    QList<QObject *> fontList() const;
    void setFontList(const QList<QObject *> &fontList);

    QString currentFont() const;
    void setCurrentFont(const QString &currentFont);

    QString currentStyle() const;
    void setCurrentStyle(const QString &currentStyle);

signals:
    void fontListChanged();
    void currentStyleChanged();
    void currentFontChanged(QString font);

public slots:
    QVariant getFonts();

private slots:
    void addFontItem(SystemFontItem*font);

private:
    QList<QObject*> m_fontList;

    QString m_currentFont;
    QString m_currentStyle;

};

#endif // FONTSHELPER_H
