#include <string>
#include <list>
#include <gtkmm.h>

using namespace std;

class AddressList
{
	public:
		AddressList();

		int num;
		list<string> addresses;
		list<Glib::ustring> names;

		void addAddress(string, Glib::ustring);
		void parseRecipients(Glib::ustring);
};
