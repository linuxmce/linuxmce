#ifndef __UPDATE_XML_H__
#define __UPDATE_XML_H__

#include <string>
#include <map>
#include <vector>

#include <libxml/parser.h>
#include <libxml/tree.h>

using namespace std;

typedef map<string, string> StringMap;
typedef StringMap::iterator StringMapIt;
typedef StringMap::const_iterator StringMapConstIt;
typedef map<string, int> ModelMap;
typedef ModelMap::iterator ModelMapIt;
typedef ModelMap::const_iterator ModelMapConstIt;

class UpdateProperty
{
	public:
		
		string name;
		string value;
		StringMap attributesMap;
};

class UpdateNode
{
	public:
	
		enum UpdatePriority { Normal=0, ASAP, Urgent };
		
		UpdateNode(unsigned id=0);
		~UpdateNode();
		
		unsigned UpdateId() const
		{
			return updateId;
		}
		void SetUpdateId(unsigned id)
		{
			updateId = id;
		}
		
		bool IsRelease(const string & sRelease) const
		{
			return release == sRelease;
		}
		void SetRelease(const string & sRelease)
		{
			release = sRelease;
		}
		
		bool IsModel(const string & model) const;
		void SetModels(const string&);
		
		UpdatePriority Priority() const
		{
			return priority;
		}
		
		void SetPriority(UpdatePriority pri)
		{
			priority = pri;
		}
		
		void SetDescription(const char * text)
		{
			description = text;
		}
		
		const char * Description() const
		{
			return description.c_str();
		}
		
		string GetAttribute(const string&);
		
        StringMap& Attributes()
		{
			return attributesMap;
		}
		
		void AddFile(UpdateProperty * prop)
		{
			files.push_back(prop);
		}
		
		vector<UpdateProperty*> & Files()
		{
			return files;
		}
		
		void AddOption(UpdateProperty * prop)
		{
			options.push_back(prop);
		}
		
		vector<UpdateProperty*> & Options()
		{
			return options;
		}
		
		void AddRequire(UpdateProperty * prop)
		{
			requires.push_back(prop);
		}
		
		vector<UpdateProperty*> & Requires()
		{
			return requires;
		}
		
		void AddProperty(const string & name, UpdateProperty * prop)
		{
			properties[name] = prop;
		}
		
		map<string, UpdateProperty*> & Properties()
		{
			return properties;
		}
		
	private:
	
		unsigned updateId;
		ModelMap models;
		UpdatePriority priority;
		string description;
		string release;
		StringMap attributesMap;
		map<string, UpdateProperty*> properties;
		vector<UpdateProperty*> files;
		vector<UpdateProperty*> options;
		vector<UpdateProperty*> requires;
};

class UpdatesXML
{
	public:
	
		static char * tagUpdate;
		static char * tagId;
		static char * tagPriority;
		static char * tagDescription;
		static char * tagRelease;
		static char * tagModels;
		static char * tagFiles;
		static char * tagOptions;
		static char * tagRequires;
		static char * attrOptionsType;
		static char * attrOptionsPre;
		static char * attrOptionsPost;
		static char * attrRequiresModel;
		static char * attrRequiresUpdate;
		
		UpdatesXML(std::string sXmlFile = "");
		
		~UpdatesXML();
		
		void ParseXML(xmlNode *pNode = NULL);
		
		void GenerateXML(const char * outFileName);
		
		void GenerateUpdateXML(UpdateNode * pUpdateNode, const char * outFileName, xmlNodePtr pNode=NULL);
		
		bool Failed() { return m_bFailed; }
		
		std::string & GetXML()
		{
			return m_sXmlFile;
		}
		
		void SetXML(std::string sXmlFile)
		{
			m_sXmlFile = sXmlFile;
		}
		
		vector<UpdateNode*>& Updates()
		{
			return m_updates;
		}
		
		void Updates(vector<UpdateNode*>&, const string&) const;
		
		void Clean();
		
		static void xmlExample(const char * name);

	private:
	
		std::string m_sXmlFile;
		bool m_bFailed;
		vector<UpdateNode*> m_updates;

		void ParseUpdate(xmlNode *pNode);

		//helpers
		static void ParseAttributes(xmlNode *pNode, StringMap& mapAttributes);
};
#endif //__UPDATE_XML_H__
