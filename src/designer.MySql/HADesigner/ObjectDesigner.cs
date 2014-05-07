using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

using System.Data;
using HAData.Common;
using HAData.DataAccess;
using HAData.Common.TreeBuilder;

namespace HADesigner
{
	/// <summary>
	/// Summary description for DesignObjDesigner.
	/// </summary>
	public class DesignObjDesigner : System.Windows.Forms.Form
	{
		private bool updateImageBlocked = false; // Used to avoid multiple refreshes

		public bool BlockUpdateImage()
		{
			bool origValue = this.updateImageBlocked;
			this.updateImageBlocked = true;
			return origValue;
		}
		public void UnBlockUpdateImage(bool origBlock)
		{
			this.updateImageBlocked = origBlock;
		}

		private void UpdateImage()
		{
			this.UpdateImage(this.updateImageBlocked);
		}
		private void UpdateImage(bool origBlock)
		{
			this.updateImageBlocked = origBlock;
			if (!origBlock) this.m_objUIDesignObjDisplayControl.UpdateImage();
		}

		public bool warnSaveChanges = true;
		private int m_intCommandSelected = -1;
		private int intDesignObjID;
		private int languageID=-1;
		private int skinID=-1;
		StringPair[] spStyles;
		private IContainer components;
		
		public int LanguageID
		{
			get {return this.languageID;}
		}
		public int SkinID
		{
			get {return this.skinID;}
		}


		private MDIForm MainForm 
		{
			get {return (MDIForm)this.MdiParent;}
		}

		public string GraphicsDirectory
		{
			get { return MainForm.GraphicsDirectory;}
		}

		private MyDataSet mds
		{
			get	{return HADataConfiguration.m_mdsCache;}
		}

		private UIDesignObj selectedUIDesignObj
		{
			get 
			{
				if (this.lbChildrenDesignObjs.SelectedIndex == -1) return null;
				else return (UIDesignObj)this.lbChildrenDesignObjs.Items[this.lbChildrenDesignObjs.SelectedIndex];
			}
		}

		private UIDesignObjVariation selectedUIDesignObjVariation // returns only 1 selection
		{
			get {return (UIDesignObjVariation) this.lbVariations.SelectedItems[0];}
		}

		private UIText selectedUIText
		{
			get 
			{
				if (this.lbText.SelectedIndex == -1) return null;
				else return (UIText)this.lbText.Items[this.lbText.SelectedIndex];
			}
		}

		private UIDesignObj selectedUIChild
		{
			get
			{
				if (this.lbChildrenDesignObjs.SelectedIndex == -1) return null;
				else return (UIDesignObj)this.lbChildrenDesignObjs.Items[this.lbChildrenDesignObjs.SelectedIndex];
			}
		}

		private UITextSkinLanguage selectedUITSL
		{
			get {return (UITextSkinLanguage)this.cbLanguageSkin.SelectedItem;}
		}

		private UIChildSkinLanguage selectedUICSL
		{
			get 
			{
				return (UIChildSkinLanguage)this.cbLanguageSkin_Child.SelectedItem;
			}
		}

		public UIChildSkinLanguage FormUICSL
		{
			get 
			{
				if(null != m_objUIDesignObj && null != m_objUIDesignObjDisplayControl)
					return m_objUIDesignObj.GetCurrentChildSkinLanguage(
						m_objUIDesignObjDisplayControl.CurrentLanguageForChild(),
						m_objUIDesignObjDisplayControl.CurrentSkinForChild());
				else 
					return null;
			}
		}

		private UIDesignObjDisplayControl m_objUIDesignObjDisplayControl = null;

		private UIDesignObj m_objUIDesignObj = null;
		private System.Windows.Forms.MainMenu mnuDesignObjXDesigner;
		private System.Windows.Forms.MenuItem mnuZoom;
		private System.Windows.Forms.MenuItem miZoom25;
		private System.Windows.Forms.MenuItem miZoom50;
		private System.Windows.Forms.MenuItem miZoom100;
		private System.Windows.Forms.MenuItem miZoom200;
		private System.Windows.Forms.MenuItem mnuSkin;
		private System.Windows.Forms.MenuItem mnuLanguage;
		private System.Windows.Forms.ListBox lbText;
		private System.Windows.Forms.ComboBox cbLanguageSkin;
		private System.Windows.Forms.TextBox tbY;
		private System.Windows.Forms.Button btnLoadSelectedDesignObj;
		private System.Windows.Forms.ListBox lbVariations;
		private System.Windows.Forms.Button btnRemoveText;
		private System.Windows.Forms.Button button6;
		private System.Windows.Forms.Button btnCancel;
		private System.Windows.Forms.Button btnEditText;
		private System.Windows.Forms.TextBox tbWidth;
		private System.Windows.Forms.TextBox tbHeight;
		private System.Windows.Forms.TextBox tbX;
		private System.Windows.Forms.Button btnRemoveDesignObj;
		private System.Windows.Forms.ComboBox cbAvailableVariations;
		private System.Windows.Forms.Button btnSaveChanges;
		private System.Windows.Forms.ListBox lbChildrenDesignObjs;
		private System.Windows.Forms.Panel panelPreview;
		private System.Windows.Forms.Button btnRemoveSelectedVariation;
		
		private System.Windows.Forms.Panel pnParameters;
		


		private System.Windows.Forms.Button btnAddSkinLanguage;

		private System.Windows.Forms.Button btnRemoveSkinLanguage;
		private System.Windows.Forms.ComboBox cbButton;
		private System.Windows.Forms.ComboBox cbStyle;
		private System.Windows.Forms.TextBox tbDescription;

		private TreeNode tnDesignObj;
		private System.Windows.Forms.TabPage tabParameters;
		private System.Windows.Forms.TabPage tabOnActivate;
		private System.Windows.Forms.TabPage tabOnLoad;
		private System.Windows.Forms.TabPage tabOnUnload;
		private System.Windows.Forms.TabPage tabOnStartup;
		private System.Windows.Forms.TabPage tabOnHighlight;
		private System.Windows.Forms.TabPage tabOnUnhighlight;
		private System.Windows.Forms.TabPage tabOnTimeout;
		private System.Windows.Forms.TabPage tabOnSelected_WithChange;
		private System.Windows.Forms.TabPage tabOnSelected_NoChange;
		private System.Windows.Forms.TabPage tabOnHighlighted;
		private System.Windows.Forms.ComboBox cbAlignH;
		private System.Windows.Forms.ComboBox cbAlignV;
		private System.Windows.Forms.NumericUpDown nudPriority;
		private System.Windows.Forms.Label lblPriority;
		private System.Windows.Forms.Button btnVariationControllerCriteria;
		private System.Windows.Forms.CheckBox chCantGoBack;
		private System.Windows.Forms.TextBox tbRotate;
		private System.Windows.Forms.TextBox tbOpacity;
		


		public bool m_blnInterfaceLocked = false;
		private System.Windows.Forms.CheckBox cbAnimate;
		private System.Windows.Forms.Button btnBGColorTSL;
		private System.Windows.Forms.TabPage tabChildren;
		private System.Windows.Forms.TabPage tabButtons;
		private System.Windows.Forms.TabControl tabAllPages;
		private System.Windows.Forms.Label lVariationOwner;
		private System.Windows.Forms.ComboBox cbVariationStability;
		private System.Windows.Forms.CheckBox chChildBehindBG;
		private System.Windows.Forms.TextBox tbTS_Up;
		private System.Windows.Forms.TextBox tbTS_Down;
		private System.Windows.Forms.TextBox tbTS_Right;
		private System.Windows.Forms.TextBox tbTS_Left;
		private System.Windows.Forms.TextBox tbTiedTo;
		private System.Windows.Forms.Label lDesignObjOwner;
		private System.Windows.Forms.ComboBox cbDesignObjStability;
		private System.Windows.Forms.CheckBox chCanBeHidden;
		private System.Windows.Forms.CheckBox chHideByDefault;
		private System.Windows.Forms.CheckBox chChildBeforeText;
		private System.Windows.Forms.CheckBox chDontMergeBG;
		private System.Windows.Forms.CheckBox chRegenerateForEachScreen;
		private System.Windows.Forms.CheckBox chIsTabStop;
		private System.Windows.Forms.CheckBox chDontResetSelectedState;
		private System.Windows.Forms.TextBox tbVisibleStates;
		private System.Windows.Forms.Button btnChildUp;
		private System.Windows.Forms.Button btnChildDown;
		private System.Windows.Forms.Panel pnEffectsSelectChange;
		private System.Windows.Forms.Label lbEffect;
		private System.Windows.Forms.ComboBox cbEffectsSelectChange;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.ComboBox cbEffectsSelectNoChange;
		private System.Windows.Forms.Panel panel2;
		private System.Windows.Forms.ComboBox cbEffectsHighlight;
		private System.Windows.Forms.Panel pnCommandGroupInfo;
		private System.Windows.Forms.Label lMasterDevice;
		private System.Windows.Forms.CheckBox chRelToSender;
		private System.Windows.Forms.TextBox tbDeviceCategory;
		private System.Windows.Forms.TextBox tbMasterDeviceList;
		private System.Windows.Forms.ComboBox cbBroadcast;
		private System.Windows.Forms.Button btnOnActivateGoto;
		private System.Windows.Forms.ListBox lbSelectedCommands;
		private System.Windows.Forms.Button btnOnActivateDown;
		private System.Windows.Forms.TreeView tvCommand;
		private System.Windows.Forms.Button btnOnActivateUp;
		private System.Windows.Forms.Button btnOnActivateRemoveAG;
		private System.Windows.Forms.Button btnOnActivateAddAG;
		private System.Windows.Forms.Panel pnOnActivateParameters;
		private System.Windows.Forms.Label labAvailableCommands;
		private System.Windows.Forms.Label labSelectedCommands;
		private System.Windows.Forms.ComboBox cbLanguageSkin_Child;
		private System.Windows.Forms.Button btnAddSkinLanguage_Child;
		private System.Windows.Forms.Button btnRemoveSkinLanguage_Child;
		private Button btnAddExplicitText;
		private Button btnAddExplicitDesignObj;

		private CommonMethods m_objCommon = new CommonMethods();

	
		public bool IsInterfaceLocked()
		{
			return m_blnInterfaceLocked;
		}

		public void LockInterface()
		{
			m_blnInterfaceLocked = true;
		}
		
		public void UnlockInterface()
		{
			m_blnInterfaceLocked = false;
		}

		public DesignObjDesigner(int intDesignObjID, TreeNode tnDesignObj)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			/*
			foreach (Control ctl in this.Controls)
			{
				ctl.KeyDown += new KeyEventHandler(this.DesignObjDesigner_KeyDown);
			}
			*/

			this.tnDesignObj = tnDesignObj;
			this.intDesignObjID = intDesignObjID;
		}

		private string defaultDirectory;

		private void DesignObjDesigner_Load(object sender, System.EventArgs e)
		{
			this.defaultDirectory = MainForm.GraphicsDirectory + "/" + mds.tSkin[1].fDataSubdirectory;

			//TODO ENDER
			m_objUIDesignObj = new UIDesignObj(null, -1, intDesignObjID, this.GraphicsDirectory);
			m_objUIDesignObjDisplayControl = new UIDesignObjDisplayControl(this, m_objUIDesignObj, 25);
			m_objUIDesignObjDisplayControl.Setup();
			panelPreview.Controls.Add(m_objUIDesignObjDisplayControl);

			this.Text = m_objUIDesignObj.Description; // + " (" + intDesignObjID + ")";

			chCantGoBack.Checked = m_objUIDesignObj.CantGoBack;

			this.miZoom25.Checked = true;

			UpdateCButtonList();
			UpdateAvailableVariationsList();
			UpdateVariationsList();
			UpdateDevices();
			UpdateChildrenDesignObjs();
			UpdateCommandGroups();
			UpdateEffects();
			
			clearLanguagesSkins();
			this.loadAvailableStyles();

			if( lbVariations.Items.Count==0 )
			{
				MessageBox.Show("Error: This object has no variations!  Database is corrupt.");
				return;
			}

			lbVariations.SelectedIndex = 0;
			
			foreach(DataRow dr in mds.tLanguage.Rows)
			{
				LanguageDataRow ldr =  new LanguageDataRow(dr);
				MenuItemID miLanguage = new MenuItemID(ldr.fPK_Language,ldr.fDescription);
				miLanguage.Click += new System.EventHandler(this.miLanguage_Click);
				this.mnuLanguage.MenuItems.Add(miLanguage);
			}
			this.mnuLanguage.MenuItems[0].Checked = true;

			foreach(DataRow dr in mds.tSkin.Rows)
			{
				SkinDataRow sdr = new SkinDataRow(dr);
				MenuItemID miSkin = new MenuItemID(sdr.fPK_Skin,sdr.fDescription);
				miSkin.Click += new System.EventHandler(this.miSkin_Click);
				this.mnuSkin.MenuItems.Add(miSkin);
			}
			this.mnuSkin.MenuItems[0].Checked = true;

			this.tbDescription.Text = this.m_objUIDesignObj.DescriptionOnly;
			//			this.nudPriority.Value = this.m_objUIDesignObj.Priority;
			//			this.cbAnimate.Checked = this.m_objUIDesignObj.Animate;
			//get the actions
			CommandTree.CreateTree(this.tvCommand);



		}


	


		//for now, we can only select one object

		public void UpdateVariationsList()
		{
			lbVariations.Items.Clear();
			foreach(Object obj in m_objUIDesignObj.UIDesignObjVariations)
			{
				UIDesignObjVariation objVariation = (UIDesignObjVariation) obj;
				if(objVariation.Include)
				{
					lbVariations.Items.Add(objVariation);
				}
			}
		}

		private void UpdateAvailableVariationsList()
		{
			//update the available variations list
			DataRow[] drCriterias = mds.tUI.Select("", UIData.DESCRIPTION_FIELD);
			cbAvailableVariations.Items.Clear();
			//			cbAvailableVariations.DisplayMember = "Description";
			//			cbAvailableVariations.ValueMember = "ID";
			foreach(DataRow dr in drCriterias)
			{
				UIDataRow drCriteria = new UIDataRow(dr);

				int intUIID = drCriteria.fPK_UI;
				//see if there is a variation with the given criteria id

				StringPair objPair = new StringPair(Convert.ToString(intUIID), drCriteria.fDescription);
				cbAvailableVariations.Items.Add(objPair);
			}
		}

		private void clearLanguagesSkins()
		{
			this.cbLanguageSkin.Items.Clear();
			this.cbLanguageSkin.Enabled = this.cbAlignV.Enabled = this.cbAlignH.Enabled = this.cbStyle.Enabled = false;
		}

		private void loadAvailableStyles()
		{
			this.spStyles = new StringPair[mds.tStyle.Rows.Count+1]; // +1 For Default
			this.cbStyle.Items.Clear();
			
			this.spStyles[0] = new StringPair("-1","Default");
			this.cbStyle.Items.Add(this.spStyles[0]);
			
			int iStyle = 1;
			foreach (DataRow dr in mds.tStyle.Rows)
			{
				StyleDataRow sdr = new StyleDataRow(dr);
				this.spStyles[iStyle] = new StringPair(sdr.fPK_Style.ToString(),sdr.fDescription);// TODO: Style Description can be null ?
				this.cbStyle.Items.Add(this.spStyles[iStyle++]);	
			}
		}

		private void loadAvailableLanguageSkins() // for currently selected text item
		{
			this.cbLanguageSkin.Items.Clear();
			this.cbLanguageSkin.Enabled = true;

			if(null != this.selectedUIText)
			{
				foreach (UITextSkinLanguage uiTSL in this.selectedUIText.TextSkinLanguages)
				{
					if (uiTSL.Include) this.cbLanguageSkin.Items.Add(uiTSL);
				}
				if (this.selectedUIText.TextSkinLanguages.Count > 0) // should always be true
				{
					this.cbLanguageSkin.SelectedIndex = 0;
					this.cbAlignV.Enabled = this.cbAlignH.Enabled = this.cbStyle.Enabled = true;
				}
			}
		}

		private void loadAvailableLanguageSkins_Child() // for currently selected text item
		{
			this.cbLanguageSkin_Child.Items.Clear();
			this.cbLanguageSkin_Child.Enabled = true;

			if(null != this.selectedUIChild)
			{
				foreach (UIChildSkinLanguage uiCSL in this.selectedUIChild.ChildSkinLanguages)
				{
					if (uiCSL.Include) this.cbLanguageSkin_Child.Items.Add(uiCSL);
				}
			}
		}

		public void UpdateChildrenDesignObjs() // also updates text
		{
			lbChildrenDesignObjs.Items.Clear();
			this.lbText.Items.Clear();

			foreach(Object obj in m_objUIDesignObj.UIDesignObjVariations)
			{
				UIDesignObjVariation objVariation = (UIDesignObjVariation) obj;
				if(objVariation.Selected)
				{

					//we want to put a blank space between each set of variation objects
					if(lbChildrenDesignObjs.Items.Count > 0)
					{
						UIDesignObj objBlank = new UIDesignObj(null, -1, -1, this.GraphicsDirectory);
						lbChildrenDesignObjs.Items.Add(objBlank);
					}


					foreach(UIDesignObj objDesignObj in objVariation.DesignObjs)
					{
						if(objDesignObj.Include)
						{
							int intNewIndex = lbChildrenDesignObjs.Items.Add(objDesignObj);
							if(objDesignObj.Selected)
							{
								lbChildrenDesignObjs.SelectedIndex = intNewIndex; 
							}
						}
					}

					if(lbText.Items.Count > 0)
					{
						UIText textBlank = new UIText();
						lbText.Items.Add(textBlank);
					}

					foreach(UIText uitext in objVariation.Text) 
					{						
						if(uitext.Include)
						{
							int newIndex = this.lbText.Items.Add(uitext);
							
							if(uitext.Selected)
							{
								lbText.SelectedIndex = newIndex;
							}
						}
					}
				}
			}
		}

		public void UpdateVariationParameters()
		{
			if(lbVariations.SelectedItems.Count != 1)
			{
				pnParameters.Controls.Clear();
			}
			else
			{

				pnParameters.Controls.Clear();

				UIDesignObjVariation objSelectedVariation = (UIDesignObjVariation) lbVariations.SelectedItems[0];

				int intCount = -1;
				foreach(UIDesignObjVariationParameter objUIParameter in objSelectedVariation.Parameters)
				{
					if(objUIParameter.Include)
					{
						intCount++;

						ParameterControl objParameter = new ParameterControl(this.defaultDirectory);
						objParameter.UIDesignObjVariationParameter = objUIParameter;
						objParameter.FK_DesignObjParameter = objUIParameter.CParameterID;
						objParameter.Description = objUIParameter.Description + " (" + objUIParameter.CParameterID + ")";


						objParameter.TextBoxValue = new TextBox();
						objParameter.TextBoxValue.Top = 25 * intCount;
						objParameter.TextBoxValue.Left = 125;
						objParameter.TextBoxValue.Width = 250;

						
						objParameter.TextBoxValue.Text = objUIParameter.Value;


						objParameter.TextBoxValue.KeyDown += new System.Windows.Forms.KeyEventHandler(objParameter.KeyPressTextBox);
						objParameter.TextBoxValue.Leave += new System.EventHandler(objParameter.TextBoxLeave);
						
						Label objLabel = new Label();
						objLabel.Width = 120;
						objLabel.Top = 25 * intCount;
						objLabel.Left = 5;
						objLabel.Text = objParameter.Description;

						pnParameters.Controls.Add(objLabel);
						pnParameters.Controls.Add(objParameter.TextBoxValue);

						//CommonMethods objCommon = new CommonMethods();

						switch (objUIParameter.CParameterID)
						{
							case DesignObjParameterData.GRAPHIC_FILENAME_CONST:
								Button objButton = new Button();
								objButton.Text = "File";
								objButton.Width = 50;
								objButton.Top = 25 * intCount;
								objButton.Left = 380;

								// objParameter.FileButton = objButton;

								objButton.Click += new System.EventHandler(objParameter.OpenFile);
								pnParameters.Controls.Add(objButton);
								break;
							case DesignObjParameterData.ALTERNATE_CELL_COLOR_CONST:
							case DesignObjParameterData.CELL_COLOR_CONST:
							case DesignObjParameterData.FIRST_COLUMN_COLOR_CONST:
							case DesignObjParameterData.FIRST_ROW_COLOR_CONST:
							case DesignObjParameterData.SELECTED_CELL_COLOR_CONST:
								Button colorButton = new Button();

								
								if(CommonMethods.IsNumeric(objParameter.TextBoxValue.Text))
								{
									colorButton.BackColor = (objParameter.TextBoxValue.Text=="")? SystemColors.Control : Color.FromArgb(Convert.ToInt32(objParameter.TextBoxValue.Text));
								}
								else
								{
									colorButton.BackColor = SystemColors.Control;
								}
								colorButton.Width = 50;
								colorButton.Top = 25 * intCount;
								colorButton.Left = 380;

								objParameter.ColorButton = colorButton;

								colorButton.Click += new System.EventHandler(objParameter.SelectColor);
								pnParameters.Controls.Add(colorButton);
								break;
						}
					}
				}
			}
		}


		private void UpdateAGParms(UIDesignObjVariationCommand objCommand)
		{
			if(lbVariations.SelectedItems.Count != 1)
			{
				tbMasterDeviceList.Text="";
				tbDeviceCategory.Text="";
				chRelToSender.Checked=false;
				lMasterDevice.Text="";
				cbBroadcast.SelectedIndex=0;
				
				tbMasterDeviceList.Enabled=false;
				tbDeviceCategory.Enabled=false;
				chRelToSender.Enabled=false;
				lMasterDevice.Enabled=false;
				cbBroadcast.Enabled=false;
				
				pnOnActivateParameters.Controls.Clear();

			}
			else
			{
				tbMasterDeviceList.Enabled=true;
				tbDeviceCategory.Enabled=true;
				chRelToSender.Enabled=true;
				lMasterDevice.Enabled=true;
				cbBroadcast.Enabled=true;

				tbMasterDeviceList.Text = objCommand.m_intMasterDeviceList==-1 ? "" : objCommand.m_intMasterDeviceList.ToString();
				tbDeviceCategory.Text = objCommand.m_intDeviceCategory==-1 ? "" : objCommand.m_intDeviceCategory.ToString();
				int intSelectIndex;
				int intCount;

				intSelectIndex = 0;
				intCount = -1;
				foreach(StringPair objPair in cbBroadcast.Items)
				{
					intCount++;
					if(objPair.ID == Convert.ToString(objCommand.m_intBroadcast))
					{
						intSelectIndex = intCount;
					}
				}
				cbBroadcast.SelectedIndex = intSelectIndex;

				pnOnActivateParameters.Controls.Clear();

				intCount = -1;
				foreach(Object obj in objCommand.Parameters)
				{

					UIDesignObjVariationCommandParameter objUICommandParameter = (UIDesignObjVariationCommandParameter) obj;


					if(objUICommandParameter.Include)
					{
						intCount++;



						ParameterControl objCommandParameter = new ParameterControl(this.defaultDirectory);
						objCommandParameter.UIDesignObjVariationCommandParameter = objUICommandParameter;
						//objCommandParameter.FK_DesignObjParameter = objUIParameter.CParameterID;
						objCommandParameter.Description = objUICommandParameter.Description + " (" + objUICommandParameter.ParameterID + ")";



						objCommandParameter.TextBoxValue = new TextBox();
						objCommandParameter.TextBoxValue.Top = 25 * intCount;
						objCommandParameter.TextBoxValue.Left = 275;
						objCommandParameter.TextBoxValue.Width = 100;
						objCommandParameter.TextBoxValue.Text = objUICommandParameter.Value;
					
						objCommandParameter.TextBoxValue.KeyDown += new System.Windows.Forms.KeyEventHandler(objCommandParameter.KeyPressTextBox);
						objCommandParameter.TextBoxValue.Leave += new System.EventHandler(objCommandParameter.TextBoxLeave);
					

						Label objLabel = new Label();
						objLabel.Width = 170;
						objLabel.Top = 25 * intCount;
						objLabel.Left = 5;
						objLabel.Text = objCommandParameter.Description;

						pnOnActivateParameters.Controls.Add(objLabel);
						pnOnActivateParameters.Controls.Add(objCommandParameter.TextBoxValue);

						if(objUICommandParameter.ParameterID == CommandParameterData.FILENAME_CONST )
						{
							Button objButton = new Button();
							objButton.Text = "File";
							objButton.Width = 50;
							objButton.Top = 25 * intCount;
							objButton.Left = 430;

							// objCommandParameter.FileButton = objButton;

							objButton.Click += new System.EventHandler(objCommandParameter.OpenFile);
							pnOnActivateParameters.Controls.Add(objButton);
						}
						
					}

				}
				


			}

		}


		private void UpdateCButtonList()
		{
			cbButton.Items.Clear();

			DataRow[] drButtons = this.mds.tButton.Select(null, ButtonData.DESCRIPTION_FIELD);
			ButtonDataRow drButton;
			foreach(DataRow dr in drButtons)
			{
				drButton = new ButtonDataRow(dr);
				cbButton.Items.Add(new StringPair(Convert.ToString(drButton.fPK_Button), drButton.fDescription));
			}
		}

		//this will populate
		private void UpdateEffects()
		{
			cbEffectsSelectChange.Items.Clear();
			cbEffectsSelectNoChange.Items.Clear();
			cbEffectsHighlight.Items.Clear();

			DataRow[] drEffects = this.mds.tEffectType.Select(null, EffectTypeData.DESCRIPTION_FIELD);
			EffectTypeDataRow drEffect;
			foreach(DataRow dr in drEffects)
			{
				drEffect = new EffectTypeDataRow(dr);

				String PK_EffectType = Convert.ToString(drEffect.fPK_EffectType);
				StringPair item = new StringPair(PK_EffectType, drEffect.fDescription);

				cbEffectsSelectChange.Items.Add(item);
				cbEffectsSelectNoChange.Items.Add(item);
				cbEffectsHighlight.Items.Add(item);
			}
		}

		private void UpdateDevices()
		{
			cbBroadcast.Items.Clear();
			cbBroadcast.Items.Add(new StringPair("-1", ""));
		}

		private void UpdateVariationCommands()
		{
			if(lbVariations.SelectedItems.Count != 1)
			{
				//they must have one and only one variation selected to view the actions

				//btnFindGotoDesignObj.Enabled = false;
				cbButton.SelectedIndex = -1;
				cbButton.Enabled = false;
				chDontResetSelectedState.Enabled = false;


				//tbGotoDesignObj.Text = "";

			}
			else
			{

				UIDesignObjVariation objSelectedVariation = (UIDesignObjVariation) lbVariations.SelectedItems[0];



				//btnFindGotoDesignObj.Enabled = true;
				cbButton.Enabled = true;
				chDontResetSelectedState.Enabled = true;
				chDontResetSelectedState.Checked = objSelectedVariation.DontResetSelectedState;


				//if(objSelectedVariation.GotoUIDesignObjID != -1)
				//{
				//	tbGotoDesignObj.Text = this.mds.tDesignObj[objSelectedVariation.GotoUIDesignObjID].fDescription + " (" + objSelectedVariation.GotoUIDesignObjID + ")";
				//}
				//else
				//{
				//	tbGotoDesignObj.Text = "";
				//}


				//select the appropriate CButton
				StringPair objSelectedPair = null;
				foreach(StringPair objPair in cbButton.Items)
				{
					if(Convert.ToInt32(objPair.ID) == objSelectedVariation.CButton)
					{
						objSelectedPair = objPair;
						break;
					}
				}
				if(objSelectedPair == null)
				{
					cbButton.SelectedIndex = -1;
				}
				else
				{
					cbButton.SelectedItem = objSelectedPair;
				}

				//effects

				//select with change
				StringPair objSelectedPair_EffectSelectWithChange = null;
				foreach(StringPair objPair in cbEffectsSelectChange.Items)
				{
					if(Convert.ToInt32(objPair.ID) == objSelectedVariation.CEffectWithChange)
					{
						objSelectedPair_EffectSelectWithChange = objPair;
						break;
					}
				}
				if(objSelectedPair_EffectSelectWithChange == null)
				{
					cbEffectsSelectChange.SelectedIndex = 0;
				}
				else
				{
					cbEffectsSelectChange.SelectedItem = objSelectedPair_EffectSelectWithChange;
				}


				//select no change
				StringPair objSelectedPair_EffectSelectNoChange = null;
				foreach(StringPair objPair in cbEffectsSelectNoChange.Items)
				{
					if(Convert.ToInt32(objPair.ID) == objSelectedVariation.CEffectNoChange)
					{
						objSelectedPair_EffectSelectNoChange = objPair;
						break;
					}
				}
				if(objSelectedPair_EffectSelectNoChange == null)
				{
					cbEffectsSelectNoChange.SelectedIndex = 0;
				}
				else
				{
					cbEffectsSelectNoChange.SelectedItem = objSelectedPair_EffectSelectNoChange;
				}

				//highlight
				StringPair objSelectedPair_EffectHighlight = null;
				foreach(StringPair objPair in cbEffectsHighlight.Items)
				{
					if(Convert.ToInt32(objPair.ID) == objSelectedVariation.CEffectHightlight)
					{
						objSelectedPair_EffectHighlight = objPair;
						break;
					}
				}
				if(objSelectedPair_EffectHighlight == null)
				{
					cbEffectsHighlight.SelectedIndex = 0;
				}
				else
				{
					cbEffectsHighlight.SelectedItem = objSelectedPair_EffectHighlight;
				}
			}

		}


		public void UpdateCommandGroups()
		{
			// make sure one and only one variation is selected
			if (lbVariations.SelectedItems.Count != 1)
			{
				tvCommand.Enabled = false;
				btnOnActivateAddAG.Enabled = false;
				btnOnActivateRemoveAG.Enabled = false;
				lbSelectedCommands.Enabled = false;
			}
			else
			{
				pnCommandGroupInfo.Visible = tabAllPages.SelectedIndex > 2 && tabAllPages.SelectedIndex < 10;
				if (pnCommandGroupInfo.Visible)
				{
					tvCommand.Enabled = true;
					lbSelectedCommands.Enabled = true;

					UIDesignObjVariationCommandGroup objAG = GetSelectedCommandGroup();

					lbSelectedCommands.Items.Clear();
					
					//load the action group
					if(objAG != null)
					{					
						//update the selected actions list
						foreach(UIDesignObjVariationCommand objA in objAG.Commands)
						{
							if(objA.Include)
							{
								lbSelectedCommands.Items.Add(objA);
							}
						}
					}
					btnOnActivateRemoveAG.Enabled = (lbSelectedCommands.Items.Count > 0);

					//clear the parameters
					pnOnActivateParameters.Controls.Clear();
					pnOnActivateParameters.Visible = true;

					tbMasterDeviceList.Text="";
					tbDeviceCategory.Text="";
					chRelToSender.Checked=false;
					lMasterDevice.Text="";
					cbBroadcast.SelectedIndex=0;
				
					tbMasterDeviceList.Enabled=false;
					tbDeviceCategory.Enabled=false;
					chRelToSender.Enabled=false;
					lMasterDevice.Enabled=false;
					cbBroadcast.Enabled=false;
				}
			}
		}



		public void SelectDesignObj(UIDesignObj objSelectedUIDesignObj)
		{
			if(objSelectedUIDesignObj == null)
			{
				lbChildrenDesignObjs.SelectedIndex = -1;
			}
			else
			{
				lbChildrenDesignObjs.SelectedItem = objSelectedUIDesignObj;
			}
		}
		




		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.Windows.Forms.Label label1;
			System.Windows.Forms.Label label2;
			System.Windows.Forms.Label label3;
			System.Windows.Forms.Label label4;
			System.Windows.Forms.Label label6;
			System.Windows.Forms.Label label20;
			System.Windows.Forms.Label label13;
			System.Windows.Forms.Label label12;
			System.Windows.Forms.Label label11;
			System.Windows.Forms.Label label21;
			System.Windows.Forms.Label label19;
			System.Windows.Forms.Label label17;
			System.Windows.Forms.Label label18;
			System.Windows.Forms.Label label16;
			System.Windows.Forms.Label label15;
			System.Windows.Forms.Label label14;
			System.Windows.Forms.Label label5;
			System.Windows.Forms.Label label7;
			System.Windows.Forms.Label label22;
			System.Windows.Forms.Label label23;
			System.Windows.Forms.Label label10;
			System.Windows.Forms.Label label9;
			System.Windows.Forms.Label label8;
			this.panelPreview = new System.Windows.Forms.Panel();
			this.lbVariations = new System.Windows.Forms.ListBox();
			this.button6 = new System.Windows.Forms.Button();
			this.btnSaveChanges = new System.Windows.Forms.Button();
			this.cbAvailableVariations = new System.Windows.Forms.ComboBox();
			this.lbChildrenDesignObjs = new System.Windows.Forms.ListBox();
			this.btnLoadSelectedDesignObj = new System.Windows.Forms.Button();
			this.btnCancel = new System.Windows.Forms.Button();
			this.btnRemoveDesignObj = new System.Windows.Forms.Button();
			this.tbX = new System.Windows.Forms.TextBox();
			this.tbY = new System.Windows.Forms.TextBox();
			this.tbWidth = new System.Windows.Forms.TextBox();
			this.tbHeight = new System.Windows.Forms.TextBox();
			this.pnParameters = new System.Windows.Forms.Panel();
			this.btnRemoveSelectedVariation = new System.Windows.Forms.Button();
			this.lbText = new System.Windows.Forms.ListBox();
			this.btnEditText = new System.Windows.Forms.Button();
			this.btnRemoveText = new System.Windows.Forms.Button();
			this.mnuDesignObjXDesigner = new System.Windows.Forms.MainMenu(this.components);
			this.mnuZoom = new System.Windows.Forms.MenuItem();
			this.miZoom25 = new System.Windows.Forms.MenuItem();
			this.miZoom50 = new System.Windows.Forms.MenuItem();
			this.miZoom100 = new System.Windows.Forms.MenuItem();
			this.miZoom200 = new System.Windows.Forms.MenuItem();
			this.mnuSkin = new System.Windows.Forms.MenuItem();
			this.mnuLanguage = new System.Windows.Forms.MenuItem();
			this.cbLanguageSkin = new System.Windows.Forms.ComboBox();
			this.cbAlignH = new System.Windows.Forms.ComboBox();
			this.chCantGoBack = new System.Windows.Forms.CheckBox();
			this.btnVariationControllerCriteria = new System.Windows.Forms.Button();
			this.chRegenerateForEachScreen = new System.Windows.Forms.CheckBox();
			this.chChildBehindBG = new System.Windows.Forms.CheckBox();
			this.chCanBeHidden = new System.Windows.Forms.CheckBox();
			this.cbAlignV = new System.Windows.Forms.ComboBox();
			this.btnRemoveSkinLanguage = new System.Windows.Forms.Button();
			this.cbStyle = new System.Windows.Forms.ComboBox();
			this.btnAddSkinLanguage = new System.Windows.Forms.Button();
			this.tabButtons = new System.Windows.Forms.TabPage();
			this.cbButton = new System.Windows.Forms.ComboBox();
			this.tabAllPages = new System.Windows.Forms.TabControl();
			this.tabParameters = new System.Windows.Forms.TabPage();
			this.cbDesignObjStability = new System.Windows.Forms.ComboBox();
			this.lDesignObjOwner = new System.Windows.Forms.Label();
			this.cbVariationStability = new System.Windows.Forms.ComboBox();
			this.chDontResetSelectedState = new System.Windows.Forms.CheckBox();
			this.lVariationOwner = new System.Windows.Forms.Label();
			this.tabChildren = new System.Windows.Forms.TabPage();
			this.btnAddExplicitText = new System.Windows.Forms.Button();
			this.btnAddExplicitDesignObj = new System.Windows.Forms.Button();
			this.cbLanguageSkin_Child = new System.Windows.Forms.ComboBox();
			this.btnAddSkinLanguage_Child = new System.Windows.Forms.Button();
			this.btnRemoveSkinLanguage_Child = new System.Windows.Forms.Button();
			this.btnChildDown = new System.Windows.Forms.Button();
			this.btnChildUp = new System.Windows.Forms.Button();
			this.tbVisibleStates = new System.Windows.Forms.TextBox();
			this.tbTiedTo = new System.Windows.Forms.TextBox();
			this.tbTS_Right = new System.Windows.Forms.TextBox();
			this.tbTS_Left = new System.Windows.Forms.TextBox();
			this.tbTS_Down = new System.Windows.Forms.TextBox();
			this.tbTS_Up = new System.Windows.Forms.TextBox();
			this.chIsTabStop = new System.Windows.Forms.CheckBox();
			this.chDontMergeBG = new System.Windows.Forms.CheckBox();
			this.chChildBeforeText = new System.Windows.Forms.CheckBox();
			this.tbOpacity = new System.Windows.Forms.TextBox();
			this.tbRotate = new System.Windows.Forms.TextBox();
			this.btnBGColorTSL = new System.Windows.Forms.Button();
			this.chHideByDefault = new System.Windows.Forms.CheckBox();
			this.tabOnActivate = new System.Windows.Forms.TabPage();
			this.tabOnLoad = new System.Windows.Forms.TabPage();
			this.tabOnUnload = new System.Windows.Forms.TabPage();
			this.tabOnStartup = new System.Windows.Forms.TabPage();
			this.tabOnHighlight = new System.Windows.Forms.TabPage();
			this.tabOnUnhighlight = new System.Windows.Forms.TabPage();
			this.tabOnTimeout = new System.Windows.Forms.TabPage();
			this.tabOnSelected_WithChange = new System.Windows.Forms.TabPage();
			this.pnEffectsSelectChange = new System.Windows.Forms.Panel();
			this.lbEffect = new System.Windows.Forms.Label();
			this.cbEffectsSelectChange = new System.Windows.Forms.ComboBox();
			this.tabOnSelected_NoChange = new System.Windows.Forms.TabPage();
			this.panel1 = new System.Windows.Forms.Panel();
			this.cbEffectsSelectNoChange = new System.Windows.Forms.ComboBox();
			this.tabOnHighlighted = new System.Windows.Forms.TabPage();
			this.panel2 = new System.Windows.Forms.Panel();
			this.cbEffectsHighlight = new System.Windows.Forms.ComboBox();
			this.tbDescription = new System.Windows.Forms.TextBox();
			this.nudPriority = new System.Windows.Forms.NumericUpDown();
			this.lblPriority = new System.Windows.Forms.Label();
			this.cbAnimate = new System.Windows.Forms.CheckBox();
			this.pnCommandGroupInfo = new System.Windows.Forms.Panel();
			this.lMasterDevice = new System.Windows.Forms.Label();
			this.chRelToSender = new System.Windows.Forms.CheckBox();
			this.tbDeviceCategory = new System.Windows.Forms.TextBox();
			this.tbMasterDeviceList = new System.Windows.Forms.TextBox();
			this.cbBroadcast = new System.Windows.Forms.ComboBox();
			this.btnOnActivateGoto = new System.Windows.Forms.Button();
			this.lbSelectedCommands = new System.Windows.Forms.ListBox();
			this.btnOnActivateDown = new System.Windows.Forms.Button();
			this.tvCommand = new System.Windows.Forms.TreeView();
			this.btnOnActivateUp = new System.Windows.Forms.Button();
			this.btnOnActivateRemoveAG = new System.Windows.Forms.Button();
			this.btnOnActivateAddAG = new System.Windows.Forms.Button();
			this.pnOnActivateParameters = new System.Windows.Forms.Panel();
			this.labAvailableCommands = new System.Windows.Forms.Label();
			this.labSelectedCommands = new System.Windows.Forms.Label();
			label1 = new System.Windows.Forms.Label();
			label2 = new System.Windows.Forms.Label();
			label3 = new System.Windows.Forms.Label();
			label4 = new System.Windows.Forms.Label();
			label6 = new System.Windows.Forms.Label();
			label20 = new System.Windows.Forms.Label();
			label13 = new System.Windows.Forms.Label();
			label12 = new System.Windows.Forms.Label();
			label11 = new System.Windows.Forms.Label();
			label21 = new System.Windows.Forms.Label();
			label19 = new System.Windows.Forms.Label();
			label17 = new System.Windows.Forms.Label();
			label18 = new System.Windows.Forms.Label();
			label16 = new System.Windows.Forms.Label();
			label15 = new System.Windows.Forms.Label();
			label14 = new System.Windows.Forms.Label();
			label5 = new System.Windows.Forms.Label();
			label7 = new System.Windows.Forms.Label();
			label22 = new System.Windows.Forms.Label();
			label23 = new System.Windows.Forms.Label();
			label10 = new System.Windows.Forms.Label();
			label9 = new System.Windows.Forms.Label();
			label8 = new System.Windows.Forms.Label();
			this.tabButtons.SuspendLayout();
			this.tabAllPages.SuspendLayout();
			this.tabParameters.SuspendLayout();
			this.tabChildren.SuspendLayout();
			this.tabOnSelected_WithChange.SuspendLayout();
			this.pnEffectsSelectChange.SuspendLayout();
			this.tabOnSelected_NoChange.SuspendLayout();
			this.panel1.SuspendLayout();
			this.tabOnHighlighted.SuspendLayout();
			this.panel2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.nudPriority)).BeginInit();
			this.pnCommandGroupInfo.SuspendLayout();
			this.SuspendLayout();
			// 
			// label1
			// 
			label1.Location = new System.Drawing.Point(400, 104);
			label1.Name = "label1";
			label1.Size = new System.Drawing.Size(16, 23);
			label1.TabIndex = 17;
			label1.Text = "X";
			// 
			// label2
			// 
			label2.Location = new System.Drawing.Point(464, 104);
			label2.Name = "label2";
			label2.Size = new System.Drawing.Size(16, 23);
			label2.TabIndex = 19;
			label2.Text = "Y";
			// 
			// label3
			// 
			label3.Location = new System.Drawing.Point(400, 128);
			label3.Name = "label3";
			label3.Size = new System.Drawing.Size(40, 23);
			label3.TabIndex = 21;
			label3.Text = "W";
			// 
			// label4
			// 
			label4.Location = new System.Drawing.Point(464, 128);
			label4.Name = "label4";
			label4.Size = new System.Drawing.Size(40, 23);
			label4.TabIndex = 23;
			label4.Text = "H";
			// 
			// label6
			// 
			label6.Location = new System.Drawing.Point(32, 24);
			label6.Name = "label6";
			label6.Size = new System.Drawing.Size(100, 23);
			label6.TabIndex = 13;
			label6.Text = "Button";
			// 
			// label20
			// 
			label20.Location = new System.Drawing.Point(608, 88);
			label20.Name = "label20";
			label20.Size = new System.Drawing.Size(100, 16);
			label20.TabIndex = 51;
			label20.Text = "Stability:";
			// 
			// label13
			// 
			label13.Location = new System.Drawing.Point(608, 8);
			label13.Name = "label13";
			label13.Size = new System.Drawing.Size(112, 16);
			label13.TabIndex = 48;
			label13.Text = "Global options";
			// 
			// label12
			// 
			label12.Location = new System.Drawing.Point(456, 136);
			label12.Name = "label12";
			label12.Size = new System.Drawing.Size(100, 16);
			label12.TabIndex = 47;
			label12.Text = "Stability:";
			// 
			// label11
			// 
			label11.Location = new System.Drawing.Point(456, 8);
			label11.Name = "label11";
			label11.Size = new System.Drawing.Size(136, 16);
			label11.TabIndex = 43;
			label11.Text = "Variation specific options";
			// 
			// label21
			// 
			label21.Location = new System.Drawing.Point(664, 128);
			label21.Name = "label21";
			label21.Size = new System.Drawing.Size(80, 16);
			label21.TabIndex = 63;
			label21.Text = "Visible States:";
			// 
			// label19
			// 
			label19.Location = new System.Drawing.Point(664, 88);
			label19.Name = "label19";
			label19.Size = new System.Drawing.Size(72, 16);
			label19.TabIndex = 61;
			label19.Text = "Tie state to:";
			// 
			// label17
			// 
			label17.Location = new System.Drawing.Point(712, 64);
			label17.Name = "label17";
			label17.Size = new System.Drawing.Size(16, 23);
			label17.TabIndex = 59;
			label17.Text = "R:";
			label17.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label18
			// 
			label18.Location = new System.Drawing.Point(688, 64);
			label18.Name = "label18";
			label18.Size = new System.Drawing.Size(16, 23);
			label18.TabIndex = 57;
			label18.Text = "L:";
			label18.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label16
			// 
			label16.Location = new System.Drawing.Point(712, 40);
			label16.Name = "label16";
			label16.Size = new System.Drawing.Size(16, 23);
			label16.TabIndex = 55;
			label16.Text = "D:";
			label16.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label15
			// 
			label15.Location = new System.Drawing.Point(688, 40);
			label15.Name = "label15";
			label15.Size = new System.Drawing.Size(16, 23);
			label15.TabIndex = 53;
			label15.Text = "U:";
			label15.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label14
			// 
			label14.Location = new System.Drawing.Point(656, 24);
			label14.Name = "label14";
			label14.Size = new System.Drawing.Size(100, 16);
			label14.TabIndex = 52;
			label14.Text = "Tab Stop Help";
			// 
			// label5
			// 
			label5.Location = new System.Drawing.Point(400, 152);
			label5.Name = "label5";
			label5.Size = new System.Drawing.Size(40, 23);
			label5.TabIndex = 44;
			label5.Text = "R";
			// 
			// label7
			// 
			label7.Location = new System.Drawing.Point(464, 152);
			label7.Name = "label7";
			label7.Size = new System.Drawing.Size(40, 23);
			label7.TabIndex = 46;
			label7.Text = "O";
			// 
			// label22
			// 
			label22.Location = new System.Drawing.Point(16, 16);
			label22.Name = "label22";
			label22.Size = new System.Drawing.Size(216, 16);
			label22.TabIndex = 1;
			label22.Text = "Effect";
			// 
			// label23
			// 
			label23.Location = new System.Drawing.Point(16, 16);
			label23.Name = "label23";
			label23.Size = new System.Drawing.Size(216, 16);
			label23.TabIndex = 1;
			label23.Text = "Effect";
			// 
			// label10
			// 
			label10.Location = new System.Drawing.Point(440, 32);
			label10.Name = "label10";
			label10.Size = new System.Drawing.Size(96, 23);
			label10.TabIndex = 20;
			label10.Text = "Device Category:";
			label10.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label9
			// 
			label9.Location = new System.Drawing.Point(496, 8);
			label9.Name = "label9";
			label9.Size = new System.Drawing.Size(80, 16);
			label9.TabIndex = 19;
			label9.Text = "Master Device:";
			label9.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label8
			// 
			label8.Location = new System.Drawing.Point(344, 16);
			label8.Name = "label8";
			label8.Size = new System.Drawing.Size(64, 16);
			label8.TabIndex = 18;
			label8.Text = "Broadcast:";
			// 
			// panelPreview
			// 
			this.panelPreview.AllowDrop = true;
			this.panelPreview.AutoScroll = true;
			this.panelPreview.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelPreview.Location = new System.Drawing.Point(0, 0);
			this.panelPreview.Name = "panelPreview";
			this.panelPreview.Size = new System.Drawing.Size(936, 406);
			this.panelPreview.TabIndex = 0;
			this.panelPreview.DragDrop += new System.Windows.Forms.DragEventHandler(this.panelPreview_DragDrop);
			this.panelPreview.DragEnter += new System.Windows.Forms.DragEventHandler(this.panelPreview_DragEnter);
			this.panelPreview.Paint += new System.Windows.Forms.PaintEventHandler(this.panelPreview_Paint);
			// 
			// lbVariations
			// 
			this.lbVariations.AllowDrop = true;
			this.lbVariations.DisplayMember = "Description";
			this.lbVariations.Location = new System.Drawing.Point(4, 476);
			this.lbVariations.Name = "lbVariations";
			this.lbVariations.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
			this.lbVariations.Size = new System.Drawing.Size(128, 108);
			this.lbVariations.TabIndex = 5;
			this.lbVariations.Tag = "Selected";
			this.lbVariations.ValueMember = "ID";
			this.lbVariations.SelectedIndexChanged += new System.EventHandler(this.lbVariations_SelectedIndexChanged);
			this.lbVariations.DragDrop += new System.Windows.Forms.DragEventHandler(this.lbVariations_DragDrop);
			this.lbVariations.DragEnter += new System.Windows.Forms.DragEventHandler(this.lbVariations_DragEnter);
			this.lbVariations.DragOver += new System.Windows.Forms.DragEventHandler(this.lbVariations_DragOver);
			// 
			// button6
			// 
			this.button6.Location = new System.Drawing.Point(100, 588);
			this.button6.Name = "button6";
			this.button6.Size = new System.Drawing.Size(32, 23);
			this.button6.TabIndex = 7;
			this.button6.Text = "Add";
			this.button6.Click += new System.EventHandler(this.button6_Click);
			// 
			// btnSaveChanges
			// 
			this.btnSaveChanges.Location = new System.Drawing.Point(680, 182);
			this.btnSaveChanges.Name = "btnSaveChanges";
			this.btnSaveChanges.Size = new System.Drawing.Size(88, 20);
			this.btnSaveChanges.TabIndex = 8;
			this.btnSaveChanges.Text = "Save Changes";
			this.btnSaveChanges.Click += new System.EventHandler(this.btnSaveChanges_Click);
			// 
			// cbAvailableVariations
			// 
			this.cbAvailableVariations.DisplayMember = "Description";
			this.cbAvailableVariations.Location = new System.Drawing.Point(4, 588);
			this.cbAvailableVariations.Name = "cbAvailableVariations";
			this.cbAvailableVariations.Size = new System.Drawing.Size(88, 21);
			this.cbAvailableVariations.TabIndex = 9;
			this.cbAvailableVariations.ValueMember = "ID";
			// 
			// lbChildrenDesignObjs
			// 
			this.lbChildrenDesignObjs.DisplayMember = "Description";
			this.lbChildrenDesignObjs.Location = new System.Drawing.Point(0, 0);
			this.lbChildrenDesignObjs.Name = "lbChildrenDesignObjs";
			this.lbChildrenDesignObjs.Size = new System.Drawing.Size(224, 147);
			this.lbChildrenDesignObjs.TabIndex = 10;
			this.lbChildrenDesignObjs.ValueMember = "ID";
			this.lbChildrenDesignObjs.SelectedIndexChanged += new System.EventHandler(this.lbDesignObjsOrText_SelectedIndexChanged);
			this.lbChildrenDesignObjs.DoubleClick += new System.EventHandler(this.lbChildrenDesignObjs_DoubleClick);
			this.lbChildrenDesignObjs.MouseDown += new System.Windows.Forms.MouseEventHandler(this.lbChildrenDesignObjs_MouseDown);
			// 
			// btnLoadSelectedDesignObj
			// 
			this.btnLoadSelectedDesignObj.Location = new System.Drawing.Point(0, 152);
			this.btnLoadSelectedDesignObj.Name = "btnLoadSelectedDesignObj";
			this.btnLoadSelectedDesignObj.Size = new System.Drawing.Size(40, 23);
			this.btnLoadSelectedDesignObj.TabIndex = 11;
			this.btnLoadSelectedDesignObj.Text = "Load";
			this.btnLoadSelectedDesignObj.Click += new System.EventHandler(this.btnLoadSelectedDesignObj_Click);
			// 
			// btnCancel
			// 
			this.btnCancel.Location = new System.Drawing.Point(582, 182);
			this.btnCancel.Name = "btnCancel";
			this.btnCancel.Size = new System.Drawing.Size(88, 20);
			this.btnCancel.TabIndex = 13;
			this.btnCancel.Text = "Cancel";
			this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
			// 
			// btnRemoveDesignObj
			// 
			this.btnRemoveDesignObj.Location = new System.Drawing.Point(46, 152);
			this.btnRemoveDesignObj.Name = "btnRemoveDesignObj";
			this.btnRemoveDesignObj.Size = new System.Drawing.Size(56, 23);
			this.btnRemoveDesignObj.TabIndex = 14;
			this.btnRemoveDesignObj.Text = "Remove";
			this.btnRemoveDesignObj.Click += new System.EventHandler(this.btnRemoveDesignObj_Click);
			// 
			// tbX
			// 
			this.tbX.Enabled = false;
			this.tbX.Location = new System.Drawing.Point(416, 104);
			this.tbX.Name = "tbX";
			this.tbX.Size = new System.Drawing.Size(40, 20);
			this.tbX.TabIndex = 15;
			this.tbX.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tbX_KeyDown);
			this.tbX.Leave += new System.EventHandler(this.tbX_Leave);
			// 
			// tbY
			// 
			this.tbY.Enabled = false;
			this.tbY.Location = new System.Drawing.Point(480, 104);
			this.tbY.Name = "tbY";
			this.tbY.Size = new System.Drawing.Size(40, 20);
			this.tbY.TabIndex = 18;
			this.tbY.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tbY_KeyDown);
			this.tbY.Leave += new System.EventHandler(this.tbY_Leave);
			// 
			// tbWidth
			// 
			this.tbWidth.Enabled = false;
			this.tbWidth.Location = new System.Drawing.Point(416, 128);
			this.tbWidth.Name = "tbWidth";
			this.tbWidth.Size = new System.Drawing.Size(40, 20);
			this.tbWidth.TabIndex = 20;
			this.tbWidth.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tbWidth_KeyDown);
			this.tbWidth.Leave += new System.EventHandler(this.tbWidth_Leave);
			// 
			// tbHeight
			// 
			this.tbHeight.Enabled = false;
			this.tbHeight.Location = new System.Drawing.Point(480, 128);
			this.tbHeight.Name = "tbHeight";
			this.tbHeight.Size = new System.Drawing.Size(40, 20);
			this.tbHeight.TabIndex = 22;
			this.tbHeight.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tbHeight_KeyDown);
			this.tbHeight.Leave += new System.EventHandler(this.tbHeight_Leave);
			// 
			// pnParameters
			// 
			this.pnParameters.AutoScroll = true;
			this.pnParameters.Location = new System.Drawing.Point(0, 8);
			this.pnParameters.Name = "pnParameters";
			this.pnParameters.Size = new System.Drawing.Size(456, 176);
			this.pnParameters.TabIndex = 24;
			// 
			// btnRemoveSelectedVariation
			// 
			this.btnRemoveSelectedVariation.Location = new System.Drawing.Point(4, 612);
			this.btnRemoveSelectedVariation.Name = "btnRemoveSelectedVariation";
			this.btnRemoveSelectedVariation.Size = new System.Drawing.Size(128, 23);
			this.btnRemoveSelectedVariation.TabIndex = 0;
			this.btnRemoveSelectedVariation.Text = "Remove Selected";
			this.btnRemoveSelectedVariation.Click += new System.EventHandler(this.btnRemoveSelectedVariation_Click);
			// 
			// lbText
			// 
			this.lbText.DisplayMember = "Description";
			this.lbText.Location = new System.Drawing.Point(232, 48);
			this.lbText.Name = "lbText";
			this.lbText.Size = new System.Drawing.Size(152, 95);
			this.lbText.TabIndex = 25;
			this.lbText.ValueMember = "ID";
			this.lbText.SelectedIndexChanged += new System.EventHandler(this.lbDesignObjsOrText_SelectedIndexChanged);
			this.lbText.MouseDown += new System.Windows.Forms.MouseEventHandler(this.lbText_MouseDown);
			// 
			// btnEditText
			// 
			this.btnEditText.Location = new System.Drawing.Point(232, 152);
			this.btnEditText.Name = "btnEditText";
			this.btnEditText.Size = new System.Drawing.Size(40, 23);
			this.btnEditText.TabIndex = 26;
			this.btnEditText.Text = "Edit";
			this.btnEditText.Click += new System.EventHandler(this.btnEditText_Click);
			// 
			// btnRemoveText
			// 
			this.btnRemoveText.Location = new System.Drawing.Point(278, 152);
			this.btnRemoveText.Name = "btnRemoveText";
			this.btnRemoveText.Size = new System.Drawing.Size(56, 23);
			this.btnRemoveText.TabIndex = 27;
			this.btnRemoveText.Text = "Remove";
			this.btnRemoveText.Click += new System.EventHandler(this.btnRemoveText_Click);
			// 
			// mnuDesignObjXDesigner
			// 
			this.mnuDesignObjXDesigner.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.mnuZoom,
            this.mnuSkin,
            this.mnuLanguage});
			// 
			// mnuZoom
			// 
			this.mnuZoom.Index = 0;
			this.mnuZoom.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.miZoom25,
            this.miZoom50,
            this.miZoom100,
            this.miZoom200});
			this.mnuZoom.Text = "Zoom";
			// 
			// miZoom25
			// 
			this.miZoom25.Index = 0;
			this.miZoom25.Text = "25%";
			this.miZoom25.Click += new System.EventHandler(this.miZoom_Click);
			// 
			// miZoom50
			// 
			this.miZoom50.Index = 1;
			this.miZoom50.Text = "50%";
			this.miZoom50.Click += new System.EventHandler(this.miZoom_Click);
			// 
			// miZoom100
			// 
			this.miZoom100.Index = 2;
			this.miZoom100.Text = "100%";
			this.miZoom100.Click += new System.EventHandler(this.miZoom_Click);
			// 
			// miZoom200
			// 
			this.miZoom200.Index = 3;
			this.miZoom200.Text = "200%";
			this.miZoom200.Click += new System.EventHandler(this.miZoom_Click);
			// 
			// mnuSkin
			// 
			this.mnuSkin.Index = 1;
			this.mnuSkin.Text = "Skin";
			// 
			// mnuLanguage
			// 
			this.mnuLanguage.Index = 2;
			this.mnuLanguage.Text = "Language";
			// 
			// cbLanguageSkin
			// 
			this.cbLanguageSkin.DisplayMember = "LanguageSkin";
			this.cbLanguageSkin.Location = new System.Drawing.Point(232, 0);
			this.cbLanguageSkin.Name = "cbLanguageSkin";
			this.cbLanguageSkin.Size = new System.Drawing.Size(152, 21);
			this.cbLanguageSkin.TabIndex = 30;
			this.cbLanguageSkin.Text = "Language - Skin";
			this.cbLanguageSkin.ValueMember = "ID";
			this.cbLanguageSkin.SelectedIndexChanged += new System.EventHandler(this.cbLanguageSkin_SelectedIndexChanged);
			// 
			// cbAlignH
			// 
			this.cbAlignH.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbAlignH.Items.AddRange(new object[] {
            "Default",
            "Left",
            "Center",
            "Right"});
			this.cbAlignH.Location = new System.Drawing.Point(464, 80);
			this.cbAlignH.MaxDropDownItems = 9;
			this.cbAlignH.Name = "cbAlignH";
			this.cbAlignH.Size = new System.Drawing.Size(56, 21);
			this.cbAlignH.TabIndex = 31;
			this.cbAlignH.SelectedIndexChanged += new System.EventHandler(this.cbAlignment_SelectedIndexChanged);
			// 
			// chCantGoBack
			// 
			this.chCantGoBack.Location = new System.Drawing.Point(608, 32);
			this.chCantGoBack.Name = "chCantGoBack";
			this.chCantGoBack.Size = new System.Drawing.Size(104, 24);
			this.chCantGoBack.TabIndex = 42;
			this.chCantGoBack.Text = "Can\'t Go Back";
			this.chCantGoBack.CheckedChanged += new System.EventHandler(this.chCantGoBack_CheckedChanged);
			// 
			// btnVariationControllerCriteria
			// 
			this.btnVariationControllerCriteria.Location = new System.Drawing.Point(608, 136);
			this.btnVariationControllerCriteria.Name = "btnVariationControllerCriteria";
			this.btnVariationControllerCriteria.Size = new System.Drawing.Size(112, 23);
			this.btnVariationControllerCriteria.TabIndex = 41;
			this.btnVariationControllerCriteria.Text = "Controller Criteria";
			this.btnVariationControllerCriteria.Click += new System.EventHandler(this.btnVariationControllerCriteria_Click);
			// 
			// chRegenerateForEachScreen
			// 
			this.chRegenerateForEachScreen.Enabled = false;
			this.chRegenerateForEachScreen.Location = new System.Drawing.Point(536, 160);
			this.chRegenerateForEachScreen.Name = "chRegenerateForEachScreen";
			this.chRegenerateForEachScreen.Size = new System.Drawing.Size(120, 16);
			this.chRegenerateForEachScreen.TabIndex = 39;
			this.chRegenerateForEachScreen.Text = "Regen per screen";
			this.chRegenerateForEachScreen.CheckedChanged += new System.EventHandler(this.chRegenerateForEachScreen_CheckedChanged);
			// 
			// chChildBehindBG
			// 
			this.chChildBehindBG.Enabled = false;
			this.chChildBehindBG.Location = new System.Drawing.Point(536, 56);
			this.chChildBehindBG.Name = "chChildBehindBG";
			this.chChildBehindBG.Size = new System.Drawing.Size(168, 16);
			this.chChildBehindBG.TabIndex = 40;
			this.chChildBehindBG.Text = "Children behind bg";
			this.chChildBehindBG.CheckedChanged += new System.EventHandler(this.chBGOnTop_CheckedChanged);
			// 
			// chCanBeHidden
			// 
			this.chCanBeHidden.Enabled = false;
			this.chCanBeHidden.Location = new System.Drawing.Point(536, 8);
			this.chCanBeHidden.Name = "chCanBeHidden";
			this.chCanBeHidden.Size = new System.Drawing.Size(72, 16);
			this.chCanBeHidden.TabIndex = 38;
			this.chCanBeHidden.Text = "Can Hide";
			this.chCanBeHidden.CheckedChanged += new System.EventHandler(this.chCanBeHidden_CheckedChanged);
			// 
			// cbAlignV
			// 
			this.cbAlignV.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbAlignV.Items.AddRange(new object[] {
            "Default",
            "Top",
            "Middle",
            "Bottom"});
			this.cbAlignV.Location = new System.Drawing.Point(408, 80);
			this.cbAlignV.Name = "cbAlignV";
			this.cbAlignV.Size = new System.Drawing.Size(56, 21);
			this.cbAlignV.TabIndex = 37;
			this.cbAlignV.SelectedIndexChanged += new System.EventHandler(this.cbAlignment_SelectedIndexChanged);
			// 
			// btnRemoveSkinLanguage
			// 
			this.btnRemoveSkinLanguage.Location = new System.Drawing.Point(328, 24);
			this.btnRemoveSkinLanguage.Name = "btnRemoveSkinLanguage";
			this.btnRemoveSkinLanguage.Size = new System.Drawing.Size(56, 23);
			this.btnRemoveSkinLanguage.TabIndex = 34;
			this.btnRemoveSkinLanguage.Text = "remove";
			this.btnRemoveSkinLanguage.Click += new System.EventHandler(this.btnRemoveSkinLanguage_Click);
			// 
			// cbStyle
			// 
			this.cbStyle.DisplayMember = "Description";
			this.cbStyle.Enabled = false;
			this.cbStyle.Location = new System.Drawing.Point(408, 56);
			this.cbStyle.Name = "cbStyle";
			this.cbStyle.Size = new System.Drawing.Size(112, 21);
			this.cbStyle.TabIndex = 35;
			this.cbStyle.Text = "Style";
			this.cbStyle.ValueMember = "ID";
			this.cbStyle.SelectedIndexChanged += new System.EventHandler(this.cbStyle_SelectedIndexChanged);
			// 
			// btnAddSkinLanguage
			// 
			this.btnAddSkinLanguage.Location = new System.Drawing.Point(232, 24);
			this.btnAddSkinLanguage.Name = "btnAddSkinLanguage";
			this.btnAddSkinLanguage.Size = new System.Drawing.Size(80, 23);
			this.btnAddSkinLanguage.TabIndex = 33;
			this.btnAddSkinLanguage.Text = "Add skin/lang";
			this.btnAddSkinLanguage.Click += new System.EventHandler(this.btnAddSkinLanguage_Click);
			// 
			// tabButtons
			// 
			this.tabButtons.Controls.Add(this.cbButton);
			this.tabButtons.Controls.Add(label6);
			this.tabButtons.Location = new System.Drawing.Point(4, 22);
			this.tabButtons.Name = "tabButtons";
			this.tabButtons.Size = new System.Drawing.Size(768, 230);
			this.tabButtons.TabIndex = 1;
			this.tabButtons.Text = "Button";
			// 
			// cbButton
			// 
			this.cbButton.DisplayMember = "Description";
			this.cbButton.Location = new System.Drawing.Point(112, 24);
			this.cbButton.Name = "cbButton";
			this.cbButton.Size = new System.Drawing.Size(208, 21);
			this.cbButton.TabIndex = 14;
			this.cbButton.ValueMember = "ID";
			this.cbButton.SelectedIndexChanged += new System.EventHandler(this.changeButton);
			// 
			// tabAllPages
			// 
			this.tabAllPages.Controls.Add(this.tabParameters);
			this.tabAllPages.Controls.Add(this.tabChildren);
			this.tabAllPages.Controls.Add(this.tabButtons);
			this.tabAllPages.Controls.Add(this.tabOnActivate);
			this.tabAllPages.Controls.Add(this.tabOnLoad);
			this.tabAllPages.Controls.Add(this.tabOnUnload);
			this.tabAllPages.Controls.Add(this.tabOnStartup);
			this.tabAllPages.Controls.Add(this.tabOnHighlight);
			this.tabAllPages.Controls.Add(this.tabOnUnhighlight);
			this.tabAllPages.Controls.Add(this.tabOnTimeout);
			this.tabAllPages.Controls.Add(this.tabOnSelected_WithChange);
			this.tabAllPages.Controls.Add(this.tabOnSelected_NoChange);
			this.tabAllPages.Controls.Add(this.tabOnHighlighted);
			this.tabAllPages.Location = new System.Drawing.Point(148, 412);
			this.tabAllPages.Name = "tabAllPages";
			this.tabAllPages.SelectedIndex = 0;
			this.tabAllPages.Size = new System.Drawing.Size(776, 256);
			this.tabAllPages.TabIndex = 32;
			this.tabAllPages.SelectedIndexChanged += new System.EventHandler(this.tabAllPages_SelectedIndexChanged);
			// 
			// tabParameters
			// 
			this.tabParameters.Controls.Add(label20);
			this.tabParameters.Controls.Add(this.cbDesignObjStability);
			this.tabParameters.Controls.Add(this.lDesignObjOwner);
			this.tabParameters.Controls.Add(label13);
			this.tabParameters.Controls.Add(label12);
			this.tabParameters.Controls.Add(this.cbVariationStability);
			this.tabParameters.Controls.Add(this.chDontResetSelectedState);
			this.tabParameters.Controls.Add(this.lVariationOwner);
			this.tabParameters.Controls.Add(label11);
			this.tabParameters.Controls.Add(this.chCantGoBack);
			this.tabParameters.Controls.Add(this.btnVariationControllerCriteria);
			this.tabParameters.Controls.Add(this.pnParameters);
			this.tabParameters.Location = new System.Drawing.Point(4, 22);
			this.tabParameters.Name = "tabParameters";
			this.tabParameters.Size = new System.Drawing.Size(768, 230);
			this.tabParameters.TabIndex = 0;
			this.tabParameters.Text = "General Info";
			// 
			// cbDesignObjStability
			// 
			this.cbDesignObjStability.Items.AddRange(new object[] {
            "Unstable",
            "Testing",
            "Stable"});
			this.cbDesignObjStability.Location = new System.Drawing.Point(608, 104);
			this.cbDesignObjStability.Name = "cbDesignObjStability";
			this.cbDesignObjStability.Size = new System.Drawing.Size(121, 21);
			this.cbDesignObjStability.TabIndex = 50;
			// 
			// lDesignObjOwner
			// 
			this.lDesignObjOwner.Location = new System.Drawing.Point(608, 64);
			this.lDesignObjOwner.Name = "lDesignObjOwner";
			this.lDesignObjOwner.Size = new System.Drawing.Size(120, 16);
			this.lDesignObjOwner.TabIndex = 49;
			// 
			// cbVariationStability
			// 
			this.cbVariationStability.Items.AddRange(new object[] {
            "Unstable",
            "Testing",
            "Stable"});
			this.cbVariationStability.Location = new System.Drawing.Point(456, 152);
			this.cbVariationStability.Name = "cbVariationStability";
			this.cbVariationStability.Size = new System.Drawing.Size(121, 21);
			this.cbVariationStability.TabIndex = 46;
			// 
			// chDontResetSelectedState
			// 
			this.chDontResetSelectedState.Location = new System.Drawing.Point(456, 88);
			this.chDontResetSelectedState.Name = "chDontResetSelectedState";
			this.chDontResetSelectedState.Size = new System.Drawing.Size(120, 32);
			this.chDontResetSelectedState.TabIndex = 45;
			this.chDontResetSelectedState.Text = "Don\'t reset selected state";
			this.chDontResetSelectedState.CheckedChanged += new System.EventHandler(this.chDontResetSelectedState_CheckedChanged);
			// 
			// lVariationOwner
			// 
			this.lVariationOwner.Location = new System.Drawing.Point(464, 64);
			this.lVariationOwner.Name = "lVariationOwner";
			this.lVariationOwner.Size = new System.Drawing.Size(120, 16);
			this.lVariationOwner.TabIndex = 44;
			// 
			// tabChildren
			// 
			this.tabChildren.Controls.Add(this.btnAddExplicitText);
			this.tabChildren.Controls.Add(this.btnAddExplicitDesignObj);
			this.tabChildren.Controls.Add(this.cbLanguageSkin_Child);
			this.tabChildren.Controls.Add(this.btnAddSkinLanguage_Child);
			this.tabChildren.Controls.Add(this.btnRemoveSkinLanguage_Child);
			this.tabChildren.Controls.Add(this.btnChildDown);
			this.tabChildren.Controls.Add(this.btnChildUp);
			this.tabChildren.Controls.Add(this.tbVisibleStates);
			this.tabChildren.Controls.Add(this.btnSaveChanges);
			this.tabChildren.Controls.Add(this.btnCancel);
			this.tabChildren.Controls.Add(label21);
			this.tabChildren.Controls.Add(this.tbTiedTo);
			this.tabChildren.Controls.Add(label19);
			this.tabChildren.Controls.Add(this.tbTS_Right);
			this.tabChildren.Controls.Add(label17);
			this.tabChildren.Controls.Add(this.tbTS_Left);
			this.tabChildren.Controls.Add(label18);
			this.tabChildren.Controls.Add(this.tbTS_Down);
			this.tabChildren.Controls.Add(label16);
			this.tabChildren.Controls.Add(this.tbTS_Up);
			this.tabChildren.Controls.Add(label15);
			this.tabChildren.Controls.Add(label14);
			this.tabChildren.Controls.Add(this.chIsTabStop);
			this.tabChildren.Controls.Add(this.chDontMergeBG);
			this.tabChildren.Controls.Add(this.chChildBeforeText);
			this.tabChildren.Controls.Add(this.cbStyle);
			this.tabChildren.Controls.Add(this.lbChildrenDesignObjs);
			this.tabChildren.Controls.Add(label2);
			this.tabChildren.Controls.Add(this.cbAlignH);
			this.tabChildren.Controls.Add(this.tbOpacity);
			this.tabChildren.Controls.Add(this.cbAlignV);
			this.tabChildren.Controls.Add(this.btnEditText);
			this.tabChildren.Controls.Add(this.chRegenerateForEachScreen);
			this.tabChildren.Controls.Add(this.tbRotate);
			this.tabChildren.Controls.Add(this.chCanBeHidden);
			this.tabChildren.Controls.Add(this.cbLanguageSkin);
			this.tabChildren.Controls.Add(this.btnAddSkinLanguage);
			this.tabChildren.Controls.Add(this.btnRemoveText);
			this.tabChildren.Controls.Add(label1);
			this.tabChildren.Controls.Add(this.tbHeight);
			this.tabChildren.Controls.Add(this.btnRemoveSkinLanguage);
			this.tabChildren.Controls.Add(this.lbText);
			this.tabChildren.Controls.Add(label5);
			this.tabChildren.Controls.Add(label4);
			this.tabChildren.Controls.Add(this.tbY);
			this.tabChildren.Controls.Add(label7);
			this.tabChildren.Controls.Add(this.tbWidth);
			this.tabChildren.Controls.Add(this.btnLoadSelectedDesignObj);
			this.tabChildren.Controls.Add(this.btnBGColorTSL);
			this.tabChildren.Controls.Add(this.chHideByDefault);
			this.tabChildren.Controls.Add(this.chChildBehindBG);
			this.tabChildren.Controls.Add(this.tbX);
			this.tabChildren.Controls.Add(label3);
			this.tabChildren.Controls.Add(this.btnRemoveDesignObj);
			this.tabChildren.Location = new System.Drawing.Point(4, 22);
			this.tabChildren.Name = "tabChildren";
			this.tabChildren.Size = new System.Drawing.Size(768, 230);
			this.tabChildren.TabIndex = 7;
			this.tabChildren.Text = "Children";
			// 
			// btnAddExplicitText
			// 
			this.btnAddExplicitText.Location = new System.Drawing.Point(340, 152);
			this.btnAddExplicitText.Name = "btnAddExplicitText";
			this.btnAddExplicitText.Size = new System.Drawing.Size(40, 23);
			this.btnAddExplicitText.TabIndex = 71;
			this.btnAddExplicitText.Text = "Add";
			this.btnAddExplicitText.UseVisualStyleBackColor = true;
			this.btnAddExplicitText.Click += new System.EventHandler(this.btnAddExplicitText_Click);
			// 
			// btnAddExplicitDesignObj
			// 
			this.btnAddExplicitDesignObj.Location = new System.Drawing.Point(108, 152);
			this.btnAddExplicitDesignObj.Name = "btnAddExplicitDesignObj";
			this.btnAddExplicitDesignObj.Size = new System.Drawing.Size(40, 23);
			this.btnAddExplicitDesignObj.TabIndex = 70;
			this.btnAddExplicitDesignObj.Text = "Add";
			this.btnAddExplicitDesignObj.UseVisualStyleBackColor = true;
			this.btnAddExplicitDesignObj.Click += new System.EventHandler(this.btnAddExplicitDesignObj_Click);
			// 
			// cbLanguageSkin_Child
			// 
			this.cbLanguageSkin_Child.DisplayMember = "LanguageSkin";
			this.cbLanguageSkin_Child.Location = new System.Drawing.Point(392, 0);
			this.cbLanguageSkin_Child.Name = "cbLanguageSkin_Child";
			this.cbLanguageSkin_Child.Size = new System.Drawing.Size(136, 21);
			this.cbLanguageSkin_Child.TabIndex = 67;
			this.cbLanguageSkin_Child.Text = "Language - Skin";
			this.cbLanguageSkin_Child.ValueMember = "ID";
			this.cbLanguageSkin_Child.SelectedIndexChanged += new System.EventHandler(this.cbLanguageSkin_Child_SelectedIndexChanged);
			// 
			// btnAddSkinLanguage_Child
			// 
			this.btnAddSkinLanguage_Child.Location = new System.Drawing.Point(392, 24);
			this.btnAddSkinLanguage_Child.Name = "btnAddSkinLanguage_Child";
			this.btnAddSkinLanguage_Child.Size = new System.Drawing.Size(80, 24);
			this.btnAddSkinLanguage_Child.TabIndex = 68;
			this.btnAddSkinLanguage_Child.Text = "Add skin/lang";
			this.btnAddSkinLanguage_Child.Click += new System.EventHandler(this.btnAddSkinLanguage_Child_Click_1);
			// 
			// btnRemoveSkinLanguage_Child
			// 
			this.btnRemoveSkinLanguage_Child.Location = new System.Drawing.Point(472, 24);
			this.btnRemoveSkinLanguage_Child.Name = "btnRemoveSkinLanguage_Child";
			this.btnRemoveSkinLanguage_Child.Size = new System.Drawing.Size(56, 24);
			this.btnRemoveSkinLanguage_Child.TabIndex = 69;
			this.btnRemoveSkinLanguage_Child.Text = "remove";
			this.btnRemoveSkinLanguage_Child.Click += new System.EventHandler(this.btnRemoveSkinLanguage_Child_Click_1);
			// 
			// btnChildDown
			// 
			this.btnChildDown.Location = new System.Drawing.Point(200, 152);
			this.btnChildDown.Name = "btnChildDown";
			this.btnChildDown.Size = new System.Drawing.Size(24, 23);
			this.btnChildDown.TabIndex = 66;
			this.btnChildDown.Text = "D";
			this.btnChildDown.Click += new System.EventHandler(this.btnChildDown_Click);
			// 
			// btnChildUp
			// 
			this.btnChildUp.Location = new System.Drawing.Point(174, 152);
			this.btnChildUp.Name = "btnChildUp";
			this.btnChildUp.Size = new System.Drawing.Size(24, 23);
			this.btnChildUp.TabIndex = 65;
			this.btnChildUp.Text = "U";
			this.btnChildUp.Click += new System.EventHandler(this.btnChildUp_Click);
			// 
			// tbVisibleStates
			// 
			this.tbVisibleStates.Location = new System.Drawing.Point(664, 144);
			this.tbVisibleStates.Name = "tbVisibleStates";
			this.tbVisibleStates.Size = new System.Drawing.Size(88, 20);
			this.tbVisibleStates.TabIndex = 64;
			this.tbVisibleStates.TextChanged += new System.EventHandler(this.tbVisibleStates_TextChanged);
			// 
			// tbTiedTo
			// 
			this.tbTiedTo.Location = new System.Drawing.Point(664, 104);
			this.tbTiedTo.Name = "tbTiedTo";
			this.tbTiedTo.Size = new System.Drawing.Size(64, 20);
			this.tbTiedTo.TabIndex = 62;
			this.tbTiedTo.TextChanged += new System.EventHandler(this.tbTiedTo_TextChanged);
			// 
			// tbTS_Right
			// 
			this.tbTS_Right.Location = new System.Drawing.Point(728, 64);
			this.tbTS_Right.Name = "tbTS_Right";
			this.tbTS_Right.Size = new System.Drawing.Size(40, 20);
			this.tbTS_Right.TabIndex = 60;
			this.tbTS_Right.TextChanged += new System.EventHandler(this.tbTS_Right_TextChanged);
			// 
			// tbTS_Left
			// 
			this.tbTS_Left.Location = new System.Drawing.Point(672, 64);
			this.tbTS_Left.Name = "tbTS_Left";
			this.tbTS_Left.Size = new System.Drawing.Size(40, 20);
			this.tbTS_Left.TabIndex = 58;
			this.tbTS_Left.TextChanged += new System.EventHandler(this.tbTS_Left_TextChanged);
			// 
			// tbTS_Down
			// 
			this.tbTS_Down.Location = new System.Drawing.Point(728, 40);
			this.tbTS_Down.Name = "tbTS_Down";
			this.tbTS_Down.Size = new System.Drawing.Size(40, 20);
			this.tbTS_Down.TabIndex = 56;
			this.tbTS_Down.TextChanged += new System.EventHandler(this.tbTS_Down_TextChanged);
			// 
			// tbTS_Up
			// 
			this.tbTS_Up.Location = new System.Drawing.Point(672, 40);
			this.tbTS_Up.Name = "tbTS_Up";
			this.tbTS_Up.Size = new System.Drawing.Size(40, 20);
			this.tbTS_Up.TabIndex = 54;
			this.tbTS_Up.TextChanged += new System.EventHandler(this.tbTS_Up_TextChanged);
			// 
			// chIsTabStop
			// 
			this.chIsTabStop.Location = new System.Drawing.Point(536, 128);
			this.chIsTabStop.Name = "chIsTabStop";
			this.chIsTabStop.Size = new System.Drawing.Size(104, 24);
			this.chIsTabStop.TabIndex = 51;
			this.chIsTabStop.Text = "Is Tab Stop";
			this.chIsTabStop.CheckedChanged += new System.EventHandler(this.cbIsTabStop_CheckedChanged);
			// 
			// chDontMergeBG
			// 
			this.chDontMergeBG.Enabled = false;
			this.chDontMergeBG.Location = new System.Drawing.Point(536, 104);
			this.chDontMergeBG.Name = "chDontMergeBG";
			this.chDontMergeBG.Size = new System.Drawing.Size(112, 16);
			this.chDontMergeBG.TabIndex = 50;
			this.chDontMergeBG.Text = "Don\'t merge bg";
			this.chDontMergeBG.CheckedChanged += new System.EventHandler(this.chDontMergeBG_CheckedChanged);
			// 
			// chChildBeforeText
			// 
			this.chChildBeforeText.Enabled = false;
			this.chChildBeforeText.Location = new System.Drawing.Point(536, 80);
			this.chChildBeforeText.Name = "chChildBeforeText";
			this.chChildBeforeText.Size = new System.Drawing.Size(168, 16);
			this.chChildBeforeText.TabIndex = 49;
			this.chChildBeforeText.Text = "Children before text";
			this.chChildBeforeText.CheckedChanged += new System.EventHandler(this.chChildBeforeText_CheckedChanged);
			// 
			// tbOpacity
			// 
			this.tbOpacity.Enabled = false;
			this.tbOpacity.Location = new System.Drawing.Point(480, 152);
			this.tbOpacity.Name = "tbOpacity";
			this.tbOpacity.Size = new System.Drawing.Size(40, 20);
			this.tbOpacity.TabIndex = 45;
			this.tbOpacity.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tbOpacity_KeyDown);
			this.tbOpacity.Leave += new System.EventHandler(this.tbOpacity_Leave);
			// 
			// tbRotate
			// 
			this.tbRotate.Enabled = false;
			this.tbRotate.Location = new System.Drawing.Point(416, 152);
			this.tbRotate.Name = "tbRotate";
			this.tbRotate.Size = new System.Drawing.Size(40, 20);
			this.tbRotate.TabIndex = 43;
			this.tbRotate.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tbRotate_KeyDown);
			this.tbRotate.Leave += new System.EventHandler(this.tbRotate_Leave);
			// 
			// btnBGColorTSL
			// 
			this.btnBGColorTSL.Enabled = false;
			this.btnBGColorTSL.Location = new System.Drawing.Point(416, 176);
			this.btnBGColorTSL.Name = "btnBGColorTSL";
			this.btnBGColorTSL.Size = new System.Drawing.Size(104, 23);
			this.btnBGColorTSL.TabIndex = 48;
			this.btnBGColorTSL.Text = "Plain BG Color";
			this.btnBGColorTSL.Click += new System.EventHandler(this.btnBGColorTSL_Click);
			// 
			// chHideByDefault
			// 
			this.chHideByDefault.Enabled = false;
			this.chHideByDefault.Location = new System.Drawing.Point(536, 32);
			this.chHideByDefault.Name = "chHideByDefault";
			this.chHideByDefault.Size = new System.Drawing.Size(104, 16);
			this.chHideByDefault.TabIndex = 47;
			this.chHideByDefault.Text = "Hide By Default";
			this.chHideByDefault.CheckedChanged += new System.EventHandler(this.chHideByDefault_CheckedChanged);
			// 
			// tabOnActivate
			// 
			this.tabOnActivate.Location = new System.Drawing.Point(4, 22);
			this.tabOnActivate.Name = "tabOnActivate";
			this.tabOnActivate.Size = new System.Drawing.Size(768, 230);
			this.tabOnActivate.TabIndex = 2;
			this.tabOnActivate.Text = "OnActivate";
			// 
			// tabOnLoad
			// 
			this.tabOnLoad.Location = new System.Drawing.Point(4, 22);
			this.tabOnLoad.Name = "tabOnLoad";
			this.tabOnLoad.Size = new System.Drawing.Size(768, 230);
			this.tabOnLoad.TabIndex = 3;
			this.tabOnLoad.Text = "OnLoad";
			// 
			// tabOnUnload
			// 
			this.tabOnUnload.Location = new System.Drawing.Point(4, 22);
			this.tabOnUnload.Name = "tabOnUnload";
			this.tabOnUnload.Size = new System.Drawing.Size(768, 230);
			this.tabOnUnload.TabIndex = 4;
			this.tabOnUnload.Text = "OnUnload";
			// 
			// tabOnStartup
			// 
			this.tabOnStartup.Location = new System.Drawing.Point(4, 22);
			this.tabOnStartup.Name = "tabOnStartup";
			this.tabOnStartup.Size = new System.Drawing.Size(768, 230);
			this.tabOnStartup.TabIndex = 5;
			this.tabOnStartup.Text = "OnStartup";
			// 
			// tabOnHighlight
			// 
			this.tabOnHighlight.Location = new System.Drawing.Point(4, 22);
			this.tabOnHighlight.Name = "tabOnHighlight";
			this.tabOnHighlight.Size = new System.Drawing.Size(768, 230);
			this.tabOnHighlight.TabIndex = 6;
			this.tabOnHighlight.Text = "OnHighlight";
			// 
			// tabOnUnhighlight
			// 
			this.tabOnUnhighlight.Location = new System.Drawing.Point(4, 22);
			this.tabOnUnhighlight.Name = "tabOnUnhighlight";
			this.tabOnUnhighlight.Size = new System.Drawing.Size(768, 230);
			this.tabOnUnhighlight.TabIndex = 7;
			this.tabOnUnhighlight.Text = "OnUnhighlight";
			// 
			// tabOnTimeout
			// 
			this.tabOnTimeout.Location = new System.Drawing.Point(4, 22);
			this.tabOnTimeout.Name = "tabOnTimeout";
			this.tabOnTimeout.Size = new System.Drawing.Size(768, 230);
			this.tabOnTimeout.TabIndex = 8;
			this.tabOnTimeout.Text = "OnTimeout";
			// 
			// tabOnSelected_WithChange
			// 
			this.tabOnSelected_WithChange.Controls.Add(this.pnEffectsSelectChange);
			this.tabOnSelected_WithChange.Location = new System.Drawing.Point(4, 22);
			this.tabOnSelected_WithChange.Name = "tabOnSelected_WithChange";
			this.tabOnSelected_WithChange.Size = new System.Drawing.Size(768, 230);
			this.tabOnSelected_WithChange.TabIndex = 9;
			this.tabOnSelected_WithChange.Text = "OnSelected(with change)";
			// 
			// pnEffectsSelectChange
			// 
			this.pnEffectsSelectChange.Controls.Add(this.lbEffect);
			this.pnEffectsSelectChange.Controls.Add(this.cbEffectsSelectChange);
			this.pnEffectsSelectChange.Location = new System.Drawing.Point(8, 8);
			this.pnEffectsSelectChange.Name = "pnEffectsSelectChange";
			this.pnEffectsSelectChange.Size = new System.Drawing.Size(456, 168);
			this.pnEffectsSelectChange.TabIndex = 10;
			// 
			// lbEffect
			// 
			this.lbEffect.Location = new System.Drawing.Point(16, 16);
			this.lbEffect.Name = "lbEffect";
			this.lbEffect.Size = new System.Drawing.Size(216, 16);
			this.lbEffect.TabIndex = 11;
			this.lbEffect.Text = "Effect";
			// 
			// cbEffectsSelectChange
			// 
			this.cbEffectsSelectChange.DisplayMember = "Description";
			this.cbEffectsSelectChange.Location = new System.Drawing.Point(16, 40);
			this.cbEffectsSelectChange.Name = "cbEffectsSelectChange";
			this.cbEffectsSelectChange.Size = new System.Drawing.Size(264, 21);
			this.cbEffectsSelectChange.TabIndex = 12;
			this.cbEffectsSelectChange.ValueMember = "ID";
			this.cbEffectsSelectChange.SelectedIndexChanged += new System.EventHandler(this.cbEffectsSelectChange_SelectedIndexChanged);
			// 
			// tabOnSelected_NoChange
			// 
			this.tabOnSelected_NoChange.Controls.Add(this.panel1);
			this.tabOnSelected_NoChange.Location = new System.Drawing.Point(4, 22);
			this.tabOnSelected_NoChange.Name = "tabOnSelected_NoChange";
			this.tabOnSelected_NoChange.Size = new System.Drawing.Size(768, 230);
			this.tabOnSelected_NoChange.TabIndex = 13;
			this.tabOnSelected_NoChange.Text = "OnSelected(no change)";
			// 
			// panel1
			// 
			this.panel1.Controls.Add(label22);
			this.panel1.Controls.Add(this.cbEffectsSelectNoChange);
			this.panel1.Location = new System.Drawing.Point(8, 8);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(456, 168);
			this.panel1.TabIndex = 3;
			// 
			// cbEffectsSelectNoChange
			// 
			this.cbEffectsSelectNoChange.DisplayMember = "Description";
			this.cbEffectsSelectNoChange.Location = new System.Drawing.Point(16, 40);
			this.cbEffectsSelectNoChange.Name = "cbEffectsSelectNoChange";
			this.cbEffectsSelectNoChange.Size = new System.Drawing.Size(264, 21);
			this.cbEffectsSelectNoChange.TabIndex = 0;
			this.cbEffectsSelectNoChange.ValueMember = "ID";
			this.cbEffectsSelectNoChange.SelectedIndexChanged += new System.EventHandler(this.cbEffectsSelectNoChange_SelectedIndexChanged);
			// 
			// tabOnHighlighted
			// 
			this.tabOnHighlighted.Controls.Add(this.panel2);
			this.tabOnHighlighted.Location = new System.Drawing.Point(4, 22);
			this.tabOnHighlighted.Name = "tabOnHighlighted";
			this.tabOnHighlighted.Size = new System.Drawing.Size(768, 230);
			this.tabOnHighlighted.TabIndex = 9;
			this.tabOnHighlighted.Text = "OnHightlighted";
			// 
			// panel2
			// 
			this.panel2.Controls.Add(label23);
			this.panel2.Controls.Add(this.cbEffectsHighlight);
			this.panel2.Location = new System.Drawing.Point(8, 8);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(456, 168);
			this.panel2.TabIndex = 3;
			// 
			// cbEffectsHighlight
			// 
			this.cbEffectsHighlight.DisplayMember = "Description";
			this.cbEffectsHighlight.Location = new System.Drawing.Point(16, 40);
			this.cbEffectsHighlight.Name = "cbEffectsHighlight";
			this.cbEffectsHighlight.Size = new System.Drawing.Size(264, 21);
			this.cbEffectsHighlight.TabIndex = 0;
			this.cbEffectsHighlight.ValueMember = "ID";
			this.cbEffectsHighlight.SelectedIndexChanged += new System.EventHandler(this.cbEffectsHighlight_SelectedIndexChanged);
			// 
			// tbDescription
			// 
			this.tbDescription.Location = new System.Drawing.Point(12, 412);
			this.tbDescription.Name = "tbDescription";
			this.tbDescription.Size = new System.Drawing.Size(128, 20);
			this.tbDescription.TabIndex = 36;
			this.tbDescription.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tbDescription_KeyDown);
			this.tbDescription.Leave += new System.EventHandler(this.tbDescription_Leave);
			// 
			// nudPriority
			// 
			this.nudPriority.Location = new System.Drawing.Point(68, 436);
			this.nudPriority.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
			this.nudPriority.Name = "nudPriority";
			this.nudPriority.Size = new System.Drawing.Size(48, 20);
			this.nudPriority.TabIndex = 37;
			this.nudPriority.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
			this.nudPriority.ValueChanged += new System.EventHandler(this.nudPriority_ValueChanged);
			this.nudPriority.Leave += new System.EventHandler(this.nudPriority_Leave);
			// 
			// lblPriority
			// 
			this.lblPriority.Location = new System.Drawing.Point(12, 436);
			this.lblPriority.Name = "lblPriority";
			this.lblPriority.Size = new System.Drawing.Size(48, 16);
			this.lblPriority.TabIndex = 38;
			this.lblPriority.Text = "Priority";
			this.lblPriority.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// cbAnimate
			// 
			this.cbAnimate.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.cbAnimate.Location = new System.Drawing.Point(12, 452);
			this.cbAnimate.Name = "cbAnimate";
			this.cbAnimate.Size = new System.Drawing.Size(72, 24);
			this.cbAnimate.TabIndex = 40;
			this.cbAnimate.Text = "Animate";
			this.cbAnimate.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.cbAnimate.Click += new System.EventHandler(this.cbAnimate_Click);
			// 
			// pnCommandGroupInfo
			// 
			this.pnCommandGroupInfo.Controls.Add(this.lMasterDevice);
			this.pnCommandGroupInfo.Controls.Add(this.chRelToSender);
			this.pnCommandGroupInfo.Controls.Add(this.tbDeviceCategory);
			this.pnCommandGroupInfo.Controls.Add(this.tbMasterDeviceList);
			this.pnCommandGroupInfo.Controls.Add(label10);
			this.pnCommandGroupInfo.Controls.Add(label9);
			this.pnCommandGroupInfo.Controls.Add(label8);
			this.pnCommandGroupInfo.Controls.Add(this.cbBroadcast);
			this.pnCommandGroupInfo.Controls.Add(this.btnOnActivateGoto);
			this.pnCommandGroupInfo.Controls.Add(this.lbSelectedCommands);
			this.pnCommandGroupInfo.Controls.Add(this.btnOnActivateDown);
			this.pnCommandGroupInfo.Controls.Add(this.tvCommand);
			this.pnCommandGroupInfo.Controls.Add(this.btnOnActivateUp);
			this.pnCommandGroupInfo.Controls.Add(this.btnOnActivateRemoveAG);
			this.pnCommandGroupInfo.Controls.Add(this.btnOnActivateAddAG);
			this.pnCommandGroupInfo.Controls.Add(this.pnOnActivateParameters);
			this.pnCommandGroupInfo.Controls.Add(this.labAvailableCommands);
			this.pnCommandGroupInfo.Controls.Add(this.labSelectedCommands);
			this.pnCommandGroupInfo.Location = new System.Drawing.Point(156, 436);
			this.pnCommandGroupInfo.Name = "pnCommandGroupInfo";
			this.pnCommandGroupInfo.Size = new System.Drawing.Size(752, 200);
			this.pnCommandGroupInfo.TabIndex = 41;
			// 
			// lMasterDevice
			// 
			this.lMasterDevice.Location = new System.Drawing.Point(640, 8);
			this.lMasterDevice.Name = "lMasterDevice";
			this.lMasterDevice.Size = new System.Drawing.Size(100, 23);
			this.lMasterDevice.TabIndex = 24;
			// 
			// chRelToSender
			// 
			this.chRelToSender.Location = new System.Drawing.Point(352, 32);
			this.chRelToSender.Name = "chRelToSender";
			this.chRelToSender.Size = new System.Drawing.Size(96, 24);
			this.chRelToSender.TabIndex = 23;
			this.chRelToSender.Text = "Rel to Sender";
			this.chRelToSender.TextChanged += new System.EventHandler(this.chRelToSender_TextChanged);
			// 
			// tbDeviceCategory
			// 
			this.tbDeviceCategory.Location = new System.Drawing.Point(536, 32);
			this.tbDeviceCategory.Name = "tbDeviceCategory";
			this.tbDeviceCategory.Size = new System.Drawing.Size(48, 20);
			this.tbDeviceCategory.TabIndex = 22;
			this.tbDeviceCategory.TextChanged += new System.EventHandler(this.tbDeviceCategory_TextChanged);
			// 
			// tbMasterDeviceList
			// 
			this.tbMasterDeviceList.Location = new System.Drawing.Point(584, 8);
			this.tbMasterDeviceList.Name = "tbMasterDeviceList";
			this.tbMasterDeviceList.Size = new System.Drawing.Size(48, 20);
			this.tbMasterDeviceList.TabIndex = 21;
			this.tbMasterDeviceList.TextChanged += new System.EventHandler(this.tbMasterDeviceList_TextChanged);
			// 
			// cbBroadcast
			// 
			this.cbBroadcast.Items.AddRange(new object[] {
            "None",
            "Direct Siblings",
            "Same Computer",
            "Same Room",
            "Same House",
            "All Houses"});
			this.cbBroadcast.Location = new System.Drawing.Point(408, 8);
			this.cbBroadcast.Name = "cbBroadcast";
			this.cbBroadcast.Size = new System.Drawing.Size(88, 21);
			this.cbBroadcast.TabIndex = 17;
			this.cbBroadcast.SelectedIndexChanged += new System.EventHandler(this.cbBroadcast_SelectedIndexChanged);
			// 
			// btnOnActivateGoto
			// 
			this.btnOnActivateGoto.Location = new System.Drawing.Point(160, 136);
			this.btnOnActivateGoto.Name = "btnOnActivateGoto";
			this.btnOnActivateGoto.Size = new System.Drawing.Size(40, 23);
			this.btnOnActivateGoto.TabIndex = 9;
			this.btnOnActivateGoto.Text = "Goto";
			this.btnOnActivateGoto.Click += new System.EventHandler(this.btnOnActivateGoto_Click);
			// 
			// lbSelectedCommands
			// 
			this.lbSelectedCommands.DisplayMember = "Description";
			this.lbSelectedCommands.Location = new System.Drawing.Point(200, 24);
			this.lbSelectedCommands.Name = "lbSelectedCommands";
			this.lbSelectedCommands.Size = new System.Drawing.Size(144, 147);
			this.lbSelectedCommands.TabIndex = 8;
			this.lbSelectedCommands.ValueMember = "CommandID";
			this.lbSelectedCommands.SelectedIndexChanged += new System.EventHandler(this.lbSelectedCommands_SelectedIndexChanged);
			// 
			// btnOnActivateDown
			// 
			this.btnOnActivateDown.Location = new System.Drawing.Point(176, 48);
			this.btnOnActivateDown.Name = "btnOnActivateDown";
			this.btnOnActivateDown.Size = new System.Drawing.Size(24, 23);
			this.btnOnActivateDown.TabIndex = 15;
			this.btnOnActivateDown.Text = "D";
			this.btnOnActivateDown.Click += new System.EventHandler(this.btnOnActivateDown_Click);
			// 
			// tvCommand
			// 
			this.tvCommand.AllowDrop = true;
			this.tvCommand.Location = new System.Drawing.Point(8, 24);
			this.tvCommand.Name = "tvCommand";
			this.tvCommand.Size = new System.Drawing.Size(152, 144);
			this.tvCommand.TabIndex = 5;
			this.tvCommand.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvCommand_AfterSelect);
			// 
			// btnOnActivateUp
			// 
			this.btnOnActivateUp.Location = new System.Drawing.Point(176, 24);
			this.btnOnActivateUp.Name = "btnOnActivateUp";
			this.btnOnActivateUp.Size = new System.Drawing.Size(24, 23);
			this.btnOnActivateUp.TabIndex = 14;
			this.btnOnActivateUp.Text = "U";
			this.btnOnActivateUp.Click += new System.EventHandler(this.btnOnActivateUp_Click);
			// 
			// btnOnActivateRemoveAG
			// 
			this.btnOnActivateRemoveAG.Enabled = false;
			this.btnOnActivateRemoveAG.Location = new System.Drawing.Point(160, 112);
			this.btnOnActivateRemoveAG.Name = "btnOnActivateRemoveAG";
			this.btnOnActivateRemoveAG.Size = new System.Drawing.Size(32, 23);
			this.btnOnActivateRemoveAG.TabIndex = 7;
			this.btnOnActivateRemoveAG.Text = "<<";
			this.btnOnActivateRemoveAG.Click += new System.EventHandler(this.btnOnActivateRemoveAG_Click);
			// 
			// btnOnActivateAddAG
			// 
			this.btnOnActivateAddAG.Enabled = false;
			this.btnOnActivateAddAG.Location = new System.Drawing.Point(160, 88);
			this.btnOnActivateAddAG.Name = "btnOnActivateAddAG";
			this.btnOnActivateAddAG.Size = new System.Drawing.Size(32, 23);
			this.btnOnActivateAddAG.TabIndex = 6;
			this.btnOnActivateAddAG.Text = ">>";
			this.btnOnActivateAddAG.Click += new System.EventHandler(this.btnOnActivateAddAG_Click);
			// 
			// pnOnActivateParameters
			// 
			this.pnOnActivateParameters.AutoScroll = true;
			this.pnOnActivateParameters.BackColor = System.Drawing.SystemColors.Control;
			this.pnOnActivateParameters.Location = new System.Drawing.Point(352, 56);
			this.pnOnActivateParameters.Name = "pnOnActivateParameters";
			this.pnOnActivateParameters.Size = new System.Drawing.Size(392, 112);
			this.pnOnActivateParameters.TabIndex = 13;
			// 
			// labAvailableCommands
			// 
			this.labAvailableCommands.Location = new System.Drawing.Point(16, 8);
			this.labAvailableCommands.Name = "labAvailableCommands";
			this.labAvailableCommands.Size = new System.Drawing.Size(112, 23);
			this.labAvailableCommands.TabIndex = 4;
			this.labAvailableCommands.Text = "Available Commands";
			// 
			// labSelectedCommands
			// 
			this.labSelectedCommands.Location = new System.Drawing.Point(200, 8);
			this.labSelectedCommands.Name = "labSelectedCommands";
			this.labSelectedCommands.Size = new System.Drawing.Size(120, 23);
			this.labSelectedCommands.TabIndex = 10;
			this.labSelectedCommands.Text = "Selected Commands";
			// 
			// DesignObjDesigner
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(936, 677);
			this.Controls.Add(this.pnCommandGroupInfo);
			this.Controls.Add(this.cbAnimate);
			this.Controls.Add(this.lblPriority);
			this.Controls.Add(this.nudPriority);
			this.Controls.Add(this.tbDescription);
			this.Controls.Add(this.cbAvailableVariations);
			this.Controls.Add(this.button6);
			this.Controls.Add(this.lbVariations);
			this.Controls.Add(this.btnRemoveSelectedVariation);
			this.Controls.Add(this.panelPreview);
			this.Controls.Add(this.tabAllPages);
			this.KeyPreview = true;
			this.Menu = this.mnuDesignObjXDesigner;
			this.Name = "DesignObjDesigner";
			this.Text = "DesignObjXDesigner";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.DesignObjDesigner_FormClosing);
			this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.DesignObjDesigner_FormClosed);
			this.Load += new System.EventHandler(this.DesignObjDesigner_Load);
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.DesignObjDesigner_KeyDown);
			this.tabButtons.ResumeLayout(false);
			this.tabAllPages.ResumeLayout(false);
			this.tabParameters.ResumeLayout(false);
			this.tabChildren.ResumeLayout(false);
			this.tabChildren.PerformLayout();
			this.tabOnSelected_WithChange.ResumeLayout(false);
			this.pnEffectsSelectChange.ResumeLayout(false);
			this.tabOnSelected_NoChange.ResumeLayout(false);
			this.panel1.ResumeLayout(false);
			this.tabOnHighlighted.ResumeLayout(false);
			this.panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.nudPriority)).EndInit();
			this.pnCommandGroupInfo.ResumeLayout(false);
			this.pnCommandGroupInfo.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}
		#endregion

		private void panelPreview_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
		{
		
		}

		private void lbVariations_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			bool origBlock = this.BlockUpdateImage();
			//select variation
			m_objUIDesignObj.DeselectAllVariations(false);
			foreach(UIDesignObjVariation objVariation in lbVariations.SelectedItems)
			{
				objVariation.Selected = true;
			}
			UpdateVariationParameters();
			UpdateVariationCommands();

			UpdateChildrenDesignObjs();
			UpdateCommandGroups();

			this.UpdateImage(origBlock);
		}

		private void button5_Click(object sender, System.EventArgs e)
		{
			HADataConfiguration.m_mdsCache = new MyDataSet(null,null,true,true);
			HADataConfiguration.m_mdsCache.SetRelations();
		}

		private void button6_Click(object sender, System.EventArgs e)
		{
			//Add Variation
			StringPair objPair = (StringPair) cbAvailableVariations.SelectedItem;
			if(objPair != null)
			{
				int intNewCriteriaID = Convert.ToInt32(objPair.ID);
				UIDesignObjVariation objVariation = m_objUIDesignObj.AddNewVariation(intNewCriteriaID, false, this.skinID);

				if(objVariation != null)
				{
					lbVariations.Items.Add(objVariation);
				}
			}

		}

		public void ResetImageBuild()
		{
			bool origBlock = this.BlockUpdateImage();
			m_objUIDesignObj.Reset();
			m_objUIDesignObj.Build(this.skinID);
			//m_objUIDesignObjDisplayControl.Zoom();
			
			this.UpdateImage(origBlock);
		}

		private void lbChildrenDesignObjs_DoubleClick(object sender, System.EventArgs e)
		{
			LoadSelectedDesignObj();
		}

		private void btnLoadSelectedDesignObj_Click(object sender, System.EventArgs e)
		{
			LoadSelectedDesignObj();
		}

		private void LoadSelectedDesignObj()
		{
			if(this.selectedUIDesignObj != null)
			{
				MainForm.newDesignObjDesignerWindow(this.selectedUIDesignObj.ID);
			}
		}

		private void DesignObjDesigner_FormClosed(object sender, System.EventArgs e)
		{
			if (this.m_objUIDesignObj != null)
			{
				this.m_objUIDesignObj.ReleaseBitmaps();
			}
			if (this.m_objUIDesignObjDisplayControl != null)
			{
				this.m_objUIDesignObjDisplayControl.ReleaseBitmap();
				this.m_objUIDesignObjDisplayControl.Dispose();
			}
			MainForm.removedDesignObjDesignerWindow(this.intDesignObjID);
		}

		private void AddNewChild(int PK_DesignObj, int X, int Y, int Width, int Height, bool IsTabStop)
		{
			//Get the selected variation

			UIDesignObjVariation objSelectedVariation = (UIDesignObjVariation)lbVariations.SelectedItems[0];
			UIDesignObj objNewDesignObj = new UIDesignObj(
				objSelectedVariation,
				-1,
				PK_DesignObj,
				this.GraphicsDirectory);

			objNewDesignObj.NeedsParentVariationLink = true;

			objNewDesignObj.SetDefaultPosition(X, Y);
			objNewDesignObj.SetDefaultIsTabStop(IsTabStop);

			if (Width >= 0 && Height >= 0)
			{
				objNewDesignObj.SetDefaultSize(Width, Height);
			}

			objSelectedVariation.DesignObjs.Add(objNewDesignObj);

			m_objUIDesignObj.DeselectAllDesignObjs(false);

			objNewDesignObj.Selected = true;

			//rebuild the object model
			m_objUIDesignObj.Build(this.skinID);

			//TODO: add a child here ?
		}

		private void AddNewText(int PK_Text)
		{
			//Get the selected variation

			UIDesignObjVariation selectedVariation = (UIDesignObjVariation)lbVariations.SelectedItems[0];
			UIText newText = new UIText(selectedVariation, -1, PK_Text);
			UITextSkinLanguage newUITSL = new UITextSkinLanguage(newText, -1);

			newText.TextSkinLanguages.Add(newUITSL);
			selectedVariation.Text.Add(newText);
		}

		private void panelPreview_DragDrop(object sender, System.Windows.Forms.DragEventArgs e)
		{
			//make sure that one and only one Variation is selected
			if(lbVariations.SelectedItems.Count != 1)
			{
				MessageBox.Show("You must have one and only one Variation Selected.");
			}
			else 
			{
				bool origBlock = this.BlockUpdateImage();
				if(e.Data.GetDataPresent("HAData.Common.TreeBuilder.DesignObjTreeNode"))
				{
					DesignObjTreeNode otn = (DesignObjTreeNode) e.Data.GetData("HAData.Common.TreeBuilder.DesignObjTreeNode");

					if (otn.DesignObjDataRow.fPK_DesignObj == this.intDesignObjID)
					{
						MessageBox.Show("You CANNOT add an object to itself.");
					}
					else
					{
						Point ptClient = this.panelPreview.PointToClient(new Point(e.X,e.Y));
						ptClient.X -= this.panelPreview.AutoScrollPosition.X;
						ptClient.Y -= this.panelPreview.AutoScrollPosition.Y;

						ptClient = new Point(
							Convert.ToInt32(Convert.ToDouble(ptClient.X)	/ m_objUIDesignObjDisplayControl.Scale), 
							Convert.ToInt32(Convert.ToDouble(ptClient.Y)	/ m_objUIDesignObjDisplayControl.Scale)
						);
						AddNewChild(otn.DesignObjDataRow.fPK_DesignObj, ptClient.X, ptClient.Y, -1, -1, false);
					}
				}
				else if(e.Data.GetDataPresent("HAData.Common.TreeBuilder.TextTreeNode"))
				{
					TextTreeNode ttn = (TextTreeNode) e.Data.GetData("HAData.Common.TreeBuilder.TextTreeNode");

					AddNewText(ttn.TextDataRow.fPK_Text);
				}
				else MessageBox.Show("ERROR: Unrecognized Drag & Drop Data");

				UpdateChildrenDesignObjs();
				this.UpdateImage(origBlock);
			}
		}

		private void panelPreview_DragEnter(object sender, System.Windows.Forms.DragEventArgs e)
		{
			if(e.Data.GetDataPresent("HAData.Common.TreeBuilder.DesignObjTreeNode") || e.Data.GetDataPresent("HAData.Common.TreeBuilder.TextTreeNode"))
			{
				e.Effect = DragDropEffects.Copy;
			}
			else e.Effect = DragDropEffects.None;
		}

		private void btnRemoveDesignObj_Click(object sender, System.EventArgs e)
		{
			if(this.selectedUIDesignObj != null)
			{
				if(this.selectedUIDesignObj.ID != -1)
				{
					bool origBlock = this.BlockUpdateImage();
					this.selectedUIDesignObj.NeedsParentVariationUnlink = true;

					//rebuild the object model
					m_objUIDesignObj.Build(this.skinID);

					//m_objUIDesignObjDisplayControl.Zoom();
					
					UpdateChildrenDesignObjs();
					this.UpdateImage(origBlock);
				}
			}
		}

		/// <summary>
		/// Shared by OVO and OVTSL
		/// </summary>
		private bool tbXYWidthHeight_Enabled
		{
			set
			{
				tbX.Enabled = tbY.Enabled = tbWidth.Enabled = tbHeight.Enabled = value;
				if (!value) tbX.Text = tbY.Text = tbWidth.Text = tbHeight.Text = "";
			}
		}

		/// <summary>
		/// Only For OVTSL
		/// </summary>
		private bool tbRO_Enabled
		{
			set
			{
				this.tbRotate.Enabled = this.tbOpacity.Enabled = this.btnBGColorTSL.Enabled = value;
				if (!value)
				{
					tbRotate.Text = tbOpacity.Text = "";
					this.btnBGColorTSL.BackColor = SystemColors.Control;
				}
			}
		}

		private void reallyClearListBoxSelection(ListBox lb)
		{
			ArrayList al = new ArrayList();
			foreach (object o in lb.Items) al.Add(o);
			lb.Items.Clear();
			foreach (object o in al) lb.Items.Add(o);
		}

		int lastSelectedDesignObjIndex = -1;
		int lastSelectedTextIndex = -1;
		bool processTextSelectChange = true;
		bool processDesignObjSelectChange = true;
		private void lbDesignObjsOrText_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			//bool origBlock = this.BlockUpdateImage();
			if (this.processDesignObjSelectChange && sender == this.lbChildrenDesignObjs && this.selectedUIDesignObj != null)
			{
				if (!this.IsInterfaceLocked())
				{
					this.LockInterface();
					m_objUIDesignObj.DeselectAllDesignObjs(false);
					this.selectedUIDesignObj.Selected = true;
					this.UnlockInterface();
				}

				this.processTextSelectChange = false;
				this.reallyClearListBoxSelection(this.lbText);
				this.lastSelectedTextIndex = -1;
				this.processTextSelectChange = true;
				
				this.lastSelectedDesignObjIndex = this.lbChildrenDesignObjs.SelectedIndex;
				
				this.clearLanguagesSkins();
				this.tbXYWidthHeight_Enabled = true;
				this.tbRO_Enabled = false;
				this.chCanBeHidden.Enabled = this.chRegenerateForEachScreen.Enabled = this.chHideByDefault.Enabled = true;
				this.chChildBehindBG.Enabled = false;

				loadAvailableLanguageSkins_Child();

				if(cbLanguageSkin_Child.Items.Count > 0)
					this.cbLanguageSkin_Child.SelectedIndex = 0;

				this.cbAlignV.Enabled = this.cbAlignH.Enabled = this.cbStyle.Enabled = false;
			}
			else if (this.processTextSelectChange && sender == this.lbText && this.selectedUIText != null)
			{
				if (!this.IsInterfaceLocked())
				{
					this.LockInterface();
					m_objUIDesignObj.DeselectAllDesignObjs(false);
					this.selectedUIText.Selected = true;
					this.UnlockInterface();
				}

				this.processDesignObjSelectChange = false;
				this.reallyClearListBoxSelection(this.lbChildrenDesignObjs);
				this.lastSelectedDesignObjIndex = -1;	
				this.processDesignObjSelectChange = true;

				this.lastSelectedTextIndex = this.lbText.SelectedIndex;
				this.tbXYWidthHeight_Enabled = true;
				this.tbRO_Enabled = true;
				this.chCanBeHidden.Enabled = this.chRegenerateForEachScreen.Enabled = this.chHideByDefault.Enabled = false;
				this.chChildBehindBG.Enabled = false;
				if (this.selectedUIText.TextID != -1) this.loadAvailableLanguageSkins();

				//tbRotate.Text = Convert.ToString(this.selectedUIDesignObj.Rotate);
				//tbOpacity.Text = Convert.ToString(this.selectedUIDesignObj.Opacity);
			}
			else
			{
				this.clearLanguagesSkins();
				this.tbXYWidthHeight_Enabled = false;
				this.tbRO_Enabled = false;
				this.chCanBeHidden.Enabled = this.chRegenerateForEachScreen.Enabled = this.chHideByDefault.Enabled = false;
				this.chChildBehindBG.Enabled = false;
			}
//			this.UpdateImage(origBlock);

			cbLanguageSkin_Child.Enabled = null != this.selectedUIDesignObj;
			cbLanguageSkin.Enabled = null != this.selectedUIText;
		}

		private void setTextLanguageSkinView(int languageID, int skinID)
		{
			bool changed = false;

			if (languageID == -1) languageID = 1;
			if (skinID == -1) skinID = 1;

			SkinLanguageStatus.Instance().TextSkinID = skinID;
			SkinLanguageStatus.Instance().TextLanguageID = languageID;

			if (languageID != this.languageID)
			{
				this.languageID = languageID;
				foreach (MenuItem mi in this.mnuLanguage.MenuItems) mi.Checked = (((MenuItemID)mi).ID == languageID);
				changed = true;
			}
			if (skinID != this.skinID)
			{
				this.skinID = skinID;
				foreach (MenuItem mi in this.mnuSkin.MenuItems) mi.Checked = (((MenuItemID)mi).ID == skinID);
				this.m_objUIDesignObj.Build(this.skinID, true);
				changed = true;
			}

			if (changed) this.UpdateImage(); // TODO: Block Update may be necessary
		}

		private void cbLanguageSkin_SelectedIndexChanged(object sender, System.EventArgs e)// sender or e could be null
		{
			// TODO: Show rectangle around selection
			// TODO: Enable Selection from drawing
	
			if (this.selectedUITSL != null) // Should never be null
			{
				//	if (this.selectedUITSL.ID != -1) // TODO: should still update if ID == -1, need to set ID for a new row after it has been inserted
				
				// lbText.SelectedIndex = -1;
				// this.enable_tbXYWidthHeight(true);

				this.setTextLanguageSkinView(this.selectedUITSL.LanguageID,this.selectedUITSL.SkinID);
				this.cbAlignH.SelectedIndex = this.selectedUITSL.HAlignmentSelection;
				this.cbAlignV.SelectedIndex = this.selectedUITSL.VAlignmentSelection;
				
				this.cbStyle.SelectedIndex = StringPair.getIndex(this.spStyles, this.selectedUITSL.StyleID.ToString());

				tbX.Text = this.selectedUITSL.X.ToString();
				tbY.Text = this.selectedUITSL.Y.ToString();
				tbWidth.Text = this.selectedUITSL.Width.ToString();
				tbHeight.Text = this.selectedUITSL.Height.ToString();
				tbRotate.Text = this.selectedUITSL.Rotate.ToString();
				tbOpacity.Text = this.selectedUITSL.Opacity.ToString();
				this.btnBGColorTSL.BackColor = Color.FromArgb(this.selectedUITSL.BGColor);

				if (!this.IsInterfaceLocked()) // TODO: REFRESH IMAGE WITH RECTANGLE AROUND SELECTION
				{
					this.LockInterface();
					m_objUIDesignObjDisplayControl.UpdateImage();
					m_objUIDesignObjDisplayControl.Refresh();
					this.UnlockInterface();
				}
			}
			// else this.enable_tbXYWidthHeight(false);
		}

		private void tbX_Leave(object sender, System.EventArgs e)
		{
			UpdateX();
		}
		
		private void tbX_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter) UpdateX();
		}

		private void tbY_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter) UpdateY();
		}

		private void tbY_Leave(object sender, System.EventArgs e)
		{
			UpdateY();
		}

		private void tbWidth_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter) UpdateWidth();
		}

		private void tbWidth_Leave(object sender, System.EventArgs e)
		{
			UpdateWidth();
		}

		private void tbRotate_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter) UpdateRotate();
		}

		private void tbRotate_Leave(object sender, System.EventArgs e)
		{
			UpdateRotate();
		}

		private void tbOpacity_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter) UpdateOpacity();
		}

		private void tbOpacity_Leave(object sender, System.EventArgs e)
		{
			UpdateOpacity();
		}

		private void tbHeight_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter) UpdateHeight();
		}

		private void tbHeight_Leave(object sender, System.EventArgs e)
		{
			UpdateHeight();
		}

		private void cbAlignment_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			if (this.selectedUITSL != null)
			{
				bool origBlock = this.BlockUpdateImage();

				if (sender!=this.cbAlignV) this.selectedUITSL.HAlignmentSelection = this.cbAlignH.SelectedIndex;
				if (sender!=this.cbAlignH) this.selectedUITSL.VAlignmentSelection = this.cbAlignV.SelectedIndex;
					
				this.UpdateImage(origBlock);
			}
		}

		private void cbStyle_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			if (this.selectedUITSL != null)
			{
				bool origBlock = this.BlockUpdateImage();
				
				this.selectedUITSL.StyleID = Convert.ToInt32(((StringPair)this.cbStyle.SelectedItem).ID);
				
				this.UpdateImage(origBlock);
			}
		}

		private void tbDescription_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter) this.tbDescription_Leave(sender, null);		
		}

		private void tbDescription_Leave(object sender, System.EventArgs e)
		{
			this.Text = this.m_objUIDesignObj.Description = this.tbDescription.Text;
		}

		private void nudPriority_ValueChanged(object sender, System.EventArgs e)
		{
			this.m_objUIDesignObj.Priority = (sbyte)this.nudPriority.Value;
		}

		private void nudPriority_Leave(object sender, System.EventArgs e)
		{
			this.nudPriority_ValueChanged(sender, e);	
		}

		private void cbAnimate_Click(object sender, System.EventArgs e)
		{
			this.m_objUIDesignObj.Animate = this.cbAnimate.Checked;		
		}

		private void UpdateX()
		{
			if(CommonMethods.IsNumeric(tbX.Text))
			{
				if(this.selectedUIDesignObj != null && this.selectedUIDesignObj.ID != -1)
				{
					if(Convert.ToString(this.selectedUICSL.ParentX) != tbX.Text)
					{
						bool origBlock = this.BlockUpdateImage();

						this.selectedUICSL.ParentX = Convert.ToInt32(tbX.Text);
						this.selectedUICSL.Build(this.SkinID);
						this.UpdateImage(origBlock);
					}
				}
				else if (this.selectedUITSL != null)
				{
					if (Convert.ToString(this.selectedUITSL.X) != this.tbX.Text)
					{
						bool origBlock = this.BlockUpdateImage();

						this.selectedUITSL.X = Convert.ToInt32(this.tbX.Text);
						
						this.UpdateImage(origBlock);
					}
				}
			}
			else
			{
				MessageBox.Show("Not a number.");
				tbX.Focus();
			}
		}
		private void UpdateY()
		{
			if(CommonMethods.IsNumeric(tbY.Text))
			{
				if(this.selectedUIDesignObj != null && this.selectedUIDesignObj.ID != -1)
				{
					if(Convert.ToString(this.selectedUICSL.ParentY) != tbY.Text)
					{
						bool origBlock = this.BlockUpdateImage();
						this.selectedUICSL.ParentY = Convert.ToInt32(tbY.Text);
						this.selectedUICSL.Build(this.SkinID);
						this.UpdateImage(origBlock);
					}
				}
				else if (this.selectedUITSL != null)
				{
					if (Convert.ToString(this.selectedUITSL.Y) != this.tbY.Text)
					{
						bool origBlock = this.BlockUpdateImage();

						this.selectedUITSL.Y = Convert.ToInt32(this.tbY.Text);
					
						this.UpdateImage(origBlock);
					}
				}
			}
			else
			{
				MessageBox.Show("Not a number.");
				tbY.Focus();
			}
			
		}
		private void UpdateWidth()
		{
			if(CommonMethods.IsNumeric(tbWidth.Text))
			{
				if(this.selectedUIDesignObj != null && this.selectedUIDesignObj.ID != -1 && null != selectedUICSL)
				{
					if(Convert.ToString(selectedUICSL.Width) != tbWidth.Text)
					{
						bool origBlock = this.BlockUpdateImage();

						selectedUICSL.Width = Convert.ToInt32(tbWidth.Text);
						this.selectedUIDesignObj.Build(this.skinID);
						this.UpdateImage(origBlock);
					}
				}
				else if (this.selectedUITSL != null)
				{
					if (Convert.ToString(this.selectedUITSL.Width) != this.tbWidth.Text)
					{
						bool origBlock = this.BlockUpdateImage();

						this.selectedUITSL.Width = Convert.ToInt32(this.tbWidth.Text);
					
						this.UpdateImage(origBlock);
					}
				}
			}
			else
			{
				MessageBox.Show("Not a number.");
				tbWidth.Focus();
			}
			
		}
		private void UpdateHeight()
		{
			if(CommonMethods.IsNumeric(tbHeight.Text))
			{
				if(this.selectedUIDesignObj != null && this.selectedUIDesignObj.ID != -1 && null != selectedUICSL)
				{
					if(Convert.ToString(selectedUICSL.Height) != tbHeight.Text)
					{
						bool origBlock = this.BlockUpdateImage();

						selectedUICSL.Height = Convert.ToInt32(tbHeight.Text);
						this.selectedUIDesignObj.Build(this.skinID);
						this.UpdateImage(origBlock);
					}
				}
				else if (this.selectedUITSL != null)
				{
					if (Convert.ToString(this.selectedUITSL.Height) != this.tbHeight.Text)
					{
						bool origBlock = this.BlockUpdateImage();

						this.selectedUITSL.Height = Convert.ToInt32(this.tbHeight.Text);
						
						this.UpdateImage(origBlock);
					}
				}
			}
			else
			{
				MessageBox.Show("Not a number.");
				tbHeight.Focus();
			}
			
		}
		private void UpdateRotate()
		{
			if(CommonMethods.IsNumeric(tbRotate.Text))
			{
				if(this.selectedUIDesignObj != null && this.selectedUIDesignObj.ID != -1)
				{
					//this shouldn't happen
					/*
					if(Convert.ToString(this.selectedUIDesignObj.Rotate) != tbRotate.Text)
					{
						// TODO: Allow to set to default
						bool origBlock = this.BlockUpdateImage();

						//this.selectedUIDesignObj.Rotate = Convert.ToInt32(tbRotate.Text);
						this.selectedUIDesignObj.Build(this.skinID);
						//m_objUIDesignObjDisplayControl.Zoom();

						this.UpdateImage(origBlock);
					}
					*/
				}
				else if (this.selectedUITSL != null)
				{
					if (Convert.ToString(this.selectedUITSL.Rotate) != this.tbRotate.Text)
					{
						bool origBlock = this.BlockUpdateImage();

						this.selectedUITSL.Rotate = Convert.ToInt32(this.tbRotate.Text);
						
						this.UpdateImage(origBlock);
					}
				}
			}
			else
			{
				MessageBox.Show("Not a number.");
				tbRotate.Focus();
			}
			
		}
		private void UpdateOpacity()
		{
			if(CommonMethods.IsNumeric(tbOpacity.Text))
			{
				int intOpacityTemp = Convert.ToInt32(tbOpacity.Text);
				if(intOpacityTemp < 0 || intOpacityTemp > 100)
				{
					MessageBox.Show("Enter a number from 0 to 100.");
					tbOpacity.Focus();
				}
				else
				{
					if(this.selectedUIDesignObj != null && this.selectedUIDesignObj.ID != -1)
					{
						//this shouldn't happen
						/*
						if(Convert.ToString(this.selectedUIDesignObj.Opacity) != tbOpacity.Text)
						{
							// TODO: Allow to set to default
							bool origBlock = this.BlockUpdateImage();

							this.selectedUIDesignObj.Opacity = Convert.ToInt32(tbOpacity.Text);
							this.selectedUIDesignObj.Build(this.skinID);
							//m_objUIDesignObjDisplayControl.Zoom();

							this.UpdateImage(origBlock);
						}
						*/
					}
					else if (this.selectedUITSL != null)
					{
						if (Convert.ToString(this.selectedUITSL.Opacity) != this.tbOpacity.Text)
						{
							bool origBlock = this.BlockUpdateImage();

							this.selectedUITSL.Opacity = Convert.ToInt32(this.tbOpacity.Text);
						
							this.UpdateImage(origBlock);
						}
					}
				}
			}
			else
			{
				MessageBox.Show("Not a number.");
				tbOpacity.Focus();
			}
			
		}

		private void btnCancel_Click(object sender, System.EventArgs e)
		{
			this.Close();
		}

		private void btnRemoveSelectedVariation_Click(object sender, System.EventArgs e)
		{
			if(lbVariations.SelectedItems.Count == 1)
			{
				if(lbVariations.SelectedItems[0] == lbVariations.Items[0])
				{
					MessageBox.Show("You cannot remove the Standard Variation.");
				}
				else 
				{
					string confirm = "Are you sure you want to delete this variation: " + this.selectedUIDesignObjVariation.Description + "?";
					if (MessageBox.Show(confirm,"Are you sure?",MessageBoxButtons.YesNo) == DialogResult.Yes)
					{
						//remove
						this.selectedUIDesignObjVariation.NeedsDelete = true;
						this.selectedUIDesignObjVariation.Selected = false;

						lbVariations.Items.Remove(this.selectedUIDesignObjVariation);
						lbVariations.SelectedIndex = 0;
					}
				}
			}
			else
			{
				MessageBox.Show("Please select one Variation.");
			}
		}

		private void checkZoomMenu(MenuItem miChecked)
		{
			foreach (MenuItem mi in this.mnuZoom.MenuItems) mi.Checked = false;
			miChecked.Checked = true;
			int zoom = Convert.ToInt32(miChecked.Text.Substring(0,miChecked.Text.Length-1));
			this.m_objUIDesignObjDisplayControl.Zoom(zoom);
		}

		private void miZoom_Click(object sender, System.EventArgs e)
		{
			this.checkZoomMenu((MenuItem)sender);
		}

		private void miLanguage_Click(object sender, System.EventArgs e)
		{
			bool origBlock = this.BlockUpdateImage();

			foreach (MenuItem mi in this.mnuLanguage.MenuItems) mi.Checked = false;
			MenuItemID miClicked = (MenuItemID)sender;
			miClicked.Checked = true;
			this.languageID = miClicked.ID;
				
			this.UpdateImage(origBlock);
		}

		private void miSkin_Click(object sender, System.EventArgs e)
		{
			foreach (MenuItem mi in this.mnuSkin.MenuItems) mi.Checked = false;
			MenuItemID miClicked = (MenuItemID)sender;
			miClicked.Checked = true;
			this.skinID = miClicked.ID;
			this.m_objUIDesignObj.Build(this.skinID, true);
				
			this.UpdateImage(false);
		}

		private void DesignObjDesigner_FormClosing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			if (this.warnSaveChanges && this.m_objUIDesignObj.WarnToSave)
			{
				switch (MessageBox.Show("Save your changes ?","Save your changes ?",MessageBoxButtons.YesNoCancel))
				{
					case DialogResult.Yes:
						this.btnSaveChanges_Click(sender, null);
						break;
					case DialogResult.No:
						break;
					case DialogResult.Cancel:
						e.Cancel = true;
						break;
				}
			}
		}

		private void btnSaveChanges_Click(object sender, System.EventArgs e)
		{
			bool descriptionChanged = m_objUIDesignObj.DescriptionChanged;
			bool blnChanges = m_objUIDesignObj.SaveToDatabase();

			if(blnChanges)
			{
				DesignObjDataRow drObj = mds.tDesignObj[m_objUIDesignObj.ID];
				m_objUIDesignObj.DesignObjModified(drObj);	
				MessageBox.Show("Changes Saved.");
				ResetImageBuild();
				UpdateChildrenDesignObjs(); // to show inserted ids
				if (descriptionChanged)
				{
					if (this.tnDesignObj!=null) this.tnDesignObj.Text = this.m_objUIDesignObj.Description;
					else 
					{
						DesignObjTree.GetNodeByID(MainForm.Browser.DesignObjTreeView,this.intDesignObjID).Text = this.m_objUIDesignObj.Description;
					}
				}
			}
			else MessageBox.Show("No Changes Made.");
		}

		private void btnFindGotoDesignObj_Click(object sender, System.EventArgs e)
		{
			ChooseDesignObj frmChooseDesignObj = new ChooseDesignObj();
			frmChooseDesignObj.Owner = this;
			frmChooseDesignObj.ShowDialog();
		}



		




	






		//private void btnDeleteGotoDesignObj_Click(object sender, System.EventArgs e)
		//{
		//	UIDesignObjVariation objSelectedVariation = (UIDesignObjVariation) lbVariations.SelectedItems[0];
		//	objSelectedVariation.GotoUIDesignObjID = -1;
		//	tbGotoDesignObj.Text = "";
		//}

		private void btnEditText_Click(object sender, System.EventArgs e)
		{
			if (this.selectedUIText != null)
			{
				if ((new TextForm(this.selectedUIText.TextID)).ShowDialog() == DialogResult.OK)
				{
					this.UpdateImage();
				}
			}
		}

		private void btnAddSkinLanguage_Click(object sender, System.EventArgs e)
		{
			if (this.selectedUIText != null)
			{
				if ((new SkinLanguageForm(this.selectedUIText)).ShowDialog() == DialogResult.OK)
				{
					bool origBlock = this.BlockUpdateImage();

					this.loadAvailableLanguageSkins();
					this.loadAvailableLanguageSkins_Child();
						
					this.UpdateImage(origBlock);
				}
			}
		}

		private void btnAddSkinLanguage_Child_Click(object sender, System.EventArgs e)
		{
			if (this.selectedUIText != null)
			{
				if ((new SkinLanguageForm(this.selectedUIText)).ShowDialog() == DialogResult.OK)
				{
					bool origBlock = this.BlockUpdateImage();

					this.loadAvailableLanguageSkins();
					this.loadAvailableLanguageSkins_Child();
						
					this.UpdateImage(origBlock);
				}
			}
		}

		private void btnRemoveText_Click(object sender, System.EventArgs e)
		{
			if(this.selectedUIText != null)
			{
				bool origBlock = this.BlockUpdateImage();

				if (this.selectedUIText.ID != -1) this.selectedUIText.NeedsDelete = true;
				else this.selectedUIText.Deleted = true; // Delete a New Item

				this.UpdateChildrenDesignObjs(); // (Also updates text)

				this.UpdateImage(origBlock);
			}
		}

		private void btnRemoveSkinLanguage_Click(object sender, System.EventArgs e)
		{
			if (this.selectedUITSL != null)
			{
				if (this.selectedUITSL.LanguageID == -1 && this.selectedUITSL.SkinID == -1)
				{
					MessageBox.Show("You cannot delete the default skin-language");
				}
				else
				{
					bool origBlock = this.BlockUpdateImage();

					if (this.selectedUITSL.ID != -1) this.selectedUITSL.NeedsDelete = true;
					else this.selectedUITSL.Deleted = true; // Delete a New Item

					this.loadAvailableLanguageSkins();
					this.loadAvailableLanguageSkins_Child();

					this.UpdateImage(origBlock);
				}
			}
		}

		private void changeButton(object sender, System.EventArgs e)
		{
			//get the selected variation
			if(lbVariations.SelectedItems.Count == 1)
			{
				UIDesignObjVariation objSelectedVariation = (UIDesignObjVariation) lbVariations.SelectedItems[0];
				if(cbButton.SelectedItem != null)
				{
					StringPair objPair = (StringPair) cbButton.SelectedItem;
					objSelectedVariation.CButton = Convert.ToInt32(objPair.ID);
				}

			}

		}

		private void DesignObjDesigner_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if (e.Modifiers == Keys.Control)
			{
				switch (e.KeyCode)
				{
					case Keys.O:
						this.LoadSelectedDesignObj();
						break;
					case Keys.S:
						this.btnSaveChanges_Click(sender,null);
						break;
					case Keys.X:
						this.Close();
						break;
				}
			}
			else if (e.KeyCode == Keys.F5)
			{
				MDIForm mainForm = this.MainForm; // Can't get after closing
				this.Close();
				mainForm.newDesignObjDesignerWindow(this.intDesignObjID,this.tnDesignObj); // if you cancel, it's not a new window ;)
			}
		}

		private void tabAllPages_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			UpdateCommandGroups();
		}

		private void lbSelectedCommands_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			if(lbSelectedCommands.SelectedItems.Count == 1)
			{
				UpdateAGParms((UIDesignObjVariationCommand) lbSelectedCommands.SelectedItems[0]);
			}
		}

		private void tvCommand_AfterSelect(object sender, System.Windows.Forms.TreeViewEventArgs e)
		{
			CommandDataRow odr = null;

			bool isCommand = false;
			//bool isCategory = false;
			
			CommandTreeNode tn = (CommandTreeNode) tvCommand.SelectedNode;
			
			if ( tn!=null )
			{
				odr = tn.CommandDataRow;
				if( odr==null ) // not Command, check if category (& get category row)
				{
					/*
					CommandCategoryDataRow ocdr = null;
					ocdr = tn.CommandCategoryDataRow;
					//if (ocdr!=null) isCategory = true;
					*/
				}
				else isCommand = true; // odr !null => is Command
			}

			if (isCommand)
			{
				btnOnActivateAddAG.Enabled = true;
				m_intCommandSelected = odr.fPK_Command;
			}
			else
			{
				btnOnActivateAddAG.Enabled = false;
				m_intCommandSelected = -1;
			}
		}

		private void btnOnActivateAddAG_Click(object sender, System.EventArgs e)
		{
			UIDesignObjVariationCommandGroup objAG = GetSelectedCommandGroup();
			if(objAG != null)
			{
				objAG.Commands.Add(new UIDesignObjVariationCommand(objAG, -1, m_intCommandSelected));
				UpdateCommandGroups();
			}
		}

		private void btnOnActivateRemoveAG_Click(object sender, System.EventArgs e)
		{
			UIDesignObjVariationCommandGroup objAG = GetSelectedCommandGroup();
			if(objAG != null)	//should never be null
			{
				if(lbSelectedCommands.SelectedItems.Count == 1)
				{
					UIDesignObjVariationCommand objA = (UIDesignObjVariationCommand) lbSelectedCommands.SelectedItems[0];
					objA.NeedsDelete = true;
					UpdateCommandGroups();
				}
			}
		}


		

		private UIDesignObjVariation GetSelectedVariation()
		{
			UIDesignObjVariation objSelectedVariation = null;
			if(lbVariations.SelectedItems.Count == 1)
			{
				objSelectedVariation = (UIDesignObjVariation) lbVariations.SelectedItems[0];
			}
			return objSelectedVariation;
		}


		private UIDesignObjVariationCommandGroup GetSelectedCommandGroup()
		{
			UIDesignObjVariationCommandGroup objAG = null;
			if(lbVariations.SelectedItems.Count == 1)
			{
				UIDesignObjVariation objSelectedVariation = (UIDesignObjVariation) lbVariations.SelectedItems[0];
				if(this.tabAllPages.SelectedTab == this.tabOnActivate)
				{
					objAG = objSelectedVariation.CommandGroup_D_Touch; 
				}
				if(this.tabAllPages.SelectedTab == this.tabOnLoad)
				{
					objAG = objSelectedVariation.CommandGroup_D_Load; 
				}
				if(this.tabAllPages.SelectedTab == this.tabOnUnload)
				{
					objAG = objSelectedVariation.CommandGroup_D_Unload;
				}
				if(this.tabAllPages.SelectedTab == this.tabOnStartup)
				{
					objAG = objSelectedVariation.CommandGroup_D_Startup;
				}
				if(this.tabAllPages.SelectedTab == this.tabOnHighlight)
				{
					objAG = objSelectedVariation.CommandGroup_D_Highlight;
				}
				if(this.tabAllPages.SelectedTab == this.tabOnUnhighlight)
				{
					objAG = objSelectedVariation.CommandGroup_D_Unhighlight;
				}
				if(this.tabAllPages.SelectedTab == this.tabOnTimeout)
				{
					objAG = objSelectedVariation.CommandGroup_D_Timeout;
				}
			}
			return objAG;
		}

		private void lbChildrenDesignObjs_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if (this.selectedUIDesignObj != null && this.selectedUIDesignObj.ID != -1)
			{
				lbChildrenDesignObjs.DoDragDrop(new DragUIDesignObj(this,this.selectedUIDesignObj),DragDropEffects.All);
			}

			bool alreadyCleared = false;
			if (this.lbChildrenDesignObjs.SelectedIndex == this.lastSelectedDesignObjIndex)
			{
				if (this.lastSelectedDesignObjIndex == -1) alreadyCleared = true;
				else
				{
					this.reallyClearListBoxSelection(this.lbChildrenDesignObjs);
					this.lastSelectedDesignObjIndex = -1;
					
					this.LockInterface();
					m_objUIDesignObj.DeselectAllDesignObjs(false);
					this.UnlockInterface();
				}
			}

			if (!alreadyCleared) this.lbDesignObjsOrText_SelectedIndexChanged(sender, null);
		}
		private void lbText_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if (this.selectedUIText != null && this.selectedUIText.ID != -1)
			{
				lbText.DoDragDrop(new DragUIText(this,this.selectedUIText),DragDropEffects.All);
			}

			bool alreadyCleared = false;
			if (this.lbText.SelectedIndex == this.lastSelectedTextIndex)
			{
				if (this.lastSelectedTextIndex == -1) alreadyCleared = true;
				else
				{
					this.reallyClearListBoxSelection(this.lbText);
					this.lastSelectedTextIndex = -1;

					this.LockInterface();
					m_objUIDesignObj.DeselectAllDesignObjs(false);
					this.UnlockInterface();
				}
			}

			if (!alreadyCleared) this.lbDesignObjsOrText_SelectedIndexChanged(sender, null);
		}



		private object draggedUIO_origUIOV;
		private void lbVariations_DragEnter(object sender, System.Windows.Forms.DragEventArgs e)
		{
			if (e.Data.GetDataPresent("HADesigner.DragUIDesignObj"))
			{
				this.draggedUIO_origUIOV = ((DragUIDesignObj)e.Data.GetData("HADesigner.DragUIDesignObj")).uiDesignObj.ParentUIDesignObjVariation;
			}
			else if (e.Data.GetDataPresent("HADesigner.DragUIText"))
			{
				this.draggedUIO_origUIOV = ((DragUIText)e.Data.GetData("HADesigner.DragUIText")).uiText.ParentUIDesignObjVariation;
			}
		}

		private void lbVariations_DragOver(object sender, System.Windows.Forms.DragEventArgs e)
		{
			if (e.Data.GetDataPresent("HADesigner.DragUIDesignObj") || e.Data.GetDataPresent("HADesigner.DragUIText"))
			{
				int indexDrop = this.lbVariations.IndexFromPoint(this.lbVariations.PointToClient(new Point(e.X,e.Y)));
				
				if (indexDrop == -1 || this.lbVariations.Items[indexDrop] == this.draggedUIO_origUIOV) e.Effect = DragDropEffects.None; 
				else
				{
					if ((e.KeyState & 0x08) == 0x08) e.Effect = DragDropEffects.Copy;
					else e.Effect = DragDropEffects.Move;
				}
			}
			else e.Effect = DragDropEffects.None;
		}

		private void lbVariations_DragDrop(object sender, System.Windows.Forms.DragEventArgs e)
		{
			UIDesignObjVariation dropOV = (UIDesignObjVariation) this.lbVariations.Items[this.lbVariations.IndexFromPoint(this.lbVariations.PointToClient(new Point(e.X,e.Y)))];

			bool isDesignObj = e.Data.GetDataPresent("HADesigner.DragUIDesignObj");
			bool isText = e.Data.GetDataPresent("HADesigner.DragUIText"); 
			
			DragUI dragUI = null;
			DragUIDesignObj dragUIO = null;
			DragUIText dragUIT = null;
			
			if (isDesignObj) dragUI = dragUIO = (DragUIDesignObj)e.Data.GetData("HADesigner.DragUIDesignObj");
			else if (isText) dragUI = dragUIT = (DragUIText)e.Data.GetData("HADesigner.DragUIText");
			else return;

			bool needToSaveDrag = (dragUI.objectDesigner.m_objUIDesignObj.WarnToSave && (e.Effect == DragDropEffects.Copy || (e.Effect == DragDropEffects.Move && dragUI.objectDesigner != this)));
			bool needToSaveDrop = dropOV.NeedsInsert && (!needToSaveDrag || dragUI.objectDesigner != this);
			bool needToSave = needToSaveDrag || needToSaveDrop;
			if (needToSave)
			{
				string saveChanges = "You must save your changes to move this object, Save your Changes ?"; 
				if (MessageBox.Show(saveChanges,"Save your Changes ?",MessageBoxButtons.YesNo) == DialogResult.Yes)
				{
					if (needToSaveDrag)
					{
						dragUI.objectDesigner.btnSaveChanges_Click(sender,null);
					}
					if (needToSaveDrop)
					{
						this.btnSaveChanges_Click(sender, null);
					}
					needToSave = false;
				}
				else needToSave = true;
			}
			
			if (!needToSave)
			{
				bool origBlock = this.BlockUpdateImage();
				bool copiedText = false;
				UIDesignObj dragO = null;
				UIText dragT = null;
				if (e.Effect == DragDropEffects.Move)
				{
					if (isDesignObj)
					{
						dragO = dragUIO.uiDesignObj;

						dragO.NeedsParentVariationUnlink = true;
						dragO.SaveLinkToDatabase();
						dragO.ParentUIDesignObjVariation.DesignObjs.Remove(dragO);
					}
					else
					{
						dragT = dragUIT.uiText;
						dragT.ParentUIDesignObjVariation.Text.Remove(dragT);
					}
				}
				else if (e.Effect == DragDropEffects.Copy) 
				{
					if (isDesignObj)
					{
						dragO = new UIDesignObj(dropOV, -1, dragUIO.uiDesignObj.ID, this.GraphicsDirectory);
						UIChildSkinLanguage child = new UIChildSkinLanguage(dragO, -1, 
							m_objUIDesignObjDisplayControl.CurrentLanguageForChild(),
							m_objUIDesignObjDisplayControl.CurrentSkinForChild(), GraphicsDirectory);
						dragO.ChildSkinLanguages.Add(child);

						UIChildSkinLanguage ChildSource = dragUIO.uiDesignObj.GetCurrentChildSkinLanguage(
							m_objUIDesignObjDisplayControl.CurrentLanguageForChild(),
							m_objUIDesignObjDisplayControl.CurrentSkinForChild());

						if(null != ChildSource)
						{
							child.Width = ChildSource.Width;
							child.Height = ChildSource.Height;
							child.ParentX = ChildSource.ParentX;
							child.ParentY = ChildSource.ParentY;
							child.ParentDisplayOrder = ChildSource.ParentDisplayOrder;
						}
					}
					else
					{
						dragT = new UIText(dropOV,-1,dragUIT.uiText.TextID);
						foreach (UITextSkinLanguage uiTSLorig in dragUIT.uiText.TextSkinLanguages)
						{
							dragT.TextSkinLanguages.Add(uiTSLorig.Copy(dragT));
						}
						copiedText = true;
					}
				}
				else return;
					
				if (isDesignObj)
				{
					dragO.ParentUIDesignObjVariation = dropOV;
					dragO.NeedsParentVariationLink = true;
					dragO.SaveLinkToDatabase();
					dragO.ParentUIDesignObjVariation.DesignObjs.Add(dragO);
				}
				else
				{
					dragT.ParentUIDesignObjVariation = dropOV;
					dragT.SaveToDatabase();
					dragT.SaveLinkToDatabase(copiedText);
					dragT.ParentUIDesignObjVariation.Text.Add(dragT);
				}

				this.m_objUIDesignObj.Build(this.skinID);
				this.UpdateChildrenDesignObjs(); // also updates text

				if (e.Effect == DragDropEffects.Move && dragUI.objectDesigner != this) // moved from another window
				{
					bool origDragBlock = dragUI.objectDesigner.BlockUpdateImage();

					dragUI.objectDesigner.m_objUIDesignObj.Build(this.skinID);
					dragUI.objectDesigner.UpdateChildrenDesignObjs();

					dragUI.objectDesigner.UpdateImage(origDragBlock);
				}

				this.UpdateImage(origBlock);
			}
		}

		private void btnOnActivateUp_Click(object sender, System.EventArgs e)
		{

			UIDesignObjVariationCommandGroup objAG = GetSelectedCommandGroup();

			if(objAG != null && lbSelectedCommands.SelectedItems.Count == 1)
			{
				UIDesignObjVariationCommand objA = (UIDesignObjVariationCommand) lbSelectedCommands.SelectedItems[0];
				
				//get the previous included action to make the swap
				int intSelectedIndex = -1;
				int intPreviousIndex = -1;

				for(int intCount=objAG.Commands.Count-1; intCount>=0; intCount--)
				{
					if(((UIDesignObjVariationCommand) objAG.Commands[intCount]).Include)
					{
						if(intSelectedIndex == -1)
						{
							if(objAG.Commands[intCount] == objA)
							{
								intSelectedIndex = intCount;
							}
						}
						else
						{
							intPreviousIndex = intCount;
							break;
						}
					}
				}

				int intOrderTemp;
				if(intSelectedIndex != -1 && intPreviousIndex != -1)
				{
					//switch em
					objA = (UIDesignObjVariationCommand) objAG.Commands[intSelectedIndex];
					objAG.Commands[intSelectedIndex] = objAG.Commands[intPreviousIndex];
					objAG.Commands[intPreviousIndex] = objA;

					//switch the order nums
					intOrderTemp = ((UIDesignObjVariationCommand) objAG.Commands[intSelectedIndex]).OrderNum;
					((UIDesignObjVariationCommand) objAG.Commands[intSelectedIndex]).OrderNum = ((UIDesignObjVariationCommand) objAG.Commands[intPreviousIndex]).OrderNum;
					((UIDesignObjVariationCommand) objAG.Commands[intPreviousIndex]).OrderNum = intOrderTemp;


					
					
				
				}


				UpdateCommandGroups();


				int intSelectIndex;
				
				if(intPreviousIndex == -1)
				{
					intSelectIndex = intSelectedIndex;
				}
				else
				{
					intSelectIndex = intPreviousIndex;
				}

			
				int intCount2 = -1;
				foreach(UIDesignObjVariationCommand obj in lbSelectedCommands.Items)
				{
					intCount2++;
					if(obj == objAG.Commands[intSelectIndex])
					{
						lbSelectedCommands.SelectedIndex = intCount2;
						break;
					}
				}
			

			}
		}

		private void btnOnActivateDown_Click(object sender, System.EventArgs e)
		{
			UIDesignObjVariationCommandGroup objAG = GetSelectedCommandGroup();

			if(objAG != null && lbSelectedCommands.SelectedItems.Count == 1)
			{
				UIDesignObjVariationCommand objA = (UIDesignObjVariationCommand) lbSelectedCommands.SelectedItems[0];
				
				//get the Next included action to make the swap
				int intSelectedIndex = -1;
				int intNextIndex = -1;

				for(int intCount=0; intCount<=objAG.Commands.Count-1; intCount++)
				{
					if(((UIDesignObjVariationCommand) objAG.Commands[intCount]).Include)
					{
						if(intSelectedIndex == -1)
						{
							if(objAG.Commands[intCount] == objA)
							{
								intSelectedIndex = intCount;
							}
						}
						else
						{
							intNextIndex = intCount;
							break;
						}
					}
				}

				int intOrderTemp;
				if(intSelectedIndex != -1 && intNextIndex != -1)
				{
					//switch em
					objA = (UIDesignObjVariationCommand) objAG.Commands[intSelectedIndex];
					objAG.Commands[intSelectedIndex] = objAG.Commands[intNextIndex];
					objAG.Commands[intNextIndex] = objA;

					//switch the order nums
					intOrderTemp = ((UIDesignObjVariationCommand) objAG.Commands[intSelectedIndex]).OrderNum;
					((UIDesignObjVariationCommand) objAG.Commands[intSelectedIndex]).OrderNum = ((UIDesignObjVariationCommand) objAG.Commands[intNextIndex]).OrderNum;
					((UIDesignObjVariationCommand) objAG.Commands[intNextIndex]).OrderNum = intOrderTemp;
				}


				UpdateCommandGroups();


				int intSelectIndex;
				
				if(intNextIndex == -1)
				{
					intSelectIndex = intSelectedIndex;
				}
				else
				{
					intSelectIndex = intNextIndex;
				}

			
				int intCount2 = -1;
				foreach(UIDesignObjVariationCommand obj in lbSelectedCommands.Items)
				{
					intCount2++;
					if(obj == objAG.Commands[intSelectIndex])
					{
						lbSelectedCommands.SelectedIndex = intCount2;
						break;
					}
				}
			

			}
		}

		private void btnOnActivateGoto_Click(object sender, System.EventArgs e)
		{

			int intGotoCommandID = CommandData.GOTO_SCREEN_CONST;

			int intGotoIndex = -1;

			int intCount = -1;
			//if there is no goto action, add it
			foreach(UIDesignObjVariationCommand objCommand in lbSelectedCommands.Items)
			{
				intCount++;
				if(objCommand.CommandID == intGotoCommandID)
				{
					intGotoIndex = intCount;
					break;
				}
			}

			if(intGotoIndex == -1)
			{
				//add it
				UIDesignObjVariationCommandGroup objAG = GetSelectedCommandGroup();
				if(objAG != null)
				{
					objAG.Commands.Add(new UIDesignObjVariationCommand(objAG, -1, intGotoCommandID));
					UpdateCommandGroups();
				}
				intGotoIndex = lbSelectedCommands.Items.Count - 1;
			}
			

			//select it
			lbSelectedCommands.SelectedIndex = intGotoIndex;
			

			//popup the objectchooser
			ChooseDesignObj frmChooseDesignObj = new ChooseDesignObj();
			frmChooseDesignObj.Owner = this;
			frmChooseDesignObj.ShowDialog();
			

		}

		public void ChooseGotoDesignObj(int intDesignObjID)
		{
			//we know that there is exactly one variation selected because this function is called from a modal window
			//that only opens when there is exactly on variation selected
			//UIDesignObjVariation objSelectedVariation = (UIDesignObjVariation) lbVariations.SelectedItems[0];
			//objSelectedVariation.GotoUIDesignObjID = intDesignObjID;
			//tbGotoDesignObj.Text = this.mds.tDesignObj[intDesignObjID].fDescription + " (" + intDesignObjID + ")";


			//this isn't elegant, but it works...

			//we know that the first textbox (third control) in the list of parameters is the object id, so populate it
			pnOnActivateParameters.Controls[2].Text = Convert.ToString(intDesignObjID);

			//add the value to the object model
			UIDesignObjVariationCommand objCommand = (UIDesignObjVariationCommand) lbSelectedCommands.SelectedItems[0];
			UIDesignObjVariationCommandParameter objParameter = (UIDesignObjVariationCommandParameter) objCommand.Parameters[0];
			objParameter.Value = Convert.ToString(intDesignObjID);
		}

		private void chCanBeHidden_CheckedChanged(object sender, System.EventArgs e)
		{
			if(this.selectedUICSL != null)
			{
				this.selectedUICSL.CanBeHidden = this.chCanBeHidden.Checked;
			}
		}

		private void chRegenerateForEachScreen_CheckedChanged(object sender, System.EventArgs e)
		{
			if(this.selectedUICSL != null)
			{
				this.selectedUICSL.RegenerateForEachScreen = this.chRegenerateForEachScreen.Checked;
			}
		}

		private void chChildBehindBG_CheckedChanged(object sender, System.EventArgs e)
		{
			if(this.selectedUICSL != null)
			{
				this.selectedUICSL.BGOnTop = this.chChildBehindBG.Checked;
			}
		}

		private void btnVariationControllerCriteria_Click(object sender, System.EventArgs e)
		{
		}

		private void chCantGoBack_CheckedChanged(object sender, System.EventArgs e)
		{
			m_objUIDesignObj.CantGoBack = chCantGoBack.Checked;
		}

		#region IsNumeric
		private bool IsNumeric(string strCheck)
		{
			try
			{
				int.Parse(strCheck);
				return true;
			}
			catch
			{
				return false;
			}
		}
		#endregion

		private void chHideByDefault_CheckedChanged(object sender, System.EventArgs e)
		{
			if( chHideByDefault.Checked )
				chCanBeHidden.Checked = true;
			if(this.selectedUICSL != null)
			{
				this.selectedUICSL.HideByDefault = this.chHideByDefault.Checked;
			}
		}

		private void btnBGColorTSL_Click(object sender, System.EventArgs e)
		{
			object o = ColorPicker.getColor();
			if (o != null)
			{
				if (this.selectedUITSL != null)
				{
					this.btnBGColorTSL.BackColor = (Color) o;
					// bool origBlock = this.BlockUpdateImage();
					this.selectedUITSL.BGColor = this.btnBGColorTSL.BackColor.ToArgb();
					// this.UpdateImage(origBlock);
				}
			}
		}

		private void cbBroadcast_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			if(lbSelectedCommands.SelectedItems.Count == 1)
			{
				UIDesignObjVariationCommand objA = (UIDesignObjVariationCommand) lbSelectedCommands.SelectedItems[0];
				StringPair objSelectedDevice = (StringPair) cbBroadcast.Items[cbBroadcast.SelectedIndex];
				objA.m_intBroadcast = Convert.ToInt32(objSelectedDevice.ID);
			}
		}

		private void tbMasterDeviceList_TextChanged(object sender, System.EventArgs e)
		{
			if(lbSelectedCommands.SelectedItems.Count == 1)
			{
				UIDesignObjVariationCommand objA = (UIDesignObjVariationCommand) lbSelectedCommands.SelectedItems[0];
				objA.m_intMasterDeviceList = (tbMasterDeviceList.Text=="" || tbMasterDeviceList.Text=="-" ? -1 : Convert.ToInt32(tbMasterDeviceList.Text));  // The user may start entering -201 for example
			}
		}

		private void tbDeviceCategory_TextChanged(object sender, System.EventArgs e)
		{
			if(lbSelectedCommands.SelectedItems.Count == 1)
			{
				UIDesignObjVariationCommand objA = (UIDesignObjVariationCommand) lbSelectedCommands.SelectedItems[0];
				objA.m_intDeviceCategory = (tbDeviceCategory.Text == "" ? -1 : Convert.ToInt32(tbDeviceCategory.Text));
			}
		}

		private void chRelToSender_TextChanged(object sender, System.EventArgs e)
		{
			if(lbSelectedCommands.SelectedItems.Count == 1)
			{
				UIDesignObjVariationCommand objA = (UIDesignObjVariationCommand) lbSelectedCommands.SelectedItems[0];
				objA.m_boolRelToSender = chRelToSender.Checked;
			}
		}

		private void chBGOnTop_CheckedChanged(object sender, System.EventArgs e)
		{
			if(this.selectedUICSL != null)
			{
				this.selectedUICSL.BGOnTop = this.chChildBehindBG.Checked;
			}
		}

		private void chChildBeforeText_CheckedChanged(object sender, System.EventArgs e)
		{
			if(this.selectedUICSL != null)
			{
				this.selectedUICSL.ChildBeforeText = this.chChildBeforeText.Checked;
			}
		}

		private void chDontMergeBG_CheckedChanged(object sender, System.EventArgs e)
		{
			if(this.selectedUICSL != null)
			{
				this.selectedUICSL.DontMergeBG = this.chDontMergeBG.Checked;
			}
		}

		private void cbIsTabStop_CheckedChanged(object sender, System.EventArgs e)
		{
			if(this.selectedUICSL != null)
			{
				this.selectedUICSL.IsTabStop = this.chIsTabStop.Checked;
			}
		}

		private void tbTS_Up_TextChanged(object sender, System.EventArgs e)
		{
			if(this.selectedUICSL != null)
			{
				this.selectedUICSL.TS_Up = this.tbTS_Up.Text;
			}
		}

		private void tbTS_Down_TextChanged(object sender, System.EventArgs e)
		{
			if(this.selectedUICSL != null)
			{
				this.selectedUICSL.TS_Down = this.tbTS_Down.Text;
			}
		}

		private void tbTS_Left_TextChanged(object sender, System.EventArgs e)
		{
			if(this.selectedUICSL != null)
			{
				this.selectedUICSL.TS_Left = this.tbTS_Left.Text;
			}
		}

		private void tbTS_Right_TextChanged(object sender, System.EventArgs e)
		{
			if(this.selectedUICSL != null)
			{
				this.selectedUICSL.TS_Right = this.tbTS_Right.Text;
			}
		}

		private void tbTiedTo_TextChanged(object sender, System.EventArgs e)
		{
			if(this.selectedUICSL != null)
			{
				this.selectedUICSL.TiedTo = this.tbTiedTo.Text;
			}
		}

		private void chDontResetSelectedState_CheckedChanged(object sender, System.EventArgs e)
		{
			if(this.selectedUIDesignObjVariation != null)
			{
				this.selectedUIDesignObjVariation.DontResetSelectedState = chDontResetSelectedState.Checked;
			}
		}

		private void tbVisibleStates_TextChanged(object sender, System.EventArgs e)
		{
			if(this.selectedUIDesignObj != null)
			{
				this.selectedUIDesignObj.VisibleStates = this.tbVisibleStates.Text;
			}
		
		}

		private void btnChildDown_Click(object sender, System.EventArgs e)
		{
			if(selectedUIDesignObj != null && selectedUIDesignObjVariation != null && lbChildrenDesignObjs.SelectedIndex<lbChildrenDesignObjs.Items.Count-1 )
			{
				UIDesignObj UIDesignObj_Next = (UIDesignObj) lbChildrenDesignObjs.Items[ lbChildrenDesignObjs.SelectedIndex+1 ];
				int i = UIDesignObj_Next.ParentDisplayOrder;
				UIDesignObj_Next.ParentDisplayOrder = selectedUIDesignObj.ParentDisplayOrder;
				selectedUIDesignObj.ParentDisplayOrder = i;
				lbChildrenDesignObjs.Items[ lbChildrenDesignObjs.SelectedIndex+1 ] = selectedUIDesignObj;
				lbChildrenDesignObjs.Items[ lbChildrenDesignObjs.SelectedIndex ] = UIDesignObj_Next;
				lbChildrenDesignObjs.SelectedIndex++;
			}
		}

		private void btnChildUp_Click(object sender, System.EventArgs e)
		{
			if(selectedUIDesignObj != null && selectedUIDesignObjVariation != null && lbChildrenDesignObjs.SelectedIndex>0 )
			{
				UIDesignObj UIDesignObj_Prior = (UIDesignObj) lbChildrenDesignObjs.Items[ lbChildrenDesignObjs.SelectedIndex-1 ];
				int i = UIDesignObj_Prior.ParentDisplayOrder;
				UIDesignObj_Prior.ParentDisplayOrder = selectedUIDesignObj.ParentDisplayOrder;
				selectedUIDesignObj.ParentDisplayOrder = i;
				lbChildrenDesignObjs.Items[ lbChildrenDesignObjs.SelectedIndex-1 ] = selectedUIDesignObj;
				lbChildrenDesignObjs.Items[ lbChildrenDesignObjs.SelectedIndex ] = UIDesignObj_Prior;
				lbChildrenDesignObjs.SelectedIndex--;
			}
		}

		private void cbEffectsHighlight_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			//get the selected variation
			if(lbVariations.SelectedItems.Count == 1)
			{
				UIDesignObjVariation objSelectedVariation = (UIDesignObjVariation) lbVariations.SelectedItems[0];
				if(cbEffectsHighlight.SelectedItem != null)
				{
					StringPair objPair = (StringPair) cbEffectsHighlight.SelectedItem;
					objSelectedVariation.CEffectHightlight = Convert.ToInt32(objPair.ID);
				}
			}
		}

		private void cbEffectsSelectChange_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			//get the selected variation
			if(lbVariations.SelectedItems.Count == 1)
			{
				UIDesignObjVariation objSelectedVariation = (UIDesignObjVariation) lbVariations.SelectedItems[0];
				if(cbEffectsSelectChange.SelectedItem != null)
				{
					StringPair objPair = (StringPair) cbEffectsSelectChange.SelectedItem;
					objSelectedVariation.CEffectWithChange = Convert.ToInt32(objPair.ID);
				}
			}
		}

		private void cbEffectsSelectNoChange_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			//get the selected variation
			if(lbVariations.SelectedItems.Count == 1)
			{
				UIDesignObjVariation objSelectedVariation = (UIDesignObjVariation) lbVariations.SelectedItems[0];
				if(cbEffectsSelectNoChange.SelectedItem != null)
				{
					StringPair objPair = (StringPair) cbEffectsSelectNoChange.SelectedItem;
					objSelectedVariation.CEffectNoChange = Convert.ToInt32(objPair.ID);
				}
			}
		}

		private void btnAddSkinLanguage_Child_Click_1(object sender, System.EventArgs e)
		{
			if (this.selectedUIDesignObj != null)
			{
				if ((new SkinLanguageForm(this.selectedUIDesignObj, GraphicsDirectory)).ShowDialog() == DialogResult.OK)
				{
					bool origBlock = this.BlockUpdateImage();

					this.loadAvailableLanguageSkins();
					this.loadAvailableLanguageSkins_Child();
						
					this.UpdateImage(origBlock);
				}
			}
		}

		private void btnRemoveSkinLanguage_Child_Click_1(object sender, System.EventArgs e)
		{
			if (this.selectedUICSL != null)
			{
				if (this.selectedUICSL.LanguageID == -1 && this.selectedUICSL.SkinID == -1)
				{
					MessageBox.Show("You cannot delete the default skin-language");
				}
				else
				{
					bool origBlock = this.BlockUpdateImage();

					if (this.selectedUICSL.ID != -1) this.selectedUICSL.NeedsDelete = true;
					else this.selectedUICSL.Deleted = true; // Delete a New Item

					this.loadAvailableLanguageSkins();
					this.loadAvailableLanguageSkins_Child();

					if(cbLanguageSkin_Child.Items.Count > 0)
						cbLanguageSkin_Child.SelectedIndex = 0;

					this.UpdateImage(origBlock);
				}
			}
		}

		private void cbLanguageSkin_Child_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			if (this.selectedUICSL != null) // Should never be null
			{
				SkinLanguageStatus.Instance().DesignObjLanguageID = selectedUICSL.LanguageID;
				SkinLanguageStatus.Instance().DesignObjSkinID = selectedUICSL.SkinID;

				this.chCanBeHidden.Checked = this.selectedUICSL.CanBeHidden;
				this.chHideByDefault.Checked = this.selectedUICSL.HideByDefault;
				this.chChildBehindBG.Checked = this.selectedUICSL.ChildBehindBG;
				this.tbTS_Up.Text = this.selectedUICSL.TS_Up;
				this.tbTS_Down.Text = this.selectedUICSL.TS_Down;
				this.tbTS_Right.Text = this.selectedUICSL.TS_Right;
				this.tbTS_Left.Text = this.selectedUICSL.TS_Left;
				this.tbTiedTo.Text = this.selectedUICSL.TiedTo;
				this.tbVisibleStates.Text = selectedUICSL.VisibleStates;
				this.chChildBeforeText.Checked = this.selectedUICSL.ChildBeforeText;
				this.chDontMergeBG.Checked = this.selectedUICSL.DontMergeBG;
				this.chIsTabStop.Checked = this.selectedUICSL.IsTabStop;
				this.chDontResetSelectedState.Checked = this.selectedUICSL.DontResetSelectedState;
				this.chRegenerateForEachScreen.Checked = this.selectedUICSL.RegenerateForEachScreen;
				tbX.Text = Convert.ToString(this.selectedUICSL.ParentX);
				tbY.Text = Convert.ToString(this.selectedUICSL.ParentY);
				tbWidth.Text = Convert.ToString(this.selectedUICSL.Width);
				tbHeight.Text = Convert.ToString(this.selectedUICSL.Height);

				if (!this.IsInterfaceLocked()) // TODO: REFRESH IMAGE WITH RECTANGLE AROUND SELECTION
				{
					this.LockInterface();
					m_objUIDesignObjDisplayControl.UpdateImage();
					m_objUIDesignObjDisplayControl.Refresh();
					this.UnlockInterface();
				}
			}
		}

		private void btnAddExplicitDesignObj_Click(object sender, EventArgs e)
		{
			AddExplicitDesignObj DesignObjDialog = new AddExplicitDesignObj();
			if (DesignObjDialog.ShowDialog() == DialogResult.OK)
			{
				Int32 PK_DesignObj, X, Y, Width, Height;
				bool IsTabStop;
				
				try { PK_DesignObj = Convert.ToInt32(DesignObjDialog.txtDesignObj.Text);}
					catch { PK_DesignObj = -1; }
				try { X = Convert.ToInt32(DesignObjDialog.txtX.Text); }
					catch { X = 0; }
				try { Y = Convert.ToInt32(DesignObjDialog.txtY.Text); }
					catch { Y = 0; }
				try { Width = Convert.ToInt32(DesignObjDialog.txtWidth.Text); }
					catch { Width = -1; }
				try { Height = Convert.ToInt32(DesignObjDialog.txtHeight.Text); }
					catch { Height = -1; }
				IsTabStop = DesignObjDialog.checkIsTabStop.Checked;
				
				if (PK_DesignObj > 0)
				{
					bool origBlock = this.BlockUpdateImage();
					
					AddNewChild(PK_DesignObj, X, Y, Width, Height, IsTabStop);
					
					UpdateChildrenDesignObjs();
					this.UpdateImage(origBlock);
				}
			}
		}

		private void btnAddExplicitText_Click(object sender, EventArgs e)
		{
			AddExplicitTextObj TextObjDialog = new AddExplicitTextObj();
			if (TextObjDialog.ShowDialog() == DialogResult.OK)
			{
				Int32 PK_TextObj;
				
				try { PK_TextObj = Convert.ToInt32(TextObjDialog.txtTextObj.Text);}
					catch { PK_TextObj = -1; }
				
				if (PK_TextObj > 0)
				{
					bool origBlock = this.BlockUpdateImage();
					
					AddNewText(PK_TextObj);
					
					UpdateChildrenDesignObjs();
					this.UpdateImage(origBlock);
				}
			}
		}

	}

	public class UIDesignObjDisplayControl : Control
	{
		private UIDesignObj m_UIDesignObj;
		private Bitmap m_objBitmap; // was public
		private int m_intZoom;
		private double m_dblScale;

		private DesignObjDesigner m_objParentForm;

		/// <summary>
		/// WARNING: This hides System.Windows.Control.Scale(float), TODO: Find all uses & rename
		/// </summary>
		public new double Scale
		{
			get	{return m_dblScale;}
		}

		public int CurrentLanguageForChild()
		{
			return m_objParentForm.LanguageID;
		}

		public int CurrentSkinForChild()
		{
			return m_objParentForm.SkinID;
		}

		public void ReleaseBitmap()
		{
			if (this.m_objBitmap != null) this.m_objBitmap.Dispose();
		}

		public UIDesignObjDisplayControl(Form objParentForm, UIDesignObj objUIDesignObj, int intZoom)
		{
			m_objParentForm = (DesignObjDesigner) objParentForm;
			m_UIDesignObj = objUIDesignObj;
			m_intZoom = intZoom;
			this.MouseDown  += new System.Windows.Forms.MouseEventHandler(this.HandleMouseDown);
		}

		public void Setup()
		{
			Zoom(m_intZoom);
			m_objParentForm.LockInterface();
			UpdateImage(false);
			m_objParentForm.UnlockInterface();
		}

		protected override void OnPaint(System.Windows.Forms.PaintEventArgs e)
		{
			if (m_objBitmap == null)
				return;
			e.Graphics.DrawImage(m_objBitmap, new Rectangle(0,0,this.Width,this.Height));
		}

		public void Zoom()
		{
			this.Zoom(m_intZoom);
		}

		public void Zoom(int intZoom)
		{
			if(!m_objParentForm.IsInterfaceLocked())
			{
				m_objParentForm.LockInterface();

				m_dblScale = Convert.ToDouble(intZoom)/100.0;

				int intNewWidth = 0;
				int intNewHeight = 0;
				if(null != m_objParentForm.FormUICSL)
				{
					intNewWidth = Convert.ToInt32(m_dblScale * Convert.ToDouble(m_objParentForm.FormUICSL.Width));
					intNewHeight = Convert.ToInt32(m_dblScale * Convert.ToDouble(m_objParentForm.FormUICSL.Height));
				}

				this.Width = intNewWidth;
				this.Height = intNewHeight;
				m_intZoom = intZoom;

				this.Refresh();

				m_objParentForm.UnlockInterface();
			}

		}


		private void HandleMouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if(!m_objParentForm.IsInterfaceLocked())
			{
				m_objParentForm.LockInterface();
				int intScaledX = Convert.ToInt32(Convert.ToDouble(e.X) / m_dblScale);
				int intScaledY = Convert.ToInt32(Convert.ToDouble(e.Y) / m_dblScale);

				if(m_UIDesignObj.ClickImage(intScaledX, intScaledY, this.m_objParentForm.LanguageID,this.m_objParentForm.SkinID))
				{
					bool origBlock = this.m_objParentForm.BlockUpdateImage();
					m_objParentForm.SelectDesignObj(m_UIDesignObj.SelectedDesignObj);
					this.m_objParentForm.UnBlockUpdateImage(origBlock);
					this.UpdateImage();
				}
				m_objParentForm.UnlockInterface();
			}
		}

		private void HandleMouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{

		}

		private void HandleMouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
		{

		}

		public void UpdateImage()
		{
			this.UpdateImage(true);
		}

		public void UpdateImage(bool refresh)
		{
			if(m_objBitmap != null) m_objBitmap.Dispose();

			if(null != m_objParentForm.FormUICSL && m_objParentForm.FormUICSL.Width > 0 && m_objParentForm.FormUICSL.Height > 0)
			{
				m_objBitmap = new Bitmap(m_objParentForm.FormUICSL.Width, m_objParentForm.FormUICSL.Height);
				using (Graphics objGraphics = Graphics.FromImage(m_objBitmap))
				{
					objGraphics.Clear(Color.White);

					SkinLanguageStatus.Instance().GlobalLanguageID = this.m_objParentForm.LanguageID;
					SkinLanguageStatus.Instance().GlobalSkinID = this.m_objParentForm.SkinID;
					m_UIDesignObj.Draw(objGraphics,this.m_objParentForm.LanguageID,this.m_objParentForm.SkinID);
				}
			}
			if (refresh) this.Refresh();
		}
	}


	public class MenuItemID: MenuItem
	{
		public int ID;

		public MenuItemID()
		{
		}

		public MenuItemID(int id, string description)
		{
			this.ID = id;
			this.Text = description;
		}
	}


	public class DragUI
	{
		public DesignObjDesigner objectDesigner;		
		public DragUI(DesignObjDesigner od)
		{
			this.objectDesigner = od;
		}
	}

	public class DragUIDesignObj: DragUI
	{
		public UIDesignObj uiDesignObj;
		public DragUIDesignObj(DesignObjDesigner od, UIDesignObj uio):base(od)
		{
			this.uiDesignObj = uio;
		}
	}
	public class DragUIText: DragUI
	{
		public UIText uiText;
		public DragUIText(DesignObjDesigner od, UIText uit):base(od)
		{
			this.uiText = uit;
		}
	}
}
