/* ============================================================
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include <iostream>
#include <unistd.h>

// myth
#include <mythcontext.h>
#include <mythversion.h>
#include <mythpluginapi.h>
#include <mythmainwindow.h>
#include <myththemedmenu.h>
#include <mythuihelper.h>

//zone minder
#include "zmsettings.h"
#include "zmconsole.h"
#include "zmliveplayer.h"
#include "zmevents.h"
#include "zmclient.h"

using namespace std;

void runZMConsole(void);
void runZMLiveView(void);
void runZMEventView(void);

void setupKeys(void)
{
    REG_JUMP(QT_TRANSLATE_NOOP("MythControls", "ZoneMinder Console"),
        "", "", runZMConsole);
    REG_JUMP(QT_TRANSLATE_NOOP("MythControls", "ZoneMinder Live View"),
        "", "", runZMLiveView);
    REG_JUMP(QT_TRANSLATE_NOOP("MythControls", "ZoneMinder Events"),
        "", "", runZMEventView);
}

bool checkConnection(void)
{
    if (!ZMClient::get()->connected())
    {
        if (!ZMClient::setupZMClient())
            return false;
    }

    return true;
}

int mythplugin_init(const char *libversion)
{
    if (!gContext->TestPopupVersion("mythzoneminder",
                                    libversion,
                                    MYTH_BINARY_VERSION))
        return -1;

    setupKeys();

    return 0;
}

void runZMConsole(void)
{
    if (!checkConnection())
        return;

    MythScreenStack *mainStack = GetMythMainWindow()->GetMainStack();

    ZMConsole *console = new ZMConsole(mainStack);

    if (console->Create())
        mainStack->AddScreen(console);
}

void runZMLiveView(void)
{
    if (!checkConnection())
        return;


    MythScreenStack *mainStack = GetMythMainWindow()->GetMainStack();

    ZMLivePlayer *player = new ZMLivePlayer(mainStack);

    if (player->Create())
        mainStack->AddScreen(player);
}

void runZMEventView(void)
{
    if (!checkConnection())
        return;

    MythScreenStack *mainStack = GetMythMainWindow()->GetMainStack();

    ZMEvents *events = new ZMEvents(mainStack);

    if (events->Create())
        mainStack->AddScreen(events);
}

void ZoneMinderCallback(void *data, QString &selection)
{
    (void) data;

    QString sel = selection.toLower();

    if (sel == "zm_console")
        runZMConsole();
    else if (sel == "zm_live_viewer")
        runZMLiveView();
    else if (sel == "zm_event_viewer")
        runZMEventView();
}

int runMenu(QString which_menu)
{
    QString themedir = GetMythUI()->GetThemeDir();

    MythThemedMenu *diag = new MythThemedMenu(
        themedir, which_menu, GetMythMainWindow()->GetMainStack(),
        "zoneminder menu");

    diag->setCallback(ZoneMinderCallback, NULL);
    diag->setKillable();

    if (diag->foundTheme())
    {
        GetMythMainWindow()->GetMainStack()->AddScreen(diag);
        return 0;
    }
    else
    {
        VERBOSE(VB_IMPORTANT, QString("Couldn't find menu %1 or theme %2")
                              .arg(which_menu).arg(themedir));
        delete diag;
        return -1;
    }
}

int mythplugin_run(void)
{
    // setup a connection to the mythzmserver
    if (!ZMClient::setupZMClient())
    {
        return -1;
    }

    return runMenu("zonemindermenu.xml");
}

int mythplugin_config(void)
{
    ZMSettings settings;
    settings.exec();

    return 0;
}



