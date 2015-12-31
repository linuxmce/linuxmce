using System;
using System.Windows.Forms;
using System.Data;

using HAData.Common;
using HAData.DataAccess;

namespace HAData.Common.TreeBuilder
{
	/// <summary>
	/// Summary description for CommandGroupGroupTree.
	/// </summary>
	public class CommandGroupGroupTree
	{
		private static MyDataSet mds
		{
			get {return HADataConfiguration.m_mdsCache;}
		}

		/// <summary>
		/// Insert 1 CommandGroupGroup (new CommandGroupGroup, no children) to the selected node in the tree which should be it's corresponding category.
		/// </summary>
		public static void InsertCommandGroupGroupToTree(TreeView tvCommandGroupGroup, int id_CommandGroupGroup)
		{
			CommandGroupGroupDataRow odr = mds.tCommandGroupGroup[id_CommandGroup];
			CommandGroupCategoryDataRow ocdr = odr.fFK_CommandGroupCategory_DataRow;
			string description = odr.fDescription + "(" + odr.fPK_CommandGroup_D + ")";
			tvCommandGroup.SelectedNode.Nodes.Add(new CommandGroupTreeNode(odr, ocdr, description));
		}

		public static void CreateTree(TreeView tvCommandGroup)
		{
			foreach(DataRow d in mds.tCommandGroupCategory.Rows )
			{
				CommandGroupCategoryDataRow ct = new CommandGroupCategoryDataRow(d);

				if( ct.fFK_CommandGroupCategory_ParentIsNull )
				{
					CommandGroupTreeNode tn = new CommandGroupTreeNode(null, ct,ct.fDescription);
					tvCommandGroup.Nodes.Add(tn);
					
					AddCommandGroupCommandGroups(ct,tn);
					AddCommandGroupChildren(ct,tn);
					int i=tn.Index;
				}
			}
		}
		private static void AddCommandGroupCommandGroups(CommandGroupCategoryDataRow c,TreeNode tn)
		{
			//DataRow[] drc = c.dr.GetChildRows(HARelation.rCommandGroupCategory__CommandGroupHeader_CommandGroupCategory);
			DataRow[] drc = c.dr.GetChildRows(MyDataSet.MyRelations.CommandGroup_FK_CommandGroupCategory);
						
			foreach( DataRow dr in drc )
			{

				// CommandGroup_CommandGroupCategoryDataRow ctChild = new CommandGroup_CommandGroupCategoryDataRow(dr);
				// CommandGroup_DDataRow chChild = HADataConfiguration.m_mdsCache.tCommandGroup_D[ctChild.fFK_CommandGroup_D];

				CommandGroup_DDataRow chChild = new CommandGroup_DDataRow(dr);
				string description = chChild.fDescription + "(" + chChild.fPK_CommandGroup_D + ")";
				CommandGroupTreeNode tnChild = new CommandGroupTreeNode(chChild,chChild.fFK_CommandGroupCategory_DataRow,description);
				tn.Nodes.Add(tnChild);
			}
		}

		private static void AddCommandGroupChildren(CommandGroupCategoryDataRow c,TreeNode tn)
		{
			// DataRow[] drc = c.dr.GetChildRows(HARelation.rCommandGroupCategory__CommandGroupCategory_Parent);
			DataRow[] drc = c.dr.GetChildRows(MyDataSet.MyRelations.CommandGroupCategory_FK_CommandGroupCategory_Parent);

			foreach( DataRow dr in drc )
			{
				CommandGroupCategoryDataRow ctChild = new CommandGroupCategoryDataRow(dr);
				CommandGroupTreeNode tnChild = new CommandGroupTreeNode(null,ctChild,ctChild.fDescription);
				tn.Nodes.Add(tnChild);
				tnChild.ImageIndex = -1;
				AddCommandGroupCommandGroups(ctChild,tnChild);
				AddCommandGroupChildren(ctChild,tnChild);
			}
		}
	}
	public class CommandGroupTreeNode : TreeNode
	{
		CommandGroup_DDataRow m_drCommandGroupHeader;
		CommandGroupCategoryDataRow m_drCommandGroupCategory;

		public CommandGroupTreeNode(CommandGroup_DDataRow drOH, CommandGroupCategoryDataRow drOC, string Description) : base(Description)
		{
			m_drCommandGroupHeader = drOH;
			m_drCommandGroupCategory = drOC;
			if( m_drCommandGroupHeader==null )
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

		public CommandGroup_DDataRow CommandGroup_DDataRow
		{
			get	{return m_drCommandGroupHeader;}
		}
		public CommandGroupCategoryDataRow CommandGroupCategoryDataRow
		{
			get	{return m_drCommandGroupCategory;}
		}

	}
}
