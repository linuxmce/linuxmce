using System.Data;
using System;
using System.Runtime.Serialization;

namespace HAData.Common
{
	//----------------------------------------------------------------
	// Namespace: ShopNet.SystemFramework.Framework
	// Class: SerializableDataSet
	//
	// Description: 
	//   A base class that adds ISerializable support to a DataSet,
	//     providing for automatica serialization of a DataSet with an
	//     out-of-process session state configuration. The same ISerializable
	//     mechanism used for session state can also be employed to support
	//     passing a DataSet to a method on a marshaled COM+ object.
	//
	// Special considerations:
	//   A subclass of this object must implement a constructor to be called
	//     by the runtime to deserialize the data. That constructor must
	//     first build the schema for the DataSet, then call SetObjectData on
	//     the base class. The constructor should be private so that only
	//     the runtime can call it.
	//
	//Private Overloads Sub New(ByVal info As SerializationInfo, ByVal context As StreamingContext)
	//    MyBase.New
	//
	//    //Build the schema
	//    BuildDataTables
	//
	//    //Populate the DataSet
	//    MyBase.SetObjectData info, context
	//End Sub
	//
	//   A subclass of this object can add serialization entries using the 
	//     following code as an example.
	//
	//Protected Overrides Sub GetDesignObjData(ByVal info As SerializationInfo, ByVal context As StreamingContext)
	//    MyBase.GetDesignObjData info, context
	//    With info
	//        .AddValue KEY_CUSTOMERID, m_CustomerID
	//        .AddValue KEY_ITEMCOUNT, m_ItemCount
	//    End With
	//End Sub
	//
	//Private Overloads Sub New(ByVal info As SerializationInfo, ByVal context As StreamingContext)
	//    MyBase.New
	//
	//    //Build the schema
	//    BuildDataTables
	//
	//    //Populate the DataSet
	//    MyBase.SetObjectData info, context
	//
	//    //Read off other values
	//    m_ItemCount = info.GetValue(KEY_ITEMCOUNT, sometype)
	//    m_CustomerID = info.GetValue(KEY_CUSTOMERID, sometype)
	//End Sub
	//----------------------------------------------------------------
	public class SerializableDataSet : DataSet, ISerializable 
	{

		//The key used to store and retrieve information from the SerializationInfo object
		private const String KEY_XMLDATA = "XmlData";

		//----------------------------------------------------------------
		// Sub GetObjectData:
		//   Use the XmlData property of the DataSet to persist the object as a
		//     string and write it to a SerializationInfo object.
		// Parameters:
		//   [in] info: The SerializationInfo object to write to
		//   [in] context: Information on who is calling this method 
		//----------------------------------------------------------------
		void ISerializable.GetObjectData(SerializationInfo info, StreamingContext context) {
			GetDesignObjData(info, context);
		}
		protected virtual void GetDesignObjData(SerializationInfo info, StreamingContext context) {
			//
			// Populate the SerializationInfo object. The runtime creates this
			//   object before we get it, so we just add a value. Note that
			//   Me.GetXml() always returns something in practice, so the extra
			//   check is just for robustness.
			//

			//Retrieve the string data from the DataSet
			String strData = GetXml();

			//Write the data to the SerializationInfo object
			if (strData != String.Empty) {
				info.AddValue(KEY_XMLDATA, strData);
			}
		}

		//----------------------------------------------------------------
		// Sub SetObjectData:
		//   Reinitialize the object to a persisted state using the "XmlData"
		//     property in the SerializationInfo object. The schema must be
		//     initialized before calling this function. See example of a
		//     a class derived from SerializableDataSet shown above.
		// Parameters:
		//   [in] info: The SerializationInfo object to read from
		//   [in] context: Information on who is calling this method
		//----------------------------------------------------------------
		protected void SetObjectData(SerializationInfo info, StreamingContext context) {
			//
			// Read the data from the provided info object and 
			//   initialize this object using the XmlData property.
			//
			String strData = (String)info.GetValue(KEY_XMLDATA, typeof(System.String));
			if (strData != String.Empty) {
			//		this.ReadXml(ms);
			}
		}
	} //class SerializableDataSet
} //namespace ShopNet.Common