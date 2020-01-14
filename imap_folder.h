#include <string>
#include <imap_message.h>

using namespace std;

class IMAPFolder
{
	public:
		IMAPFolder();
		IMAPFolder(string);
		int messageNum;
		int unreadNum;
		string name;
		list<IMAPMessage> messageList;
		string getFolderName();
		void setFolderName(string);
};
