using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Windows.Forms;
using HAData.Common;
using HAData.DataAccess;
using System.Data;


namespace HADesigner
{
	/// <summary>
	/// Manage UIDesignObjVariationTextSkinLanguage table
	/// </summary>
	public class UITextSkinLanguage: UI
	{
		// to make public fields private, rename & create property with this name
		public UIText ParentUIText;
		
		public int SkinID = -1;
		public int LanguageID = -1;
		
		public int X=0;
		public int Y=0;
		public int Width=0;
		public int Height=0;
		public int Rotate=0;
		public int Opacity=100;
		public int BGColor = SystemColors.Control.ToArgb();
		private int HorizontalAlignmentID = -1; 
		private int VertAlignmentID = -1;
		private int styleID = -1;
		public int StyleID
		{
			get	{return this.styleID;}
			set	{this.updateStyle(value);}
		}

		private int origX=0;
		private int origY=0;
		private int origWidth=0;
		private int origHeight=0;
		private int origRotate=0;
		private int origOpacity=100;
		private int origBGColor = SystemColors.Control.ToArgb();
		private int origHorizontalAlignmentID = -1;
		private int origVertAlignmentID = -1;
		private int origStyleID = -1;

		private UIStyleVariation style = new UIStyleVariation();

		public bool WarnToSave
		{
			get {return (NeedsInsert||NeedsDelete||OriginalsChanged);}
		}

		private StringAlignment HorizontalAlignment
		{
			get {return (StringAlignment) (this.style.HorizontalAlignmentID-1);}
		}
		private StringAlignment VerticalAlignment
		{
			get	{return (StringAlignment) (this.style.VerticalAlignmentID-1);}
		}

		public int HAlignmentSelection
		{
			get {return (this.HorizontalAlignmentID == -1) ? 0 : this.HorizontalAlignmentID;}
			set
			{
				this.HorizontalAlignmentID = (value==0) ? -1 : value;
				this.updateStyle();
			}
		}
		public int VAlignmentSelection
		{
			get {return (this.VertAlignmentID == -1) ? 0 : this.VertAlignmentID;}
			set
			{
				this.VertAlignmentID = (value==0) ? -1 : value;
				this.updateStyle();
			}
		}

		/*
		public int AlignmentSelection
		{
			get
			{
				int hAlign = (this.HorizontalAlignmentID < 1 || this.HorizontalAlignmentID > 3) ? 0 : this.HorizontalAlignmentID-1;
				int vAlign = (this.VertAlignmentID < 1 || this.VertAlignmentID > 3) ? 0 : this.VertAlignmentID-1;
				return (vAlign*3 + hAlign);
			}
			set
			{
				this.VertAlignmentID = (value / 3) + 1;
                this.HorizontalAlignmentID = (value % 3) + 1;
				this.updateStyle();
			}
		}
		*/
	
		public string LanguageSkin
		{
			get
			{
				string language = (this.LanguageID == -1) ? "Default" : mds.tLanguage[this.LanguageID].fDescription;
				string skin = (this.SkinID == -1) ? "Default" : mds.tSkin[this.SkinID].fDescription;
				return language + " - " + skin;
			}
		}

		private DesignObjVariation_Text_Skin_LanguageDataRow drOVTSL
		{
			get {return mds.tDesignObjVariation_Text_Skin_Language[this.id];}
		}

		public int DesignObjVariation_TextID
		{
			get {return this.ParentUIText.ID;}
		}



		/// <summary>
		/// UITextSkinLanguage constructor
		/// </summary>
		/// <param name="parentUIText">parent UIText object</param>
		/// <param name="id">DesignObjVariation_Text_Skin_Language.PK_DesignObjVariation_Text_Skin_Language</param>
		public UITextSkinLanguage(UIText parentUIText, int id)
		{
			this.ParentUIText = parentUIText;
			this.id = id;
			
			if (id == -1)
			{
				this.NeedsInsert = true;
				this.updateStyle();
			}
			else this.LoadFromDatabase();
		}
        
		public override bool OriginalsChanged
		{
			get	{return (origX != X || origY != Y || origWidth != Width || origHeight != Height || origRotate != Rotate || origOpacity != Opacity || this.origBGColor != this.BGColor || origHorizontalAlignmentID != HorizontalAlignmentID || origVertAlignmentID != VertAlignmentID || origStyleID != StyleID);}
		}

		public void Draw(Graphics g, string text, int parentX, int parentY)
		{
			if (this.Include)
			{
				UIChildSkinLanguage obj = ParentUIText.ParentUIDesignObjVariation.ParentUIDesignObj.GetCurrentChildSkinLanguage(LanguageID, SkinID);

				int objectWidth = obj.Width;
				int objectHeight = obj.Height;
				int width = (this.Width==0)? objectWidth - this.X : this.Width;
				int height = (this.Height==0)? objectHeight - this.Y : this.Height;

				Font f = new Font(this.style.Font,this.style.EMHeight,this.style.FontStyle);

				//Rectangle r = new Rectangle(parentX + this.X, parentY + this.Y, width, height);
				Rectangle r = new Rectangle(0, 0, width, height);
				StringFormat sf = new StringFormat();
				sf.Alignment = this.HorizontalAlignment;
				sf.LineAlignment = this.VerticalAlignment;
			
				
				// g.MeasureString

				g.TranslateTransform(parentX + this.X, parentY + this.Y, MatrixOrder.Append);
				g.RotateTransform(this.Rotate, MatrixOrder.Prepend);
				
				Brush b = new System.Drawing.SolidBrush(Color.FromArgb( Convert.ToInt32(this.Opacity*2.55), Color.FromArgb( this.style.Color )));
				g.TextRenderingHint = TextRenderingHint.AntiAlias;

				g.DrawString(text,f,b,r,sf);
				g.RotateTransform(0-this.Rotate, MatrixOrder.Prepend);
				if (this.ParentUIText.Selected) g.DrawRectangle(new Pen(Color.Red,4),r.X,r.Y,r.Width-4,r.Height-4);
				g.TranslateTransform(-(parentX + this.X),-(parentY + this.Y), MatrixOrder.Append);
			}
		}

		private bool canLoadFromDB()
		{
			// IF PROGRAM IS CORRECT, These 3 Error Messages should never show !
			if (this.ID==-1)
			{
				MessageBox.Show("Error: Tried to load Text from Database & ID not set");
				return false;
			}
			else if (this.ParentUIText==null)
			{
				MessageBox.Show("Error: Tried to load Text from Database & Text not set");
				return false;
			}
			else if (this.DesignObjVariation_TextID != this.drOVTSL.fFK_DesignObjVariation_Text)
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
				this.SkinID = drOVTSL.fFK_SkinIsNull ? -1 : drOVTSL.fFK_Skin;
				this.LanguageID = drOVTSL.fFK_LanguageIsNull ? -1 : drOVTSL.fFK_Language;
                this.HorizontalAlignmentID = drOVTSL.fFK_HorizAlignmentIsNull ? -1 : drOVTSL.fFK_HorizAlignment;
				this.VertAlignmentID = drOVTSL.fFK_VertAlignmentIsNull ? -1 : drOVTSL.fFK_VertAlignment;
				this.StyleID = drOVTSL.fFK_StyleIsNull ? -1 : drOVTSL.fFK_Style;
				
				this.X = drOVTSL.fX;
				this.Y = drOVTSL.fY;
				this.Width = drOVTSL.fWidth;
				this.Height = drOVTSL.fHeight;
				this.Rotate = drOVTSL.fRotate;
				this.Opacity = drOVTSL.fOpacity;
				this.BGColor = drOVTSL.fPlainBackgroundColorIsNull ? SystemColors.Control.ToArgb() : drOVTSL.fPlainBackgroundColor;
				this.updateStyle();
			}

			this.needsDBInsert = false; // What if didn't load correctly ?
			this.needsDBDelete = false; // SURE DON'T WANT TO DELETE if didn't LOAD correctly!
			this.ResetOriginals();
		}


		public UITextSkinLanguage Copy(UIText parentUIT)
		{
			UITextSkinLanguage copy = new UITextSkinLanguage(parentUIT, -1);

			copy.SkinID = this.SkinID;
			copy.LanguageID = this.LanguageID;
			copy.X = this.X;
			copy.Y = this.Y;
			copy.Width = this.Width;
			copy.Height = this.Height;
			copy.Rotate = this.Rotate;
			copy.Opacity = this.Opacity;
			copy.BGColor = this.BGColor;
			copy.HorizontalAlignmentID = this.HorizontalAlignmentID;
			copy.VertAlignmentID = this.VertAlignmentID;
			copy.styleID = this.styleID;

			copy.ResetOriginals();
			copy.updateStyle();
			return copy;
		}

		public void updateStyle()
		{
			this.updateStyle(this.styleID);
		}
	
	
		private void updateStyle(int styleID)
		{
			this.styleID = styleID;

			int realStyleID = this.StyleID;
			StyleVariationDataRow drStyleV = null;
				
			if (this.StyleID == -1 && this.SkinID != -1 && !mds.tSkin[this.SkinID].fFK_StyleIsNull) // try to get style from skin
			{
				realStyleID = mds.tSkin[this.SkinID].fFK_Style;
			}

			if (realStyleID == -1) realStyleID = 1; // style not found, set to basic.
			
			StyleDataRow drStyle = mds.tStyle[realStyleID];
			DataRow[] drsStyleV = drStyle.dr.GetChildRows(MyDataSet.MyRelations.StyleVariation_FK_Style);
			foreach (DataRow dr in drsStyleV)
			{
				StyleVariationDataRow drStyleVPossible = new StyleVariationDataRow(dr);

				int styleCriteria = drStyleVPossible.fFK_UIIsNull ? -1 : drStyleVPossible.fFK_UI;
				if (styleCriteria == this.ParentUIText.ParentUIDesignObjVariation.CriteriaID)
				{
					drStyleV = drStyleVPossible;
					break;
				}
				else if (styleCriteria == -1) // default criteria if didn't find exact match.
				{
					drStyleV = drStyleVPossible;
				}
			}
			
			if (drStyleV != null && drStyleV.bIsValid)
			{
				this.style.Font = drStyleV.fFont;
				this.style.PixelHeight = drStyleV.fPixelHeight;
				this.style.Color = drStyleV.fForeColor;
				this.style.Bold = drStyleV.fBold;
				this.style.Italic = drStyleV.fItalic;
				this.style.Underline = drStyleV.fUnderline;
				
				this.style.HorizontalAlignmentID = (this.HorizontalAlignmentID == -1) ? drStyleV.fFK_HorizAlignment : this.HorizontalAlignmentID;
				this.style.VerticalAlignmentID = (this.VertAlignmentID == -1) ? drStyleV.fFK_VertAlignment : this.VertAlignmentID;
			}
			else
			{
				this.style.HorizontalAlignmentID = (this.HorizontalAlignmentID == -1) ? 1 : this.HorizontalAlignmentID;
				this.style.VerticalAlignmentID = (this.VertAlignmentID == -1) ? 1 : this.VertAlignmentID;
			}
		}

		private void setDataRowFields(DesignObjVariation_Text_Skin_LanguageDataRow drOVTSL)
		{
			drOVTSL.fFK_DesignObjVariation_Text = this.DesignObjVariation_TextID;
					
			if (this.SkinID == -1) drOVTSL.fFK_SkinSetNull();
			else drOVTSL.fFK_Skin = this.SkinID;

			if (this.LanguageID == -1) drOVTSL.fFK_LanguageSetNull();
			else drOVTSL.fFK_Language = this.LanguageID;

			if (this.HorizontalAlignmentID == -1) drOVTSL.fFK_HorizAlignmentSetNull();
			else drOVTSL.fFK_HorizAlignment = this.HorizontalAlignmentID;

			if (this.VertAlignmentID == -1) drOVTSL.fFK_VertAlignmentSetNull();
			else drOVTSL.fFK_VertAlignment = this.VertAlignmentID;

			if (this.StyleID == -1) drOVTSL.fFK_StyleSetNull();
			else drOVTSL.fFK_Style = this.StyleID;

			drOVTSL.fX = this.X;
			drOVTSL.fY = this.Y;
			drOVTSL.fWidth = this.Width;
			drOVTSL.fHeight = this.Height;
			drOVTSL.fRotate = this.Rotate;
			drOVTSL.fOpacity = this.Opacity;

			if (this.BGColor != SystemColors.Control.ToArgb())
			{
				drOVTSL.fPlainBackgroundColor = this.BGColor;
			}
		}

		public override bool SaveToDatabase()
		{
			bool changed = false;
	
			if(!this.Deleted)
			{
				if(this.NeedsDelete)
				{
					this.drOVTSL.dr.Delete();
					this.deleted = true;
					changed = true;
				}
				else if(this.NeedsInsert) // Insert to OVTSL
				{
					DesignObjVariation_Text_Skin_LanguageDataRow drOVTSL = new DesignObjVariation_Text_Skin_LanguageDataRow(mds.tDesignObjVariation_Text_Skin_Language.NewRow());
					this.setDataRowFields(drOVTSL);
					mds.tDesignObjVariation_Text_Skin_Language.Rows.Add(drOVTSL.dr);
					mds.tDesignObjVariation_Text_Skin_Language.Update(1,mds.m_conn,mds.m_trans); // insert now to get PKID
					this.id = drOVTSL.fPK_DesignObjVariation_Text_Skin_Language;
					changed = true;
				}
				else if(this.OriginalsChanged) //update this object
				{
					this.setDataRowFields(this.drOVTSL);
					changed = true;
				}
                
				this.ResetOriginals();
				this.NeedsInsert = false;
				this.NeedsDelete = false;
			}
			return changed;
		}

		public void SaveLinkToDatabase()
		{
			DesignObjVariation_Text_Skin_LanguageDataRow drOVTSL = mds.tDesignObjVariation_Text_Skin_Language[this.id];
			drOVTSL.fFK_DesignObjVariation_Text = this.ParentUIText.ID;
			mds.tDesignObjVariation_Text_Skin_Language.Update(1,mds.m_conn,mds.m_trans);
		}

		protected override void ResetOriginals()
		{
			this.origX = this.X;
			this.origY = this.Y;
			this.origWidth = this.Width;
			this.origHeight = this.Height;
			this.origRotate = this.Rotate;
			this.origOpacity = this.Opacity;
			this.origBGColor = this.BGColor;
			this.origHorizontalAlignmentID = this.HorizontalAlignmentID;
			this.origVertAlignmentID = this.VertAlignmentID;
			this.origStyleID = this.StyleID;
		}
	}




	public class UIStyleVariation
	{
		// public int id = -1; // may need later

		public string Font = "Arial";
		
		private int pixelHeight = 50;
		public int PixelHeight
		{
			get {return this.pixelHeight;}
			set
			{
				this.pixelHeight = this.emHeight = value;
				
				Font f = new Font(this.Font,value);
				float ls = f.FontFamily.GetLineSpacing(this.FontStyle);
				// MessageBox.Show(f.Height.ToString()); maybe this is the height to use ???
				float em = f.FontFamily.GetEmHeight(this.FontStyle);

				this.emHeight = (int) (value * em/ls);
			}
		}
        
		private int emHeight = 43;
		public int EMHeight
		{
			get {return this.emHeight;}
		}

		public bool Bold = false;
		public bool Italic = false;
		public bool Underline = false;
		public int Color = -1; // black: -16777216 white: -1

		public FontStyle FontStyle
		{
			get
			{
				FontStyle fs = FontStyle.Regular;
				if (this.Bold) fs |= FontStyle.Bold;
				if (this.Italic) fs |= FontStyle.Italic;
				if (this.Underline) fs |= FontStyle.Underline;
				return fs;
			}
		}

		public int HorizontalAlignmentID = 1;
		public int VerticalAlignmentID = 1;
        
		public UIStyleVariation()
		{
		}
	}
}