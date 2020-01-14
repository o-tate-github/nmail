#include <string>
#include <imap_folder.h>


IMAPFolder::IMAPFolder()
{
	IMAPFolder((string)"INBOX");
}

IMAPFolder::IMAPFolder(string _folderName)
{
	messageNum = 0;
	unreadNum  = 0;
	name = _folderName;
}

string IMAPFolder::getFolderName()
{
	return name;
}

void IMAPFolder::setFolderName(string _folderName)
{
	name = _folderName;
}
