using System;
using System.Text.RegularExpressions;


namespace HADesigner
{
	/// <summary>
	/// Summary description for classCommonMethods.
	/// </summary>
	public class CommonMethods
	{
		public CommonMethods()
		{
			//
			// TODO: Add constructor logic here
			//
		}



		public static bool IsNumeric(object value) 
		{ 
			try 
			{ 
				double d = System.Double.Parse(value.ToString(), System.Globalization.NumberStyles.Any); 
				return true; 
			} 
			catch (FormatException) 
			{ 
				return false; 
			} 
		}





		public string JSParameter(string strIn)
		{
			string strOut;
			strOut = Regex.Replace(strIn, "'", "");
			return strOut;
		}


		public string Trim(string input)
		{
			string output = null;
			Regex  re;
		
			//pattern to trim leading and trailing white space from string
			re = new Regex(
				"^\\s*|\\s*$", 
				RegexOptions.IgnoreCase | RegexOptions.Singleline
				);

			//if matches
			if (re.IsMatch(input))
			{
				//remove whitespace
				output = re.Replace(input, "");
			}

			//cleanup
			re = null;

			//return the email with no leading or trailing white space
			return output;
		}


		public string Right(string strIn, int intLength)
		{
			if(strIn.Length < intLength) intLength = strIn.Length;
			return strIn.Substring(strIn.Length - intLength, intLength);
		}

		public string Left(string strIn, int intLength)
		{
			if(strIn.Length < intLength) intLength = strIn.Length;
			return strIn.Substring(0, intLength);
		}


		public string TrimLeft(string strIn, int intLength)
		{
			if(strIn.Length < intLength) intLength = strIn.Length;
			return Right(strIn, strIn.Length - intLength);
		}

		public string TrimRight(string strIn, int intLength)
		{
			if(strIn.Length < intLength) intLength = strIn.Length;
			return Left(strIn, strIn.Length - intLength);
		}


		public string PadZerosLeft(string strIn, int intLength)
		{
			string strZeros = "";
			for(int intCount=0; intCount < intLength; intCount++)
			{
				strZeros += "0";
			}
			return Right((strZeros + strIn), intLength);
		}
		




	}
}
