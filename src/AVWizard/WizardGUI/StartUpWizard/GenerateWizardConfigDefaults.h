/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef GenerateWizardConfigDefaults_H_
#define GenerateWizardConfigDefaults_H_

#include <string>

#include "SettingsDictionaryTree.h"

class GenerateWizardConfigDefaults
{
	/**
	 *	Path to font name
	 */
	std::string FontName; 
	std::string FontTitle, FontText, FontMiniTitle; 
	int FontHeight;
	std::string Color; 
	std::string Style;

	/**
	 *	Generate welcome page of the wizard: Welcome.xml
	 */
	void GeneratePage_Welcome(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate first page of the wizard: VideoRatio.xml
	 */
	void GeneratePage_VideoResolution(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate page 2 of the wizard: VideoOutput.xml
	 */
	void GeneratePage_VideoResolutionConfirm(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);


	/**
	 *	Generate page 2 of the wizard: UISwitcher.xml
	 */
	void GeneratePage_UISwitcher(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	void GeneratePage_UISwitcherTest(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate page 4 of the wizard: VideoAdjustSize.xml
	 */
	void GeneratePage_VideoAdjustSize(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 * 	Generate page ## of the wizard: AudioSoundCard.xml
	 */
	void GeneratePage_AudioSoundCard(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate page 5 of the wizard: AudioConnector.xml
	 */
	void GeneratePage_AudioConnector(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate page 6 of the wizard: AudioVolume.xml
	 */
	void GeneratePage_AudioVolume(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate page 7 of the wizard: DolbyTest.xml
	 */
	void GeneratePage_DolbyTest(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate page 8 of the wizard: DTSTest.xml
	 */
	void GeneratePage_DTSTest(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate page 9 of the wizard: FinalSelections.xml
	 */
	void GeneratePage_FinalSelections(std::string FolderToSave, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate container with tabs and highlight the specified item
	 *	Too generates text items
	 */
	SettingsDictionaryTree* GenerateTabContainer(int NoSelectedTab, std::string ImageFolder, std::string FontFolder);
	/**
	 *	Generate image control with minimum parameters
	 */
	SettingsDictionaryTree* CreateControlImage(std::string ControlName, 
		std::string Picture, 
		int Left, 
		int Top);
	/**
	 *	Generate background control with minimum parameters
	 */
	SettingsDictionaryTree* CreateControlBackground(
		std::string ControlName, 
		std::string Picture, 
		int Left, int Top);
	/**
	 *	Generate label control with minimum parameters
	 */
	SettingsDictionaryTree* CreateControlLabel(std::string ControlName, 
		std::string Caption, 
		int Left, 
		int Top,
		std::string Align);

	SettingsDictionaryTree* CreateControlPage(std::string ControlName, 
		int Width, int Height,
		bool FullScreen,
		std::string Caption);

	SettingsDictionaryTree* CreateControlScrollList(
		std::string ControlName,
		int Left, int Top,
		int Width, int Height,
		std::string BackColor,
		std::string BackFocusedColor,
		std::string HighBackColor,
		std::string HighBackFocusedColor,
		bool Focused
	);

	/**
	 *	Adds to a specified page's dictionary default images for buttons
	 */
	void SetDefaultBtnImages(SettingsDictionary* Dictionary, std::string ImageFolder);


	/**
	 *	Setup the current font, is used to set up current font style for controls
	 */
	void SetFontStyle(
		int FontHeight, 
		std::string Color,
		std::string Style
		);

	/**
	 *	Setup the name of the current font
	 */
	void SetFontName(std::string FontName);

	/**
	 *	Creates a definition for one button
	 */
	SettingsDictionaryTree* CreateControlButton(
		std::string ControlName, 
		std::string Caption, 
		int Left, int Top,
		int Width,
		bool Expands);

	SettingsDictionaryTree* CreateControlListBox(
		std::string ControlName, 
		std::string Caption, 
		int Left, int Top,
		int Width, int Height,
		bool Expands);
public:
	GenerateWizardConfigDefaults(void);
	virtual ~GenerateWizardConfigDefaults(void);

	void GenerateDefaults(std::string FolderToSave = "");

	void GenerateDefaultPages(
		std::string FolderToSave = "", 
		std::string ImageFolder = "", 
		std::string FontFolder = "");
};

#endif
