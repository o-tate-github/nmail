#include <iostream>
#include <curl/curl.h>
#include <imap_folder.h>

using namespace std;

class IMAPClient
{
	public:
		IMAPClient();
		~IMAPClient();
		void syncOfAccount();
		void getMail();
		void getMailHeader();
		void getMailBody();
		void cleanup();
		list<IMAPFolder> getFolders();

	private:
		CURL *curl;
		list<IMAPFolder> folderList;
		static size_t parseInitResult(char*, size_t, size_t, void*);
		static size_t parseSelectResult(char*, size_t, size_t, void*);
		static size_t parseSearchResult(char*, size_t, size_t, void*);
		static size_t parseFetchResult(char*, size_t, size_t, void*);
		static size_t parseBodyStructureResult(char*, size_t, size_t, void*);
		static string iso_8859_1_to_utf8(string&);
		static size_t calcDecodeLength(char*);
		static int Base64Decode(char*, unsigned char**, size_t*);
		static int b64_op(char*, int, char*, int, int);
		static int MimeQPDecode(char*, int, char*);
};
