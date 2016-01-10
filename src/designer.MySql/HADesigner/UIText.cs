using System;
using System.Collections;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using HAData.Common;
using HAData.DataAccess;


namespace HADesigner
{
	/// <summary>
	/// Represents a particular DesignObjVariation_Text row  shown in a particular language// _Skin_Language row. ???
	/// ID represents Text table
	/// </summary>
	public class UIText: UI
	{
		//MEMBER VARIABLES

		private UIDesignObjVariation parentUIDesignObjVariation;
		private int textID;
		private int textID_OverrideFromHeader = -1;

		private string text_description; // from Text

		private bool selected = false;

		private ArrayList alTextSkinLanguage = new ArrayList();

		// NOTHING CAN CHANGE in ovt BUT WE CAN STILL CREATE OR DELETE ROWS

		//PROPERTIES

		public bool WarnToSave
		{
			get 
			{
				if (NeedsInsert||NeedsDelete||OriginalsChanged) return true;
				//save all parameters to database
				foreach(object uiTSL in this.alTextSkinLanguage)
				{
					if (((UITextSkinLanguage)uiTSL).WarnToSave) return true;
				}
				return false;
			}
		}

		private DesignObjVariation_TextDataRow drOVT
		{
			get {return mds.tDesignObjVariation_Text[this.ID];}
		}

		/// <summary>
		/// List of UIText_Skin_Language objects
		/// </summary>
		public ArrayList TextSkinLanguages
		{
			get {return this.alTextSkinLanguage;}
			set {this.alTextSkinLanguage = value;}
		}

		/// <summary>
		/// Set in Constructor
		/// </summary>
		public UIDesignObjVariation ParentUIDesignObjVariation
		{
			get	{return this.parentUIDesignObjVariation;}
			set	{this.parentUIDesignObjVariation = value;}
		}

		/// <summary>
		/// Get ID from ParentUIDesignObjVariation
		/// </summary>
		public int DesignObjVariationID
		{
			get	{return this.parentUIDesignObjVariation.ID;}
		}

		/// <summary>
		/// Set in Constructor
		/// </summary>
 		public int TextID
		{
			get	{return this.textID;}
			set	{this.textID = value;}
		}

		/// <summary>
		/// Not set in constructor, allows null, TODO: implement
		/// </summary>
		public int TextID_OverrideFromHeader
		{
			get {return this.textID_OverrideFromHeader;}
			set	{this.textID_OverrideFromHeader = value;}
		}

		/// <summary>
		/// Set in constructor: From Table Text
		/// </summary>
		public string Description
		{
			get {return TextID + " " + text_description;}
			set	{this.text_description = value;}
		}
						
		public override bool OriginalsChanged
		{
			get	{return false;} // NOTHING CAN CHANGE !
		}

		public bool Selected
		{
			get {return this.selected;}
			set {this.selected = value;}
		}

		//CONSTRUCTORS
		public UIText()
		{
			this.id = this.textID = -1;
		}

		/// <summary>
		/// Manage DesignObjVariation_Text Table
		/// IntID should be -1 for new row
		/// Otherwise can set textID to -1, but must specify languageID anyway
		/// id - DesignObjVariation_Text.PK_DesignObjVariation_Text
		/// textid - Text.PK_Text
		/// </summary>
		public UIText(UIDesignObjVariation objUIDesignObjVariation, int id, int textID)
		{
			this.parentUIDesignObjVariation = objUIDesignObjVariation;
			this.id = id;
			this.textID = textID;

			if (id == -1)
			{
				this.NeedsInsert = true;
                
				TextDataRow tdr = mds.tText[textID];
                this.text_description = tdr.fDescription;
			}
            else this.LoadFromDatabase();
		}
		
		//METHODS
		private bool canLoadFromDB()
		{
			// IF PROGRAM IS CORRECT, These 3 Error Messages should never show !
			if (this.ID==-1)
			{
				MessageBox.Show("Error: Tried to load Text from Database & ID not set");
				return false;
			}
			else if (this.ParentUIDesignObjVariation==null)
			{
				MessageBox.Show("Error: Tried to load Text from Database & DesignObj Variation not set");
				return false;
			}
			else if (this.DesignObjVariationID != this.drOVT.fFK_DesignObjVariation)
			{
				MessageBox.Show("Error: DesignObj Variation ID in DesignObjVariation_Text does not match ID from DesignObjVariation");
				return false;
			}
			else return true;
		}

		public override void LoadFromDatabase()
		{
			// (remember to set null values to "" for str & -1 for int) 
			// ID & ParentUIDesignObjVariation already set
			if (this.canLoadFromDB())
			{
				TextDataRow drt = mds.tText[this.textID];
				
				this.TextID = drOVT.fFK_Text; // not null
				this.TextID_OverrideFromHeader = drOVT.fFK_Text_OverrideFromHeaderIsNull ? -1 : drOVT.fFK_Text_OverrideFromHeader; // maybe null

				this.text_description = drt.fDescription;

				// get the text_skin_language
				DataRow[] drTSL = drOVT.dr.GetChildRows(MyDataSet.MyRelations.DesignObjVariation_Text_Skin_Language_FK_DesignObjVariation_Text);
					//drVariation.dr.GetChildRows(MyDataSet.MyRelations.DesignObjVariation_Text_FK_DesignObjVariation);
				UITextSkinLanguage uitsl;
				foreach(DataRow dr in drTSL)
				{
					DesignObjVariation_Text_Skin_LanguageDataRow drOVTSL = new DesignObjVariation_Text_Skin_LanguageDataRow(dr);
					uitsl = new UITextSkinLanguage(this, drOVTSL.fPK_DesignObjVariation_Text_Skin_Language);
					this.alTextSkinLanguage.Add(uitsl);
				}
				if (this.alTextSkinLanguage.Count==0) // No OVTSL rows for this OVT, Add a default row.
				{
                    uitsl = new UITextSkinLanguage(this,-1);
                    this.alTextSkinLanguage.Add(uitsl);
					uitsl.SaveToDatabase();
				}
			}
			this.needsDBInsert = false;
			this.needsDBDelete = false; // DON'T WANT TO DELETE if didn't LOAD correctly!
			this.ResetOriginals();
		}

		/// <summary>
		/// Returns whether anything has been saved to the database
		/// </summary>
		public override bool SaveToDatabase()
		{
			bool changed = false;
				
			if(!this.Deleted)
			{
				if(this.NeedsDelete)
				{
					DataRow[] drsOVTSL = drOVT.dr.GetChildRows(MyDataSet.MyRelations.DesignObjVariation_Text_Skin_Language_FK_DesignObjVariation_Text);
					foreach(DataRow drOVTSL in drsOVTSL)
					{
						drOVTSL.Delete();
					}
					mds.tDesignObjVariation_Text_Skin_Language.Update(1,mds.m_conn,mds.m_trans);
					drOVT.dr.Delete();

					this.deleted = true;
					changed = true;
				}
				else 
				{
					if(this.NeedsInsert)
					{
						// Insert to OVT

						DesignObjVariation_TextDataRow drOVT = new DesignObjVariation_TextDataRow(mds.tDesignObjVariation_Text.NewRow());
						drOVT.fFK_DesignObjVariation = this.DesignObjVariationID;
						drOVT.fFK_Text = this.TextID;
						if (this.TextID_OverrideFromHeader == -1) drOVT.fFK_Text_OverrideFromHeaderSetNull();
						else drOVT.fFK_Text_OverrideFromHeader = this.TextID_OverrideFromHeader; 

						mds.tDesignObjVariation_Text.Rows.Add(drOVT.dr);
						mds.tDesignObjVariation_Text.Update(1,mds.m_conn,mds.m_trans); // insert now to get PKID

						this.id = drOVT.fPK_DesignObjVariation_Text;

						//MessageBox.Show("New ID: "+this.id);

						changed = true;
					}
					else if(this.OriginalsChanged)
					{
						//update this object
						// Nothing to change, nothing to update
						// blnChanged = true; (commented cause nothing has changed)
					}
					foreach(object uiTSL in this.alTextSkinLanguage)
					{
						changed = ((UITextSkinLanguage)uiTSL).SaveToDatabase() || changed;
					}
				}

				this.ResetOriginals();
				this.NeedsInsert = false;
				this.NeedsDelete = false;
			}
			return changed;
		}

		public void SaveLinkToDatabase(bool SaveChildTextSkinLanguageRows)
		{
			DesignObjVariation_TextDataRow drOVT = mds.tDesignObjVariation_Text[this.id];
			drOVT.fFK_DesignObjVariation = this.parentUIDesignObjVariation.ID;
			mds.tDesignObjVariation_Text.Update(1,mds.m_conn,mds.m_trans);

			foreach (UITextSkinLanguage uiTSL in this.TextSkinLanguages)
			{
				uiTSL.updateStyle();
			}

			if (SaveChildTextSkinLanguageRows)
			{
				foreach (UITextSkinLanguage uiTSL in this.TextSkinLanguages)
				{
					uiTSL.SaveToDatabase();
					uiTSL.SaveLinkToDatabase();
				}
			}
		}

		protected override void ResetOriginals()
		{
			// NO ORIGINALS TO RESET
		}

		public string textLS_description(int languageID)
		{
            return this.textLS_description(languageID,true);           
		}
		public string textLS_description(int languageID, bool returnEnglish)
		{
            return mds.tText[this.textID].fDescription_LS(languageID,returnEnglish);
		}

		public void Draw(Graphics g, int parentX, int parentY)
		{
			int skinID = SkinLanguageStatus.Instance().GlobalSkinID;
			int languageID = SkinLanguageStatus.Instance().GlobalLanguageID;

			if (this.Include)
			{
				bool matchSkin = false;
				bool matchLanguage = false;
				UITextSkinLanguage uitslMatch = null;
				// int iExactMatch = iSkinMatch = iLanguageMatch = iDefault = -1;
				
				foreach (UITextSkinLanguage uitsl in this.alTextSkinLanguage)
				{
					if (uitsl.SkinID == skinID && uitsl.LanguageID == languageID)
					{
						uitslMatch = uitsl;
						break;
					}
					else if (uitsl.SkinID == skinID && uitsl.LanguageID == -1)
					{
						uitslMatch = uitsl;
						matchSkin = true;
					}
					else if (uitsl.LanguageID == languageID && uitsl.SkinID == -1 && !matchSkin)
					{
						uitslMatch = uitsl;
						matchLanguage = true;
					}
					else if (uitsl.LanguageID == -1 && uitsl.SkinID == -1 && !matchSkin && !matchLanguage)
					{
						uitslMatch = uitsl;
					}
				}
				if (uitslMatch != null)
				{
					uitslMatch.Draw(g,this.textLS_description(languageID), parentX, parentY);
				}
			}
		}

	}
}