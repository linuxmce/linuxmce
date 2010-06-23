using System;
using HAData.DataAccess;
using HAData.Common;

namespace HADesigner
{
	/// <summary>
	/// Abstract class for interacting with particular database tables from forms.
	/// </summary>
	public abstract class UI
	{
		protected int id = -1;

		protected bool needsDBInsert = false;
		protected bool needsDBDelete = false;
		protected bool deleted = false;			//this becomes true AFTER the delete

		protected MyDataSet mds
		{
			get	{return HADataConfiguration.m_mdsCache;}
		}

		/// <summary>
		/// Set in constructor, -1 if needs insert
		/// </summary>
		public int ID 
		{	
			get {return this.id;}
			set	{this.id = value;}
		}

		public bool NeedsInsert
		{	
			get {return this.needsDBInsert;}
			set	{this.needsDBInsert = value;}
		}
		public bool NeedsDelete
		{
			get	{return this.needsDBDelete;}
			set	{this.needsDBDelete = value;}
		}
		public bool Deleted
		{
			get	{return this.deleted;}
			set {this.deleted = value;} // To delete a new item
		}

		public virtual bool Include
		{
			get {return (!this.Deleted && !this.NeedsDelete);}
		}

		/// <summary>
		/// With current structure, can't define in UI
		/// </summary>
		public abstract bool OriginalsChanged {get;}

		public abstract void LoadFromDatabase();
		public abstract bool SaveToDatabase();
		protected abstract void ResetOriginals();
	}
}
