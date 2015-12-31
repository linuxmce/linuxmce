#pragma once

#include "string"
#include "vector"
#include "fstream"

using namespace std;

struct Contact
{
public:
	string firstName, middleName, lastName;
	string title, jobDescription, companyName;

	string phoneHome, homeBusiness, phoneMobile;
	string phoneFax;

	string homeAddress, businessAddress;
	string email;
};

fstream& operator << (fstream& os, const Contact& contatct);

class ContactsList : public vector<Contact *>
{
public:

	~ContactsList();
	void addContact( Contact *pContact);
};

