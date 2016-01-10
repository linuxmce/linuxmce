using System;
using System.Windows.Forms;
using System.Data;
using System.Collections;

using HAData.Common;
using HAData.DataAccess;

namespace HAData.Common.TreeBuilder
{
	/// <summary>
	/// Summary description for TextTree.
	/// </summary>
	public class TextTree
	{
		private static MyDataSet mds
		{
			get {return HADataConfiguration.m_mdsCache;}
		}

		public static void InsertDesignObjToTree(TreeView tvText, int id_text)
		{
			TextDataRow tdr = mds.tText[id_text];
			if (tdr==null)
			{
				MessageBox.Show("Error: No Text Row with id: " + id_text.ToString());
				return;
			}
            TextCategoryDataRow tcdr = tdr.fFK_TextCategory_DataRow;
			string description = tdr.fDescription + "(" + tdr.fPK_Text + ")";
			TextTreeNode ttn = new TextTreeNode(tdr,tcdr,description);
			tvText.SelectedNode.Nodes.Add(ttn);
			tvText.SelectedNode = ttn;
		}


		public static void CreateTree(TreeView tvText)
		{
			// foreach(DataRow d in mds.tTextCategory.Rows )
			foreach(DataRow d in mds.tTextCategory.Select(null,"Description"))
			{
				TextCategoryDataRow ct = new TextCategoryDataRow(d);

				if( ct.fFK_TextCategory_ParentIsNull )
				{
					TextTreeNode tn = new TextTreeNode(null,ct,ct.fDescription);
					tvText.Nodes.Add(tn);
					
					AddTextChildren(ct,tn);
					AddTextDesignObjs(ct,tn);
					int i=tn.Index;
				}
			}
		}
		private static void AddTextDesignObjs(TextCategoryDataRow c,TreeNode tn)
		{
			// DataRow[] drc = c.dr.GetChildRows(HARelation.rTextCategory__Text);
			// DataRow[] drc = c.dr.GetChildRows(MyDataSet.MyRelations.Text_FK_TextCategory);
			DataRow[] drc = mds.tText.Select("FK_TextCategory="+c.fPK_TextCategory,"Description");
			foreach( DataRow dr in drc )
			{
				TextDataRow ctChild = new TextDataRow(dr);
				string description = ctChild.fDescription + "(" + ctChild.fPK_Text + ")";
				TextTreeNode tnChild = new TextTreeNode(ctChild,c,description);
				tn.Nodes.Add(tnChild);
			}
		}

		private static void AddTextChildren(TextCategoryDataRow c,TreeNode tn)
		{
			// DataRow[] drc = c.dr.GetChildRows(HARelation.rTextCategory__TextCategory_Parent);
			// DataRow[] drc = c.dr.GetChildRows(MyDataSet.MyRelations.TextCategory_FK_TextCategory_Parent);
			DataRow[] drc = mds.tTextCategory.Select("FK_TextCategory_Parent="+c.fPK_TextCategory,"Description");

			foreach( DataRow dr in drc )
			{
				TextCategoryDataRow ctChild = new TextCategoryDataRow(dr);
				TextTreeNode tnChild = new TextTreeNode(null,ctChild,ctChild.fDescription);
				tn.Nodes.Add(tnChild);
				tnChild.ImageIndex = -1;
				AddTextChildren(ctChild,tnChild);
				AddTextDesignObjs(ctChild,tnChild);
			}
		}

		public static TextTreeNode GetNodeByID(TreeView tv,int id)
		{
			TextDataRow tdr = mds.tText[id];
			if (!tdr.bIsValid) return null;

			TreeNodeCollection tnc = tv.Nodes;
			foreach (int catID in GetCategoryPath(tdr))
			{
				tnc = getNodeWithCategoryID(tnc,catID).Nodes;
			}
			return getNodeWithTextID(tnc,id);
		}

		private static ArrayList GetCategoryPath(TextDataRow tdr)
		{
			ArrayList al = new ArrayList();
			TextCategoryDataRow tcdr = tdr.fFK_TextCategory_DataRow;
			while (tcdr != null)
			{
				al.Add(tcdr.fPK_TextCategory);				
				tcdr = (tcdr.fFK_TextCategory_ParentIsNull) ? null : tcdr.fFK_TextCategory_Parent_DataRow;
			}
			al.Reverse();
			return al;
		}

		private static bool isNodeWithTextID(TextTreeNode ttn, int id)
		{
			return (ttn.TextDataRow != null && ttn.TextDataRow.fPK_Text == id);
		}
		private static bool isNodeWithCategoryID(TextTreeNode ttn, int id)
		{
			return (ttn.TextCategoryDataRow != null && ttn.TextCategoryDataRow.fPK_TextCategory == id);
		}
		
		private static TextTreeNode getNodeWithTextID(TreeNodeCollection tnc, int id) // find at one level
		{
			foreach (TextTreeNode ttn in tnc)
			{
				if (isNodeWithTextID(ttn,id)) return ttn;
			}
			return null;
		}
		private static TextTreeNode getNodeWithCategoryID(TreeNodeCollection tnc, int id) // find at one level
		{
			foreach (TextTreeNode ttn in tnc)
			{
				if (isNodeWithCategoryID(ttn,id)) return ttn;
			}
			return null;
		}
	}
	public class TextTreeNode : TreeNode
	{
		public TextDataRow m_drText;
		public TextCategoryDataRow m_drTextCategory;

		public TextTreeNode(TextDataRow dr,TextCategoryDataRow drTC,string Description) : base(Description)
		{
			m_drText = dr;
			m_drTextCategory = drTC;
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

		public TextDataRow TextDataRow
		{
			get
			{
				return m_drText;
			}
		}

		public TextCategoryDataRow TextCategoryDataRow
		{
			get
			{
				return m_drTextCategory;
			}
		}
	}
}
