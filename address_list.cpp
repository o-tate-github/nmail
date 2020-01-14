#include <string>
#include <gtkmm.h>
#include <address_list.h>

AddressList::AddressList()
{
	num = 0;
}

void AddressList::addAddress(string _address, Glib::ustring _name)
{
	addresses.push_back(_address);
	names.push_back(_name);
	num++;
}

void AddressList::parseRecipients(Glib::ustring text)
{

}

