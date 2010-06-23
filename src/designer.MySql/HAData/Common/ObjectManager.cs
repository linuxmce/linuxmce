using System;
using System.Collections;
using System.Data;
using System.Windows.Forms;

using HAData.Common;
using HAData.DataAccess;

namespace HAData.Common
{  // om[29390].f  or om[29390].
	/// <summary>
	/// Summary description for DesignObjManager.
	/// </summary>

	public class DesignObjManager
	{
		Hashtable m_htDesignObj = new Hashtable();
		Hashtable m_htStyle = new Hashtable();
		MyDataSet m_mdsCache=HADataConfiguration.m_mdsCache;

		public DesignObjManager()
		{
			foreach(DataRow d in m_mdsCache.tDesignObjHeader.Rows )
			{
				DesignObjHeaderDataRow oh = new DesignObjHeaderDataRow(d);
				AddHeader(oh);
			}

			foreach(DataRow d in m_mdsCache.tDesignObjDetail.Rows )
			{
				DesignObjDetailDataRow od = new DesignObjDetailDataRow(d);
				AddDetail(od);
			}
			foreach(DataRow d in m_mdsCache.tStyleHeader.Rows )
			{
				StyleHeaderDataRow sh = new StyleHeaderDataRow(d);
				AddStyleHeader(sh);
			}

			foreach(DataRow d in m_mdsCache.tStyleDetail.Rows )
			{
				StyleDetailDataRow sd = new StyleDetailDataRow(d);
				AddStyleDetail(sd);
			}
		}

		public void AddHeader(DesignObjHeaderDataRow drOH)
		{
			int PK_DesignObjHeader = drOH.fPK_DesignObjHeader;
			Hashtable htSystem = new Hashtable();
			m_htDesignObj.Add(PK_DesignObjHeader,htSystem);
		}

		public void	AddDetail(DesignObjDetailDataRow od)
		{
			int iSystem = od.fFK_SystemIsNull ? 0 : od.fFK_System;
			int iController = od.fFK_ControllerIsNull ? 0 : od.fFK_Controller;
			int iInstallation = od.fFK_InstallationIsNull ? 0 : od.fFK_Installation;
			int iSkin = od.fFK_SkinIsNull ? 0 : od.fFK_Skin;
			int iSize = od.fFK_SizeIsNull ? 0 : od.fFK_Size;
			int iVersion = od.fFK_VersionIsNull ? 0 : od.fFK_Version;
			int iLanguage = od.fFK_LanguageIsNull ? 0 : od.fFK_Language;

			Hashtable htSystem = (Hashtable) m_htDesignObj[od.fFK_DesignObjHeader];

			Hashtable htController = (Hashtable) htSystem[iSystem];
			if( htController==null )
			{
				htController = new Hashtable();
				htSystem.Add(iSystem,htController);
			}

			Hashtable htInstallation = (Hashtable) htController[iController];
			if( htInstallation==null )
			{
				htInstallation = new Hashtable();
				htController.Add(iController,htInstallation);
			}

			Hashtable htSkin = (Hashtable) htInstallation[iInstallation];
			if( htSkin==null )
			{
				htSkin = new Hashtable();
				htInstallation.Add(iInstallation,htSkin);
			}

			Hashtable htSize = (Hashtable) htSkin[iSkin];
			if( htSize==null )
			{
				htSize = new Hashtable();
				htSkin.Add(iSkin,htSize);
			}
				
			Hashtable htVersion = (Hashtable) htSize[iSize];
			if( htVersion==null )
			{
				htVersion = new Hashtable();
				htSize.Add(iSize,htVersion);
			}

			Hashtable htLanguage = (Hashtable) htVersion[iVersion];
			if( htLanguage==null )
			{
				htLanguage = new Hashtable();
				htVersion.Add(iVersion,htLanguage);
			}

			DesignObjDetailDataRow odExisting = (DesignObjDetailDataRow) htLanguage[iLanguage];
			if( odExisting==null )
				htLanguage.Add(iLanguage,od);
			else
			{
				MessageBox.Show("There are 2 identical variations for object " + 
					od.fFK_DesignObjHeader_DataRow.fDescription + ": ID " + od.fPK_DesignObjDetail + 
					" " + od.fDescription + " and ID " + odExisting.fPK_DesignObjDetail + " " + 
					odExisting.fDescription);
			}
		}

		public void AddStyleHeader(StyleHeaderDataRow drSH)
		{
			int PK_StyleHeader = drSH.fPK_StyleHeader;
			Hashtable htSystem = new Hashtable();
			m_htStyle.Add(PK_StyleHeader,htSystem);
		}

		public void	AddStyleDetail(StyleDetailDataRow sd)
		{
			int iSystem = sd.fFK_SystemIsNull ? 0 : sd.fFK_System;
			int iController = sd.fFK_ControllerIsNull ? 0 : sd.fFK_Controller;
			int iInstallation = sd.fFK_InstallationIsNull ? 0 : sd.fFK_Installation;
			int iSkin = sd.fFK_SkinIsNull ? 0 : sd.fFK_Skin;
			int iSize = sd.fFK_SizeIsNull ? 0 : sd.fFK_Size;
			int iVersion = sd.fFK_VersionIsNull ? 0 : sd.fFK_Version;
			int iLanguage = sd.fFK_LanguageIsNull ? 0 : sd.fFK_Language;

			Hashtable htSystem = (Hashtable) m_htStyle[sd.fFK_StyleHeader];

			Hashtable htController = (Hashtable) htSystem[iSystem];
			if( htController==null )
			{
				htController = new Hashtable();
				htSystem.Add(iSystem,htController);
			}

			Hashtable htInstallation = (Hashtable) htController[iController];
			if( htInstallation==null )
			{
				htInstallation = new Hashtable();
				htController.Add(iController,htInstallation);
			}

			Hashtable htSkin = (Hashtable) htInstallation[iInstallation];
			if( htSkin==null )
			{
				htSkin = new Hashtable();
				htInstallation.Add(iInstallation,htSkin);
			}

			Hashtable htSize = (Hashtable) htSkin[iSkin];
			if( htSize==null )
			{
				htSize = new Hashtable();
				htSkin.Add(iSkin,htSize);
			}
				
			Hashtable htVersion = (Hashtable) htSize[iSize];
			if( htVersion==null )
			{
				htVersion = new Hashtable();
				htSize.Add(iSize,htVersion);
			}

			Hashtable htLanguage = (Hashtable) htVersion[iVersion];
			if( htLanguage==null )
			{
				htLanguage = new Hashtable();
				htVersion.Add(iVersion,htLanguage);
			}

			StyleDetailDataRow odExisting = (StyleDetailDataRow) htLanguage[iLanguage];
			if( odExisting==null )
				htLanguage.Add(iLanguage,sd);
			else
			{
				MessageBox.Show("There are 2 identical variations for Style " + 
					sd.fFK_StyleHeader_DataRow.fDescription + ": ID " + sd.fPK_StyleDetail + 
					" " + sd.fDescription + " and ID " + odExisting.fPK_StyleDetail + " " + 
					odExisting.fDescription);
			}
		}

		public DesignObjDetailDataRow GetDesignObj(int iPK_DesignObjHeader,int iSystem,int iInstallation,int iController,int iSkin, int iSize, int iVersion, int iLanguage)
		{
			Hashtable htSystem = (Hashtable) m_htDesignObj[iPK_DesignObjHeader];

			Hashtable htController = (Hashtable) htSystem[iSystem];
			if( htController==null && iSystem!=0 )
				htController = (Hashtable) htSystem[0];
			if( htController==null )
			{
				MessageBox.Show("Unable to find a suitable variation for object ID: " + iPK_DesignObjHeader + " " + m_mdsCache.tDesignObjHeader[iPK_DesignObjHeader].fDescription);
				IDictionaryEnumerator en = htSystem.GetEnumerator();
				en.MoveNext();
				htController = (Hashtable) en.Value;
			}

			Hashtable htInstallation = (Hashtable) htController[iController];
			if( htInstallation==null && iController!=0 )
				htInstallation = (Hashtable) htController[0];
			if( htInstallation==null )
			{
				MessageBox.Show("Unable to find a suitable variation for object ID: " + iPK_DesignObjHeader + " " + m_mdsCache.tDesignObjHeader[iPK_DesignObjHeader].fDescription);
				IDictionaryEnumerator en = htController.GetEnumerator();
				en.MoveNext();
				htInstallation = (Hashtable) en.Value;
			}

			Hashtable htSkin = (Hashtable) htInstallation[iInstallation];
			if( htSkin==null && iInstallation!=0 )
				htSkin = (Hashtable) htInstallation[0];
			if( htSkin==null )
			{
				MessageBox.Show("Unable to find a suitable variation for object ID: " + iPK_DesignObjHeader + " " + m_mdsCache.tDesignObjHeader[iPK_DesignObjHeader].fDescription);
				IDictionaryEnumerator en = htInstallation.GetEnumerator();
				en.MoveNext();
				htSkin = (Hashtable) en.Value;
			}

			Hashtable htSize = (Hashtable) htSkin[iSkin];
			if( htSize==null && iSkin!=0 )
				htSize = (Hashtable) htSkin[0];
			if( htSize==null )
			{
				MessageBox.Show("Unable to find a suitable variation for object ID: " + iPK_DesignObjHeader + " " + m_mdsCache.tDesignObjHeader[iPK_DesignObjHeader].fDescription);
				IDictionaryEnumerator en = htSkin.GetEnumerator();
				en.MoveNext();
				htSize = (Hashtable) en.Value;
			}

			Hashtable htVersion = (Hashtable) htSize[iSize];
			if( htVersion==null && iSize!=0 )
				htVersion = (Hashtable) htSize[0];
			if( htVersion==null )
			{
				MessageBox.Show("Unable to find a suitable variation for object ID: " + iPK_DesignObjHeader + " " + m_mdsCache.tDesignObjHeader[iPK_DesignObjHeader].fDescription);
				IDictionaryEnumerator en = htSize.GetEnumerator();
				en.MoveNext();
				htVersion = (Hashtable) en.Value;
			}

			Hashtable htLanguage = (Hashtable) htVersion[iVersion];
			if( htLanguage==null && iVersion!=0 )
				htLanguage = (Hashtable) htVersion[0];
			if( htLanguage==null )
			{
				MessageBox.Show("Unable to find a suitable variation for object ID: " + iPK_DesignObjHeader + " " + m_mdsCache.tDesignObjHeader[iPK_DesignObjHeader].fDescription);
				IDictionaryEnumerator en = htVersion.GetEnumerator();
				en.MoveNext();
				htLanguage = (Hashtable) en.Value;
			}

			DesignObjDetailDataRow od = (DesignObjDetailDataRow) htLanguage[iLanguage];
			if( od==null && iLanguage!=0 )
				od = (DesignObjDetailDataRow) htLanguage[0];
			if( od==null )
			{
				MessageBox.Show("Unable to find a suitable variation for object ID: " + iPK_DesignObjHeader + " " + m_mdsCache.tDesignObjHeader[iPK_DesignObjHeader].fDescription);
				IDictionaryEnumerator en = htLanguage.GetEnumerator();
				en.MoveNext();
				od = (DesignObjDetailDataRow) en.Value;
			}

			return od;
		}

		public DesignObjDetailDataRow GetDesignObj(int iPK_DesignObjHeader)
		{
			return GetDesignObj(iPK_DesignObjHeader,
				HADataConfiguration.iFK_System,
				HADataConfiguration.iFK_Installation,
				HADataConfiguration.iFK_Controller,
				HADataConfiguration.iFK_Skin,
				HADataConfiguration.iFK_Size,
				HADataConfiguration.iFK_Language,
				HADataConfiguration.iFK_Version);
		}
		public StyleDetailDataRow GetStyle(int iPK_StyleHeader,int iSystem,int iInstallation,int iController,int iSkin, int iSize, int iVersion, int iLanguage)
		{
			Hashtable htSystem = (Hashtable) m_htStyle[iPK_StyleHeader];

			Hashtable htController = (Hashtable) htSystem[iSystem];
			if( htController==null && iSystem!=0 )
				htController = (Hashtable) htSystem[0];
			if( htController==null )
			{
				MessageBox.Show("Unable to find a suitable variation for Style ID: " + iPK_StyleHeader + " " + m_mdsCache.tStyleHeader[iPK_StyleHeader].fDescription);
				IDictionaryEnumerator en = htSystem.GetEnumerator();
				en.MoveNext();
				htController = (Hashtable) en.Value;
			}

			Hashtable htInstallation = (Hashtable) htController[iController];
			if( htInstallation==null && iController!=0 )
				htInstallation = (Hashtable) htController[0];
			if( htInstallation==null )
			{
				MessageBox.Show("Unable to find a suitable variation for Style ID: " + iPK_StyleHeader + " " + m_mdsCache.tStyleHeader[iPK_StyleHeader].fDescription);
				IDictionaryEnumerator en = htController.GetEnumerator();
				en.MoveNext();
				htInstallation = (Hashtable) en.Value;
			}

			Hashtable htSkin = (Hashtable) htInstallation[iInstallation];
			if( htSkin==null && iInstallation!=0 )
				htSkin = (Hashtable) htInstallation[0];
			if( htSkin==null )
			{
				MessageBox.Show("Unable to find a suitable variation for Style ID: " + iPK_StyleHeader + " " + m_mdsCache.tStyleHeader[iPK_StyleHeader].fDescription);
				IDictionaryEnumerator en = htInstallation.GetEnumerator();
				en.MoveNext();
				htSkin = (Hashtable) en.Value;
			}

			Hashtable htSize = (Hashtable) htSkin[iSkin];
			if( htSize==null && iSkin!=0 )
				htSize = (Hashtable) htSkin[0];
			if( htSize==null )
			{
				MessageBox.Show("Unable to find a suitable variation for Style ID: " + iPK_StyleHeader + " " + m_mdsCache.tStyleHeader[iPK_StyleHeader].fDescription);
				IDictionaryEnumerator en = htSkin.GetEnumerator();
				en.MoveNext();
				htSize = (Hashtable) en.Value;
			}

			Hashtable htVersion = (Hashtable) htSize[iSize];
			if( htVersion==null && iSize!=0 )
				htVersion = (Hashtable) htSize[0];
			if( htVersion==null )
			{
				MessageBox.Show("Unable to find a suitable variation for Style ID: " + iPK_StyleHeader + " " + m_mdsCache.tStyleHeader[iPK_StyleHeader].fDescription);
				IDictionaryEnumerator en = htSize.GetEnumerator();
				en.MoveNext();
				htVersion = (Hashtable) en.Value;
			}

			Hashtable htLanguage = (Hashtable) htVersion[iVersion];
			if( htLanguage==null && iVersion!=0 )
				htLanguage = (Hashtable) htVersion[0];
			if( htLanguage==null )
			{
				MessageBox.Show("Unable to find a suitable variation for Style ID: " + iPK_StyleHeader + " " + m_mdsCache.tStyleHeader[iPK_StyleHeader].fDescription);
				IDictionaryEnumerator en = htVersion.GetEnumerator();
				en.MoveNext();
				htLanguage = (Hashtable) en.Value;
			}

			StyleDetailDataRow sd = (StyleDetailDataRow) htLanguage[iLanguage];
			if( sd==null && iLanguage!=0 )
				sd = (StyleDetailDataRow) htLanguage[0];
			if( sd==null )
			{
				MessageBox.Show("Unable to find a suitable variation for Style ID: " + iPK_StyleHeader + " " + m_mdsCache.tStyleHeader[iPK_StyleHeader].fDescription);
				IDictionaryEnumerator en = htLanguage.GetEnumerator();
				en.MoveNext();
				sd = (StyleDetailDataRow) en.Value;
			}

			return sd;
		}

		public StyleDetailDataRow GetStyle(int iPK_StyleHeader)
		{
			return GetStyle(iPK_StyleHeader,
				HADataConfiguration.iFK_System,
				HADataConfiguration.iFK_Installation,
				HADataConfiguration.iFK_Controller,
				HADataConfiguration.iFK_Skin,
				HADataConfiguration.iFK_Size,
				HADataConfiguration.iFK_Language,
				HADataConfiguration.iFK_Version);
		}
	}
}
