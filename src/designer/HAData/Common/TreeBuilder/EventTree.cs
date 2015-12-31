using System;
using System.Windows.Forms;
using System.Data;

using HAData.Common;
using HAData.DataAccess;

namespace HAData.Common.TreeBuilder
{
	/// <summary>
	/// Summary description for EventTree.
	/// </summary>
	public class EventTree
	{
		public EventTree(TreeView tvEvent)
		{
			/*
			foreach(DataRow d in HADataConfiguration.m_mdsCache.tEventCategory.Rows )
			{
				EventCategoryDataRow ct = new EventCategoryDataRow(d);

				if( ct.fFK_EventCategory_ParentIsNull )
				{
					EventTreeNode tn = new EventTreeNode(null,ct.fDescription);
					tvEvent.Nodes.Add(tn);
					
					AddEventDesignObjs(ct,tn);
					AddEventChildren(ct,tn);
					int i=tn.Index;
				}
			}
			*/
		}
		private void AddEventDesignObjs(EventCategoryDataRow c,TreeNode tn)
		{
			DataRow[] drc = c.dr.GetChildRows(HARelation.rEventCategory__Event);
			foreach( DataRow dr in drc )
			{
				EventDataRow ctChild = new EventDataRow(dr);
				EventTreeNode tnChild = new EventTreeNode(ctChild,ctChild.fDescription);
				tn.Nodes.Add(tnChild);
			}
		}

		private void AddEventChildren(EventCategoryDataRow c,TreeNode tn)
		{
			/*
			DataRow[] drc = c.dr.GetChildRows(HARelation.rEventCategory__EventCategory_Parent);
			foreach( DataRow dr in drc )
			{
				EventCategoryDataRow ctChild = new EventCategoryDataRow(dr);
				EventTreeNode tnChild = new EventTreeNode(null,ctChild.fDescription);
				tn.Nodes.Add(tnChild);
				tnChild.ImageIndex = -1;
				AddEventDesignObjs(ctChild,tnChild);
				AddEventChildren(ctChild,tnChild);
			}
			*/
		}
	}
	public class EventTreeNode : TreeNode
	{
		EventDataRow m_dr;

		public EventTreeNode(EventDataRow dr,string Description) : base(Description)
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

		public EventDataRow DataRow
		{
			get
			{
				return m_dr;
			}
		}
	}
}
