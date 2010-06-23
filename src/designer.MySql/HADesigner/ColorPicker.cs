using System;
using System.Windows.Forms;
using HAData.Common;

namespace HADesigner
{
	/// <summary>
	/// Summary description for ColorPicker.
	/// </summary>
	public class ColorPicker
	{
		/// <summary>
		/// object to "Box" Color and be able to return null
		/// </summary>
		public static object getColor()
		{
			HADataConfiguration.GetDetaults();
			ColorDialog cd = new ColorDialog();
			cd.AllowFullOpen=true;
			cd.AnyColor=true;
			if( HADataConfiguration.iCustomColors!=null ) cd.CustomColors = HADataConfiguration.iCustomColors;
			
			if (cd.ShowDialog() != DialogResult.OK) return null;

			if( cd.CustomColors!=null )
			{
				HADataConfiguration.iCustomColors = cd.CustomColors;
				HADataConfiguration.SaveDefaults();
			}
			Clipboard.SetDataObject(cd.Color.ToArgb().ToString(),true);
			return cd.Color;
		}
	}
}
