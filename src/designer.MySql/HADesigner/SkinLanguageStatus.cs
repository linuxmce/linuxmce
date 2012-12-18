using System;

namespace HADesigner
{
	/// <summary>
	/// Summary description for SkinLanguageStatus.
	/// </summary>
	public class SkinLanguageStatus
	{
		int m_nTextSkinID = -1;
		int m_nTextLanguageID = -1;
		int m_nDesignObjSkinID = -1;
		int m_nDesignObjLanguageID = -1;
		int m_nGlobalSkinID = -1;
		int m_nGlobalLanguageID = -1;
		static private SkinLanguageStatus m_Instance = null;

		public int TextSkinID
		{
			get	{return m_nTextSkinID;}
			set	{m_nTextSkinID = value;}
		}
		
		public int TextLanguageID
		{
			get	{return m_nTextLanguageID;}
			set	{m_nTextLanguageID = value;}
		}

		public int DesignObjSkinID
		{
			get	{return m_nDesignObjSkinID;}
			set	{m_nDesignObjSkinID = value;}
		}

		public int DesignObjLanguageID
		{
			get	{return m_nDesignObjLanguageID;}
			set	{m_nDesignObjLanguageID = value;}
		}

		public int GlobalSkinID
		{
			get	{return m_nGlobalSkinID;}
			set	{m_nGlobalSkinID = value;}
		}

		public int GlobalLanguageID
		{
			get	{return m_nGlobalLanguageID;}
			set	{m_nGlobalLanguageID = value;}
		}

		static public SkinLanguageStatus Instance()
		{
			if(null == m_Instance)
				m_Instance = new SkinLanguageStatus();

			return m_Instance;
		}

		private SkinLanguageStatus()
		{
		}
	}
}
