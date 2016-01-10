using System;
using System.Windows.Forms;
using System.Data;

using HAData.Common;
using HAData.DataAccess;

namespace HAData.Common.TreeBuilder
{
	/// <summary>
	/// Summary description for CommandTree.
	/// </summary>
	public class CommandTree
	{
		private static MyDataSet mds
		{
			get {return HADataConfiguration.m_mdsCache;}
		}

		/// <summary>
		/// Insert 1 Command (new Command, no children) to the selected node in the tree which should be it's corresponding category.
		/// </summary>
		public static void InsertCommandToTree(TreeView tvCommand, int id_Command)
		{
			CommandDataRow odr = mds.tCommand[id_Command];
			CommandCategoryDataRow ocdr = odr.fFK_CommandCategory_DataRow;
			string description = odr.fDescription + "(" + odr.fPK_Command + ")";
			tvCommand.SelectedNode.Nodes.Add(new CommandTreeNode(odr, ocdr, description));
		}

		public static void CreateTree(TreeView tvCommand)
		{
			foreach(DataRow d in mds.tCommandCategory.Rows )
			{
				CommandCategoryDataRow ct = new CommandCategoryDataRow(d);

				if( ct.fFK_CommandCategory_ParentIsNull )
				{
					CommandTreeNode tn = new CommandTreeNode(null, ct,ct.fDescription);
					tvCommand.Nodes.Add(tn);
					
					AddCommandCommands(ct,tn);
					AddCommandChildren(ct,tn);
					int i=tn.Index;
				}
			}
		}
		private static void AddCommandCommands(CommandCategoryDataRow c,TreeNode tn)
		{
			//DataRow[] drc = c.dr.GetChildRows(HARelation.rCommandCategory__CommandHeader_CommandCategory);
			DataRow[] drc = c.dr.GetChildRows(MyDataSet.MyRelations.Command_FK_CommandCategory);
						
			foreach( DataRow dr in drc )
			{

				// Command_CommandCategoryDataRow ctChild = new Command_CommandCategoryDataRow(dr);
				// CommandDataRow chChild = HADataConfiguration.m_mdsCache.tCommand[ctChild.fFK_Command];

				CommandDataRow chChild = new CommandDataRow(dr);
				string description = chChild.fDescription + "(" + chChild.fPK_Command + ")";
				CommandTreeNode tnChild = new CommandTreeNode(chChild,chChild.fFK_CommandCategory_DataRow,description);
				tn.Nodes.Add(tnChild);
			}
		}

		private static void AddCommandChildren(CommandCategoryDataRow c,TreeNode tn)
		{
			// DataRow[] drc = c.dr.GetChildRows(HARelation.rCommandCategory__CommandCategory_Parent);
			DataRow[] drc = c.dr.GetChildRows(MyDataSet.MyRelations.CommandCategory_FK_CommandCategory_Parent);

			foreach( DataRow dr in drc )
			{
				CommandCategoryDataRow ctChild = new CommandCategoryDataRow(dr);
				CommandTreeNode tnChild = new CommandTreeNode(null,ctChild,ctChild.fDescription);
				tn.Nodes.Add(tnChild);
				tnChild.ImageIndex = -1;
				AddCommandCommands(ctChild,tnChild);
				AddCommandChildren(ctChild,tnChild);
			}
		}
	}
	public class CommandTreeNode : TreeNode
	{
		CommandDataRow m_drCommandHeader;
		CommandCategoryDataRow m_drCommandCategory;

		public CommandTreeNode(CommandDataRow drOH, CommandCategoryDataRow drOC, string Description) : base(Description)
		{
			m_drCommandHeader = drOH;
			m_drCommandCategory = drOC;
			if( m_drCommandHeader==null )
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

		public CommandDataRow CommandDataRow
		{
			get	{return m_drCommandHeader;}
		}
		public CommandCategoryDataRow CommandCategoryDataRow
		{
			get	{return m_drCommandCategory;}
		}
	}
}
