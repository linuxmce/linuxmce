#include "OrbiterScreen.h"
//
OrbiterScreen::OrbiterScreen(QObject *parent)
	: QObject(parent), FlatImage(NULL), ScreenXml(NULL), InProgress(false)
{
}
//
OrbiterScreen::~OrbiterScreen()
{
	setFlatImage(NULL);
	setScreenXml(NULL);
	clearDatagrids();
}
//
void OrbiterScreen::setFlatImage(QPixmap *Pixmap)
{
	delete FlatImage;
	FlatImage = Pixmap;
}
//
QPixmap *OrbiterScreen::getFlatImage()
{
	return FlatImage;
}
//
void OrbiterScreen::setScreenXml(QDomDocument *Xml)
{
	delete ScreenXml;
	ScreenXml = Xml;
}
//
QDomDocument *OrbiterScreen::getScreenXml()
{
	return ScreenXml;
}
//
void OrbiterScreen::setDatagridXml(const QString &GridID, QDomDocument *Xml)
{
	if (DatagridXml.find(GridID) != DatagridXml.end())
		delete DatagridXml[GridID];
	DatagridXml[GridID] = Xml;
}
//
QDomDocument *OrbiterScreen::getDatagridXml(const QString &GridID)
{
	if (DatagridXml.find(GridID) == DatagridXml.end())
		return NULL;
	return DatagridXml[GridID];
}
//
QStringList OrbiterScreen::getDatagridNames()
{
	return DatagridXml.keys();
}
//
void OrbiterScreen::clearDatagrids()
{
	QMap<QString, QDomDocument *>::iterator It;
	for (It = DatagridXml.begin(); It != DatagridXml.end(); It++)
		delete It.value();
	DatagridXml.clear();
}
//
void OrbiterScreen::setInProgress(bool bInProgress)
{
	InProgress = bInProgress;
}
//
bool OrbiterScreen::getInProgress()
{
	return InProgress;
}
