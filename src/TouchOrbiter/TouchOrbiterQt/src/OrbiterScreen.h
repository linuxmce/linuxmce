#ifndef ORBITERSCREEN_H
#define ORBITERSCREEN_H
//

#include <QObject>

#include <QPixmap>
#include <QDomDocument>
#include <QMap>
#include <QStringList>

//
class OrbiterScreen : public QObject  
{
Q_OBJECT

public:
	OrbiterScreen(QObject *parent = 0);
	virtual ~OrbiterScreen();
	
	// FlatImage
	void setFlatImage(QPixmap *Pixmap);
	QPixmap *getFlatImage();
	
	// ScreenXml
	void setScreenXml(QDomDocument *Xml);
	QDomDocument *getScreenXml();
	
	// DatagridXml
	void setDatagridXml(const QString &GridID, QDomDocument *Xml);
	QDomDocument *getDatagridXml(const QString &GridID);
	QStringList getDatagridNames();
	void clearDatagrids();
	
	// InProgress
	void setInProgress(bool bInProgress);
	bool getInProgress(); 
	
private:
	QPixmap *FlatImage;
	QDomDocument *ScreenXml;
	QMap<QString, QDomDocument *> DatagridXml;
	bool InProgress;
};
#endif
