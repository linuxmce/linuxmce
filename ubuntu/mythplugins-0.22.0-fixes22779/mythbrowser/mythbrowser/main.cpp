#include <iostream>
#include <unistd.h>

// qt
#include <QApplication>

// myth
#include <mythtv/mythcontext.h>
#include <mythtv/mythversion.h>
#include <mythtv/mythdialogs.h>
#include <mythtv/mythplugin.h>
#include <mythtv/langsettings.h>
#include <mythtv/mythpluginapi.h>

// mythbrowser
#include "bookmarkmanager.h"
#include "browserdbutil.h"
#include "mythbrowser.h"

using namespace std;

int handleMedia(const QString &url, const QString &, const QString &, const QString &, const QString &, int, int, int, const QString &)
{
    if (url.isEmpty())
    {
        VERBOSE(VB_IMPORTANT, "MythBrowser: handleMedia got empty url!");
        return 1;
    }

    QStringList urls = url.split(" ", QString::SkipEmptyParts);
    float zoom = gContext->GetSetting("WebBrowserZoomLevel", "1.4").toFloat();

    MythScreenStack *mainStack = GetMythMainWindow()->GetMainStack();

    MythBrowser *mythbrowser = new MythBrowser(mainStack, urls, zoom);

    if (mythbrowser->Create())
        mainStack->AddScreen(mythbrowser);
    else
        delete mythbrowser;

    return 0;
}

void setupKeys(void)
{
    REG_KEY("Browser", "NEXTTAB", QT_TRANSLATE_NOOP("MythControls",
        "Move to next browser tab"), "P");
    REG_KEY("Browser", "PREVTAB", QT_TRANSLATE_NOOP("MythControls",
        "Move to previous browser tab"), "");

    REG_MEDIAPLAYER("WebBrowser", QT_TRANSLATE_NOOP("MythControls",
        "Internal Web Browser"), handleMedia);
}

int mythplugin_init(const char *libversion)
{
    if (!gContext->TestPopupVersion("mythbrowser", libversion, MYTH_BINARY_VERSION))
        return -1;
    LanguageSettings::load("mythbrowser");

    UpgradeBrowserDatabaseSchema();

    setupKeys();

    return 0;
}

int mythplugin_run(void)
{
    MythScreenStack *mainStack = GetMythMainWindow()->GetMainStack();

    BookmarkManager *manager = new BookmarkManager(mainStack, "bookmarkmanager");

    if (manager->Create())
    {
        mainStack->AddScreen(manager);
        return 0;
    }
    else
    {
        delete manager;
        return -1;
    }
}

int mythplugin_config(void)
{
    MythScreenStack *mainStack = GetMythMainWindow()->GetMainStack();

    BrowserConfig *config = new BrowserConfig(mainStack, "browserconfig");

    if (config->Create())
    {
        mainStack->AddScreen(config);
        return 0;
    }
    else
    {
        delete config;
        return -1;
    }
}
