using System;
using System.Windows.Forms;
using System.Data;
using System.Collections;

using HAData.Common;
using HAData.DataAccess;

namespace HAData.Common.TreeBuilder
{
	/// <summary>
	/// Summary description for DesignObjTree.
	/// </summary>
	public class DesignObjTree
	{
		private static MyDataSet mds
		{
			get {return HADataConfiguration.m_mdsCache;}
		}

		/// <summary>
		/// Insert 1 object (new object, no children) to the selected node in the tree which should be it's corresponding category.
		/// </summary>
		public static void InsertDesignObjToTree(TreeView tvDesignObj, int id_object)
		{
            DesignObjDataRow odr = mds.tDesignObj[id_object];
            DesignObjCategoryDataRow ocdr = odr.fFK_DesignObjCategory_DataRow;
			string description = odr.fDescription + "(" + odr.fPK_DesignObj + ")";
			DesignObjTreeNode otn = new DesignObjTreeNode(odr, ocdr, description);
			tvDesignObj.SelectedNode.Nodes.Add(otn);
			tvDesignObj.SelectedNode = otn;
		}

		public static void CreateTree(TreeView tvDesignObj)
		{
			//foreach(DataRow d in mds.tDesignObjCategory.Rows )
			foreach(DataRow d in mds.tDesignObjCategory.Select(null,"Description"))
			{
				DesignObjCategoryDataRow ct = new DesignObjCategoryDataRow(d);

				if( ct.fFK_DesignObjCategory_ParentIsNull )
				{
					DesignObjTreeNode tn = new DesignObjTreeNode(null, ct,ct.fDescription);
					tvDesignObj.Nodes.Add(tn);
					
					AddDesignObjChildren(ct,tn);
					AddDesignObjDesignObjs(ct,tn);
					int i=tn.Index;
				}
			}
		}
		private static void AddDesignObjDesignObjs(DesignObjCategoryDataRow c,TreeNode tn)
		{
			//DataRow[] drc = c.dr.GetChildRows(HARelation.rDesignObjCategory__DesignObjHeader_DesignObjCategory);
			//DataRow[] drc = c.dr.GetChildRows(MyDataSet.MyRelations.DesignObj_FK_DesignObjCategory);

			DataRow[] drc = mds.tDesignObj.Select("FK_DesignObjCategory="+c.fPK_DesignObjCategory,"Description");
					
			foreach( DataRow dr in drc )
			{

				// DesignObj_DesignObjCategoryDataRow ctChild = new DesignObj_DesignObjCategoryDataRow(dr);
				// DesignObjDataRow chChild = HADataConfiguration.m_mdsCache.tDesignObj[ctChild.fFK_DesignObj];

				DesignObjDataRow chChild = new DesignObjDataRow(dr);
                string description = chChild.fDescription + "(" + chChild.fPK_DesignObj + ")";
				DesignObjTreeNode tnChild = new DesignObjTreeNode(chChild,chChild.fFK_DesignObjCategory_DataRow,description);
				tn.Nodes.Add(tnChild);
			}
		}

		private static void AddDesignObjChildren(DesignObjCategoryDataRow c,TreeNode tn)
		{
			//DataRow[] drc = c.dr.GetChildRows(HARelation.rDesignObjCategory__DesignObjCategory_Parent);
			//DataRow[] drc = c.dr.GetChildRows(MyDataSet.MyRelations.DesignObjCategory_FK_DesignObjCategory_Parent);
			
			DataRow[] drc = mds.tDesignObjCategory.Select("FK_DesignObjCategory_Parent="+c.fPK_DesignObjCategory,"Description");

			foreach( DataRow dr in drc )
			{
				DesignObjCategoryDataRow ctChild = new DesignObjCategoryDataRow(dr);
				DesignObjTreeNode tnChild = new DesignObjTreeNode(null,ctChild,ctChild.fDescription);
				tn.Nodes.Add(tnChild);
				tnChild.ImageIndex = -1;
				AddDesignObjChildren(ctChild,tnChild);
				AddDesignObjDesignObjs(ctChild,tnChild);
			}
		}
		
		public static DesignObjTreeNode GetNodeByID(TreeView tv,int id)
		{
			DesignObjDataRow odr = mds.tDesignObj[id];
			if (!odr.bIsValid) return null;

			TreeNodeCollection tnc = tv.Nodes;
			foreach (int catID in GetCategoryPath(odr))
			{
				tnc = getNodeWithCategoryID(tnc,catID).Nodes;
			}
			return getNodeWithDesignObjID(tnc,id);
		}

		private static ArrayList GetCategoryPath(DesignObjDataRow odr)
		{
			ArrayList al = new ArrayList();
			DesignObjCategoryDataRow ocdr = odr.fFK_DesignObjCategory_DataRow;
			while (ocdr != null)
			{
                al.Add(ocdr.fPK_DesignObjCategory);				
				ocdr = (ocdr.fFK_DesignObjCategory_ParentIsNull) ? null : ocdr.fFK_DesignObjCategory_Parent_DataRow;
			}
			al.Reverse();
			return al;
		}

		private static bool isNodeWithDesignObjID(DesignObjTreeNode otn, int id)
		{
			return (otn.DesignObjDataRow != null && otn.DesignObjDataRow.fPK_DesignObj == id);
		}
		private static bool isNodeWithCategoryID(DesignObjTreeNode otn, int id)
		{
			return (otn.DesignObjCategoryDataRow != null && otn.DesignObjCategoryDataRow.fPK_DesignObjCategory == id);
		}
		
		private static DesignObjTreeNode getNodeWithDesignObjID(TreeNodeCollection tnc, int id) // find at one level
		{
			foreach (DesignObjTreeNode otn in tnc)
			{
                if (isNodeWithDesignObjID(otn,id)) return otn;
			}
			return null;
		}
		private static DesignObjTreeNode getNodeWithCategoryID(TreeNodeCollection tnc, int id) // find at one level
		{
			foreach (DesignObjTreeNode otn in tnc)
			{
				if (isNodeWithCategoryID(otn,id)) return otn;
			}
			return null;
		}
	}


	public class DesignObjTreeNode : TreeNode
	{
		DesignObjDataRow m_drDesignObjHeader;
		DesignObjCategoryDataRow m_drDesignObjCategory;

		public DesignObjTreeNode(DesignObjDataRow drOH, DesignObjCategoryDataRow drOC, string Description) : base(Description)
		{
			m_drDesignObjHeader = drOH;
			m_drDesignObjCategory = drOC;
			if( m_drDesignObjHeader==null )
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

		public DesignObjDataRow DesignObjDataRow
		{
			get	{return m_drDesignObjHeader;}
		}
		public DesignObjCategoryDataRow DesignObjCategoryDataRow
		{
			get	{return m_drDesignObjCategory;}
		}
	}
}
