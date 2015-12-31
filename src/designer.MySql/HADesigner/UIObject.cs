using System;
using System.Collections;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using HAData.Common;
using HAData.DataAccess;
using System.Diagnostics;

using System.Reflection;


namespace HADesigner
{
	/// <summary>
	/// Summary description for UIDesignObj.
	/// </summary>
	public class UIDesignObj
	{
		public static int NoSetValue = -2;

		//MEMBER VARIABLES
		private string graphicsDirectory;
		public string GraphicsDirectory
		{
			get {return this.graphicsDirectory;}
			set {this.graphicsDirectory = value;}
		}

		private ArrayList alChildSkinLanguages = new ArrayList();
		public ArrayList ChildSkinLanguages
		{
			get {return this.alChildSkinLanguages;}
			set {this.alChildSkinLanguages = value;}
		}

		private int m_intDesignObjID = -1;
		private int m_intID = -1;
		private string m_strDescription;
		private sbyte m_intPriority;
		private bool m_blnAnimate;
		private int m_intUIDesignObjType;
		private int m_intDesignObjCategoryID;
		private ArrayList m_alUIDesignObjVariations = new ArrayList();
		private UIDesignObjVariation m_objParentUIDesignObjVariation;
		private bool m_blnCantGoBack;


		private bool m_blnMainBackgroundDrawn = false;


		//TODO: this will change when we have multiple select capability
		private UIDesignObj m_UIDesignObjSelected = null;
		
		

		//things that can change
		private string m_strDescriptionOriginal;
		private sbyte m_intPriorityOriginal;
		private bool m_blnAnimateOriginal;
		private int m_intUIDesignObjTypeOriginal;
		private int m_intDesignObjCategoryIDOriginal;
		private bool m_blnCantGoBackOriginal;
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
		private bool m_blnNeedsDBInsert = false;
		private bool m_blnNeedsDBDelete = false;
		private bool m_blnNeedsDBParentVariationUnlink = false;
		private bool m_blnNeedsDBParentVariationLink = false;
		private bool m_blnDeleted = false;			//this becomes true AFTER the delete
		private bool m_blnParentVariationLinked = false;


		private bool m_blnShow = true;


		private bool m_blnSelected = false;

		//used for drawing
		private float m_fltScale;
		private bool m_bitRegenerateForEachScreen;
		private bool m_bitCanBeHidden;
		private bool m_bitDontResetSelectedState;
		private bool m_bitHideByDefault;
		//private int m_intParentX;		//relative to parent UIDesignObjVariation
		//private int m_intParentY;
		private int m_intParentDisplayOrder;

		private int m_intLinkID;	//this is the id in the objectvariation_oject table
		



		
		//private bool m_blnIsVariation = false;		//if is true, then we will draw differently
		//private bool m_blnIsSelected = false;
		//private bool m_blnAutoSelectChildren = false;		//if true and this is selected, also select children
		

		//PROPERTIES
		public int ID
		{
			get {return this.m_intDesignObjID;}
			set {this.m_intDesignObjID = value;}
		}

		public int DOV_DO_ID
		{
			get {return this.m_intID;}
			set {this.m_intID = value;}
		}

		public string DescriptionOnly
		{
			get {return this.m_strDescription;}
			set {this.m_strDescription = value;}
		}
		public string Description
		{
			get
			{
				if (this.m_strDescription == null) return "";
				else return m_intDesignObjID + " " + m_strDescription + (m_bitHideByDefault ? " h" : "");
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

		public UIDesignObjVariation ParentUIDesignObjVariation
		{
			get {return m_objParentUIDesignObjVariation;}
			set	{m_objParentUIDesignObjVariation = value;}
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
		public int ParentDisplayOrderOriginal
		{
			get	{return m_intParentDisplayOrderOriginal;}
			set	{m_intParentDisplayOrderOriginal = value;}
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
        
		public bool NeedsInsert
		{	
			get {return m_blnNeedsDBInsert;}
			set	{m_blnNeedsDBInsert = value;}
		}
		public bool	NeedsDelete
		{
			get	{return m_blnNeedsDBDelete;}
			set	{m_blnNeedsDBDelete = value;}
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

		public bool Deleted
		{
			get	{return m_blnDeleted;}
		}

		public bool ParentVariationLinked
		{
			get	{return m_blnParentVariationLinked;}
			set	{m_blnParentVariationLinked = value;}
		}

		
		public bool Selected
		{
			get	{return m_blnSelected;}
			set	{m_blnSelected = value;}
		}



		public bool Show
		{
			get	{return m_blnShow;}
			set {m_blnShow = value;}
		}
		public bool Include
		{
			get
			{
				return  (!this.Deleted && !this.NeedsParentVariationUnlink && (this.ParentVariationLinked || (this.ParentUIDesignObjVariation == null)) && !this.NeedsDelete);

			}
		}



		public bool OriginalsChanged
		{
			get
			{
				return m_intDesignObjCategoryIDOriginal != m_intDesignObjCategoryID || m_strDescription != m_strDescriptionOriginal || m_intPriority != m_intPriorityOriginal || this.m_blnAnimate != this.m_blnAnimateOriginal || m_intUIDesignObjType != m_intUIDesignObjTypeOriginal || m_blnCantGoBack != m_blnCantGoBackOriginal;
			}
		}
		public bool LinkOriginalsChanged
		{
			get
			{
				return BGOnTop != BGOnTopOriginal || RegenerateForEachScreen != RegenerateForEachScreenOriginal || CanBeHidden != CanBeHiddenOriginal || HideByDefault != HideByDefaultOriginal ||  
					ParentDisplayOrder != ParentDisplayOrderOriginal || IsTabStop != IsTabStopOriginal || ChildBeforeText != ChildBeforeTextOriginal ||
					ChildBehindBG != ChildBehindBGOriginal ||
					m_blnChildBeforeTextOriginal != m_blnChildBeforeText || m_blnIsTabStopOriginal != m_blnIsTabStop || m_iTiedToOriginal != m_iTiedTo || m_sVisibleStatesOriginal != m_sVisibleStates ||
					m_iTS_UpOriginal != m_iTS_Up || m_iTS_DownOriginal != m_iTS_Down || m_iTS_LeftOriginal != m_iTS_Left || m_iTS_RightOriginal != m_iTS_Right;
			}
		}

		public bool DescriptionChanged
		{
			get
			{
				return (this.m_strDescription != this.m_strDescriptionOriginal);
			}
		}

		public bool MainBackgroundDrawn
		{
			get	{return m_blnMainBackgroundDrawn;}
			set	{m_blnMainBackgroundDrawn = value;}
		}

		/// <summary>
		/// UIDesignObj constructor
		/// </summary>
		/// <param name="objParentUIDesignObjVariation">parent variation object</param>
		/// <param name="intID">DesignObjVariation_DesignObj.PK_DesignObjVariation_DesignObj</param>
		/// <param name="intDesignObjID">DesignObj.PK_DesignObj</param>
		/// <param name="graphicsDir">the folder to graphic files</param>
		public UIDesignObj(UIDesignObjVariation objParentUIDesignObjVariation, int intID,
			int intDesignObjID, string graphicsDir)
		{
			this.graphicsDirectory = graphicsDir;

			m_objParentUIDesignObjVariation = objParentUIDesignObjVariation;

			ID = intDesignObjID;
			DOV_DO_ID = intID;

			if(objParentUIDesignObjVariation == null)
			{
				this.m_bitCanBeHidden = false;
				this.m_bitHideByDefault = false;
				this.m_bitRegenerateForEachScreen = false;
				this.ParentVariationLinked = false;
			}
			else
			{
				this.ParentVariationLinked = true;
			}


			if(intDesignObjID != -1)
			{
				LoadFromDatabase();
			}

			if(objParentUIDesignObjVariation == null)
			{
				Build(1, true); // Start with default skin
				this.ResetLinkOriginals();
			}
		}


		// DESTRUCTOR LIKE

		public void ReleaseBitmaps()
		{
			foreach (UIDesignObjVariation uiov in this.m_alUIDesignObjVariations)
			{
                uiov.ReleaseBitmaps();				
			}
		}



		//METHODS

		public void LoadFromDatabase()
		{
			MyDataSet mds = HADataConfiguration.m_mdsCache;
	
			DesignObjDataRow drDesignObj = mds.tDesignObj[m_intDesignObjID];

			m_strDescription = drDesignObj.fDescription;
			m_intPriority = drDesignObj.fPriority;
			this.m_blnAnimate = drDesignObj.fAnimate;
			m_intUIDesignObjType = drDesignObj.fFK_DesignObjType;

			m_blnCantGoBack = drDesignObj.fCantGoBack;
			


			this.DesignObjCategory = drDesignObj.fFK_DesignObjCategory;


			
			//set the originals so we know how and when to save
			this.ResetOriginals();


			m_blnNeedsDBInsert = false;
			m_blnNeedsDBDelete = false;
			
			//load the UIDesignObjVariations
			//no sort for now
			DataRow[] drVariations = mds.tDesignObjVariation.Select(DesignObjVariationData.FK_DESIGNOBJ_FIELD + "=" + m_intDesignObjID, DesignObjVariationData.PK_DESIGNOBJVARIATION_FIELD);

			foreach(DataRow dr in drVariations)
			{
				DesignObjVariationDataRow drVariation = new DesignObjVariationDataRow(dr);

				UIDesignObjVariation newObjVar = new UIDesignObjVariation(this, drVariation.fPK_DesignObjVariation, -1);
				m_alUIDesignObjVariations.Add(newObjVar);
			}

			if(m_objParentUIDesignObjVariation != null && DOV_DO_ID == -1)
			{
				DesignObjVariation_DesignObjDataRow drOVD = new DesignObjVariation_DesignObjDataRow(mds.tDesignObjVariation_DesignObj.NewRow());
				
				if(null != m_objParentUIDesignObjVariation)
					drOVD.fFK_DesignObjVariation_Parent = m_objParentUIDesignObjVariation.ID;
				else
					drOVD.fFK_DesignObjVariation_ParentSetNull();

				drOVD.fFK_DesignObj_Child = ID;
				mds.tDesignObjVariation_DesignObj.Rows.Add(drOVD.dr);
				mds.tDesignObjVariation_DesignObj.Update(1,mds.m_conn,mds.m_trans); // insert now to get PKID
				DOV_DO_ID = drOVD.fPK_DesignObjVariation_DesignObj;
			}

			UIChildSkinLanguage uidsl;
			if(m_intID != -1)
			{
				DataRow[] drDSL = mds.tDesignObjVariation_DesignObj_Skin_Language.Select(DesignObjVariation_DesignObj_Skin_LanguageData.FK_DESIGNOBJVARIATION_DESIGNOBJ_FIELD + "=" + m_intID);

				foreach(DataRow drc in drDSL)
				{
					DesignObjVariation_DesignObj_Skin_LanguageDataRow drOVDSL = new DesignObjVariation_DesignObj_Skin_LanguageDataRow(drc);
					uidsl = new UIChildSkinLanguage(this, 
						drOVDSL.fPK_DesignObjVariation_DesignObj_Skin_Language,
						drOVDSL.fFK_SkinIsNull ? -1 : drOVDSL.fFK_Skin, 
						drOVDSL.fFK_LanguageIsNull ? -1 : drOVDSL.fFK_Language,
						graphicsDirectory);
					this.alChildSkinLanguages.Add(uidsl);
				}
			}

			if (this.alChildSkinLanguages.Count==0) // No OVTSL rows for this OVT, Add a default row.
			{
				uidsl = new UIChildSkinLanguage(this,-1,-1,-1,graphicsDirectory);
				this.alChildSkinLanguages.Add(uidsl);
				uidsl.SaveToDatabase();
			}
		}

		public bool SaveToDatabase()
		{
			//returns whether anything has been saved to the database
			bool blnChanged = false;

			MyDataSet mds = HADataConfiguration.m_mdsCache;
			
			if(!this.Deleted)
			{
				if(this.NeedsDelete)
				{

					//TODO nlink from ALL parent variations
					this.NeedsParentVariationUnlink = true;
					DataRow[] drLinks = mds.tDesignObjVariation_DesignObj.Select(DesignObjVariation_DesignObjData.FK_DESIGNOBJ_CHILD_FIELD + "=" + m_intDesignObjID, null);
					foreach(DataRow dr in drLinks)
					{
						dr.Delete();
					}
					mds.tDesignObjVariation_DesignObj.Update(1,mds.m_conn,mds.m_trans);

					//delete the variations
					foreach(Object obj in this.UIDesignObjVariations)
					{
						UIDesignObjVariation objVariation = (UIDesignObjVariation) obj;
						objVariation.NeedsDelete = true;
						objVariation.SaveToDatabase();
					}

					if(!this.NeedsInsert)
					{
						//delete this object
						DesignObjDataRow drDesignObj = mds.tDesignObj[m_intDesignObjID];
						drDesignObj.dr.Delete();
						mds.tDesignObj.Update(1,mds.m_conn,mds.m_trans);
					}

					blnChanged = true;
					m_blnDeleted = true;
				}
				else
				{

					if(this.NeedsInsert)
					{
						//insert this object
						DesignObjDataRow drDesignObj = new DesignObjDataRow(mds.tDesignObj.NewRow());
						drDesignObj.fDescription = m_strDescription;
						drDesignObj.fCantGoBack = m_blnCantGoBack;
						drDesignObj.fPriority = m_intPriority;
						drDesignObj.fAnimate = this.m_blnAnimate;
						drDesignObj.fFK_DesignObjType = m_intUIDesignObjType;
						drDesignObj.fFK_DesignObjCategory = this.DesignObjCategory;
						mds.tDesignObj.Rows.Add(drDesignObj.dr);

						//we need the new id for the new variations below
						mds.tDesignObj.Update(1,mds.m_conn,mds.m_trans);
				
						this.m_intDesignObjID = drDesignObj.fPK_DesignObj;

						blnChanged = true;

					}
					else
					{
						if(this.OriginalsChanged)
						{
							//update this object
							DesignObjDataRow drDesignObj = mds.tDesignObj[m_intDesignObjID];

							drDesignObj.fDescription = m_strDescription;
							drDesignObj.fCantGoBack = m_blnCantGoBack;
							drDesignObj.fPriority = m_intPriority;
							drDesignObj.fAnimate = this.m_blnAnimate;
							drDesignObj.fFK_DesignObjType = m_intUIDesignObjType;
							drDesignObj.fFK_DesignObjCategory = this.DesignObjCategory;
							
							blnChanged = true;

						}
					}

					//Save link changes
					blnChanged = this.SaveLinkToDatabase() || blnChanged;

					//save all variations to database
					//this must be done after an object insertion because we need the new object id
					foreach(Object obj in this.UIDesignObjVariations)
					{
						UIDesignObjVariation objVariation = (UIDesignObjVariation) obj;
						blnChanged = objVariation.SaveToDatabase() || blnChanged;
					}

					foreach(Object obj in this.ChildSkinLanguages)
					{
						UIChildSkinLanguage child = (UIChildSkinLanguage)obj;
						if(child.SaveToDatabase())
							blnChanged = true;
					}
				}
	
				//set the originals so we know how and when to save
				this.ResetOriginals();


				this.NeedsInsert = false;
				this.NeedsDelete = false;

			}


			//if this is the root node, then update the tables if needed
			if(this.ParentUIDesignObjVariation == null && blnChanged)
			{
				mds.Update(1,mds.m_conn,mds.m_trans);
			}

			return blnChanged;
		}
	
		public bool SaveLinkToDatabase()
		{
			MyDataSet mds = HADataConfiguration.m_mdsCache;
			bool blnChanged = false;

			if(this.NeedsParentVariationUnlink)
			{
				if(!this.NeedsParentVariationLink)
				{
					if(this.ParentUIDesignObjVariation == null)
					{
						//this should not happen unless it was a deletion of a root object
						//in which case all links were removed in SaveToDatabase
					}
					else
					{
						//delete the language - skin children
						foreach(UIChildSkinLanguage objChild in alChildSkinLanguages)
						{
							objChild.NeedsDelete = true;
							objChild.SaveToDatabase();
						}
						
						//delete the row
						DesignObjVariation_DesignObjDataRow drLink = mds.tDesignObjVariation_DesignObj[DOV_DO_ID];
						drLink.dr.Delete();
						mds.tDesignObjVariation_DesignObj.Update(1,mds.m_conn,mds.m_trans);
					}
				}

				this.ParentVariationLinked = false;
				blnChanged = true;
			}
			else
			{
				//TODO Ender

				//set the originals
				this.ResetLinkOriginals();
			}

			return blnChanged;

		}

		public UIChildSkinLanguage GetCurrentChildSkinLanguage(int languageID, int skinID, ref UIDesignObjVariation objVariation)
		{
			bool bNoneSelected = true;
			foreach(Object obj in this.UIDesignObjVariations)
			{
				objVariation = (UIDesignObjVariation) obj;
				if(objVariation.Selected)
				{
					bNoneSelected = false;
					break;
				}
			}

			foreach(Object obj in this.UIDesignObjVariations)
			{
				objVariation = (UIDesignObjVariation) obj;
				//only draw the selected variations if this is the root
				if(bNoneSelected || objVariation.Selected)
				{
					bool matchSkin = false;
					bool matchLanguage = false;
					UIChildSkinLanguage uidslMatch = null;
		
					foreach (UIChildSkinLanguage uidsl in this.ChildSkinLanguages)
					{
						if (uidsl.SkinID == skinID && uidsl.LanguageID == languageID)
						{
							uidslMatch = uidsl;
							break;
						}
						else if (uidsl.SkinID == skinID && uidsl.LanguageID == -1)
						{
							uidslMatch = uidsl;
							matchSkin = true;
						}
						else if (uidsl.LanguageID == languageID && uidsl.SkinID == -1 && !matchSkin)
						{
							uidslMatch = uidsl;
							matchLanguage = true;
						}
						else if (uidsl.LanguageID == -1 && uidsl.SkinID == -1 && !matchSkin && !matchLanguage)
						{
							uidslMatch = uidsl;
						}
					}
					if (uidslMatch != null)
					{
						return uidslMatch;
					}
				}
			}

			objVariation = null;
			return null;
		}

		public UIChildSkinLanguage GetCurrentChildSkinLanguage(int languageID, int skinID)
		{
			UIDesignObjVariation objVariation = null;
			return GetCurrentChildSkinLanguage(languageID, skinID, ref objVariation);
		}

		public void Draw(Graphics objGraphics, int languageID, int skinID)
		{

			//if this is the top level object, reset whether the mainBackground has been drawn
			if(this.ParentUIDesignObjVariation == null)
			{
				this.MainBackgroundDrawn = false;
			}

			if(this.Include && this.Show)	//don't include if deleted or set to be deleted or unlinked
			{
				//determine how to show.  this will depend on what variation is selected and how they want things
				//they may want to hide non selected vriations, maybe outlined or ghosted
				UIDesignObjVariation objVariation = null;
				UIChildSkinLanguage obj = GetCurrentChildSkinLanguage(languageID, skinID, ref objVariation);
				if(null != obj)
					obj.Draw(objGraphics, objVariation);
			}
		}

		public void Reset()
		{
			foreach(Object obj in m_alUIDesignObjVariations)
			{
				UIDesignObjVariation objUIDesignObjVariation = (UIDesignObjVariation) obj;
				objUIDesignObjVariation.Reset();
			}
		}

		public void Build(int SkinID)
		{
			this.Build(SkinID, false);
		}

		public void Build(int SkinID, bool SkinChanged)
		{
			if(this.Include)	//don't include if deleted or set to be deleted or unlinked
			{
				//RECURSE
				foreach(UIDesignObjVariation objUIDesignObjVariation in m_alUIDesignObjVariations)
				{
					objUIDesignObjVariation.Build(SkinID, SkinChanged);
				}

				foreach(UIChildSkinLanguage objChild in alChildSkinLanguages)
					objChild.Build(SkinID);
			}
		}

		public bool SelectVariationIndex(int intIndex)
		{
			UIDesignObjVariation objVariation = (UIDesignObjVariation) this.UIDesignObjVariations[intIndex];
			objVariation.Selected = true;
			return true;
		}
		public bool DeSelectVariationIndex(int intIndex)
		{
			UIDesignObjVariation objVariation = (UIDesignObjVariation) this.UIDesignObjVariations[intIndex];
			objVariation.Selected = false;
			return true;
		}

		public bool Contains(UIDesignObjVariation objVariation, int intX, int intY, int languageID, int skinID)
		{
			UIDesignObjVariation objvar = null;
			UIChildSkinLanguage obj = GetCurrentChildSkinLanguage(intX, intY, ref objvar);
			if(null != obj)
			{
				return obj.Contains(intX, intY);
			}
			
			return false;
		}

		public bool ClickImage(int intX, int intY, int languageID, int skinID)
		{
			ArrayList alFound = new ArrayList();

			foreach(Object obj in this.UIDesignObjVariations)
			{
				UIDesignObjVariation objVariation = (UIDesignObjVariation) obj;
				if(objVariation.Include)
				{
					if(objVariation.Show)
					{
						//go thru and see if any DesignObjs have been clicked
						foreach(Object obj2 in objVariation.DesignObjs)
						{
							UIDesignObj objUIDesignObj = (UIDesignObj)obj2;
							if(objUIDesignObj.Contains(objVariation, intX, intY, languageID, skinID))
							{
								alFound.Add(objUIDesignObj);
							}
							else
							{
								objUIDesignObj.Selected = false;
							}
						}
					}
				}
			}


			//TODO this will change when we have multiple select capability
			//now go thru the found array
			//if none are selected, select the first one
			//else select the next one
			
			//int intNewSelectedIndex = -1;
			
			if(alFound.Count > 0)
			{
				this.DeselectAllText(); // TODO: integrate text into this procedure

				if(alFound.Count == 1)
				{
					//toggle the item
					UIDesignObj objUIDesignObj = (UIDesignObj) alFound[0];
					objUIDesignObj.Selected = !objUIDesignObj.Selected;

					//deselect the old one
					if(m_UIDesignObjSelected != null && m_UIDesignObjSelected != objUIDesignObj)
					{
						m_UIDesignObjSelected.Selected = false;
					}

					if(objUIDesignObj.Selected)
					{
						m_UIDesignObjSelected = objUIDesignObj;
					}
					else
					{
						m_UIDesignObjSelected = null;
					}

				}
				else
				{
					int intSelectedIndex = -1;
					int intNewSelectedIndex = -1;

					
					//find the selected item if there is one
					for(int intCount=0; intCount < alFound.Count; intCount++)
					{
						UIDesignObj objUIDesignObj = (UIDesignObj) alFound[intCount];
						if(objUIDesignObj.Selected) 
						{
							intSelectedIndex = intCount;
							break;
						}

					}
					if(intSelectedIndex == -1)
					{
						//none selected, so select the first one
						intNewSelectedIndex = 0;
					}
					else
					{
						if(intSelectedIndex == (alFound.Count - 1))
						{
							//we are at the end of the line, so loop around to 0
							intNewSelectedIndex = 0;
						}
						else
						{
							intNewSelectedIndex = intSelectedIndex + 1;
						}
					}

					
					

					if(intSelectedIndex != -1)
					{
						UIDesignObj objUIDesignObjSelectedTemp;
						objUIDesignObjSelectedTemp = (UIDesignObj) alFound[intSelectedIndex];
						objUIDesignObjSelectedTemp.Selected = false;
					}

					UIDesignObj objNewUIDesignObjSelectedTemp;
					objNewUIDesignObjSelectedTemp = (UIDesignObj) alFound[intNewSelectedIndex];
					objNewUIDesignObjSelectedTemp.Selected = true;

					if(m_UIDesignObjSelected != null)
					{
						m_UIDesignObjSelected.Selected = false;
					}
					m_UIDesignObjSelected = objNewUIDesignObjSelectedTemp;

				}
			}

			return (alFound.Count > 0);

		}


		public UIDesignObjVariation AddNewVariation(int intCriteria_DID, bool blnSelect, int SkinID)
		{
			//make sure that a variation with the give criteria id is not already there
			bool blnFound = false;
			foreach(Object obj in this.UIDesignObjVariations)
			{
				UIDesignObjVariation objVariation = (UIDesignObjVariation) obj;
				if (objVariation.CriteriaID == intCriteria_DID && objVariation.Include)
				{
					blnFound = true;
					break;
				}
			}
			if(!blnFound)
			{
				UIDesignObjVariation objVariation = new UIDesignObjVariation(this, -1, intCriteria_DID);
				objVariation.NeedsInsert = true;
				this.UIDesignObjVariations.Add(objVariation);
				objVariation.Selected = blnSelect;

				this.Build(SkinID, true); //we need to rebuild

				//return true;
				return objVariation;
			}
			else
			{
				//return false;
				return null;
			}
		}


		public void DeselectAllDesignObjs(bool blnRecursive) // Also Text
		{
			foreach(UIDesignObjVariation objVariation in this.UIDesignObjVariations)
			{
				foreach(UIDesignObj objUIDesignObj in objVariation.DesignObjs)
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
		public void DeselectAllText()
		{
			foreach(UIDesignObjVariation objVariation in this.UIDesignObjVariations)
			{
				foreach (UIText uit in objVariation.Text)
				{
					uit.Selected = false;
				}
			}
		}
		public void DeselectAllVariations(bool blnRecursive)
		{
			foreach(Object obj in this.UIDesignObjVariations)
			{
				UIDesignObjVariation objVariation = (UIDesignObjVariation) obj;
				objVariation.Selected = false;
				if(blnRecursive)
				{
					foreach(UIDesignObj objUIDesignObj in objVariation.DesignObjs)
					{
						objUIDesignObj.DeselectAllVariations(true);
					}
				}
			}
		}


		public void ResetOriginals()
		{
			m_strDescriptionOriginal = m_strDescription;
			m_blnCantGoBackOriginal = m_blnCantGoBack;
			m_intPriorityOriginal = m_intPriority;
			this.m_blnAnimateOriginal = this.m_blnAnimate;
			m_intUIDesignObjTypeOriginal = m_intUIDesignObjType;
			m_intDesignObjCategoryIDOriginal = m_intDesignObjCategoryID;
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
			this.ParentDisplayOrderOriginal = this.ParentDisplayOrder;
		}

		public void DesignObjModified(DesignObjDataRow drObject)
		{
			MyDataSet mds = HADataConfiguration.m_mdsCache;
			
			string SQL = "UPDATE DesignObj SET psc_mod='" + DateTime.Now.ToString("yyMMddhhmmss") + "' WHERE PK_DesignObj=" + drObject.fPK_DesignObj.ToString();
			new MySql.Data.MySqlClient.MySqlCommand(SQL,mds.m_conn,mds.m_trans).ExecuteNonQuery();

			/*
			int PK_DesignObj=0;

			// Here are all the variations that depend on us
			foreach(DataRow dr in drObject.dr.GetChildRows(MyDataSet.MyRelations.DesignObjVariation_DesignObj_FK_DesignObj_Child) )
			{
				DesignObjVariation_DesignObjDataRow drV_O = new DesignObjVariation_DesignObjDataRow(dr);
				if( drV_O.fFK_DesignObjVariation_Parent_DataRow.fFK_DesignObj!=PK_DesignObj )
				{
					// Minor optimization, if we hit the same object twice in a row don't reset it twice
					PK_DesignObj = drV_O.fFK_DesignObjVariation_Parent_DataRow.fFK_DesignObj;
					DesignObjModified(drV_O.fFK_DesignObjVariation_Parent_DataRow.fFK_DesignObj_DataRow);
				}
			}
			*/
		}

		public void SetDefaultPosition(int x, int y)
		{
			UIChildSkinLanguage child = GetCurrentChildSkinLanguage(-1, -1);
			if(null != child)
			{
				child.ParentX = x;
				child.ParentY = y;
			}
		}

		public void SetDefaultSize(int Width, int Height)
		{
			UIChildSkinLanguage child = GetCurrentChildSkinLanguage(-1, -1);
			if(null != child)
			{
				child.Width = Width;
				child.Height = Height;
			}
		}

		public void SetDefaultIsTabStop(bool IsTabStop)
		{
			UIChildSkinLanguage child = GetCurrentChildSkinLanguage(-1, -1);
			if(null != child)
			{
				child.IsTabStop = IsTabStop;
			}
		}
	}
}
