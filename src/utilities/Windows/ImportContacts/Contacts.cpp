//#include "StdAfx.h"
#include ".\contacts.h"

ContactsList::~ContactsList()
{
	vector<Contact *>::iterator it;
	for(it=begin();it!=end();it++)
		delete *it;

	clear();
}

fstream& operator << (fstream& f, const Contact& contact)
{
	f << "Read contact" << endl;
	f << "First name:" << contact.firstName << endl;
	f << "Last name:" << contact.lastName << endl;

	f << contact.title << endl;
	f << contact.jobDescription << endl;
	f << "Company name:" << contact.companyName << endl;

	f << "Home phone:" << contact.phoneHome << endl;
	f << "Bussiness phone:" << contact.homeBusiness << endl;
	f << "Mobile phone:" << contact.phoneMobile << endl;
	f << "Fax phone" << contact.phoneFax << endl;

	f << "Home address:" << contact.homeAddress << endl;
	f << "business address:" << contact.businessAddress << endl;

	f << "Email:" << contact.email << endl;
	return f;
}

void ContactsList::addContact( Contact *pContact)
{
	push_back( pContact );
};

