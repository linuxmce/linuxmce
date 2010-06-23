using System;
using System.Collections;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Windows.Forms;
using HAData.Common;
using HAData.DataAccess;
using System.Data;
using System.Diagnostics;


namespace HADesigner
{
	/// <summary>
	/// Manage DesignObjVariation_DesignObj_Skin_Language table
	/// </summary>
	public class UIChildSkinLanguage: UI
	{
		public static int NoSetValue = -2;

		//internal variables
		public int m_FK_SkinID = -1;
		public int m_FK_LanguageID = -1;

		//current values
		private int m_intParentX = 0;		//relative to parent UIDesignObjVariation
		private int m_intParentY = 0;
		private int m_intRootX = 0;
		private int m_intRootY = 0;
		private int m_intWidth = NoSetValue;
		private int m_intHeight = NoSetValue;

		//clones
		private int m_intParentXOriginal = 0;		
		private int m_intParentYOriginal = 0;
		private int m_intWidthOriginal = NoSetValue;
		private int m_intHeightOriginal = NoSetValue;

		//unordered variables and properties

		private int m_intParentDisplayOrder;

		//MEMBER VARIABLES
		private string graphicsDirectory;
		public string GraphicsDirectory
		{
			get {return this.graphicsDirectory;}
			set {this.graphicsDirectory = value;}
		}

		private string m_strDescription;
		private sbyte m_intPriority;
		private bool m_blnAnimate;
		private int m_intUIDesignObjType;
		private int m_intDesignObjCategoryID;
		private ArrayList m_alUIDesignObjVariations = new ArrayList();
		private UIDesignObj m_objParentUIDesignObj;
		private bool m_blnCantGoBack;


		private bool m_blnMainBackgroundDrawn = false;


		//TODO: this will change when we have multiple select capability
		private UIDesignObj m_UIDesignObjSelected = null;
		
		

		//things that can change
		//the following change, but are handled in UIDesignObjVariation object
		private bool m_bitRegenerateForEachScreenOriginal;
		private bool m_bitCanBeHiddenOriginal;
		private bool m_bitHideByDefaultOriginal;
		private bool m_bitChildBehindBGOriginal;
		private bool m_bitChildBehindBG;
		private bool m_blnBGOnTopOriginal;
		private bool m_blnBGOnTop;
		private bool m_blnDontMergeBGOriginal;
		private bool m_blnDontMergeBG;
		private bool m_blnChildBeforeTextOriginal;
		private bool m_blnChildBeforeText;
		private bool m_blnIsTabStopOriginal;
		private bool m_blnIsTabStop;
		private string m_sVisibleStates;
		private string m_sVisibleStatesOriginal;
		private string m_iTiedToOriginal;
		private string m_iTiedTo;
		private string m_iTS_UpOriginal;
		private string m_iTS_Up;
		private string m_iTS_DownOriginal;
		private string m_iTS_Down;
		private string m_iTS_LeftOriginal;
		private string m_iTS_Left;
		private string m_iTS_RightOriginal;
		private string m_iTS_Right;
		private int m_intParentDisplayOrderOriginal;




		//used for database updates
		private bool m_blnNeedsDBParentVariationUnlink = false;
		private bool m_blnNeedsDBParentVariationLink = false;
		private bool m_blnParentVariationLinked = false;


		private bool m_blnShow = true;


		private bool m_blnSelected = false;

		//used for drawing
		private float m_fltScale;
		private bool m_bitRegenerateForEachScreen;
		private bool m_bitCanBeHidden;
		private bool m_bitDontResetSelectedState;
		private bool m_bitHideByDefault;
		private int m_intLinkID;	//this is the id in the objectvariation_oject table

		//PROPERTIES
		public string DescriptionOnly
		{
			get {return this.m_strDescription;}
			set {this.m_strDescription = value;}
		}

		public int SkinID
		{
			get {return this.m_FK_SkinID;}
			set {this.m_FK_SkinID = value;}
		}


		public int LanguageID
		{
			get {return this.m_FK_LanguageID;}
			set {this.m_FK_LanguageID = value;}
		}

		public string Description
		{
			get
			{
				if (this.m_strDescription == null) 
					return "";
				else 
					return ID + " " + m_strDescription + (m_bitHideByDefault ? " h" : "");
			}
			set	{m_strDescription = value;}
		}
		public bool CantGoBack
		{
			get {return m_blnCantGoBack; }
			set {m_blnCantGoBack = value; }
		}
		public sbyte Priority
		{
			get {return this.m_intPriority;}
			set {this.m_intPriority = value;}
		}
		public bool Animate
		{
			get {return this.m_blnAnimate;}
			set {this.m_blnAnimate = value;}
		}
		public override string ToString()
		{
			return this.Description;
		}
		public int UIDesignObjType
		{
			get	{return m_intUIDesignObjType;}
			set	{m_intUIDesignObjType = value;}
		}

		public int DesignObjCategory
		{
			get	{return m_intDesignObjCategoryID;}
			set	{m_intDesignObjCategoryID = value;}
		}

		public ArrayList UIDesignObjVariations
		{
			get {return m_alUIDesignObjVariations;}
			set {m_alUIDesignObjVariations = value;}
		}

		public UIDesignObj ParentUIDesignObj
		{
			get {return m_objParentUIDesignObj;}
			set	{m_objParentUIDesignObj = value;}
		}

		public int LinkID
		{
			get	{return m_intLinkID;}
			set	{m_intLinkID = value;}
		}


		public UIDesignObj SelectedDesignObj
		{
			get {return m_UIDesignObjSelected;}
		}

		
		public int Width
		{
			get	{return m_intWidth;}
			set {m_intWidth = value;}
		}
		public int Height
		{
			get	{return m_intHeight;}
			set {m_intHeight = value;}
		}
		public float Scale
		{
			get	{return m_fltScale;}
			set {m_fltScale = value;}
		}
		public bool CanBeHidden
		{
			get {return this.m_bitCanBeHidden;}
			set {this.m_bitCanBeHidden = value;}
		}
		public bool HideByDefault
		{
			get {return this.m_bitHideByDefault;}
			set {this.m_bitHideByDefault = value;}
		}
		public bool ChildBehindBG
		{
			get {return this.m_bitChildBehindBG;}
			set {this.m_bitChildBehindBG = value;}
		}
		public bool ChildBehindBGOriginal
		{
			get {return this.m_bitChildBehindBGOriginal;}
			set {this.m_bitChildBehindBGOriginal = value;}
		}
		public bool RegenerateForEachScreen
		{
			get {return this.m_bitRegenerateForEachScreen;}
			set {this.m_bitRegenerateForEachScreen = value;}
		}
		public bool DontResetSelectedState
		{
			get {return this.m_bitDontResetSelectedState;}
			set {this.m_bitDontResetSelectedState = value;}
		}
		public int ParentX
		{
			get	{return m_intParentX;}
			set {m_intParentX = value;}
		}
		public int ParentY
		{
			get	{return m_intParentY;}
			set {m_intParentY = value;}
		}
		public int RootX
		{
			get	{return m_intRootX;}
			set {m_intRootX = value;}
		}
		public int RootY
		{
			get	{return m_intRootY;}
			set {m_intRootY = value;}
		}
		public int ParentDisplayOrder
		{
			get	{return m_intParentDisplayOrder;}
			set	{m_intParentDisplayOrder = value;}
		}
		public bool CanBeHiddenOriginal
		{
			get {return this.m_bitCanBeHiddenOriginal;}
			set {this.m_bitCanBeHiddenOriginal = value;}
		}
		public bool HideByDefaultOriginal
		{
			get {return this.m_bitHideByDefaultOriginal;}
			set {this.m_bitHideByDefaultOriginal = value;}
		}

		public bool BGOnTopOriginal
		{
			get { return m_blnBGOnTopOriginal; }
			set { m_blnBGOnTopOriginal = value; }
		}
		public bool BGOnTop
		{
			get { return m_blnBGOnTop; }
			set { m_blnBGOnTop = value; }
		}
		public bool ChildBeforeText
		{
			get { return m_blnChildBeforeText; }
			set { m_blnChildBeforeText = value; }
		}
		public bool ChildBeforeTextOriginal
		{
			get { return m_blnChildBeforeTextOriginal; }
			set { m_blnChildBeforeTextOriginal = value; }
		}
		public bool DontMergeBG
		{
			get { return m_blnDontMergeBG; }
			set { m_blnDontMergeBG = value; }
		}
		public bool DontMergeBGOriginal
		{
			get { return m_blnDontMergeBGOriginal; }
			set { m_blnDontMergeBGOriginal = value; }
		}
		public bool IsTabStop
		{
			get { return m_blnIsTabStop; }
			set { m_blnIsTabStop = value; }
		}
		public bool IsTabStopOriginal
		{
			get { return m_blnIsTabStopOriginal; }
			set { m_blnIsTabStopOriginal = value; }
		}
		public string TS_Up
		{
			get { return m_iTS_Up; }
			set { m_iTS_Up = value; }
		}
		public string TS_Down
		{
			get { return m_iTS_Down; }
			set { m_iTS_Down = value; }
		}
		public string TS_Left
		{
			get { return m_iTS_Left; }
			set { m_iTS_Left = value; }
		}
		public string TS_Right
		{
			get { return m_iTS_Right; }
			set { m_iTS_Right = value; }
		}
		public string TiedTo
		{
			get { return m_iTiedTo; }
			set { m_iTiedTo = value; }
		}
		public string VisibleStates
		{
			get { return m_sVisibleStates; }
			set { m_sVisibleStates = value; }
		}
		public bool RegenerateForEachScreenOriginal
		{
			get {return this.m_bitRegenerateForEachScreenOriginal;}
			set {this.m_bitRegenerateForEachScreenOriginal = value;}
		}

		public int WidthOriginal
		{
			get	{return	m_intWidthOriginal;}
			set	{m_intWidthOriginal	= value;}
		}
		public int HeightOriginal
		{
			get	{return	m_intHeightOriginal;}
			set	{m_intHeightOriginal = value;}
		}
		public int ParentXOriginal
		{
			get	{return	m_intParentXOriginal;}
			set	{m_intParentXOriginal =	value;}
		}
		public int ParentYOriginal
		{
			get	{return m_intParentYOriginal;}
			set {m_intParentYOriginal = value;}
		}

		public int ParentDisplayOrderOriginal
		{
			get	{return m_intParentDisplayOrderOriginal;}
			set	{m_intParentDisplayOrderOriginal = value;}
		}

		

		/// <summary>
		/// UIChildSkinLanguage constructor
		/// </summary>
		/// <param name="objParentUIDesignObj">parent UIObject</param>
		/// <param name="intID">DesignObjVariation_DesignObj_Skin_Language.PK_DesignObjVariation_DesignObj_Skin_Language</param>
		/// <param name="SkinID">skin</param>
		/// <param name="LanguageID">language</param>
		/// <param name="graphicsDir">graphic dir</param>
		public UIChildSkinLanguage(UIDesignObj objParentUIDesignObj, int intID, 
			int SkinID,  int LanguageID, string graphicsDir)
		{
			this.graphicsDirectory = graphicsDir;

			m_objParentUIDesignObj = objParentUIDesignObj;
			ID = intID;		

			m_FK_SkinID = SkinID;
			m_FK_LanguageID = LanguageID;

			if(objParentUIDesignObj == null)
			{
				this.m_bitCanBeHidden = false;
				this.m_bitHideByDefault = false;
				this.m_bitRegenerateForEachScreen = false;
				m_intParentX = 0;
				m_intParentY = 0;
				this.ParentVariationLinked = false;
			}
			else
			{
				this.ParentVariationLinked = true;
			}

			if (intID == -1)
				NeedsInsert = true;
			else 
				LoadFromDatabase();

			if(objParentUIDesignObj == null)
			{
				Build(1, true); // Start with default skin
				this.ResetLinkOriginals();
			}	
		}
        
		public override bool OriginalsChanged
		{
			get	{return true;}
		}

		public override void LoadFromDatabase()
		{
			MyDataSet mds = HADataConfiguration.m_mdsCache;
	
			DesignObjVariation_DesignObj_Skin_LanguageDataRow dr = mds.tDesignObjVariation_DesignObj_Skin_Language[ID];


			this.ParentDisplayOrder = dr.fDisplayOrder;
			this.ParentX = dr.fX;
			this.ParentY = dr.fY;
			this.Width = dr.fWidthIsNull ? 0 : dr.fWidth;
			this.Height = dr.fHeightIsNull ? 0 : dr.fHeight;

			CanBeHidden = dr.fCanBeHidden;
			HideByDefault = dr.fHideByDefault;
			ChildBehindBG = dr.fDisplayChildrenBehindBackground;

			TS_Up = dr.fFK_DesignObj_UpIsNull ? "" : dr.fFK_DesignObj_Up.ToString();
			TS_Down = dr.fFK_DesignObj_DownIsNull ? "" : dr.fFK_DesignObj_Down.ToString();
			TS_Right = dr.fFK_DesignObj_RightIsNull ? "" : dr.fFK_DesignObj_Right.ToString();
			TS_Left = dr.fFK_DesignObj_LeftIsNull ? "" : dr.fFK_DesignObj_Left.ToString();
			TiedTo = dr.fsFK_DesignObj_TiedTo;
			VisibleStates = dr.fVisibleStates;
			ChildBeforeText = dr.fDisplayChildrenBeforeText;
			DontMergeBG = dr.fDontMergeBackground;
			IsTabStop = dr.fIsTabStop;
			//DontResetSelectedState = dr.f;
			RegenerateForEachScreen = dr.fRegenerateForEachScreen;

			//set the originals so we know how and when to save
			this.ResetOriginals();
		}

		private DesignObjVariation_DesignObj_Skin_LanguageDataRow drOVDSL
		{
			get {return mds.tDesignObjVariation_DesignObj_Skin_Language[ID];}
		}

		private void setDataRowFields(DesignObjVariation_DesignObj_Skin_LanguageDataRow drOVDSL)
		{
			drOVDSL.fDisplayOrder = ParentDisplayOrder;
			drOVDSL.fX = ParentX;
			drOVDSL.fY = ParentY;
			drOVDSL.fWidth = Width;
			drOVDSL.fHeight = Height;

			drOVDSL.fCanBeHidden = CanBeHidden;
			drOVDSL.fHideByDefault = HideByDefault;
			drOVDSL.fDisplayChildrenBehindBackground = ChildBehindBG;

			if(TS_Up == null  || TS_Up == "")
				drOVDSL.fFK_DesignObj_UpSetNull();
			else
				drOVDSL.fFK_DesignObj_Up = Int32.Parse(TS_Up);

			if(TS_Down == null  || TS_Down == "")
				drOVDSL.fFK_DesignObj_DownSetNull();
			else
				drOVDSL.fFK_DesignObj_Down = Int32.Parse(TS_Down);

			if(TS_Right == null  || TS_Right == "")
				drOVDSL.fFK_DesignObj_RightSetNull();
			else
				drOVDSL.fFK_DesignObj_Right = Int32.Parse(TS_Right);

			if(TS_Left == null  || TS_Left == "")
				drOVDSL.fFK_DesignObj_LeftSetNull();
			else
				drOVDSL.fFK_DesignObj_Left = Int32.Parse(TS_Left);


			drOVDSL.fsFK_DesignObj_TiedTo = TiedTo;
			drOVDSL.fVisibleStates = VisibleStates;
			drOVDSL.fDisplayChildrenBeforeText = ChildBeforeText;
			drOVDSL.fDontMergeBackground = DontMergeBG;
			drOVDSL.fIsTabStop = IsTabStop;
			drOVDSL.fRegenerateForEachScreen = RegenerateForEachScreen;
		}

		public override bool SaveToDatabase()
		{
			bool changed = false;

			Debug.WriteLine("Saving for ID " + ID + " and m_objParentUIDesignObj.ID " + m_objParentUIDesignObj.ID);
	
			if(!this.Deleted)
			{
				if(this.NeedsDelete)
				{
					this.drOVDSL.dr.Delete();
					this.deleted = true;
					changed = true;
				}
				else if(this.NeedsInsert && ParentUIDesignObj.DOV_DO_ID != -1) // Insert to OVTSL
				{
					DesignObjVariation_DesignObj_Skin_LanguageDataRow drOVDSL = new DesignObjVariation_DesignObj_Skin_LanguageDataRow(mds.tDesignObjVariation_DesignObj_Skin_Language.NewRow());
					drOVDSL.fFK_DesignObjVariation_DesignObj = ParentUIDesignObj.DOV_DO_ID;

					if(SkinID != -1)
						drOVDSL.fFK_Skin = SkinID;
					else
						drOVDSL.fFK_SkinSetNull();

					if(LanguageID != -1)
                        drOVDSL.fFK_Language = LanguageID;
					else
						drOVDSL.fFK_LanguageSetNull();

					mds.tDesignObjVariation_DesignObj_Skin_Language.Rows.Add(drOVDSL.dr);
					mds.tDesignObjVariation_DesignObj_Skin_Language.Update(1,mds.m_conn,mds.m_trans); // insert now to get PKID

					this.id = drOVDSL.fPK_DesignObjVariation_DesignObj_Skin_Language;
					this.setDataRowFields(drOVDSL);

					changed = true;
				}
				else if(this.OriginalsChanged) //update this object
				{
					if(ID != -1)
					{
						this.setDataRowFields(this.drOVDSL);
						changed = true;
					}
				}
                
				this.ResetOriginals();
				this.NeedsInsert = false;
				this.NeedsDelete = false;
			}
	
			return changed;
		}

		protected override void ResetOriginals()
		{
			
		}

		public void ResetLinkOriginals()
		{
			this.CanBeHiddenOriginal = this.CanBeHidden;
			this.HideByDefaultOriginal = this.HideByDefault;

			this.m_sVisibleStatesOriginal = this.m_sVisibleStates;
			this.m_iTiedToOriginal = this.m_iTiedTo;
			this.m_iTS_UpOriginal = this.m_iTS_Up;
			this.m_iTS_DownOriginal = this.m_iTS_Down;
			this.m_iTS_LeftOriginal = this.m_iTS_Left;
			this.m_iTS_RightOriginal = this.m_iTS_Right;
			this.BGOnTopOriginal = this.BGOnTop;
			this.RegenerateForEachScreenOriginal = this.RegenerateForEachScreen;
			this.ParentXOriginal = this.ParentX;
			this.ParentYOriginal = this.ParentY;
			this.WidthOriginal = this.Width;
			this.HeightOriginal = this.Height;
			this.ParentDisplayOrderOriginal = this.ParentDisplayOrder;
		}


		public void Build(int SkinID)
		{
			this.Build(SkinID, false);
		}

		public void Build(int ParmSkinID, bool SkinChanged)
		{
			if(this.Include)	//don't include if deleted or set to be deleted or unlinked
			{
				//RECURSE
				foreach(UIDesignObjVariation objUIDesignObjVariation in m_alUIDesignObjVariations)
				{
					objUIDesignObjVariation.Build(ParmSkinID, SkinChanged);
				}

				if(this.Width == UIDesignObj.NoSetValue || this.Height == UIDesignObj.NoSetValue)
				{
					int intMaxWidth = 0;
					int intMaxHeight = 0;
					//calculate the width from the children
					foreach(Object obj in ParentUIDesignObj.UIDesignObjVariations)
					{
						UIDesignObjVariation objVariation = (UIDesignObjVariation) obj;

						if((this.Width == UIDesignObj.NoSetValue) && (objVariation.TotalWidth > intMaxWidth)) intMaxWidth = objVariation.TotalWidth;
						if((this.Height == UIDesignObj.NoSetValue) && (objVariation.TotalHeight > intMaxHeight)) intMaxHeight = objVariation.TotalHeight;
					}

					if(this.Width == UIDesignObj.NoSetValue) this.Width = intMaxWidth;
					if(this.Height == UIDesignObj.NoSetValue) this.Height = intMaxHeight;
				}			
			}
		}

		public bool Selected
		{
			get	{return m_blnSelected;}
			set	{m_blnSelected = value;}
		}

		public bool MainBackgroundDrawn
		{
			get	{return m_blnMainBackgroundDrawn;}
			set	{m_blnMainBackgroundDrawn = value;}
		}

		public void Draw(Graphics objGraphics, UIDesignObjVariation objVariation)
		{
			//refresh offsets
			if(null != this.ParentUIDesignObj.ParentUIDesignObjVariation && null != this.ParentUIDesignObj.ParentUIDesignObjVariation.ParentUIDesignObj)
			{
				UIChildSkinLanguage parentLS = this.ParentUIDesignObj.ParentUIDesignObjVariation.ParentUIDesignObj.GetCurrentChildSkinLanguage(LanguageID, SkinID);
				if(null != parentLS)
				{
					this.RootX = parentLS.RootX + parentLS.ParentX;
					this.RootY = parentLS.RootY + parentLS.ParentY;
				}
			}

			objVariation.Draw(this, objGraphics, SkinLanguageStatus.Instance().GlobalLanguageID, SkinLanguageStatus.Instance().GlobalSkinID);

			//if this is selected, draw a big ol' rectangle around it
			if(ParentUIDesignObj.Selected)
			{
				objGraphics.DrawRectangle(new Pen(Color.Red, 4), 
					this.RootX + this.ParentX, this.RootY + this.ParentY, this.Width - 4, this.Height - 4);
			}
		}

		public bool Show
		{
			get	{return m_blnShow;}
			set {m_blnShow = value;}
		}

		public bool WarnToSave
		{
			get 
			{
				if (NeedsInsert||NeedsDelete||OriginalsChanged||NeedsParentVariationUnlink||NeedsParentVariationLink||LinkOriginalsChanged)return true;
				foreach(Object uiOV in this.UIDesignObjVariations)
				{
					if (((UIDesignObjVariation)uiOV).WarnToSave)return true;
				}
				return false;
			}
		}

		public bool NeedsParentVariationUnlink
		{
			get	{return m_blnNeedsDBParentVariationUnlink;}
			set	{m_blnNeedsDBParentVariationUnlink = value;}
		}
		public bool NeedsParentVariationLink
		{
			get	{return m_blnNeedsDBParentVariationLink;}
			set	{m_blnNeedsDBParentVariationLink = value;}
		}

		public bool ParentVariationLinked
		{
			get	{return m_blnParentVariationLinked;}
			set	{m_blnParentVariationLinked = value;}
		}

		public bool LinkOriginalsChanged
		{
			get
			{
				return BGOnTop != BGOnTopOriginal || RegenerateForEachScreen != RegenerateForEachScreenOriginal || CanBeHidden != CanBeHiddenOriginal || HideByDefault != HideByDefaultOriginal || Width != WidthOriginal || Height != HeightOriginal || ParentX != ParentXOriginal || ParentY != ParentYOriginal || 
					ParentDisplayOrder != ParentDisplayOrderOriginal || IsTabStop != IsTabStopOriginal || ChildBeforeText != ChildBeforeTextOriginal ||
					ChildBehindBG != ChildBehindBGOriginal ||
					m_blnChildBeforeTextOriginal != m_blnChildBeforeText || m_blnIsTabStopOriginal != m_blnIsTabStop || m_iTiedToOriginal != m_iTiedTo || m_sVisibleStatesOriginal != m_sVisibleStates ||
					m_iTS_UpOriginal != m_iTS_Up || m_iTS_DownOriginal != m_iTS_Down || m_iTS_LeftOriginal != m_iTS_Left || m_iTS_RightOriginal != m_iTS_Right;
			}
		}

		public void ReleaseBitmaps()
		{
			foreach (UIDesignObjVariation uiov in this.m_alUIDesignObjVariations)
			{
				uiov.ReleaseBitmaps();				
			}
		}		

		public void DesignObjModified(DesignObjDataRow drObject)
		{
			MyDataSet mds = HADataConfiguration.m_mdsCache;
			
			string SQL = "UPDATE DesignObj SET psc_mod='" + DateTime.Now.ToString("yyMMddhhmmss") + "' WHERE PK_DesignObj=" + drObject.fPK_DesignObj.ToString();
			new Microsoft.Data.Odbc.OdbcCommand(SQL,mds.m_conn,mds.m_trans).ExecuteNonQuery();
		}

		public void DeselectAllDesignObjs(bool blnRecursive) // Also Text
		{
			foreach(UIDesignObjVariation objVariation in this.UIDesignObjVariations)
			{
				foreach(UIChildSkinLanguage objUIDesignObj in objVariation.DesignObjs)
				{
					objUIDesignObj.Selected = false;
					if(blnRecursive)
					{
						objUIDesignObj.DeselectAllDesignObjs(true);
					}
				}
				foreach (UIText uit in objVariation.Text)
				{
					uit.Selected = false;
				}
			}
		}

		public bool Contains(int intX, int intY)
		{
			if (this.Include) 
			{
				return 
					((intX >= this.ParentX + this.RootX) && 
					(intX <= (this.ParentX + this.RootX + this.Width)) && 
					(intY >= this.ParentY + this.RootY) && 
					(intY <= (this.ParentY + this.RootY + this.Height)));
			}
			else return false;
		}

		public string LanguageSkin
		{
			get
			{
				string language = (this.LanguageID == -1) ? "Default" : mds.tLanguage[this.LanguageID].fDescription;
				string skin = (this.SkinID == -1) ? "Default" : mds.tSkin[this.SkinID].fDescription;
				return language + " - " + skin;
			}
		}
	}
}