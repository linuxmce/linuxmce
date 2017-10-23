#include "fontshelper.h"
#include "qdebug.h"

#include "qfont.h"

FontsHelper::FontsHelper(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<SystemFontItem*>("SystemFontItem*");



    QFontDatabase fontDb;

    foreach (const QString &family, fontDb.families()){
        SystemFontItem *item = new SystemFontItem();

        // qDebug() << "QFontDatabase located font " << family;
        item->setFamilyName(family);
        item->setStyles(fontDb.styles(family));
        addFontItem(item);
    }
}

QList<QObject*> FontsHelper::fontList() const
{
    return m_fontList;
}

QVariant FontsHelper::getFonts(){
    return QVariant::fromValue(m_fontList);
}

void FontsHelper::setFontList(const QList<QObject*> &fontList)
{
    m_fontList = fontList;
    emit fontListChanged();
}

void FontsHelper::addFontItem(SystemFontItem *font)
{

    if(m_fontList.contains(font) )
        return;

    m_fontList.append(font);

    // qDebug() << " Styles \t" << font->styles();
    emit fontListChanged();

}
QString FontsHelper::currentStyle() const
{
    return m_currentStyle;
}

void FontsHelper::setCurrentStyle(const QString &currentStyle)
{
    m_currentStyle = currentStyle;
    emit currentStyleChanged();
}

QString FontsHelper::currentFont() const
{
    return m_currentFont;
}

void FontsHelper::setCurrentFont(const QString &currentFont)
{
    qDebug() << Q_FUNC_INFO << currentFont;
    m_currentFont = currentFont;
    emit currentFontChanged(m_currentFont);


}

QString SystemFontItem::familyName() const
{
    return m_familyName;
}

void SystemFontItem::setFamilyName(const QString &familyName)
{
    m_familyName = familyName;
}

QStringList SystemFontItem::styles() const
{
    return m_styles;
}

void SystemFontItem::setStyles(const QStringList &styles)
{
    m_styles = styles;
}

QList<int *> SystemFontItem::sizes() const
{
    return m_sizes;
}

void SystemFontItem::setSizes(const QList<int *> &sizes)
{
    m_sizes = sizes;
}


