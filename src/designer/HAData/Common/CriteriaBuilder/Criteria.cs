using System;
using System.Collections;
using System.Web;
using System.Web.UI;


namespace HAData.Common.CriteriaBuilder
{
	/// <summary>
	/// Summary description for Criteria.
	/// </summary>
	public class Criterion
	{

		public bool m_blnAllowFreeFormText;
		public ArrayList m_arrComparisonDesignObjs;
		public ArrayList m_arrComparisonOperators;
		public ArrayList m_arrComparisonValues;




		public Criterion()
		{
			//
			// TODO: Add constructor logic here
			//
		}


		public void AttachComparisonDesignObjs(SelectList objList)
		{
			foreach(Object obj in this.m_arrComparisonDesignObjs)
			{
				ComparisonDesignObj objEnum = (ComparisonDesignObj) obj;
				objList.Items.Add(new ListItem(objEnum.m_name,objEnum.m_name));
			}
		}



	}


	public class ComparisonDesignObj
	{
		public string m_name;
		public ComparisonDesignObj()
		{

		}
		public ComparisonDesignObj(string strName)
		{
			m_name = strName;
		}

	}



	public class ComparisonOperator
	{
		public ComparisonOperator()
		{
			

		}
	}


	public class ComparisonValue
	{
		public ComparisonValue()
		{
			

		}
	}



	public class ControllerCriterion : Criterion
	{
		public ControllerCriterion()
		{

			this.m_arrComparisonDesignObjs = new ArrayList();
			this.m_arrComparisonDesignObjs.Add(new ComparisonDesignObj("Screen Width"));
			this.m_arrComparisonDesignObjs.Add(new ComparisonDesignObj("Screen Height"));
			this.m_arrComparisonDesignObjs.Add(new ComparisonDesignObj("Screen Aspect Ratio"));
			this.m_arrComparisonDesignObjs.Add(new ComparisonDesignObj("OS"));

		}
	}



}
