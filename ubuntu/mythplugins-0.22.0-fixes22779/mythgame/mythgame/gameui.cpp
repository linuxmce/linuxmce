
#include "gameui.h"

#include <mythcontext.h>
#include <mythuibuttontree.h>
#include <mythuiimage.h>
#include <mythuitext.h>
#include <mythuistatetype.h>
#include <mythdialogbox.h>
#include <mythgenerictree.h>

// MythGame headers
#include "gamehandler.h"
#include "rominfo.h"
#include "gamedetails.h"
#include "romedit.h"

class GameTreeInfo 
{
  public:
    GameTreeInfo(const QString& levels, const QString& filter)
      : m_levels(QStringList::split(" ", levels))
      , m_filter(filter)
    {
    }

    int getDepth() const                        { return m_levels.size(); }
    const QString& getLevel(unsigned i) const   { return m_levels[i]; }
    const QString& getFilter() const            { return m_filter; }

  private:
    QStringList m_levels;
    QString m_filter;
};

Q_DECLARE_METATYPE(GameTreeInfo *)

GameUI::GameUI(MythScreenStack *parent)
       : MythScreenType(parent, "GameUI")
{
}

GameUI::~GameUI()
{
}

bool GameUI::Create()
{
    if (!LoadWindowFromXML("game-ui.xml", "gameui", this))
        return false;

    bool err = false;
    UIUtilE::Assign(this, m_gameUITree, "gametreelist", &err);
    UIUtilW::Assign(this, m_gameTitleText, "title");
    UIUtilW::Assign(this, m_gameSystemText, "system");
    UIUtilW::Assign(this, m_gameYearText, "year");
    UIUtilW::Assign(this, m_gameGenreText, "genre");
    UIUtilW::Assign(this, m_gameFavouriteState, "favorite");
    UIUtilW::Assign(this, m_gamePlotText, "description");
    UIUtilW::Assign(this, m_gameImage, "screenshot");
    UIUtilW::Assign(this, m_fanartImage, "fanart");
    UIUtilW::Assign(this, m_boxImage, "coverart");

    if (err)
    {
        VERBOSE(VB_IMPORTANT, "Cannot load screen 'gameui'");
        return false;
    }
    
    connect(m_gameUITree, SIGNAL(itemClicked(MythUIButtonListItem*)),
            this, SLOT(itemClicked(MythUIButtonListItem*)));

    connect(m_gameUITree, SIGNAL(nodeChanged(MythGenericTree*)),
            this, SLOT(nodeChanged(MythGenericTree*)));

    m_gameShowFileName = gContext->GetSetting("GameShowFileNames").toInt();

    m_gameTree = new MythGenericTree("game root", 0, false);

    //  create system filter to only select games where handlers are present
    QString systemFilter;

    // The call to GameHandler::count() fills the handler list for us
    // to move through.
    unsigned handlercount = GameHandler::count();

    for (unsigned i = 0; i < handlercount; ++i)
    {
        QString system = GameHandler::getHandler(i)->SystemName();
        if (i == 0)
            systemFilter = "system in ('" + system + "'";
        else
            systemFilter += ",'" + system + "'";
    }
    if (systemFilter.isEmpty())
    {
        systemFilter = "1=0";
        VERBOSE(VB_GENERAL, QString("Couldn't find any game handlers!"));
    }
    else
        systemFilter += ")";

    m_showHashed = gContext->GetSetting("GameTreeView").toInt();

    //  create a few top level nodes - this could be moved to a config based
    //  approach with multiple roots if/when someone has the time to create
    //  the relevant dialog screens

    QString levels = gContext->GetSetting("GameFavTreeLevels");

    MythGenericTree *new_node = new MythGenericTree("Favourites", 1, true);
    new_node->SetData(qVariantFromValue(
                new GameTreeInfo(levels, systemFilter + " and favorite=1")));
    m_favouriteNode = m_gameTree->addNode(new_node);

    levels = gContext->GetSetting("GameAllTreeLevels");

    if (m_showHashed)
    {
        int pos = levels.find("gamename",0);
        if (pos >= 0)
            levels.insert(pos, " hash ");
    }

    new_node = new MythGenericTree(tr("All Games"), 1, true);
    new_node->SetData(qVariantFromValue(
                new GameTreeInfo(levels, systemFilter)));
    m_gameTree->addNode(new_node);

    new_node = new MythGenericTree(tr("-   By Genre"), 1, true);
    new_node->SetData(qVariantFromValue(
                new GameTreeInfo("genre gamename", systemFilter)));
    m_gameTree->addNode(new_node);

    new_node = new MythGenericTree(tr("-   By Year"), 1, true);
    new_node->SetData(qVariantFromValue(
                new GameTreeInfo("year gamename", systemFilter)));
    m_gameTree->addNode(new_node);

    new_node = new MythGenericTree(tr("-   By Name"), 1, true);
    new_node->SetData(qVariantFromValue(
                new GameTreeInfo("gamename", systemFilter)));
    m_gameTree->addNode(new_node);

    new_node = new MythGenericTree(tr("-   By Publisher"), 1, true);
    new_node->SetData(qVariantFromValue(
                new GameTreeInfo("publisher gamename", systemFilter)));
    m_gameTree->addNode(new_node);

    m_gameUITree->AssignTree(m_gameTree);

    BuildFocusList();

    return true;
}

bool GameUI::keyPressEvent(QKeyEvent *event)
{
    if (GetFocusWidget()->keyPressEvent(event))
        return true;

    bool handled = false;
    QStringList actions;
    handled = GetMythMainWindow()->TranslateKeyPress("Game", event, actions);

    for (int i = 0; i < actions.size() && !handled; i++)
    {
        QString action = actions[i];
        handled = true;

        if (action == "MENU")
            showMenu();
        else if (action == "INFO")
            showInfo();
        else if (action == "TOGGLEFAV")
            toggleFavorite();
        else if (action == "INCSEARCH")
            searchStart();
        else if (action == "INCSEARCHNEXT")
            searchStart();
        else
            handled = false;
    }

    if (!handled && MythScreenType::keyPressEvent(event))
        handled = true;

    return handled;
}

void GameUI::nodeChanged(MythGenericTree* node)
{
    if (!node)
        return;

    if (!isLeaf(node))
    {
        if (node->childCount() == 0 || node == m_favouriteNode)
        {
            node->deleteAllChildren();
            fillNode(node);
        }
        clearRomInfo();
    }
    else
    {
        RomInfo *romInfo = qVariantValue<RomInfo *>(node->GetData());
        if (!romInfo)
            return;
        if (romInfo->Romname().isEmpty())
            romInfo->fillData();
        updateRomInfo(romInfo);
        if (!romInfo->Screenshot().isEmpty() || !romInfo->Fanart().isEmpty() ||
            !romInfo->Boxart().isEmpty())
            showImages();
        else
        {
            if (m_gameImage)
                m_gameImage->Reset();
            if (m_fanartImage)
                m_fanartImage->Reset();
            if (m_boxImage)
                m_boxImage->Reset();
        }
    }
}

void GameUI::itemClicked(MythUIButtonListItem*)
{
    MythGenericTree *node = m_gameUITree->GetCurrentNode();
    if (isLeaf(node))
    {
        RomInfo *romInfo = qVariantValue<RomInfo *>(node->GetData());
        if (!romInfo)
            return;
        if (romInfo->RomCount() == 1)
        {
            GameHandler::Launchgame(romInfo, NULL);
        }
        else
        {
            QString msg = tr("Choose System for") +
                              ":\n" + node->getString();
            MythScreenStack *popupStack = GetMythMainWindow()->
                                              GetStack("popup stack");
            MythDialogBox *chooseSystemPopup = new MythDialogBox(
                msg, popupStack, "chooseSystemPopup");

            if (chooseSystemPopup->Create())
            {
                chooseSystemPopup->SetReturnEvent(this, "chooseSystemPopup");
                QString all_systems = romInfo->AllSystems();
                QStringList players = QStringList::split(",", all_systems);
                for (QStringList::Iterator it = players.begin();
                     it != players.end(); ++it)
                {
                    chooseSystemPopup->AddButton(*it);
                }
                chooseSystemPopup->AddButton(tr("Cancel"));
                popupStack->AddScreen(chooseSystemPopup);
            }
            else
                delete chooseSystemPopup;
        }
    }
}

void GameUI::showImages(void)
{
    if (m_gameImage)
        m_gameImage->Load();
    if (m_fanartImage)
        m_fanartImage->Load();
    if (m_boxImage)
        m_boxImage->Load();
}

void GameUI::searchComplete(QString string)
{
    if (!m_gameUITree->GetCurrentNode())
        return;

    MythGenericTree *parent = m_gameUITree->GetCurrentNode()->getParent();
    if (!parent)
        return;

    MythGenericTree *new_node = parent->getChildByName(string);
    if (new_node)
        m_gameUITree->SetCurrentNode(new_node);
}

void GameUI::updateRomInfo(RomInfo *rom)
{
    if (m_gameTitleText)
        m_gameTitleText->SetText(rom->Gamename());
    if (m_gameSystemText)
        m_gameSystemText->SetText(rom->System());
    if (m_gameYearText)
        m_gameYearText->SetText(rom->Year());
    if (m_gameGenreText)
        m_gameGenreText->SetText(rom->Genre());
    if (m_gamePlotText)
        m_gamePlotText->SetText(rom->Plot());

    if (m_gameFavouriteState)
    {
        if (rom->Favorite())
            m_gameFavouriteState->DisplayState("yes");
        else
            m_gameFavouriteState->DisplayState("no");
    }

    if (m_gameImage)
        m_gameImage->SetFilename(rom->Screenshot());
    if (m_fanartImage)
        m_fanartImage->SetFilename(rom->Fanart());
    if (m_boxImage)
        m_boxImage->SetFilename(rom->Boxart());
}

void GameUI::clearRomInfo(void)
{
    if (m_gameTitleText)
        m_gameTitleText->Reset();
    if (m_gameSystemText)
        m_gameSystemText->Reset();
    if (m_gameYearText)
        m_gameYearText->Reset();
    if (m_gameGenreText)
        m_gameGenreText->Reset();
    if (m_gamePlotText)
        m_gamePlotText->Reset();
    if (m_gameFavouriteState)
        m_gameFavouriteState->Reset();

    if (m_gameImage)
        m_gameImage->Reset();

    if (m_fanartImage)
        m_fanartImage->Reset();

    if (m_boxImage)
        m_boxImage->Reset();
}

void GameUI::edit(void)
{
    MythGenericTree *node = m_gameUITree->GetCurrentNode();
    if (isLeaf(node))
    {
        RomInfo *romInfo = qVariantValue<RomInfo *>(node->GetData());

        MythScreenStack *screenStack = GetScreenStack();

        EditRomInfoDialog *md_editor = new EditRomInfoDialog(screenStack,
            "mythgameeditmetadata", romInfo);

        if (md_editor->Create())
        {
            screenStack->AddScreen(md_editor);
            md_editor->SetReturnEvent(this, "editMetadata");
        }
        else
            delete md_editor;
    }
}

void GameUI::showInfo()
{
    MythGenericTree *node = m_gameUITree->GetCurrentNode();
    if (isLeaf(node))
    {
        RomInfo *romInfo = qVariantValue<RomInfo *>(node->GetData());
        if (!romInfo)
            return;
        MythScreenStack *mainStack = GetMythMainWindow()->GetMainStack();
        GameDetailsPopup *details_dialog  =
            new GameDetailsPopup(mainStack, romInfo);

        if (details_dialog->Create())
        {
            mainStack->AddScreen(details_dialog);
            details_dialog->SetReturnEvent(this, "detailsPopup");
        }
        else
            delete details_dialog;
    }
}

void GameUI::showMenu()
{
    MythGenericTree *node = m_gameUITree->GetCurrentNode();
    if (isLeaf(node))
    {
        RomInfo *romInfo = qVariantValue<RomInfo *>(node->GetData());
        if (!romInfo)
            return;
        MythScreenStack *popupStack = GetMythMainWindow()->
                                              GetStack("popup stack");
        MythDialogBox *showMenuPopup =
            new MythDialogBox(node->getString(), popupStack, "showMenuPopup");

        if (showMenuPopup->Create())
        {
            showMenuPopup->SetReturnEvent(this, "showMenuPopup");
            showMenuPopup->AddButton(tr("Show Information"));
            if (romInfo->Favorite())
                showMenuPopup->AddButton(tr("Remove Favorite"));
            else
                showMenuPopup->AddButton(tr("Make Favorite"));
            showMenuPopup->AddButton(tr("Edit Metadata"));
            popupStack->AddScreen(showMenuPopup);
        }
        else
            delete showMenuPopup;
    }
}

void GameUI::searchStart(void)
{
    MythGenericTree *parent = m_gameUITree->GetCurrentNode()->getParent();

    if (parent != NULL)
    {
        QStringList childList;
        QList<MythGenericTree*>::iterator it;
        QList<MythGenericTree*> *children = parent->getAllChildren();

        for (it = children->begin(); it != children->end(); ++it)
        {
            MythGenericTree *child = *it;
            childList << child->getString(); 
        }

        MythScreenStack *popupStack =
            GetMythMainWindow()->GetStack("popup stack");
        MythUISearchDialog *searchDialog = new MythUISearchDialog(popupStack,
            tr("Game Search"), childList, true, "");

        if (searchDialog->Create())
        {
            connect(searchDialog, SIGNAL(haveResult(QString)),
                    SLOT(searchComplete(QString)));

            popupStack->AddScreen(searchDialog);
        }
        else
            delete searchDialog;
    }
}

void GameUI::toggleFavorite(void)
{
    MythGenericTree *node = m_gameUITree->GetCurrentNode();
    if (isLeaf(node))
    {
        RomInfo *romInfo = qVariantValue<RomInfo *>(node->GetData());
        romInfo->setFavorite(true);
        updateChangedNode(node, romInfo);
    }
}

void GameUI::customEvent(QEvent *event)
{
    if (event->type() == kMythDialogBoxCompletionEventType)
    {
        DialogCompletionEvent *dce =
            dynamic_cast<DialogCompletionEvent*>(event);

        QString resultid = dce->GetId();
        QString resulttext = dce->GetResultText();

        if (resultid == "showMenuPopup")
        {
            if (resulttext == tr("Edit Metadata"))
            {
                edit();
            }
            else if (resulttext == tr("Show Information"))
            {
                showInfo();
            }
            else if (resulttext == tr("Make Favorite") ||
                     resulttext == tr("Remove Favorite"))
            {
                toggleFavorite();
            }
        }
        else if (resultid == "chooseSystemPopup")
        {
            if (!resulttext.isEmpty() && resulttext != tr("Cancel"))
            {
                MythGenericTree *node = m_gameUITree->GetCurrentNode();
                RomInfo *romInfo = qVariantValue<RomInfo *>(node->GetData());
                GameHandler::Launchgame(romInfo, resulttext);
            }
        }
        else if (resultid == "editMetadata")
        {
            MythGenericTree *node = m_gameUITree->GetCurrentNode();
            RomInfo *oldRomInfo = qVariantValue<RomInfo *>(node->GetData());
            delete oldRomInfo;

            RomInfo *romInfo = qVariantValue<RomInfo *>(dce->GetData());
            node->SetData(qVariantFromValue(romInfo));
            node->setString(romInfo->Gamename());

            romInfo->UpdateDatabase();
            updateChangedNode(node, romInfo);
        }
        else if (resultid == "detailsPopup")
        {
            // Play button pushed
            itemClicked(0);
        }
    }
}

QString GameUI::getFillSql(MythGenericTree *node) const
{
    QString layer = node->getString();
    int childDepth = node->getInt() + 1;
    QString childLevel = getChildLevelString(node);
    QString filter = getFilter(node);
    bool childIsLeaf = childDepth == getLevelsOnThisBranch(node) + 1;
    RomInfo *romInfo = qVariantValue<RomInfo *>(node->GetData());

    QString columns;
    QString conj = "where ";

    if (!filter.isEmpty())
    {
        filter = conj + filter;
        conj = " and ";
    }
    if ((childLevel == "gamename") && (m_gameShowFileName))
    {
        columns = childIsLeaf
                    ? "romname,system,year,genre,gamename"
                    : "romname";

        if (m_showHashed)
            filter += " and romname like '" + layer + "%'";

    }
    else if ((childLevel == "gamename") && (layer.length() == 1)) {
        columns = childIsLeaf
                    ? childLevel + ",system,year,genre,gamename"
                    : childLevel;

        if (m_showHashed) 
            filter += " and gamename like '" + layer + "%'";

    }
    else if (childLevel == "hash") {
        columns = "left(gamename,1)";
    }
    else {

        columns = childIsLeaf
                    ? childLevel + ",system,year,genre,gamename"
                    : childLevel;
    }

    //  this whole section ought to be in rominfo.cpp really, but I've put it
    //  in here for now to minimise the number of files changed by this mod
    if (romInfo) {
        if (!romInfo->System().isEmpty())
        {
            filter += conj + "trim(system)=:SYSTEM";
            conj = " and ";
        }
        if (!romInfo->Year().isEmpty())
        {
            filter += conj + "year=:YEAR";
            conj = " and ";
        }
        if (!romInfo->Genre().isEmpty())
        {
            filter += conj + "trim(genre)=:GENRE";
            conj = " and ";
        }
        if (!romInfo->Plot().isEmpty())
        {
            filter += conj + "plot=:PLOT";
            conj = " and ";
        }
        if (!romInfo->Publisher().isEmpty())
        {
            filter += conj + "publisher=:PUBLISHER";
            conj = " and ";
        }
        if (!romInfo->Gamename().isEmpty())
        {
            filter += conj + "trim(gamename)=:GAMENAME";
        }

    }

    filter += conj + " display = 1 ";

    QString sql;

    if ((childLevel == "gamename") && (m_gameShowFileName))
    {   
        sql = "select distinct "
                + columns
                + " from gamemetadata "
                + filter
                + " order by romname"
                + ";";
    }
    else if (childLevel == "hash") {
        sql = "select distinct "
                + columns
                + " from gamemetadata "
                + filter
                + " order by gamename,romname"
                + ";";
    }
    else
    {
        sql = "select distinct "
                + columns
                + " from gamemetadata "
                + filter
                + " order by "
                + childLevel
                + ";";
    }

    return sql;
}

QString GameUI::getChildLevelString(MythGenericTree *node) const
{
    unsigned this_level = node->getInt();
    while (node->getInt() != 1)
        node = node->getParent();

    GameTreeInfo *gi = qVariantValue<GameTreeInfo *>(node->GetData());
    return gi->getLevel(this_level - 1);
}

QString GameUI::getFilter(MythGenericTree *node) const
{
    while (node->getInt() != 1)
        node = node->getParent();
    GameTreeInfo *gi = qVariantValue<GameTreeInfo *>(node->GetData());
    return gi->getFilter();
}

int GameUI::getLevelsOnThisBranch(MythGenericTree *node) const
{
    while (node->getInt() != 1)
        node = node->getParent();

    GameTreeInfo *gi = qVariantValue<GameTreeInfo *>(node->GetData());
    return gi->getDepth();
}

bool GameUI::isLeaf(MythGenericTree *node) const
{
  return (node->getInt() - 1) == getLevelsOnThisBranch(node);
}

void GameUI::fillNode(MythGenericTree *node)
{
    QString layername = node->getString();
    RomInfo *romInfo = qVariantValue<RomInfo *>(node->GetData());

    MSqlQuery query(MSqlQuery::InitCon());

    query.prepare(getFillSql(node));

    if (romInfo)
    {
        if (!romInfo->System().isEmpty())
            query.bindValue(":SYSTEM",  romInfo->System());
        if (!romInfo->Year().isEmpty())
            query.bindValue(":YEAR", romInfo->Year());
        if (!romInfo->Genre().isEmpty())
            query.bindValue(":GENRE", romInfo->Genre());
        if (!romInfo->Plot().isEmpty())
            query.bindValue(":PLOT", romInfo->Plot());
        if (!romInfo->Publisher().isEmpty())
            query.bindValue(":PUBLISHER", romInfo->Publisher());
        if (!romInfo->Gamename().isEmpty())
            query.bindValue(":GAMENAME", romInfo->Gamename());
    }

    bool IsLeaf = node->getInt() == getLevelsOnThisBranch(node);
    if (query.exec() && query.size() > 0)
    {
        while (query.next())
        {
            QString current = query.value(0).toString().stripWhiteSpace();
            MythGenericTree *new_node =
                new MythGenericTree(current, node->getInt() + 1, false);
            if (IsLeaf)
            {
                RomInfo *temp = new RomInfo();
                temp->setSystem(query.value(1).toString().stripWhiteSpace());
                temp->setYear(query.value(2).toString());
                temp->setGenre(query.value(3).toString().stripWhiteSpace());
                temp->setGamename(query.value(4).toString().stripWhiteSpace());
                new_node->SetData(qVariantFromValue(temp));
                node->addNode(new_node);
            }
            else
            {
                RomInfo *newRomInfo;
                if (node->getInt() > 1)
                {
                    RomInfo *currentRomInfo;
                    currentRomInfo = qVariantValue<RomInfo *>(node->GetData());
                    newRomInfo = new RomInfo(*currentRomInfo);
                }
                else
                {
                    newRomInfo = new RomInfo();
                }
                new_node->SetData(qVariantFromValue(newRomInfo));
                node->addNode(new_node);
                if (getChildLevelString(node) != "hash")
                    newRomInfo->setField(getChildLevelString(node), current);
            }
        }
    }
}

void GameUI::resetOtherTrees(MythGenericTree *node)
{
    MythGenericTree *top_level = node;
    while (top_level->getParent() != m_gameTree)
    {
        top_level = top_level->getParent();
    }

    QList<MythGenericTree*>::iterator it;
    QList<MythGenericTree*> *children = m_gameTree->getAllChildren();

    for (it = children->begin(); it != children->end(); ++it)
    {
        MythGenericTree *child = *it;
        if (child != top_level)
        {
            child->deleteAllChildren();
        }
    }
}

void GameUI::updateChangedNode(MythGenericTree *node, RomInfo *romInfo)
{
    resetOtherTrees(node);

    if (node->getParent() == m_favouriteNode && romInfo->Favorite() == 0) {
        // node is being removed
        m_gameUITree->SetCurrentNode(m_favouriteNode);
    }
    else
        nodeChanged(node);
}
