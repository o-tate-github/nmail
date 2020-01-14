#include <string>
#include <address_list.h>

using namespace std;

class IMAPMessage
{
	public:
		IMAPMessage();
		int uid;
		string from;
		AddressList recipients;
		AddressList copies;
		string subject;
		string body;
		string sentTime;
		string receivedTime;
};
