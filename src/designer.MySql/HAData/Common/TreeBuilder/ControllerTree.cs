using System;
using System.Windows.Forms;
using System.Data;

using HAData.Common;
using HAData.DataAccess;

namespace HAData.Common.TreeBuilder
{
	/// <summary>
	/// Summary description for ControllerTree.
	/// </summary>
	public class ControllerTree
	{
		public ControllerTree(TreeView tvController,int PK_Installation)
		{	
			DataView dv = new DataView(HADataConfiguration.m_mdsCache.tController,ControllerData.FK_INSTALLATION_FIELD + "=" + PK_Installation,"",DataViewRowState.CurrentRows);
			for(int i=0; i<dv.Count; i++ )
			{
				ControllerDataRow dr = new ControllerDataRow(dv[i].Row);

				ControllerTreeNode tn = new ControllerTreeNode(dr,dr.fDescription);
				tvController.Nodes.Add(tn);
			}
		}
	}

	public class ControllerTreeNode : TreeNode
	{
		ControllerDataRow m_dr;

		public ControllerTreeNode(ControllerDataRow dr,string Description) : base(Description)
		{
			m_dr = dr;
		}

		public ControllerDataRow DataRow
		{
			get
			{
				return m_dr;
			}
		}
	}
}
