#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H
//
#include <QMainWindow>
#include "ui_mainwindow.h"

class QSettings;
class OrbiterBackend;

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
	void ProcessImage(const QPixmap &Image);

private:
	struct Stage
	{
		enum StageName { FirstPage, Orbiter };
	};

	Stage::StageName CurrentStage;
	QSettings *Settings;
	OrbiterBackend *Orbiter;

	void setConnectionFormEnabledState(bool Enabled);
	void StartOrbiter();

	void StartFresh(const QString &Message = "");
	void ApplyUISettings();
	void SaveUISettings();
};
#endif
