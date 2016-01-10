using System;
using System.Windows.Forms;
using System.Data;

using HAData.Common;
using HAData.DataAccess;

namespace HAData.Common.TreeBuilder
{
	/// <summary>
	/// Summary description for CollectionTree.
	/// </summary>
	public class CollectionTree
	{
		public CollectionTree(TreeView tvCollection)
		{
			foreach(DataRow d in HADataConfiguration.m_mdsCache.tCollectionType.Rows )
			{
				CollectionTypeDataRow ct = new CollectionTypeDataRow(d);

				if( ct.fFK_CollectionType_ParentIsNull )
				{
					CollectionTreeNode tn = new CollectionTreeNode(null,ct.fDescription);
					tvCollection.Nodes.Add(tn);
					
					AddCollectionDesignObjs(ct,tn);
					AddCollectionChildren(ct,tn);
					int i=tn.Index;
				}
			}
		}
		private void AddCollectionDesignObjs(CollectionTypeDataRow c,TreeNode tn)
		{
			DataRow[] drc = c.dr.GetChildRows(HARelation.rCollectionType_CollectionHeader_CollectionType);
			foreach( DataRow dr in drc )
			{
				CollectionHeader_CollectionTypeDataRow ctChild = new CollectionHeader_CollectionTypeDataRow(dr);
				CollectionHeaderDataRow chChild = HADataConfiguration.m_mdsCache.tCollectionHeader[ctChild.fFK_CollectionHeader];

				CollectionTreeNode tnChild = new CollectionTreeNode(chChild,chChild.fDescription);
				tn.Nodes.Add(tnChild);
			}
		}

		private void AddCollectionChildren(CollectionTypeDataRow c,TreeNode tn)
		{
			DataRow[] drc = c.dr.GetChildRows(HARelation.rCollectionType_CollectionType_Parent);
			foreach( DataRow dr in drc )
			{
				CollectionTypeDataRow ctChild = new CollectionTypeDataRow(dr);
				CollectionTreeNode tnChild = new CollectionTreeNode(null,ctChild.fDescription);
				tn.Nodes.Add(tnChild);
				tnChild.ImageIndex = -1;
				AddCollectionDesignObjs(ctChild,tnChild);
				AddCollectionChildren(ctChild,tnChild);
			}
		}

		public class CollectionTreeNode : TreeNode
		{
			CollectionHeaderDataRow m_dr;

			public CollectionTreeNode(CollectionHeaderDataRow dr,string Description) : base(Description)
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

			public CollectionHeaderDataRow DataRow
			{
				get
				{
					return m_dr;
				}
			}
		}

	}
}
