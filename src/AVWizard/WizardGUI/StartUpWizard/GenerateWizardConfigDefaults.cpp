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
#include "GenerateWizardConfigDefaults.h"

#include "GUIWizardUtils.h"

#include "ConfigureCommons.h"

#include "Wizard.h"

#include <iostream>

#include "SkinGenerator.h"

GenerateWizardConfigDefaults::GenerateWizardConfigDefaults()
{
	SkinGenerator::Instance()->GetEnvironment();

	FontText = SkinGenerator::Instance()->DefaultFontText;
	if(Wizard::GetInstance()->AVWizardOptions->GetDictionary()->Exists("DefaultFontText"))
		FontText = Wizard::GetInstance()->AVWizardOptions->GetDictionary()->GetValue("DefaultFontText");

	FontTitle = SkinGenerator::Instance()->DefaultFontStep;
	if(Wizard::GetInstance()->AVWizardOptions->GetDictionary()->Exists("DefaultFontTitle"))
		FontTitle = Wizard::GetInstance()->AVWizardOptions->GetDictionary()->GetValue("DefaultFontTitle");

	FontMiniTitle = SkinGenerator::Instance()->DefaultFontMiniTitle;
	if(Wizard::GetInstance()->AVWizardOptions->GetDictionary()->Exists("DefaultFontMiniTitle"))
		FontMiniTitle = Wizard::GetInstance()->AVWizardOptions->GetDictionary()->GetValue("DefaultFontMiniTitle");

	FontName = FontText;
}

GenerateWizardConfigDefaults::~GenerateWizardConfigDefaults()
{
}

void GenerateWizardConfigDefaults::GenerateDefaults(std::string FolderToSave)
{
	if(FolderToSave.length())
	{
#ifdef WIN32
		if(FolderToSave[FolderToSave.length()-1] != '\\')
			FolderToSave += "\\";
#else
		if(FolderToSave[FolderToSave.length()-1] != '/')
			FolderToSave += "/";
#endif
	}

	SettingsDictionaryTree* Settings, *Options;
	SettingsDictionary* Dictionary;
	Settings = new SettingsDictionaryTree();

	Settings->GetDictionary()->SetName("AVWizard");
	Settings->GetDictionary()->SetType("Config_File");
	Settings->GetDictionary()->Set("Version", "0.0.1 alpha");

	Options = new SettingsDictionaryTree();

	Dictionary = Options->GetDictionary();
	Dictionary->SetName("Screens");
	Dictionary->SetType("Pages");

	Settings->AddChild(Options);

	delete Settings;
}


void GenerateWizardConfigDefaults::GenerateDefaultPages(std::string FolderToSave, std::string ImageFolder, std::string FontFolder)
{
	if(FolderToSave.length())
	{
#ifdef WIN32
		if(FolderToSave[FolderToSave.length()-1] != '\\')
			FolderToSave += "\\";
#else
		if(FolderToSave[FolderToSave.length()-1] != '/')
			FolderToSave += "/";
#endif
	}

	if(ImageFolder.length())
	{
#ifdef WIN32
		if(ImageFolder[ImageFolder.length()-1] != '\\')
			ImageFolder += "\\";
#else
		if(ImageFolder[ImageFolder.length()-1] != '/')
			ImageFolder += "/";
#endif
	}

	
	if(FontFolder.length())
	{
#ifdef WIN32
		if(FontFolder[FontFolder.length()-1] != '\\')
			FontFolder += "\\";
#else
		if(FontFolder[FontFolder.length()-1] != '/')
			FontFolder += "/";
#endif
	}

	GeneratePage_Welcome(FolderToSave, ImageFolder, FontFolder);
	GeneratePage_VideoResolution(FolderToSave, ImageFolder, FontFolder);
	GeneratePage_VideoResolutionConfirm(FolderToSave, ImageFolder, FontFolder);

	GeneratePage_UISwitcher(FolderToSave, ImageFolder, FontFolder);
	GeneratePage_UISwitcherTest(FolderToSave, ImageFolder, FontFolder);
	GeneratePage_VideoAdjustSize(FolderToSave, ImageFolder, FontFolder);
	GeneratePage_AudioSoundCard(FolderToSave, ImageFolder, FontFolder);
	GeneratePage_AudioConnector(FolderToSave, ImageFolder, FontFolder);
	GeneratePage_AudioVolume(FolderToSave, ImageFolder, FontFolder);
	GeneratePage_DolbyTest(FolderToSave, ImageFolder, FontFolder);
	GeneratePage_DTSTest(FolderToSave, ImageFolder, FontFolder);
	GeneratePage_FinalSelections(FolderToSave, ImageFolder, FontFolder);
}


void GenerateWizardConfigDefaults::GeneratePage_Welcome(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionary* Dictionary;

	Page = CreateControlPage("WelcomeStep", 640, 480, true, "AVWizard Welcome");
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Page->AddChild(GenerateTabContainer(-1, ImageFolder, FontFolder));

	const int nStrings = 4;
	std::string StringList[nStrings];
	StringList[0] = "From here you can set up the audio & video for your home";
	StringList[1] = "For selections use \"UP/DOWN/LEFT/RIGHT\" arrows";
	StringList[2] = "For validation use \"Enter\"";
	StringList[3] = "IF you want to go back one step use \"Escape\"";

	SettingsDictionaryTree* BackgroundControl = CreateControlBackground("Gray", "(none)", 0, 0);
	std::string Color = SkinGenerator::Instance()->BackgroundColor;
	BackgroundControl->GetDictionary()->Set("Color", Color);
	Page->AddChild(BackgroundControl);
	
	std::string DefaultFontColor = SkinGenerator::Instance()->DefaultFontColor;	
	int DefaultFontSize  = Utils::StringToInt32(SkinGenerator::Instance()->DefaultFontSize);

	SetFontStyle(DefaultFontSize, DefaultFontColor, "Regular");
	for (int i = 0; i < nStrings; i++)
	{
		Page->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			200+i*15,
			"Center"
			));
	}

	Page->AddChild(CreateControlButton(
		"MainBtn",
		" Continue ",
		555, 440, 0,
		true
	));



	Page->SaveToXMLFile("/tmp/Welcome.xml");
}

void GenerateWizardConfigDefaults::GeneratePage_VideoResolution(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;

	Page = CreateControlPage("ScreenStep1", 640, 480, true, "AV Wizard Configurator");
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Container = GenerateTabContainer(0, ImageFolder, FontFolder);
	Page->AddChild(Container);

	std::string StringList[4];
	StringList[0] = "Select the appropiate resolution that you consider";
	StringList[1] = "is best for your machine";
	StringList[2] = "After you choose one press \"Enter\"";

	SettingsDictionaryTree* BackgroundControl = CreateControlBackground("Gray", "(none)", 0, 0);
	std::string Color = SkinGenerator::Instance()->BackgroundColor;
	BackgroundControl->GetDictionary()->Set("Color", Color);
	Page->AddChild(BackgroundControl);
	
	std::string DefaultFontColor = SkinGenerator::Instance()->DefaultFontColor;	
	int DefaultFontSize  = Utils::StringToInt32(SkinGenerator::Instance()->DefaultFontSize);

	SetFontStyle(DefaultFontSize, DefaultFontColor, "Regular");
	for(int i = 0; i<3; i++)
	{
		Page->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			200+i*15,
			"Center"
			));
	}

	std::string ScrollBackColor = SkinGenerator::Instance()->ScrollBackColor;
	std::string ScrollBackFocusedColor = SkinGenerator::Instance()->ScrollBackFocusedColor;
	std::string HighScrollBackColor = SkinGenerator::Instance()->HighScrollBackColor;
	std::string HighScrollBackFocusedColor = SkinGenerator::Instance()->HighScrollBackFocusedColor;


	Page->AddChild(CreateControlScrollList("VideoConnectorScroll",
		30, 260,
		160, 140,
		ScrollBackColor,
		ScrollBackFocusedColor,
		HighScrollBackColor,
		HighScrollBackFocusedColor,
		false));
	Page->AddChild(CreateControlImage(
		"VideoConnectorUp",
		ImageFolder + "up_off.png",
		195, 260
		));
	Page->AddChild(CreateControlImage(
		"VideoConnectorDown",
		ImageFolder + "down_off.png",
		195, 368
		));

	Page->AddChild(CreateControlScrollList("ResolutionScroll",
		240, 260,
		170, 140,
		ScrollBackColor,
		ScrollBackFocusedColor,
		HighScrollBackColor,
		HighScrollBackFocusedColor,
		false));
	Page->AddChild(CreateControlImage(
		"ResolutionUp",
		ImageFolder + "up_off.png",
		415, 260
		));
	Page->AddChild(CreateControlImage(
		"ResolutionDown",
		ImageFolder + "down_off.png",
		415, 368
		));

	Page->AddChild(CreateControlScrollList("RefreshScroll",
		460, 260,
		120, 140,
		ScrollBackColor,
		ScrollBackFocusedColor,
		HighScrollBackColor,
		HighScrollBackFocusedColor,
		false));
	Page->AddChild(CreateControlImage(
		"RefreshUp",
		ImageFolder + "up_off.png",
		585, 260
		));
	Page->AddChild(CreateControlImage(
		"RefreshDown",
		ImageFolder + "down_off.png",
		585, 368
		));



	Page->AddChild(CreateControlButton("BtnOK",
		" OK ",
		320, 440,
		0,
		true
		));

	DefaultFontColor = SkinGenerator::Instance()->DefaultFontColor;

	Page->SaveToXMLFile("/tmp/VideoResolution.xml");
}

void GenerateWizardConfigDefaults::GeneratePage_VideoResolutionConfirm(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;

	Page = new SettingsDictionaryTree();
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Dictionary->SetName("ScreenStep1");
	Dictionary->SetType("Page");
	Dictionary->Set("Width", 640);
	Dictionary->Set("Height", 480);
	Dictionary->Set("Fullscreen", 1);
	Dictionary->Set("Caption", "AVWizard Configurator");

	Container = GenerateTabContainer(0, ImageFolder, FontFolder);
	Page->AddChild(Container);
	SettingsDictionaryTree* BackgroundControl = CreateControlBackground("Gray", "(none)", 0, 0);
	BackgroundControl->GetDictionary()->Set("Color", SkinGenerator::Instance()->BackgroundColor);
	Page->AddChild(BackgroundControl);
	
	//Create text area
	std::string StringList[3];
	StringList[0] = "You chose the following resolution:";
	StringList[1] = "If you want to keep it, choose 'Continue'";
	StringList[2] = "If you want to choose another resolution, press 'Back'";

	std::string DefaultFontColor = SkinGenerator::Instance()->DefaultFontColor;	
	int DefaultFontSize  = Utils::StringToInt32(SkinGenerator::Instance()->DefaultFontSize);

	SetFontStyle(DefaultFontSize, DefaultFontColor, "Regular");
	for(int i = 0; i<3; i++)
	{
		Container->AddChild(CreateControlLabel(
				"DescribeText"+Utils::Int32ToString(i),
				StringList[i],
				320,
				175+i*15,
				"Center"
			));
	}

	int CounterFontSize  = Utils::StringToInt32(SkinGenerator::Instance()->CounterFontSize);
	if (CounterFontSize == 0)
	{
		std::cerr<<"Warning, CounterFontSize not set!"<<std::endl;
		CounterFontSize = 55;
	}

	SetFontStyle(CounterFontSize, DefaultFontColor, "Regular");

	Container->AddChild(CreateControlLabel(
			"CounterLabel",
			" ",
			320,
			365,
			"Center"
		));


	Page->AddChild(CreateControlButton(
		"BackBtn",
		" Back ",
		65, 440,
		0,
		true
		));

	Page->AddChild(CreateControlButton(
		"ContinueBtn",
		" Continue ",
		555, 440, 0,
		true
	));

	Page->SaveToXMLFile("/tmp/VideoResolutionConfirm.xml");
}


void GenerateWizardConfigDefaults::GeneratePage_UISwitcher(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;

	Page = CreateControlPage("ScreenStep1", 640, 480, true, "AV Wizard Configurator");
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Container = GenerateTabContainer(2, ImageFolder, FontFolder);
	Page->AddChild(Container);

	std::string StringList[4];
	StringList[0] = "Select your user experience.  The Alpha blended UI is better.";
	StringList[1] = "But before choosing it read how it works and the requirements";
	StringList[2] = "at http://wiki.linuxmce.org/index.php/UI";

	SettingsDictionaryTree* BackgroundControl = CreateControlBackground("Gray", "(none)", 0, 0);
	std::string Color = SkinGenerator::Instance()->BackgroundColor;
	BackgroundControl->GetDictionary()->Set("Color", Color);
	Page->AddChild(BackgroundControl);
	
	std::string DefaultFontColor = SkinGenerator::Instance()->DefaultFontColor;	
	int DefaultFontSize  = Utils::StringToInt32(SkinGenerator::Instance()->DefaultFontSize);

	SetFontStyle(DefaultFontSize, DefaultFontColor, "Regular");
	for(int i = 0; i<3; i++)
	{
		Page->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			200+i*15,
			"Center"
			));
	}

	std::string ScrollBackColor = SkinGenerator::Instance()->ScrollBackColor;
	std::string ScrollBackFocusedColor = SkinGenerator::Instance()->ScrollBackFocusedColor;
	std::string HighScrollBackColor = SkinGenerator::Instance()->HighScrollBackColor;
	std::string HighScrollBackFocusedColor = SkinGenerator::Instance()->HighScrollBackFocusedColor;


	Page->AddChild(CreateControlScrollList("UIScroll",
		180, 280,
		280, 140,
		ScrollBackColor,
		ScrollBackFocusedColor,
		HighScrollBackColor,
		HighScrollBackFocusedColor,
		false));
	Page->AddChild(CreateControlImage(
		"UI_Up",
		ImageFolder + "up_off.png",
		465, 280
		));
	Page->AddChild(CreateControlImage(
		"UI_Down",
		ImageFolder + "down_off.png",
		465, 385
		));

	Page->AddChild(CreateControlButton("BtnTest",
		" Test ",
		220, 450,
		0,
		true
		));

	Page->AddChild(CreateControlButton("BtnOK",
		" OK ",
		420, 450,
		0,
		true
		));

	Page->SaveToXMLFile("/tmp/UISwitcher.xml");
}

void GenerateWizardConfigDefaults::GeneratePage_UISwitcherTest(std::string FolderToSave, std::string ImageFolder, std::string FontFolder)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;

	Page = new SettingsDictionaryTree();
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Dictionary->SetName("ScreenStep1");
	Dictionary->SetType("Page");
	Dictionary->Set("Width", 640);
	Dictionary->Set("Height", 480);
	Dictionary->Set("Fullscreen", 1);
	Dictionary->Set("Caption", "AVWizard Configurator");

	Container = GenerateTabContainer(2, ImageFolder, FontFolder);
	Page->AddChild(Container);
	SettingsDictionaryTree* BackgroundControl = CreateControlBackground("Gray", "(none)", 0, 0);
	BackgroundControl->GetDictionary()->Set("Color", SkinGenerator::Instance()->BackgroundColor);
	Page->AddChild(BackgroundControl);
	
	std::string DefaultFontColor = SkinGenerator::Instance()->DefaultFontColor;	
	int DefaultFontSize  = Utils::StringToInt32(SkinGenerator::Instance()->DefaultFontSize);

	int CounterFontSize = Utils::StringToInt32(SkinGenerator::Instance()->CounterFontSize);
	if (CounterFontSize == 0)
	{
		std::cerr<<"Warning, CounterFontSize not set!"<<std::endl;
		CounterFontSize = 55;
	}

	SetFontStyle(DefaultFontSize, DefaultFontColor, "Regular");

	Page->AddChild(CreateControlLabel(
			"Label1",
			"Testing chosen UI option",
			300, 240,
			"Center"
		));

	SetFontStyle(CounterFontSize, DefaultFontColor, "Regular");

	Page->AddChild(CreateControlLabel(
			"CounterLabel",
			" ",
			320,
			365,
			"Center"
		));

	Page->SaveToXMLFile("/tmp/UISwitcherTest.xml");
}

void GenerateWizardConfigDefaults::GeneratePage_VideoAdjustSize(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;

	Page = new SettingsDictionaryTree();
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Dictionary->SetName("ScreenStep");
	Dictionary->SetType("Page");
	Dictionary->Set("Width", 640);
	Dictionary->Set("Height", 480);
	Dictionary->Set("Fullscreen", 1);
	Dictionary->Set("Caption", "AVWizard Configurator");

	Page->AddChild(CreateControlBackground("BackArrows", ImageFolder+"zoom.png", 0, 0));

	Container = GenerateTabContainer(4, ImageFolder, FontFolder);
	Page->AddChild(Container);

	//Create text area
	std::string StringList[3];
	StringList[0] = "Adjust your screen position using the folowing options: ";
	StringList[1] = "Use Plus/Minus to Zoom In/Out the screen";
	StringList[2] = "Use arrows keys to center the image to the desired position";

	std::string DefaultFontColor = SkinGenerator::Instance()->DefaultFontColor;	
	int DefaultFontSize  = Utils::StringToInt32(SkinGenerator::Instance()->DefaultFontSize);

	SetFontStyle(DefaultFontSize, DefaultFontColor, "Regular");
	for(int i = 0; i<3; i++)
	{
		Container->AddChild(CreateControlLabel(
				"DescribeText"+Utils::Int32ToString(i),
				StringList[i],
				320,
				175+i*15,
				"Center"
			));
	}
	StringList[0] = "For Zoom In you should press: ";
	StringList[1] = "For Zoom Out you should press: ";
	StringList[2] = "Press Enter to go to next screen! ";
	for(int i = 0; i<2; i++)
	{
		Container->AddChild(CreateControlLabel(
				"DescribeText"+Utils::Int32ToString(i),
				StringList[i],
				320,
				255+i*30,
				"Center"
			));
	}

	Container->AddChild(CreateControlLabel(
			"DescribeText2",
			StringList[2],
			320,
			440,
			"Center"
		));

	Page->AddChild(CreateControlButton(
			"ButtonPlus",
			"+",
			520, 260,
			60,
			false
		));
	Page->AddChild(CreateControlButton(
		"ButtonMinus",
		"-",
		520, 295,
		60,
		false
		));


	Page->AddChild(CreateControlButton(
			"ButtonUp",
			"  Up  ",
			320, 350,
			0,
			true
		));

	Page->AddChild(CreateControlButton(
		"ButtonDown",
		"Down",
		320, 390,
		0,
		true
		));
	Page->AddChild(CreateControlButton(
			"ButtonLeft",
			" Left ",
			220, 370,
			0,
			true
		));

	Page->AddChild(CreateControlButton(
		"ButtonRight",
		"Right",
		420, 370,
		0,
		true
		));

	Page->AddChild(CreateControlButton(
				"ButtonOK",
				" OK ",
				480, 440,
				0,
				true
		));

	Page->SaveToXMLFile("/tmp/VideoAdjustSize.xml");
}

void GenerateWizardConfigDefaults::GeneratePage_AudioSoundCard(
	std::string FolderToSave,
	std::string ImageFolder,
	std::string FontFolder
	)
{
	std::string ScrollBackColor = SkinGenerator::Instance()->ScrollBackColor;
	std::string ScrollBackFocusedColor = SkinGenerator::Instance()->ScrollBackFocusedColor;
	std::string HighScrollBackColor = SkinGenerator::Instance()->HighScrollBackColor;
	std::string HighScrollBackFocusedColor = SkinGenerator::Instance()->HighScrollBackFocusedColor;

	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;

	Page = new SettingsDictionaryTree();
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Dictionary->SetName("ScreenStep1");
	Dictionary->SetType("Page");
	Dictionary->Set("Width", 640);
	Dictionary->Set("Height", 480);
	Dictionary->Set("Fullscreen", 1);
	Dictionary->Set("Caption", "AVWizard Configurator");

	SettingsDictionaryTree* BackgroundControl = CreateControlBackground("Gray", "(none)", 0, 0);
	BackgroundControl->GetDictionary()->Set("Color", SkinGenerator::Instance()->BackgroundColor);
	Page->AddChild(BackgroundControl);

	Container = GenerateTabContainer(5, ImageFolder, FontFolder);
	Page->AddChild(Container);

	//Create text area
	std::string StringList[1];
	StringList[0] = "Which sound card did you use?";

	std::string DefaultFontColor = SkinGenerator::Instance()->DefaultFontColor;	
	int DefaultFontSize  = Utils::StringToInt32(SkinGenerator::Instance()->DefaultFontSize);

	SetFontStyle(DefaultFontSize, DefaultFontColor, "Regular");
	Page->AddChild(CreateControlScrollList("SoundCardScroll",
		30, 260,
		580, 140,
		ScrollBackColor,
		ScrollBackFocusedColor,
		HighScrollBackColor,
		HighScrollBackFocusedColor,
		false));
	Page->AddChild(CreateControlButton("BtnOK",
		" OK ",
		320, 450,
		0,
		true
		));
	Page->SaveToXMLFile("/tmp/AudioSoundCard.xml");
}

void GenerateWizardConfigDefaults::GeneratePage_AudioConnector(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;

	Page = new SettingsDictionaryTree();
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Dictionary->SetName("ScreenStep1");
	Dictionary->SetType("Page");
	Dictionary->Set("Width", 640);
	Dictionary->Set("Height", 480);
	Dictionary->Set("Fullscreen", 1);
	Dictionary->Set("Caption", "AVWizard Configurator");

	SettingsDictionaryTree* BackgroundControl = CreateControlBackground("Gray", "(none)", 0, 0);
	BackgroundControl->GetDictionary()->Set("Color", SkinGenerator::Instance()->BackgroundColor);
	Page->AddChild(BackgroundControl);

	Container = GenerateTabContainer(5, ImageFolder, FontFolder);
	Page->AddChild(Container);

	//Create text area
	std::string StringList[1];
	StringList[0] = "Which connector did you use?";

	std::string DefaultFontColor = SkinGenerator::Instance()->DefaultFontColor;	
	int DefaultFontSize  = Utils::StringToInt32(SkinGenerator::Instance()->DefaultFontSize);

	SetFontStyle(DefaultFontSize, DefaultFontColor, "Regular");

	for(int i = 0; i<1; i++)
	{
		Container->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			175+i*15,
			"Center"
			));
	}

	Page->AddChild(CreateControlButton(
		"Btn1",
		"Analog Stereo",
		320, 250,
		0,
		true
		));

	Page->AddChild(CreateControlButton(
		"Btn2",
		"SPDIF Coaxial",
		320, 280,
		0,
		true
		));

	Page->AddChild(CreateControlButton(
		"Btn3",
		"SPDIF Optical",
		320, 310,
		0,
		true
		));

	Page->AddChild(CreateControlButton(
		"Btn4",
		"HDMI",
		320, 340,
		0,
		true
		));

	Page->AddChild(CreateControlButton(
		"Btn5",
		"No sound",
		320, 370,
		0,
		true
		));

	Page->SaveToXMLFile("/tmp/AudioConnector.xml");
}

void GenerateWizardConfigDefaults::GeneratePage_AudioVolume(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;

	Page = new SettingsDictionaryTree();
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Dictionary->SetName("ScreenStep1");
	Dictionary->SetType("Page");
	Dictionary->Set("Width", 640);
	Dictionary->Set("Height", 480);
	Dictionary->Set("Fullscreen", 1);
	Dictionary->Set("Caption", "AVWizard Configurator");

	Container = GenerateTabContainer(6, ImageFolder, FontFolder);
	Page->AddChild(Container);

	//Create text area
	std::string StringList[4];
	StringList[0] = "I am playing some music, press enter when you confirm that you can hear it";
	StringList[1] = "and the volume is adjusted at a comfortable level.";

	StringList[2] = "** NO AUDIO **";
	StringList[3] = "Bonus CD with sample audio tracks was not installed";

	std::string DefaultFontColor = SkinGenerator::Instance()->DefaultFontColor;	
	int DefaultFontSize  = Utils::StringToInt32(SkinGenerator::Instance()->DefaultFontSize);

	SetFontStyle(DefaultFontSize, DefaultFontColor, "Regular");
	for(int i = 0; i<2; i++)
	{
		Container->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			175+i*15,
			"Center"
			));
	}

	DefaultFontColor = SkinGenerator::Instance()->MiniTitleFontColor;
	SetFontStyle(DefaultFontSize, DefaultFontColor, "BOLD");
	for(int i = 2; i<4; i++)
	{
		Container->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			175+i*15,
			"Center"
			));
	}

	Page->AddChild(CreateControlImage(
		"SpeakerImage",
		ImageFolder + "speaker.png",
		270,
		250
		));

	Page->AddChild(CreateControlImage(
		"VolumeLeft",
		ImageFolder + "left_off.png",
		220, 270
		));

	Page->AddChild(CreateControlImage(
		"VolumeRight",
		ImageFolder + "right_off.png",
		410,
		270
		));

	Page->AddChild(CreateControlButton("BtnOK",
		" OK ",
		320, 450,
		0,
		true
		));

	std::string FontShadow = SkinGenerator::Instance()->StepFontColorShadow;	
	int FontSize = Utils::StringToInt32(SkinGenerator::Instance()->TitleFontSize);
	SetFontStyle(FontSize, FontShadow, "Regular");
	Page->AddChild(CreateControlLabel(
		"SpeakerVolumeTextShadow",
		"50%",
		321,
		351,
		"Center"
		));

	std::string FontColor = SkinGenerator::Instance()->MiniTitleFontColor;
	SetFontStyle(FontSize, FontColor, "Regular");
	Page->AddChild(CreateControlLabel(
		"SpeakerVolumeText",
		"50%",
		320,
		350,
		"Center"
		));

	Page->SaveToXMLFile("/tmp/AudioVolume.xml");
}

void GenerateWizardConfigDefaults::GeneratePage_DolbyTest(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;

	Page = new SettingsDictionaryTree();
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Dictionary->SetName("ScreenStep1");
	Dictionary->SetType("Page");
	Dictionary->Set("Width", 640);
	Dictionary->Set("Height", 480);
	Dictionary->Set("Fullscreen", 1);
	Dictionary->Set("Caption", "AVWizard Configurator");

	Container = GenerateTabContainer(7, ImageFolder, FontFolder);
	Page->AddChild(Container);

	//Create text area
	std::string StringList[5];
	StringList[0] = "Can your stereo play Dolby Digital?";
	StringList[1] = "I am playing some music in Dolby Digital 5.1. If you're able to hear the music,";
	StringList[2] = "press the right arrow, otherwise press the left arrow.";

	StringList[3] = "** NO AUDIO **";
	StringList[4] = "Bonus CD with sample audio tracks was not installed";

	
	std::string DefaultFontColor = SkinGenerator::Instance()->DefaultFontColor;	
	int DefaultFontSize  = Utils::StringToInt32(SkinGenerator::Instance()->DefaultFontSize);

	SetFontStyle(DefaultFontSize, DefaultFontColor, "Regular");
	for(int i = 0; i<1; i++)
	{
		Container->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			175+i*15,
			"Center"
			));
	}

	DefaultFontColor = SkinGenerator::Instance()->MiniTitleFontColor;
	SetFontStyle(DefaultFontSize, DefaultFontColor, "BOLD");
	for(int i = 3; i<5; i++)
	{
		Container->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			175+i*15,
			"Center"
			));
	}

	Page->AddChild(CreateControlImage(
		"SpeakerImage",
		ImageFolder + "speaker.png",
		270, 250
	));

	Page->AddChild(CreateControlButton(
		"Btn1",
		"No, I cannot hear it",
		155, 310,
		0,
		true
		));

	Page->AddChild(CreateControlButton(
		"Btn2",
		"Yes, I can hear it",
		495, 310,
		0,
		true
		));

	Page->SaveToXMLFile("/tmp/DolbyTest.xml");
}

void GenerateWizardConfigDefaults::GeneratePage_DTSTest(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;


	Page = new SettingsDictionaryTree();
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Dictionary->SetName("ScreenStep1");
	Dictionary->SetType("Page");
	Dictionary->Set("Width", 640);
	Dictionary->Set("Height", 480);
	Dictionary->Set("Fullscreen", 1);
	Dictionary->Set("Caption", "AVWizard Configurator");

	Container = GenerateTabContainer(8, ImageFolder, FontFolder);
	Page->AddChild(Container);

	//Create text area
	std::string StringList[5];
	StringList[0] = "Can your stereo play DTS?";
	StringList[1] = "I am playing some music in DTS. If you're able to hear the music, press the";
	StringList[2] = "right arrow, otherwise press the left arrow.";

	StringList[3] = "** NO AUDIO **";
	StringList[4] = "Bonus CD with sample audio tracks was not installed";

	std::string DefaultFontColor = SkinGenerator::Instance()->DefaultFontColor;	
	int DefaultFontSize  = Utils::StringToInt32(SkinGenerator::Instance()->DefaultFontSize);

	SetFontStyle(DefaultFontSize, DefaultFontColor, "Bold");
	Container->AddChild(CreateControlLabel(
		"DescribeText0",
		StringList[0],
		320, 175,
		"Center"
		));
	SetFontStyle(DefaultFontSize, DefaultFontColor, "Regular");
	for(int i = 1; i<3; i++)
	{
		Container->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			175+i*15,
			"Center"
			));
	}



	DefaultFontColor = SkinGenerator::Instance()->MiniTitleFontColor;
	SetFontStyle(DefaultFontSize, DefaultFontColor, "BOLD");
	for(int i = 3; i<5; i++)
	{
		Container->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			175+i*15,
			"Center"
			));
	}

	Page->AddChild(CreateControlImage(
		"SpeakerImage",
		ImageFolder + "speaker.png",
		270,
		250
		));


	Page->AddChild(CreateControlButton(
		"Btn1",
		"No, I cannot hear it",
		155, 310,
		0,
		true
		));

	Page->AddChild(CreateControlButton(
		"Btn2",
		"Yes, I can hear it",
		495, 310,
		0,
		true
		));

	Page->SaveToXMLFile("/tmp/DTSTest.xml");
}

void GenerateWizardConfigDefaults::GeneratePage_FinalSelections(
	std::string FolderToSave, 
	std::string ImageFolder, 
	std::string FontFolder
	)
{
	SettingsDictionaryTree* Page;
	SettingsDictionaryTree* Container;
	SettingsDictionary* Dictionary;

	Page = new SettingsDictionaryTree();
	Dictionary = Page->GetDictionary();
	SetDefaultBtnImages(Dictionary, ImageFolder);

	Dictionary->SetName("ScreenStep8");
	Dictionary->SetType("Page");
	Dictionary->Set("Width", 640);
	Dictionary->Set("Height", 480);
	Dictionary->Set("Fullscreen", 1);
	Dictionary->Set("Caption", "AVWizard Configurator");

	Container = GenerateTabContainer(9, ImageFolder, FontFolder);
	Page->AddChild(Container);


	//Create text area
	std::string StringList[5];
	StringList[0] = "You're all done. Here are your selections:";
	StringList[1] = "If you're satisfied with these settings, press enter and I begin creating a";
	StringList[2] = "new user experience for you that is optimized for your television and stereo.";
	StringList[3] = "This process can take up to 15 minutes, please be patient. If you want to";
	StringList[4] = "change the settings, press the menu button to start again.";

	std::string DefaultFontColor = SkinGenerator::Instance()->DefaultFontColor;	
	int DefaultFontSize  = Utils::StringToInt32(SkinGenerator::Instance()->DefaultFontSize);

	SetFontStyle(DefaultFontSize, DefaultFontColor, "Italic");
	Container->AddChild(CreateControlLabel(
		"DescribeText0",
		StringList[0],
		320,
		180,
		"Center"
		));

	SetFontStyle(DefaultFontSize, DefaultFontColor, "Regular");
	for(int i = 1; i<5; i++)
	{
		Container->AddChild(CreateControlLabel(
			"DescribeText"+Utils::Int32ToString(i),
			StringList[i],
			320,
			335+i*15,
			"Center"
			));
	}

	SetFontStyle(DefaultFontSize, DefaultFontColor, "Regular");

	for(int i = 1; i<=8; i++)
	Container->AddChild(CreateControlLabel(
		"OptionText"+Utils::Int32ToString(i),
		Utils::Int32ToString(i) + ".",
		190,
		110+(i+5)*17,
		"Left"
		));

	Page->AddChild(CreateControlButton(
		"Btn1",
		"I do not agree",
		120,
		450,
		0,
		true
		));

	Page->AddChild(CreateControlButton(
		"Btn2",
		"I agree",
		555, 450, 
		0,
		true
		));

	Page->SaveToXMLFile("/tmp/FinalSelections.xml");
}

SettingsDictionaryTree* GenerateWizardConfigDefaults::GenerateTabContainer(int NoSelectedTab, 
																		   std::string ImageFolder,
																		   std::string FontFolder)
{
	int i = NoSelectedTab;
	char Buffer[1024];
	SettingsDictionaryTree* Result;
	SettingsDictionaryTree* Container;
	SettingsDictionaryTree* Control;
	SettingsDictionary* Dictionary = NULL;

	Result = new SettingsDictionaryTree();

	Container = new SettingsDictionaryTree();
	Dictionary = Container->GetDictionary();

	Dictionary->SetName("HeadProgress");
	Dictionary->SetType("Container");
	Dictionary->Set("Visible", 0);

	Dictionary = Result->GetDictionary();
	Dictionary->SetType("Container");
	Dictionary->SetName("BaseFrame");
	Result->AddChild(Container);


	Result->AddChild(CreateControlImage("BackImage", ImageFolder+"background.png", 0, 52));


	if (NoSelectedTab >= 1)
		NoSelectedTab --;

	std::string LabelCaption;
	switch(NoSelectedTab)
	{
		case 0:
			LabelCaption = "RESOLUTION SETTINGS";
			break;
		case 1:
			LabelCaption = "GRAPHICAL ASPECT";
			break;
		case 2:
			LabelCaption = "OUTPUT CONNECTOR";
			break;
		case 3:
			LabelCaption = "ADJUST IMAGE SIZE";
			break;
		case 4:
			LabelCaption = "AUDIO OUTPUT CONNECTOR";
			break;
		case 5:
			LabelCaption = "AUDIO VOLUME";
			break;
		case 6:
			LabelCaption = "DOLBY AUDIO TEST";
			break;
		case 7:
			LabelCaption = "DTS AUDIO TEST";
			break;
		case 8:
			LabelCaption = "FINAL SETUP";
			break;
		default:
			LabelCaption = "WELCOME TO AV WIZARD";
	};



	std::string FontColor = SkinGenerator::Instance()->TitleFontColor;
	int FontSize = Utils::StringToInt32(SkinGenerator::Instance()->TitleFontSize);

	SetFontStyle(FontSize, FontColor, "Regular");

	Result->AddChild(CreateControlLabel(
		"PageDesc" + Utils::Int32ToString(i+1), 
		LabelCaption,
		194, 102,
		"Left"
		));



	LabelCaption = "STEP " + Utils::Int32ToString(NoSelectedTab+1);

	FontColor = SkinGenerator::Instance()->MiniTitleFontColor;	
	FontSize = Utils::StringToInt32(SkinGenerator::Instance()->MiniTitleFontSize);
	SetFontStyle(FontSize, FontColor, "Regular");

	Result->AddChild(CreateControlLabel(
		"PageDesc" + Utils::Int32ToString(i+1), 
		LabelCaption,
		194,
		133,
		"Left"
		)
	);

	for(i = 0; i < Wizard::GetInstance()->m_PageSequence.Size(); i++)
	{
		Control = new SettingsDictionaryTree();
		Dictionary = Control->GetDictionary();
		

		if(i <= NoSelectedTab)
			snprintf(Buffer, sizeof(Buffer), "Step%dOn", i);
		else
			snprintf(Buffer, sizeof(Buffer), "Step%dOff", i);

		std::string ControlImageName = Buffer;

		std::string FileImageName;
		if(i <= NoSelectedTab)
			FileImageName = ImageFolder + "button_on.png";
		else
			FileImageName = ImageFolder + "button_off.png";

		Container->AddChild( CreateControlImage(ControlImageName, 
			FileImageName,
			5+i*70, 0
		));
	}


	FontName = FontTitle;

	for (i = 0; i < 9; i++) // XXX: possibly hardcoded
	{
		std::string FontColor, Caption;
		Caption = "STEP " + Utils::Int32ToString(i+1);

		FontSize = Utils::StringToInt32(SkinGenerator::Instance()->StepFontSize);

		FontColor = SkinGenerator::Instance()->StepFontColorShadow;
		SetFontStyle(FontSize, FontColor, "Regular");

		Result->AddChild(CreateControlLabel(
				"PageTabShadow" + Utils::Int32ToString(i+1),
				Caption, 
				38+i*70,
				7,
				"Center"
				));

		if (i == NoSelectedTab)
			FontColor = SkinGenerator::Instance()->StepFontColorHighlight;
		else
		if(i <= NoSelectedTab)
			FontColor = SkinGenerator::Instance()->StepFontColorPassed;
		else
			FontColor = SkinGenerator::Instance()->StepFontColorStandard;
		SetFontStyle(FontSize, FontColor, "Regular");

		Result->AddChild(CreateControlLabel(
				"PageTab" + Utils::Int32ToString(i+1),
				Caption, 
				37+i*70,
				6,
				"Center"
				));
	}

	Result->AddChild(CreateControlImage("WizPageVideoImage", ImageFolder+"video_settings.png", 5, 35));
	Result->AddChild(CreateControlImage("WizPageVideoImage", ImageFolder+"audio_settings.png", 285, 35));
	Result->AddChild(CreateControlImage("WizPageVideoImage", ImageFolder+"final.png", 565, 35));

	FontName = FontMiniTitle;
	SetFontStyle(14, "FFFFFF", "BOLD");
	Result->AddChild(CreateControlLabel(
		"StepDefineVideo_INFO" + Utils::Int32ToString(i+1),
		"VIDEO SETTINGS", 
		130,
		37,
		"Center"
		));

	Result->AddChild(CreateControlLabel(
		"StepDefineAudio_INFO" + Utils::Int32ToString(i+1),
		"AUDIO SETTINGS", 
		435,
		37,
		"Center"
		));

	Result->AddChild(CreateControlLabel(
		"FINAL_INFO",
		"FINAL", 
		600,
		37,
		"Center"
		));

	FontName = FontText;

	return Result;
}

SettingsDictionaryTree* GenerateWizardConfigDefaults::CreateControlImage(std::string ControlName, std::string Picture, int Left, int Top)
{
	//Create an image control that shows a perfect square
	SettingsDictionaryTree* Result= new SettingsDictionaryTree();
	SettingsDictionary* Dictionary = Result->GetDictionary();

	Dictionary->SetName(ControlName);
	Dictionary->SetType("Image");
	Dictionary->Set("Left", Left);
	Dictionary->Set("Top", Top);
	Dictionary->Set("Picture", Picture);

	return Result;
}


SettingsDictionaryTree* GenerateWizardConfigDefaults::CreateControlBackground(
	std::string ControlName, 
	std::string Picture, 
	int Left, int Top)
{
	//Create an image control that shows a perfect square
	SettingsDictionaryTree* Result= new SettingsDictionaryTree();
	SettingsDictionary* Dictionary = Result->GetDictionary();

	Dictionary->SetName(ControlName);
	Dictionary->SetType("Background");
	Dictionary->Set("Left", Left);
	Dictionary->Set("Top", Top);
	Dictionary->Set("Picture", Picture);

	return Result;
}

SettingsDictionaryTree* GenerateWizardConfigDefaults::CreateControlLabel(std::string ControlName, 
										   std::string Caption,
										   int Left, 
										   int Top,
										   std::string Align)
{
	SettingsDictionaryTree* Result= new SettingsDictionaryTree();
	SettingsDictionary* Dictionary = Result->GetDictionary();
	Dictionary->SetName(ControlName);
	Dictionary->SetType("Label");

	Dictionary->Set("Caption", Caption); 

	Dictionary->Set("FontName", FontName); 
	Dictionary->Set("FontColor", Color);
	Dictionary->Set("FontHeight", FontHeight);
	Dictionary->Set("Style", Style);

	Dictionary->Set("Top", Top);
	Dictionary->Set("Left", Left);
	Dictionary->Set("Align", Align);

	return Result;
}

SettingsDictionaryTree* GenerateWizardConfigDefaults::CreateControlPage(std::string ControlName, 
	int Width,
	int Height,
	bool FullScreen,
	std::string Caption)
{
	SettingsDictionaryTree* Result  = new SettingsDictionaryTree();
	SettingsDictionary* Dictionary = Result->GetDictionary();

	Dictionary->SetName(ControlName);
	Dictionary->SetType("Page");
	Dictionary->Set("Width", Width);
	Dictionary->Set("Height", Height);
	if(FullScreen)
		Dictionary->Set("Fullscreen", 1);
	else
		Dictionary->Set("Fullscreen", 0);
	Dictionary->Set("Caption", Caption);

	return Result;
}

void GenerateWizardConfigDefaults::SetFontStyle(
		int FontHeight, 
		std::string Color,
		std::string Style
	)
{
	this->FontHeight = FontHeight;
	this->Style = Style;
	this->Color = Color;
}

SettingsDictionaryTree* GenerateWizardConfigDefaults::CreateControlButton(
	std::string ControlName, 
	std::string Caption, 
	int Left, int Top, int Width,
	bool Expands)
{
	SettingsDictionaryTree* Result  = new SettingsDictionaryTree();
	SettingsDictionary* Dictionary = Result->GetDictionary();

	Dictionary->SetName(ControlName);
	Dictionary->SetType("Button");
	Dictionary->Set("Left", Left);
	Dictionary->Set("Top", Top);
	Dictionary->Set("Width", Width);

	if(Expands)
		Dictionary->Set("Expands", 1);
	else
		Dictionary->Set("Expands", 0);
	
	Dictionary->Set("Caption", Caption);

	return Result;   
}

SettingsDictionaryTree* GenerateWizardConfigDefaults::CreateControlListBox(
	std::string ControlName, 
	std::string Caption, 
	int Left, int Top,
	int Width, int Height,
	bool Expands)
{
	SettingsDictionaryTree* Result  = new SettingsDictionaryTree();
	SettingsDictionary* Dictionary = Result->GetDictionary();

	Dictionary->SetName(ControlName);
	Dictionary->SetType("ListBox");
	Dictionary->Set("Left", Left);
	Dictionary->Set("Top", Top);
	Dictionary->Set("Width", Width);
	Dictionary->Set("Height", Height);

	if(Expands)
		Dictionary->Set("Expands", 1);
	else
		Dictionary->Set("Expands", 0);

	Dictionary->Set("Caption", Caption);

	return Result;   
}

void GenerateWizardConfigDefaults::SetDefaultBtnImages(SettingsDictionary* Dictionary, std::string ImageFolder)
{
	Dictionary->Set("FontColor", "ffffff");
	Dictionary->Set("FontHeight", 17);
	Dictionary->Set("FontName", FontName); 
	Dictionary->Set("Picture", ImageFolder+"button_tex.png");
	Dictionary->Set("PictureLeft", ImageFolder+"button_left.png");
	Dictionary->Set("PictureRight", ImageFolder+"button_right.png");
	Dictionary->Set("PictureHigh ", ImageFolder+"button_high_tex.png");
	Dictionary->Set("PictureHighLeft", ImageFolder+"button_high_left.png");
	Dictionary->Set("PictureHighRight", ImageFolder+"button_high_right.png");

	Dictionary->Set("PictureArrowLeft", ImageFolder+"left_off.png");
	Dictionary->Set("PictureArrowRight", ImageFolder+"right_off.png");
	Dictionary->Set("PictureHighArrowLeft", ImageFolder+"left_on.png");
	Dictionary->Set("PictureHighArrowRight", ImageFolder+"right_on.png");

}

SettingsDictionaryTree* GenerateWizardConfigDefaults::CreateControlScrollList(
	std::string ControlName,
	int Left, int Top,
	int Width, int Height,
	std::string BackColor,
	std::string BackFocusedColor,
	std::string HighBackColor,
	std::string HighBackFocusedColor,
	bool Focused
)
{
	SettingsDictionaryTree* Result  = new SettingsDictionaryTree();
	SettingsDictionary* Dictionary = Result->GetDictionary();

	Dictionary->SetName(ControlName);
	Dictionary->SetType("ScrollList");
	Dictionary->Set("Left", Left);
	Dictionary->Set("Top", Top);
	Dictionary->Set("Width", Width);
	Dictionary->Set("Height", Height);


	Dictionary->Set("ScrollFocused", Focused);
	Dictionary->Set("ScrollBackColor", BackColor);
	Dictionary->Set("ScrollBackFocusedColor", BackFocusedColor);

	Dictionary->Set("HighScrollBackColor", HighBackColor);
	Dictionary->Set("HighBackFocusedColor", HighBackFocusedColor);

	return Result;
}
