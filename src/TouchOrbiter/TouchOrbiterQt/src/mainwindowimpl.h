#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H
//
#include <QMainWindow>
#include "ui_mainwindow.h"

#include <QMap>

class QSettings;
class QString;
class OrbiterBackend;
class OrbiterScreen;

class DatagridWidget;

//
class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public:
	MainWindowImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
	virtual ~MainWindowImpl();

private slots:
	void ConnectPressed();
	void OrbiterKeyPressed(int key);
	void OrbiterTouched(int X, int Y);
	void UseProxyStateChanged(int NewState);

	void ProcessError(const QString &Message);
	void ProcessScreen(OrbiterScreen *Screen);
	
	void DatagridCellClicked(const QString &GridID, int row, int column);

private:
	struct Stage
	{
		enum StageName { FirstPage, OrbiterPage };
	};

	Stage::StageName CurrentStage;
	QSettings *Settings;
	OrbiterBackend *Orbiter;
	QMap<QString, DatagridWidget *> Datagrids;

	void setConnectionFormEnabledState(bool Enabled);
	void StartOrbiter();

	void StartFresh(const QString &Message = "");
	void ApplyUISettings();
	void SaveUISettings();
	
	void ApplyEnhancedUI(OrbiterScreen *Screen);
	void LoadDatagridIntoTable(const QString &GridID, int RowHeight = -1, int ColWidth = -1);
};
#endif
