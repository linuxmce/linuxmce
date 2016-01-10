using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

using System.Data;
using HAData.Common;
using HAData.DataAccess;

namespace HADesigner
{
	/// <summary>
	/// Summary description for Parameter.
	/// </summary>
	public class ParameterControl
	{
		private string defaultDirectory;

		public int FK_DesignObjParameter;
		public string Description;
		public TextBox TextBoxValue;

		public UIDesignObjVariationParameter UIDesignObjVariationParameter = null;
		public UIDesignObjVariationCommandParameter UIDesignObjVariationCommandParameter = null;

		// public Button FileButton; // NOT NEEDED, not doing anything to the actual button from here

		public Button ColorButton;   // NEEDED, setting the color of the button from here

		public Form ParentForm;

		public ParameterControl(string defaultDirectory)
		{
			this.defaultDirectory = defaultDirectory;
		}

		public void SelectColor(object sender, System.EventArgs e)
		{
			object o = ColorPicker.getColor();
			if (o!=null) 
			{
				this.ColorButton.BackColor = (Color)o;
				this.TextBoxValue.Text = this.ColorButton.BackColor.ToArgb().ToString();
				this.UpdateValue();
			}
		}

		public void OpenFile(object sender, System.EventArgs e)
		{
			OpenFileDialog of = new OpenFileDialog();
			of.InitialDirectory = this.defaultDirectory;
			if( of.ShowDialog()==DialogResult.OK )
			{
				string filePath = of.FileName;
				if (filePath.StartsWith(this.defaultDirectory))
				{
					if (System.IO.File.Exists(filePath))
					{
						filePath = filePath.Replace(this.defaultDirectory,"");
						if (filePath.StartsWith("\\")) filePath = filePath.Substring(1);
					}
					else
					{
						MessageBox.Show("WARNING: File does not exist");
					}
				}
				else if (filePath!="")
				{
					MessageBox.Show("WARNING: File is outside of: " + this.defaultDirectory);
				}

				this.TextBoxValue.Text = filePath;
				UpdateValue();
			}
		}

		
		public void KeyPressTextBox(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			if(Convert.ToString(e.KeyCode) == "Enter")
			{
				UpdateValue();
			}
		}


		public void TextBoxLeave(object sender, System.EventArgs e)
		{
			UpdateValue();
		}

		private void UpdateValue()
		{
			if(this.UIDesignObjVariationParameter != null)
			{
				this.UIDesignObjVariationParameter.Value = this.TextBoxValue.Text;
			}
			if(this.UIDesignObjVariationCommandParameter != null)
			{
				this.UIDesignObjVariationCommandParameter.Value = this.TextBoxValue.Text;
			}
		}
	}
}
