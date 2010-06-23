using System;
using System.Collections;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using HAData.Common;
using HAData.DataAccess;
//using Criteria;
using System.Data;

using System.IO;


namespace HADesigner
{
	/// <summary>
	/// Summary description for UIDesignObjVariation.
	/// </summary>
	public class UIDesignObjVariation
	{
		private const int emptyBmpWidth = 100;
		private const int emptyBmpHeight = 100;
		private const string strFileNotFoundBmp = "FileNotFound.bmp";

		public string GraphicsDirectory
		{
			get {return this.m_objParentUIDesignObj.GraphicsDirectory;}
			set {this.m_objParentUIDesignObj.GraphicsDirectory = value;}
		}
		private string SkinDirectory(int SkinID)
		{
			string skinDir = (mds.tSkin[SkinID].bIsValid) ? mds.tSkin[SkinID].fDataSubdirectory : mds.tSkin[1].fDataSubdirectory;
			return this.GraphicsDirectory + "\\" + skinDir;
		}
		private string pathFileNotFound
		{
			get {return this.GraphicsDirectory + "\\" + strFileNotFoundBmp;}
		}

		private MyDataSet mds
		{
			get {return HADataConfiguration.m_mdsCache;}
		}

		//MEMBER VARIABLES
		private int m_intID = -1;
		private string m_strDescription;
		//private CriteriaList m_CriteriaList;
		private UIDesignObj m_objParentUIDesignObj;

		private int m_intUIID = -1;
		private int m_intGotoUIDesignObjID = -1;
		//		private int m_intCommandID = -1;
		//		private int m_intCommandGroupID = -1;
		private int 
			m_intCommandGroup_D_TouchID = -1, m_intCommandGroup_D_LoadID = -1,m_intCommandGroup_D_UnloadID = -1, 
			m_intCommandGroup_D_StartupID = -1, m_intCommandGroup_D_HighlightID = -1, m_intCommandGroup_D_UnhighlightID = -1,
			m_intCommandGroup_D_TimeoutID = -1;
		private int m_intCButton = -1;
		public int m_iLastDisplayOrder = -1;
		private bool m_boolDontResetSelectedState = false;

		private int m_intCEffectSelectNoChange = -1;
		private int m_intCEffectSelectWithChange = -1;
		private int m_intCEffectHighlight = -1;

		private bool m_blnSelected = false;

		private bool m_blnIsStandard = false;


		private Bitmap m_objBitmap;


		//things that can change
		private int m_intUIIDOriginal = -1;
		private int m_intGotoUIDesignObjIDOriginal  = -1;
		//		private int m_intCommandIDOriginal = -1;
		private int 
			m_intCommandGroup_D_TouchIDOriginal  = -1,	m_intCommandGroup_D_LoadIDOriginal  = -1,
			m_intCommandGroup_D_UnloadIDOriginal  = -1, m_intCommandGroup_D_StartupIDOriginal = -1, 
			m_intCommandGroup_D_HighlightIDOriginal = -1, m_intCommandGroup_D_UnhighlightIDOriginal = -1,
			m_intCommandGroup_D_TimeoutIDOriginal = -1;
		private int m_intCButtonOriginal = -1;
		private bool m_boolDontResetSelectedStateOriginal = false;

		private int m_intCEffectSelectNoChangeOriginal = -1;
		private int m_intCEffectSelectWithChangeOriginal = -1;
		private int m_intCEffectHighlightOriginal = -1;


		private UIDesignObjVariationCommandGroup m_objCommandGroup_D_Touch;
		private UIDesignObjVariationCommandGroup m_objCommandGroup_D_Load;
		private UIDesignObjVariationCommandGroup m_objCommandGroup_D_Unload;
		private UIDesignObjVariationCommandGroup m_objCommandGroup_D_Startup;
		private UIDesignObjVariationCommandGroup m_objCommandGroup_D_Highlight;
		private UIDesignObjVariationCommandGroup m_objCommandGroup_D_Unhighlight;
		private UIDesignObjVariationCommandGroup m_objCommandGroup_D_Timeout;


		//these are for images.
		//x and y are 0 by default...  width and height are the bitmap size by default
		//overrides come from the parameters
		private int m_intWidth;
		private int m_intHeight;
		private int m_intX;
		private int m_intY;

		private int m_intTotalWidth;
		private int m_intTotalHeight;




		private bool m_blnNeedsDBInsert = false;
		private bool m_blnNeedsDBDelete = false;
		private bool m_blnDeleted = false;			//this becomes true AFTER the delete


		private bool m_blnShow = true;


		private ArrayList m_alDesignObjs = new ArrayList();
		/*
		public ArrayList ChildDesignObjs
		{
			get {return this.m_alDesignObjs;}
		}
		*/
		private ArrayList m_alParameters = new ArrayList();
		private ArrayList m_alCommandParameters = new ArrayList();
		private ArrayList m_alText = new ArrayList();



		private bool blnBuiltOnce = false;
		private bool blnIsRendered = false;


		//PROPERTIES
		public bool WarnToSave
		{
			get 
			{
				if (NeedsInsert||NeedsDelete||OriginalsChanged) return true;
				//save all parameters to database
				foreach(UIDesignObjVariationParameter uiOVP in this.Parameters)
				{
					if (uiOVP.WarnToSave) return true;
				}
				/*
				foreach(object uiOVAP in this.CommandParameters)
				{
					if (((UIDesignObjVariationCommandParameter)uiOVAP).WarnToSave) return true;
				}
				*/
				foreach(UIDesignObj uiO in this.DesignObjs)
				{
					if (uiO.WarnToSave) return true;
				}
				foreach(UIText uiT in this.Text)
				{
					if (uiT.WarnToSave) return true;
				}

				if(this.ParentUIDesignObj.ParentUIDesignObjVariation == null)
				{
					if(this.CommandGroup_D_Load.WarnToSave) return true;
					if(this.CommandGroup_D_Touch.WarnToSave) return true;
					if(this.CommandGroup_D_Unload.WarnToSave) return true;
					if(this.CommandGroup_D_Startup.WarnToSave) return true;
					if(this.CommandGroup_D_Highlight.WarnToSave) return true;
					if(this.CommandGroup_D_Unhighlight.WarnToSave) return true;
					if(this.CommandGroup_D_Timeout.WarnToSave) return true;
				}
				return false;
			}
		}


		public UIDesignObjVariationCommandGroup CommandGroup_D_Touch
		{
			get{ return m_objCommandGroup_D_Touch; }
			set{ m_objCommandGroup_D_Touch = value; }
		}

		public UIDesignObjVariationCommandGroup CommandGroup_D_Load
		{
			get{ return m_objCommandGroup_D_Load; }
			set{ m_objCommandGroup_D_Load = value; }
		}

		public UIDesignObjVariationCommandGroup CommandGroup_D_Unload
		{
			get{ return m_objCommandGroup_D_Unload; }
			set{ m_objCommandGroup_D_Unload = value; }
		}

		public UIDesignObjVariationCommandGroup CommandGroup_D_Startup
		{
			get{ return m_objCommandGroup_D_Startup; }
			set{ m_objCommandGroup_D_Startup = value; }
		}

		public UIDesignObjVariationCommandGroup CommandGroup_D_Highlight
		{
			get{ return m_objCommandGroup_D_Highlight; }
			set{ m_objCommandGroup_D_Highlight = value; }
		}

		public UIDesignObjVariationCommandGroup CommandGroup_D_Unhighlight
		{
			get{ return m_objCommandGroup_D_Unhighlight; }
			set{ m_objCommandGroup_D_Unhighlight = value; }
		}

		public UIDesignObjVariationCommandGroup CommandGroup_D_Timeout
		{
			get{ return m_objCommandGroup_D_Timeout; }
			set{ m_objCommandGroup_D_Timeout = value; }
		}


		public int ID
		{
			get	{return m_intID;}
			set	{m_intID = value;}
		}
		public string Description
		{
			get {return ID + " " + m_strDescription;}
			set	{m_strDescription = value;}
		}


		public UIDesignObj ParentUIDesignObj
		{
			get	{return m_objParentUIDesignObj;}
			set	{m_objParentUIDesignObj = value;}
		}



		public Bitmap Bitmap
		{
			get	{return m_objBitmap;}
			set	{m_objBitmap = value;}
		}

		public int CriteriaID
		{
			get	{return m_intUIID;}
			set {m_intUIID = value;}
		}
		public int GotoUIDesignObjID
		{
			get {return m_intGotoUIDesignObjID;}
			set	{m_intGotoUIDesignObjID = value;}
		}
		public int CommandGroup_D_TouchID
		{
			get	{return m_intCommandGroup_D_TouchID;}
			set	{m_intCommandGroup_D_TouchID = value;}
		}
		public int CommandGroup_D_LoadID
		{
			get	{return m_intCommandGroup_D_LoadID;}
			set	{m_intCommandGroup_D_LoadID = value;}
		}
		public int CommandGroup_D_UnloadID
		{
			get	{return m_intCommandGroup_D_UnloadID;}
			set	{m_intCommandGroup_D_UnloadID = value;}
		}
		public int CommandGroup_D_StartupID
		{
			get	{return m_intCommandGroup_D_StartupID;}
			set	{m_intCommandGroup_D_StartupID = value;}
		}
		public int CommandGroup_D_HighlightID
		{
			get	{return m_intCommandGroup_D_HighlightID;}
			set	{m_intCommandGroup_D_HighlightID = value;}
		}
		public int CommandGroup_D_UnhighlightID
		{
			get	{return m_intCommandGroup_D_UnhighlightID;}
			set	{m_intCommandGroup_D_UnhighlightID = value;}
		}
		public int CommandGroup_D_TimeoutID
		{
			get	{return m_intCommandGroup_D_TimeoutID;}
			set	{m_intCommandGroup_D_TimeoutID = value;}
		}
		public int CButton
		{
			get {return m_intCButton;}
			set {m_intCButton = value;}
		}
		public int CEffectNoChange
		{
			get {return m_intCEffectSelectNoChange;}
			set {m_intCEffectSelectNoChange = value;}
		}
		public int CEffectWithChange
		{
			get {return m_intCEffectSelectWithChange;}
			set {m_intCEffectSelectWithChange = value;}
		}
		public int CEffectHightlight
		{
			get {return m_intCEffectHighlight;}
			set {m_intCEffectHighlight = value;}
		}		
		public bool DontResetSelectedState
		{
			get {return m_boolDontResetSelectedState;}
			set {m_boolDontResetSelectedState = value;}
		}
		public bool Selected
		{
			get	{return m_blnSelected;}
			set	{m_blnSelected = value;}
		}


		public bool IsStandard
		{
			get {return m_blnIsStandard;}
			set	{m_blnIsStandard = value;}
		}

		public int Width
		{
			get	{return m_intWidth;}
			set	{m_intWidth = value;}
		}

		public int Height
		{
			get	{return m_intHeight;}
			set {m_intHeight = value;}
		}
		public int X
		{
			get {return m_intX;}
			set {m_intX = value;}
		}

		public int Y
		{
			get	{return m_intY;}
			set	{m_intY = value;}
		}


		public bool OriginalsChanged
		{
			get
			{
				return 
					m_intUIIDOriginal != m_intUIID || 
					m_intCommandGroup_D_TouchIDOriginal != m_intCommandGroup_D_TouchID || 
					m_intCommandGroup_D_LoadIDOriginal != m_intCommandGroup_D_LoadID || 
					m_intCommandGroup_D_UnloadIDOriginal != m_intCommandGroup_D_UnloadID || 
					m_intCommandGroup_D_StartupID != m_intCommandGroup_D_StartupIDOriginal || 
					m_intCommandGroup_D_HighlightID != m_intCommandGroup_D_HighlightIDOriginal || 
					m_intCommandGroup_D_UnhighlightID != m_intCommandGroup_D_UnhighlightIDOriginal || 
					m_intCommandGroup_D_TimeoutID != m_intCommandGroup_D_TimeoutIDOriginal || 
					m_intCButton != m_intCButtonOriginal || 
					m_intGotoUIDesignObjIDOriginal != m_intGotoUIDesignObjID || 
					m_boolDontResetSelectedState!=m_boolDontResetSelectedStateOriginal ||
					m_intCEffectSelectNoChange != m_intCEffectSelectNoChangeOriginal ||
					m_intCEffectSelectWithChange != m_intCEffectSelectWithChangeOriginal ||
					m_intCEffectHighlight != m_intCEffectHighlightOriginal;					
			}
		}


		public bool NeedsInsert
		{	
			get	{return m_blnNeedsDBInsert;}
			set	{m_blnNeedsDBInsert = value;}
		}
		public bool NeedsDelete
		{
			get	{return m_blnNeedsDBDelete;}
			set	{m_blnNeedsDBDelete = value;}
		}
		public bool Deleted
		{
			get {return m_blnDeleted;}
		}

	
		public bool Show
		{
			get	{return m_blnShow;}
			set	{m_blnShow = value;}
		}
		public bool Include
		{
			get	{return(!this.Deleted && !this.NeedsDelete);}
		}


		public int TotalWidth
		{
			get	{return m_intTotalWidth;}
			set	{m_intTotalWidth = value;}
		}
		public int TotalHeight
		{
			get	{return m_intTotalHeight;}
			set	{m_intTotalHeight = value;}
		}

		public ArrayList DesignObjs
		{
			get	{return m_alDesignObjs;}
			set	{m_alDesignObjs = value;}
		}
		public ArrayList Parameters
		{
			get	{return m_alParameters;}
			set {m_alParameters = value;}
		}
		public ArrayList CommandParameters
		{
			get {return m_alCommandParameters;}
			set {m_alCommandParameters = value;}
		}

		public ArrayList Text
		{
			get {return m_alText;}
			set {m_alText = value;}
		}



		//CONSTRUCTORS
		public UIDesignObjVariation(UIDesignObj objParentUIDesignObj, int intID, int intUIID)
		{
			this.ParentUIDesignObj = objParentUIDesignObj;
			if(intID == -1)
			{
				
				//this one doesn't exist in the database yet
				this.NeedsInsert = true;
				this.ID = -1;
				this.CriteriaID = intUIID;

				if(intUIID != -1)
				{

					this.IsStandard = false;

					//get the description of the criteria
					UIDataRow drCriteria = mds.tUI[this.CriteriaID];
					this.Description = Convert.ToString(drCriteria.fPK_UI) + " " + drCriteria.fDescription;

					//we need to create parameters for this Variation
					//create the parms based on the parent object type and look in DesignObjType_DesignObjParameter
					//default the values to the values of the standard
					DataRow[] drCParms = mds.tDesignObjType_DesignObjParameter.Select(DesignObjType_DesignObjParameterData.FK_DESIGNOBJTYPE_FIELD + "=" + this.ParentUIDesignObj.UIDesignObjType, null);
					UIDesignObjVariation objStandardVariation = (UIDesignObjVariation) this.ParentUIDesignObj.UIDesignObjVariations[0];
					foreach(DataRow dr in drCParms)
					{
						DesignObjType_DesignObjParameterDataRow drCParm = new DesignObjType_DesignObjParameterDataRow(dr);
						
						int intIDTemp = drCParm.fFK_DesignObjParameter;

						//get the default values from the standard variation
						
						string strValueTemp = objStandardVariation.GetParameterValue(intIDTemp);
						int intVariableTemp = objStandardVariation.GetParameterVariable(intIDTemp);
						this.AddNewParameter(intIDTemp, strValueTemp, intVariableTemp);
					}
					
				}
				else
				{
					this.IsStandard = true;
					this.Description = "Standard Variation";
				}

			}
			else
			{
				this.ID = intID;
				LoadFromDatabase();
			}



			//put in blank action groups if there is none existing
			if(this.ParentUIDesignObj.ParentUIDesignObjVariation == null)
			{
				if(this.CommandGroup_D_Load == null)
				{
					this.CommandGroup_D_Load = new UIDesignObjVariationCommandGroup(this, -1);
				}

				if(this.CommandGroup_D_Touch == null)
				{
					this.CommandGroup_D_Touch = new UIDesignObjVariationCommandGroup(this, -1);
				}

				if(this.CommandGroup_D_Unload == null)
				{
					this.CommandGroup_D_Unload = new UIDesignObjVariationCommandGroup(this, -1);
				}
				if(this.CommandGroup_D_Startup == null)
				{
					this.CommandGroup_D_Startup = new UIDesignObjVariationCommandGroup(this, -1);
				}
				if(this.CommandGroup_D_Highlight == null)
				{
					this.CommandGroup_D_Highlight = new UIDesignObjVariationCommandGroup(this, -1);
				}
				if(this.CommandGroup_D_Unhighlight == null)
				{
					this.CommandGroup_D_Unhighlight = new UIDesignObjVariationCommandGroup(this, -1);
				}
				if(this.CommandGroup_D_Timeout == null)
				{
					this.CommandGroup_D_Timeout = new UIDesignObjVariationCommandGroup(this, -1);
				}
			}



		}

		
		// DESTRUCTOR LIKE

		public void ReleaseBitmaps()
		{
			if (this.m_objBitmap != null) this.m_objBitmap.Dispose();
			foreach (UIDesignObj uio in this.m_alDesignObjs)
			{
				uio.ReleaseBitmaps();
			}
		}
	



		//METHODS
		public void AddNewParameter(int intCParameterID, string strValue, int intVariableID)
		{
			UIDesignObjVariationParameter objParm = new UIDesignObjVariationParameter(this, intCParameterID);
			objParm.Description = mds.tDesignObjParameter[intCParameterID].fDescription;
			objParm.Value = strValue;
			objParm.VariableID = intVariableID;
			objParm.NeedsInsert = true;
			this.Parameters.Add(objParm);
		}


		public void Draw(UIChildSkinLanguage objChildSkinLanguage, Graphics objGraphics, int languageID, int skinID)
		{

			//determine how to show.  this will depend on what variation is selected and how they want things
			//they may want to hide non selected vriations, maybe outlined or ghosted
			if(this.ParentUIDesignObj.ParentUIDesignObjVariation == null)
			{
				this.Show = this.Selected; //this is a top level variation, so only show the selected ones
			}

			if(this.Include && this.Show)
			{
				string strTemp;

				if(this.ParentUIDesignObj.UIDesignObjType == DesignObjTypeData.BITMAP_CONST)
				{
					bool blnDraw = true;

					//if this is a top level variation, and one backround has been drawn, don't draw this one
					if(this.ParentUIDesignObj.ParentUIDesignObjVariation == null)
					{
						if(this.ParentUIDesignObj.MainBackgroundDrawn) blnDraw = false;
						this.ParentUIDesignObj.MainBackgroundDrawn = true;
					}

					if(blnDraw)
					{
						int intX = objChildSkinLanguage.RootX + objChildSkinLanguage.ParentX;
						int intY = objChildSkinLanguage.RootY + objChildSkinLanguage.ParentY;

						int intWidth = this.Bitmap.Width;
						int intHeight = this.Bitmap.Height;

						//get the offsets
						strTemp = this.GetParameterValue(DesignObjParameterData.XPOS_CONST);
						if(strTemp != "") intX += Convert.ToInt32(strTemp);
						strTemp = this.GetParameterValue(DesignObjParameterData.YPOS_CONST);
						if(strTemp != "") intY += Convert.ToInt32(strTemp);
			
						//scale values
						objGraphics.DrawImage(this.Bitmap, intX, intY, intWidth, intHeight);
					}
				}

				foreach(object obj in this.DesignObjs)
				{
					UIDesignObj objDesignObj = (UIDesignObj) obj;
					objDesignObj.Draw(objGraphics,languageID,skinID);
				}
				foreach(object obj in this.Text)
				{
					UIText objText = (UIText) obj;
					objText.Draw(objGraphics,objChildSkinLanguage.ParentX + objChildSkinLanguage.RootX,
						 objChildSkinLanguage.RootY + objChildSkinLanguage.ParentY);
				}
			}
		}

		public UIDesignObjVariationParameter GetParameter(int intCParameterID)
		{
			UIDesignObjVariationParameter objReturn = null;
			foreach(Object obj in m_alParameters)
			{
				UIDesignObjVariationParameter objParameter = (UIDesignObjVariationParameter) obj;
				if(objParameter.CParameterID == intCParameterID)
				{
					objReturn = objParameter;
				}
			}
			return objReturn;
		}


		public string GetParameterValue(int intCParameterID)
		{
			UIDesignObjVariationParameter objReturn = this.GetParameter(intCParameterID);
			if(objReturn == null)
			{
				return "";
			}
			else
			{
				return objReturn.Value;
			}
		}

		public int GetParameterVariable(int intCParameterID)
		{
			UIDesignObjVariationParameter objReturn = this.GetParameter(intCParameterID);
			if(objReturn == null)
			{
				return -1;
			}
			else
			{
				return objReturn.VariableID;
			}
		}



		public void LoadFromDatabase()
		{
			DesignObjVariationDataRow drVariation = mds.tDesignObjVariation[m_intID];


			//Get the criteria and description
			if(!drVariation.fFK_UIIsNull)
			{
				this.IsStandard = false;
				m_intUIID = drVariation.fFK_UI;
				m_strDescription = drVariation.fFK_UI_DataRow.fDescription + "  (" + Convert.ToString(drVariation.fFK_UI_DataRow.fPK_UI) + ")";
			}
			else
			{
				this.IsStandard = true;
				m_strDescription = "Standard Variation";
			}

// sean			m_intCommandGroupID = (drVariation.fFK_CommandGroup_OnActivateIsNull) ? -1 : drVariation.fFK_CommandGroup_OnActivate;
			m_intCButton = (drVariation.fFK_ButtonIsNull) ? -1 : drVariation.fFK_Button;
			m_intCEffectSelectNoChange = 
				(drVariation.fFK_EffectType_Selected_NoChangeIsNull) ? -1 : drVariation.fFK_EffectType_Selected_NoChange;
			m_intCEffectSelectWithChange = 
				(drVariation.fFK_EffectType_Selected_WithChangeIsNull) ? -1 : drVariation.fFK_EffectType_Selected_WithChange;
			m_intCEffectHighlight = 
				(drVariation.fFK_EffectType_HighlightedIsNull) ? -1 : drVariation.fFK_EffectType_Highlighted;

			m_boolDontResetSelectedState = (drVariation.fDontResetSelectedState==1);
// SEAN			m_intCommandID = (drVariation.fFK_CommandIsNull) ? -1 : drVariation.fFK_Command;
			m_intGotoUIDesignObjID = (drVariation.fFK_DesignObj_GotoIsNull) ? -1 : drVariation.fFK_DesignObj_Goto;
			
			
			this.CommandGroup_D_TouchID = (drVariation.fFK_CommandGroup_D_OnActivateIsNull) ? -1 : drVariation.fFK_CommandGroup_D_OnActivate;
			this.CommandGroup_D_LoadID = (drVariation.fFK_CommandGroup_D_OnLoadIsNull) ? -1 : drVariation.fFK_CommandGroup_D_OnLoad;
			this.CommandGroup_D_UnloadID = (drVariation.fFK_CommandGroup_D_OnUnloadIsNull) ? -1 : drVariation.fFK_CommandGroup_D_OnUnload;
			this.CommandGroup_D_StartupID = (drVariation.fFK_CommandGroup_D_OnStartupIsNull) ? -1 : drVariation.fFK_CommandGroup_D_OnStartup;
			this.CommandGroup_D_HighlightID = (drVariation.fFK_CommandGroup_D_OnHighlightIsNull) ? -1 : drVariation.fFK_CommandGroup_D_OnHighlight;
			this.CommandGroup_D_UnhighlightID = (drVariation.fFK_CommandGroup_D_OnUnhighlightIsNull) ? -1 : drVariation.fFK_CommandGroup_D_OnUnhighlight;
			this.CommandGroup_D_TimeoutID = (drVariation.fFK_CommandGroup_D_OnTimeoutIsNull) ? -1 : drVariation.fFK_CommandGroup_D_OnTimeout;
			
			//Set the originals
			this.ResetOriginals();


			m_blnNeedsDBInsert = false;
			m_blnNeedsDBDelete = false;




			//get the parameters
			DataRow[] drParameters = mds.tDesignObjVariation_DesignObjParameter.Select(DesignObjVariation_DesignObjParameterData.FK_DESIGNOBJVARIATION_FIELD + "=" + m_intID, null);
			foreach(DataRow dr in drParameters)
			{
				DesignObjVariation_DesignObjParameterDataRow drParameter = new DesignObjVariation_DesignObjParameterDataRow(dr);
				m_alParameters.Add(new UIDesignObjVariationParameter(this, drParameter.fFK_DesignObjParameter));
			}


/* SEAN
			//get the action parameters
			DataRow[] drCommandParameters = mds.tDesignObjVariation_CommandParameter.Select(DesignObjVariation_CommandParameterData.FK_DESIGNOBJVARIATION_FIELD + "=" + m_intID, null);
			foreach(DataRow dr in drCommandParameters)
			{
				DesignObjVariation_CommandParameterDataRow drCommandParameter = new DesignObjVariation_CommandParameterDataRow(dr);
				m_alCommandParameters.Add(new UIDesignObjVariationCommandParameter(this, drCommandParameter.fFK_CommandParameter, true));
			}
			
*/


			//only load the action groups for the top level variations
			if(this.ParentUIDesignObj.ParentUIDesignObjVariation == null)
			{
				if(this.CommandGroup_D_LoadID != -1)
				{
					this.CommandGroup_D_Load = new UIDesignObjVariationCommandGroup(this, this.CommandGroup_D_LoadID);
				}

				if(this.CommandGroup_D_UnloadID != -1)
				{
					this.CommandGroup_D_Unload = new UIDesignObjVariationCommandGroup(this, this.CommandGroup_D_UnloadID);
				}

				if(this.CommandGroup_D_StartupID != -1)
				{
					this.CommandGroup_D_Startup = new UIDesignObjVariationCommandGroup(this, this.CommandGroup_D_StartupID);
				}

				if(this.CommandGroup_D_HighlightID != -1)
				{
					this.CommandGroup_D_Highlight = new UIDesignObjVariationCommandGroup(this, this.CommandGroup_D_HighlightID);
				}

				if(this.CommandGroup_D_UnhighlightID != -1)
				{
					this.CommandGroup_D_Unhighlight = new UIDesignObjVariationCommandGroup(this, this.CommandGroup_D_UnhighlightID);
				}

				if(this.CommandGroup_D_TimeoutID != -1)
				{
					this.CommandGroup_D_Timeout = new UIDesignObjVariationCommandGroup(this, this.CommandGroup_D_TimeoutID);
				}

				if(this.CommandGroup_D_TouchID != -1)
				{
					this.CommandGroup_D_Touch = new UIDesignObjVariationCommandGroup(this, this.CommandGroup_D_TouchID);
				}
			}

			//get the objects
			m_iLastDisplayOrder=0;  // Be sure each one has a unique display order

			DataRow[] drDesignObjs = mds.tDesignObjVariation_DesignObj.Select(
				DesignObjVariation_DesignObjData.FK_DESIGNOBJVARIATION_PARENT_FIELD + "=" + m_intID, 
				DesignObjVariation_DesignObjData.PK_DESIGNOBJVARIATION_DESIGNOBJ_FIELD);
			foreach(DataRow dr in drDesignObjs)
			{
				DesignObjVariation_DesignObjDataRow drDesignObj = new DesignObjVariation_DesignObjDataRow(dr);

				UIDesignObj objChildUIDesignObj = new UIDesignObj(this, drDesignObj.fPK_DesignObjVariation_DesignObj, 
					drDesignObj.fFK_DesignObj_Child, this.GraphicsDirectory);

				m_alDesignObjs.Add(objChildUIDesignObj);
			}

			// get the text
            DataRow[] drTexts = drVariation.dr.GetChildRows(MyDataSet.MyRelations.DesignObjVariation_Text_FK_DesignObjVariation);
			foreach(DataRow dr in drTexts)
			{
				DesignObjVariation_TextDataRow drOVT = new DesignObjVariation_TextDataRow(dr);

				UIText uitext = new UIText(this, drOVT.fPK_DesignObjVariation_Text, drOVT.fFK_Text);

				m_alText.Add(uitext); 
			}
		}


		public bool SaveToDatabase()
		{
			//returns whether anything has been saved to the database
			bool blnChanged = false;

			if(!this.Deleted)
			{

				if(this.NeedsDelete)
				{


					//delete from 
					//DesignObjVariation_Text
					foreach(DataRow dr in mds.tDesignObjVariation_Text.Select(DesignObjVariation_TextData.FK_DESIGNOBJVARIATION_FIELD + "=" + this.ID,null))
					{
						DesignObjVariation_TextDataRow objText = new DesignObjVariation_TextDataRow(dr);

						//DesignObjVariation_Text_Skin_Language
						foreach(DataRow dr2 in mds.tDesignObjVariation_Text_Skin_Language.Select(DesignObjVariation_Text_Skin_LanguageData.FK_DESIGNOBJVARIATION_TEXT_FIELD + "=" + objText.fPK_DesignObjVariation_Text,null))
						{
							dr2.Delete();
						}
						mds.tDesignObjVariation_Text_Skin_Language.Update(1,mds.m_conn,mds.m_trans);

						dr.Delete();
					}
					mds.tDesignObjVariation_Text.Update(1,mds.m_conn,mds.m_trans);


					//DesignObjVariation_Zone
					foreach(DataRow dr in mds.tDesignObjVariation_Zone.Select(DesignObjVariation_ZoneData.FK_DESIGNOBJVARIATION_FIELD + "=" + this.ID,null))
					{
						dr.Delete();
					}
					mds.tDesignObjVariation_Zone.Update(1,mds.m_conn,mds.m_trans);
					


					//delete all the paramaters from DesignObjVariation_DesignObjParameter
					foreach(Object obj in this.Parameters)
					{
						UIDesignObjVariationParameter objVariationParameter = (UIDesignObjVariationParameter) obj;
						objVariationParameter.NeedsDelete = true;
						objVariationParameter.SaveToDatabase();
					}
					
					
					//delete all the actionparamaters from DesignObjVariation_CommandParameter
					foreach(Object obj in this.CommandParameters)
					{
						UIDesignObjVariationCommandParameter objVariationCommandParameter = (UIDesignObjVariationCommandParameter) obj;
						objVariationCommandParameter.NeedsDelete = true;
						objVariationCommandParameter.SaveToDatabase();
					}
					


					//mark for UNLINK all children objects (NOT DELETE)
					foreach(Object obj in this.DesignObjs)
					{
						UIDesignObj objUIDesignObj = (UIDesignObj) obj;
						objUIDesignObj.NeedsParentVariationUnlink = true;
						objUIDesignObj.SaveToDatabase();
					}


					//delete this variation
					if(!this.NeedsInsert)
					{
						DesignObjVariationDataRow drDesignObjVariation = mds.tDesignObjVariation[m_intID];
						drDesignObjVariation.dr.Delete();
						mds.tDesignObjVariation.Update(1,mds.m_conn,mds.m_trans);
					}



					if(this.ParentUIDesignObj.ParentUIDesignObjVariation == null)
					{
						//delete the action groups
						this.CommandGroup_D_Load.NeedsDelete = true;
						this.CommandGroup_D_Load.SaveToDatabase();

						this.CommandGroup_D_Touch.NeedsDelete = true;
						this.CommandGroup_D_Touch.SaveToDatabase();

						this.CommandGroup_D_Unload.NeedsDelete = true;
						this.CommandGroup_D_Unload.SaveToDatabase();

						this.CommandGroup_D_Startup.NeedsDelete = true;
						this.CommandGroup_D_Startup.SaveToDatabase();

						this.CommandGroup_D_Highlight.NeedsDelete = true;
						this.CommandGroup_D_Highlight.SaveToDatabase();

						this.CommandGroup_D_Unhighlight.NeedsDelete = true;
						this.CommandGroup_D_Unhighlight.SaveToDatabase();

						this.CommandGroup_D_Timeout.NeedsDelete = true;
						this.CommandGroup_D_Timeout.SaveToDatabase();
					}


					blnChanged = true;
					m_blnDeleted = true;
				}
				else
				{


					//we do this first so we can get the ids
					if(this.ParentUIDesignObj.ParentUIDesignObjVariation == null)
					{
						blnChanged = this.CommandGroup_D_Load.SaveToDatabase() || blnChanged;
						blnChanged = this.CommandGroup_D_Touch.SaveToDatabase() || blnChanged;
						blnChanged = this.CommandGroup_D_Unload.SaveToDatabase() || blnChanged;
						blnChanged = this.CommandGroup_D_Startup.SaveToDatabase() || blnChanged;
						blnChanged = this.CommandGroup_D_Highlight.SaveToDatabase() || blnChanged;
						blnChanged = this.CommandGroup_D_Unhighlight.SaveToDatabase() || blnChanged;
						blnChanged = this.CommandGroup_D_Timeout.SaveToDatabase() || blnChanged;

						//these ids may be -1, or they may be the same as they were
						this.CommandGroup_D_TouchID = this.CommandGroup_D_Touch.CommandGroupID;
						this.CommandGroup_D_LoadID = this.CommandGroup_D_Load.CommandGroupID;
						this.CommandGroup_D_UnloadID = this.CommandGroup_D_Unload.CommandGroupID;
						this.CommandGroup_D_StartupID = this.CommandGroup_D_Startup.CommandGroupID;
						this.CommandGroup_D_HighlightID = this.CommandGroup_D_Highlight.CommandGroupID;
						this.CommandGroup_D_UnhighlightID = this.CommandGroup_D_Unhighlight.CommandGroupID;
						this.CommandGroup_D_TimeoutID = this.CommandGroup_D_Timeout.CommandGroupID;


					}


					


					if(this.NeedsInsert)
					{


						//insert this object
						DesignObjVariationDataRow drDesignObjVariation = new DesignObjVariationDataRow(mds.tDesignObjVariation.NewRow());

						drDesignObjVariation.fFK_DesignObj = this.ParentUIDesignObj.ID;
/*
						if(this.CommandID == -1)
							drDesignObjVariation.fFK_CommandSetNull();
						else
							drDesignObjVariation.fFK_Command = this.CommandID;

						if(this.CommandGroupID == -1)
							drDesignObjVariation.fFK_CommandGroup_OnActivateSetNull();
						else
							drDesignObjVariation.fFK_CommandGroup_OnActivate = this.CommandGroupID;
*/
						if(this.CButton == -1)
							drDesignObjVariation.fFK_ButtonSetNull();
						else
							drDesignObjVariation.fFK_Button = this.CButton;

						drDesignObjVariation.fDontResetSelectedState = (short) (this.m_boolDontResetSelectedState ? 1 : 0);

						if(this.CriteriaID == -1)
							drDesignObjVariation.fFK_UISetNull();
						else
							drDesignObjVariation.fFK_UI = this.CriteriaID;

						if(this.GotoUIDesignObjID == -1)
							drDesignObjVariation.fFK_DesignObj_GotoSetNull();
						else
							drDesignObjVariation.fFK_DesignObj_Goto = this.GotoUIDesignObjID;


						if(this.CommandGroup_D_TouchID == -1)
							drDesignObjVariation.fFK_CommandGroup_D_OnActivateSetNull();
						else
							drDesignObjVariation.fFK_CommandGroup_D_OnActivate = this.CommandGroup_D_TouchID;

						if(this.CommandGroup_D_LoadID == -1)
							drDesignObjVariation.fFK_CommandGroup_D_OnLoadSetNull();
						else
							drDesignObjVariation.fFK_CommandGroup_D_OnLoad = this.CommandGroup_D_LoadID;

						if(this.CommandGroup_D_UnloadID == -1)
							drDesignObjVariation.fFK_CommandGroup_D_OnUnloadSetNull();
						else
							drDesignObjVariation.fFK_CommandGroup_D_OnUnload = this.CommandGroup_D_UnloadID;

						if(this.CommandGroup_D_StartupID == -1)
							drDesignObjVariation.fFK_CommandGroup_D_OnStartupSetNull();
						else
							drDesignObjVariation.fFK_CommandGroup_D_OnStartup = this.CommandGroup_D_StartupID;


						if(this.CommandGroup_D_HighlightID == -1)
							drDesignObjVariation.fFK_CommandGroup_D_OnHighlightSetNull();
						else
							drDesignObjVariation.fFK_CommandGroup_D_OnHighlight = this.CommandGroup_D_HighlightID;


						if(this.CommandGroup_D_UnhighlightID == -1)
							drDesignObjVariation.fFK_CommandGroup_D_OnUnhighlightSetNull();
						else
							drDesignObjVariation.fFK_CommandGroup_D_OnUnhighlight = this.CommandGroup_D_UnhighlightID;


						if(this.CommandGroup_D_TimeoutID == -1)
							drDesignObjVariation.fFK_CommandGroup_D_OnTimeoutSetNull();
						else
							drDesignObjVariation.fFK_CommandGroup_D_OnTimeout = this.CommandGroup_D_TimeoutID;




						m_objParentUIDesignObj.DesignObjModified(drDesignObjVariation.fFK_DesignObj_DataRow);


						mds.tDesignObjVariation.Rows.Add(drDesignObjVariation.dr);

						mds.tDesignObjVariation.Update(1,mds.m_conn,mds.m_trans);

						this.ID = drDesignObjVariation.fPK_DesignObjVariation;

						blnChanged = true;

					}
					else
					{

						

						if(this.OriginalsChanged)
						{
							//update this object
							DesignObjVariationDataRow drDesignObjVariation = mds.tDesignObjVariation[m_intID];
/*	sean			
							if(this.CommandID == -1)
								drDesignObjVariation.fFK_CommandSetNull();
							else
								drDesignObjVariation.fFK_Command = this.CommandID;

							if(this.CommandGroupID == -1)
								drDesignObjVariation.fFK_CommandGroup_OnActivateSetNull();
							else
								drDesignObjVariation.fFK_CommandGroup_OnActivate = this.CommandGroupID;
*/
							if(this.CButton == -1)
								drDesignObjVariation.fFK_ButtonSetNull();
							else
								drDesignObjVariation.fFK_Button = this.CButton;

							drDesignObjVariation.fDontResetSelectedState = (short) (this.m_boolDontResetSelectedState ? 1 : 0);

							if(this.CriteriaID == -1)
								drDesignObjVariation.fFK_UISetNull();
							else
								drDesignObjVariation.fFK_UI = this.CriteriaID;

							if(this.GotoUIDesignObjID == -1)
								drDesignObjVariation.fFK_DesignObj_GotoSetNull();
							else
								drDesignObjVariation.fFK_DesignObj_Goto = this.GotoUIDesignObjID;


							if(this.CommandGroup_D_TouchID == -1)
								drDesignObjVariation.fFK_CommandGroup_D_OnActivateSetNull();
							else
								drDesignObjVariation.fFK_CommandGroup_D_OnActivate = this.CommandGroup_D_TouchID;

							if(this.CommandGroup_D_LoadID == -1)
								drDesignObjVariation.fFK_CommandGroup_D_OnLoadSetNull();
							else
								drDesignObjVariation.fFK_CommandGroup_D_OnLoad = this.CommandGroup_D_LoadID;

							if(this.CommandGroup_D_UnloadID == -1)
								drDesignObjVariation.fFK_CommandGroup_D_OnUnloadSetNull();
							else
								drDesignObjVariation.fFK_CommandGroup_D_OnUnload = this.CommandGroup_D_UnloadID;

							if(this.CommandGroup_D_StartupID == -1)
								drDesignObjVariation.fFK_CommandGroup_D_OnStartupSetNull();
							else
								drDesignObjVariation.fFK_CommandGroup_D_OnStartup = this.CommandGroup_D_StartupID;

							if(this.CommandGroup_D_HighlightID == -1)
								drDesignObjVariation.fFK_CommandGroup_D_OnHighlightSetNull();
							else
								drDesignObjVariation.fFK_CommandGroup_D_OnHighlight = this.CommandGroup_D_HighlightID;

							if(this.CommandGroup_D_UnhighlightID == -1)
								drDesignObjVariation.fFK_CommandGroup_D_OnUnhighlightSetNull();
							else
								drDesignObjVariation.fFK_CommandGroup_D_OnUnhighlight = this.CommandGroup_D_UnhighlightID;

							if(this.CommandGroup_D_TimeoutID == -1)
								drDesignObjVariation.fFK_CommandGroup_D_OnTimeoutSetNull();
							else
								drDesignObjVariation.fFK_CommandGroup_D_OnTimeout = this.CommandGroup_D_TimeoutID;

							if(this.CEffectWithChange == -1)
								drDesignObjVariation.fFK_EffectType_Selected_WithChangeSetNull();
							else
								drDesignObjVariation.fFK_EffectType_Selected_WithChange = this.CEffectWithChange;

							if(this.CEffectNoChange == -1)
								drDesignObjVariation.fFK_EffectType_Selected_NoChangeSetNull();
							else
								drDesignObjVariation.fFK_EffectType_Selected_NoChange = this.CEffectNoChange;

							if(this.CEffectHightlight == -1)
								drDesignObjVariation.fFK_EffectType_HighlightedSetNull();
							else
								drDesignObjVariation.fFK_EffectType_Highlighted = this.CEffectHightlight;

							blnChanged = true;

						


						}
					}



					



					//save all parameters to database
					foreach(Object obj in this.Parameters)
					{
						UIDesignObjVariationParameter objVariationParameter = (UIDesignObjVariationParameter) obj;
						blnChanged = objVariationParameter.SaveToDatabase() || blnChanged;
					}

					//save all actionparameters to database
					foreach(Object obj in this.CommandParameters)
					{
						UIDesignObjVariationCommandParameter objVariationCommandParameter = (UIDesignObjVariationCommandParameter) obj;
						blnChanged = objVariationCommandParameter.SaveToDatabase() || blnChanged;
					}


					//save all the children objects
					foreach(Object obj in this.DesignObjs)
					{
						UIDesignObj objUIDesignObj = (UIDesignObj) obj;
						blnChanged = objUIDesignObj.SaveToDatabase() || blnChanged;
					}

                    //save all the text
					foreach(object oUIText in this.Text)
					{
						blnChanged = ((UIText)oUIText).SaveToDatabase() || blnChanged;
					}
				}



				//set the originals so we know how and when to save
				this.ResetOriginals();



				this.NeedsInsert = false;
				this.NeedsDelete = false;

			}



			return blnChanged;

		}


		public void UpdateCommandParameters(int intCommandID)
		{
/*  sean
			MyDataSet mds = HADataConfiguration.m_mdsCache;

			//this will reset the action parameters if the action changes.
			if(m_intCommandID != intCommandID)
			{

				//get rid of all the old
				foreach(Object obj in this.CommandParameters)
				{
					UIDesignObjVariationCommandParameter objCommandParameter = (UIDesignObjVariationCommandParameter) obj;
					objCommandParameter.NeedsDelete = true;
				}

				if(intCommandID != -1)
				{
					//first get all the possible action parameters for the given action id
					DataRow[] drCommandParameters = mds.tCommand_CommandParameter.Select(Command_CommandParameterData.FK_COMMAND_FIELD + "=" + intCommandID, Command_CommandParameterData.DESCRIPTION_FIELD);
					Command_CommandParameterDataRow drCommandParameter;
					foreach(DataRow dr in drCommandParameters)
					{
						drCommandParameter = new Command_CommandParameterDataRow(dr);
						UIDesignObjVariationCommandParameter objCommandParameter = new UIDesignObjVariationCommandParameter(this, drCommandParameter.fFK_CommandParameter, false);
						objCommandParameter.NeedsInsert = true;
						this.CommandParameters.Add(objCommandParameter);
					}
				}


			}
*/
		}



		public void Reset()
		{
			//this.Width = 0;
			//this.Height = 0;
			blnBuiltOnce = false;
			foreach(Object obj in this.DesignObjs)
			{
				UIDesignObj objUIDesignObj = (UIDesignObj) obj;
				objUIDesignObj.Reset();
			}
		}

		public void Build(int ParmSkinID, bool SkinChanged)
		{
			foreach(UIDesignObj objUIDesignObj in m_alDesignObjs)
			{
				objUIDesignObj.Build(ParmSkinID, SkinChanged);
			}

			//do the bottom up stuff
			//like width and height
			//add to parents width and height

			if(!blnBuiltOnce || SkinChanged)
			{
				if(this.ParentUIDesignObj.UIDesignObjType == DesignObjTypeData.BITMAP_CONST)
				{
					blnIsRendered = true;

                    if (this.Bitmap!=null) this.Bitmap.Dispose();
					this.Bitmap = GetBitmap(this.GetFilePath(ParmSkinID));

					string strWidth = this.GetParameterValue(DesignObjParameterData.WIDTH_CONST);
					string strHeight = this.GetParameterValue(DesignObjParameterData.HEIGHT_CONST);
					try	{ Convert.ToInt32(strWidth); } catch(Exception) { strWidth=""; }
					try	{ Convert.ToInt32(strHeight); } catch(Exception) { strHeight=""; }

					if (this.Bitmap==null)
					{
						this.Width = (strWidth=="" || strWidth=="0") ? emptyBmpWidth : Convert.ToInt32(strWidth);
						this.Height = (strHeight=="" || strHeight=="0") ? emptyBmpHeight : Convert.ToInt32(strHeight);	
						this.Bitmap = new Bitmap(this.Width, this.Height);
					}
					else
					{
						this.Width = (strWidth == "" || strWidth=="0") ? this.Bitmap.Width : Convert.ToInt32(strWidth);
						this.Height = (strHeight == "" || strHeight=="0") ? this.Bitmap.Height : Convert.ToInt32(strHeight);	
					}
				}
				blnBuiltOnce = true;
			}

			

			//get the total width and height of all children
			m_intTotalWidth = 0;
			m_intTotalHeight = 0;

			if(blnIsRendered)
			{
				m_intTotalWidth = this.Width;
				m_intTotalHeight = this.Height;
			}
			
			foreach(Object obj in this.DesignObjs)
			{
				UIDesignObj objUIDesignObj = (UIDesignObj) obj;

				//WARNING!!! HARDCODED LANGUAGE
				UIChildSkinLanguage child = objUIDesignObj.GetCurrentChildSkinLanguage(-1, ParmSkinID);

				if(null != child)
				{
					int intDesignObjWidth = child.Width + child.ParentX;
					if(intDesignObjWidth > m_intTotalWidth) 
						m_intTotalWidth = intDesignObjWidth;

					int intDesignObjHeight = child.Height + child.ParentY;
					if(intDesignObjHeight > m_intTotalHeight) 
						m_intTotalHeight = intDesignObjHeight;
				}
			}
		}

		private string GetFilePath(int ParmSkinID)
		{
			string path = this.GetParameterValue(DesignObjParameterData.GRAPHIC_FILENAME_CONST);

			if (path == "" || path == " ") return "";
			else if (path.StartsWith("C:") || path.StartsWith(@"\\")) return path;
			else 
			{
				string fullPath = this.SkinDirectory(ParmSkinID) + "\\" + path;
				if (System.IO.File.Exists(fullPath)) return fullPath;
                else return this.SkinDirectory(1) + "\\" + path;
			}
		}


		private Bitmap GetBitmap(string strPath)
		{
			string strErrorMessage = "";

			if (strPath == "" || strPath == " " || strPath.IndexOf("<%")!=-1 ) return null;

			Bitmap objReturnBitmap;

			if(System.IO.File.Exists(strPath))
			{
				try
				{
					objReturnBitmap = new Bitmap(strPath);
				}
				catch(Exception e)
				{
					strErrorMessage = e.Message;
					objReturnBitmap = new Bitmap(this.pathFileNotFound);
				}
			}
			else
			{
				if( strPath.IndexOf("<%=")<0  )
					MessageBox.Show("Cannot find bitmap: " + strPath);
				objReturnBitmap = new Bitmap(this.pathFileNotFound);
			}

			return objReturnBitmap;

			//return (System.IO.File.Exists(strPath)) ? new Bitmap(strPath) : new Bitmap(this.pathFileNotFound);
		}

		


		public void ResetOriginals()
		{
			m_intUIIDOriginal = m_intUIID;
			m_intCommandGroup_D_TouchIDOriginal = m_intCommandGroup_D_TouchID;
			m_intCommandGroup_D_LoadIDOriginal = m_intCommandGroup_D_LoadID;
			m_intCommandGroup_D_UnloadIDOriginal = m_intCommandGroup_D_UnloadID;
			m_intCommandGroup_D_StartupIDOriginal = m_intCommandGroup_D_StartupID;
			m_intCommandGroup_D_HighlightIDOriginal = m_intCommandGroup_D_HighlightID;
			m_intCommandGroup_D_UnhighlightIDOriginal = m_intCommandGroup_D_UnhighlightID;
			m_intCommandGroup_D_TimeoutIDOriginal = m_intCommandGroup_D_TimeoutID;
			m_intCButtonOriginal = m_intCButton;
			m_boolDontResetSelectedStateOriginal = m_boolDontResetSelectedState;
			m_intGotoUIDesignObjIDOriginal = m_intGotoUIDesignObjID;
			m_intCEffectSelectNoChangeOriginal = m_intCEffectSelectNoChange;
			m_intCEffectSelectWithChangeOriginal = m_intCEffectSelectWithChange;
			m_intCEffectHighlightOriginal = m_intCEffectHighlight;
		}
	}
}
