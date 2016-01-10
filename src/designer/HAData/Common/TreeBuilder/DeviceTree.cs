using System;
using System.Windows.Forms;
using System.Data;

using HAData.Common;
using HAData.DataAccess;

namespace HAData.Common.TreeBuilder
{
	/// <summary>
	/// Summary description for DeviceTree.
	/// </summary>
	public class DeviceTree
	{
		public DeviceTree(TreeView tvDevice,int PK_Installation)
		{
			foreach(DataRow d in HADataConfiguration.m_mdsCache.tDeviceCategory.Rows )
			{
				DeviceCategoryDataRow ct = new DeviceCategoryDataRow(d);

				if( ct.fFK_DeviceCategory_ParentIsNull )
				{
					DeviceTreeNode tn = new DeviceTreeNode(null,ct.fDescription);
					tvDevice.Nodes.Add(tn);
					
					AddDeviceDesignObjs(ct,tn,PK_Installation);
					AddDeviceChildren(ct,tn,PK_Installation);
					int i=tn.Index;
				}
			}
		}
		private void AddDeviceDesignObjs(DeviceCategoryDataRow c,TreeNode tn,int PK_Installation)
		{
			DataView dv = new DataView(HADataConfiguration.m_mdsCache.tDevice,DeviceData.FK_INSTALLATION_FIELD + "=" + PK_Installation + " AND " + DeviceData.FK_DEVICECATEGORY_FIELD + "=" + c.fPK_DeviceCategory,"",DataViewRowState.CurrentRows);
			for(int i=0; i<dv.Count; i++ )
			{
				DeviceDataRow ctChild = new DeviceDataRow(dv[i].Row);
				DeviceTreeNode tnChild = new DeviceTreeNode(ctChild,ctChild.fDescription);
				tn.Nodes.Add(tnChild);
			}
		}

		private void AddDeviceChildren(DeviceCategoryDataRow c,TreeNode tn,int PK_Installation)
		{
			DataRow[] drc = c.dr.GetChildRows(HARelation.rDeviceCategory__DeviceCategory_Parent);
			foreach( DataRow dr in drc )
			{
				DeviceCategoryDataRow ctChild = new DeviceCategoryDataRow(dr);
				DeviceTreeNode tnChild = new DeviceTreeNode(null,ctChild.fDescription);
				tn.Nodes.Add(tnChild);
				tnChild.ImageIndex = -1;
				AddDeviceDesignObjs(ctChild,tnChild,PK_Installation);
				AddDeviceChildren(ctChild,tnChild,PK_Installation);
			}
		}
	}
	public class DeviceTreeNode : TreeNode
	{
		DeviceDataRow m_dr;

		public DeviceTreeNode(DeviceDataRow dr,string Description) : base(Description)
		{
			m_dr = dr;
			if( dr==null )
			{
				ImageIndex=0;
				SelectedImageIndex=2;
			}
			else
			{
				ImageIndex=1;
				SelectedImageIndex=3;
			}
		}

		public DeviceDataRow DataRow
		{
			get
			{
				return m_dr;
			}
		}
	}
}
