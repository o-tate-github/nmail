#include <iostream>
#include <string>
#include <list>
#include <curl/curl.h>
#include <imap_client.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

#define OP_ENCODE 0
#define OP_DECODE 1

IMAPClient::IMAPClient()
{
}

void IMAPClient::syncOfAccount()
{
	CURLcode res = CURLE_OK;

	curl = curl_easy_init();
	if(curl) {
		/* Set username and password */
		curl_easy_setopt(curl, CURLOPT_USERNAME, "test@one-start.tech");
		curl_easy_setopt(curl, CURLOPT_PASSWORD, "A-sdf_098765");

		/* Set root path for IMAP */
		curl_easy_setopt(curl, CURLOPT_URL, "imap://pop3.lolipop.jp");

		/* Set callback static function */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, IMAPClient::parseInitResult);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&folderList);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() Initial LIST failed: %s\n", curl_easy_strerror(res));
			exit(-1);
		}
		cout << "curl_easy_perform() Initial LIST completed.\n\n";
	}
}

list<IMAPFolder> IMAPClient::getFolders() {
	return folderList;
}

void IMAPClient::getMail() {

	CURLcode res = CURLE_OK;
	list<IMAPFolder>::iterator it;
	list<IMAPMessage>::iterator im;

	for (it = folderList.begin(); it !=folderList.end(); ++it)
	{
		IMAPFolder *f = &(*it);
    	string cmd = "SELECT " + f->getFolderName();

		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, cmd.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, IMAPClient::parseSelectResult);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			cout << "curl_easy_perform() SELECT failed: " << curl_easy_strerror(res) << "\n";
		cout << "curl_easy_perform() SELECT completed.\n\n";

		if (f->messageNum > 0)
		{
			CURLcode res = CURLE_OK;
			string cmd = "SEARCH ALL";

			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, cmd.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, IMAPClient::parseSearchResult);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);

			res = curl_easy_perform(curl);
			if (res != CURLE_OK)
				cout << "curl_easy_perform() SEARCH failed: " << curl_easy_strerror(res) << "\n";

			cout << "curl_easy_perform() SEARCH completed.\n\n";

			for (im = f->messageList.begin(); im != f->messageList.end(); ++im)
			{
				CURLcode res = CURLE_OK;
				char uid_char[10];
				snprintf(uid_char, 10, "%d", (*im).uid);
				string cmd = "FETCH " + string(uid_char) + " (FLAGS BODY[HEADER.FIELDS (DATE FROM SUBJECT)])";

				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, cmd.c_str());
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, IMAPClient::parseFetchResult);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &(*im));

				res = curl_easy_perform(curl);
				if (res != CURLE_OK)
					cout << "curl_easy_perform() FETCH Header failed: " << curl_easy_strerror(res) << "\n";
				cout << "curl_easy_perform() FETCH Header completed.\n\n";
				cout << "Message[" << (*im).uid << "] is:\n";
				cout << "    Date    : " << (*im).sentTime << "\n";
				cout << "    From    : " << (*im).from << "\n";
				cout << "    Subject : " << (*im).subject << "\n\n";

				cmd = "FETCH " + string(uid_char) + " (BODYSTRUCTURE RFC822.TEXT)";

				curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, cmd.c_str());
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, IMAPClient::parseBodyStructureResult);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &(*im));

				res = curl_easy_perform(curl);
				if (res != CURLE_OK)
					cout << "curl_easy_perform() FETCH BODYSTRUCTURE RFC822.TEXT failed: " << curl_easy_strerror(res) << "\n";
     			cout << "Message[" << (*im).uid << "] is:\n";
    			cout << "    Date    : " << (*im).sentTime << "\n";
    			cout << "    From    : " << (*im).from << "\n";
    			cout << "    Subject : " << (*im).subject << "\n";
    			cout << "    Body f  :\n" << (*im).body << "\n";
    			cout << "    Body t  :\n\n";
    
				cout << "curl_easy_perform() BODYSTRUCTURE RFC822.TEXT completed.\n\n";
			}
		}

	}
}

size_t IMAPClient::parseBodyStructureResult(char *instr, size_t size, size_t num, void *ptr) {

	int STRBUF_MAX_LENGTH = 65535;
	char strbuf[STRBUF_MAX_LENGTH];

	for (int i = 0; i < STRBUF_MAX_LENGTH; i++)
		strbuf[i] = 0x0;

	IMAPMessage *msg = (IMAPMessage *)ptr;

	int i_pos = 0;
	int b_pos = 0;
    while (instr[i_pos] != 0x0)
	{
		if (instr[i_pos] == '\n')
		{
			strbuf[b_pos] = 0x0;
			printf("xxxxxxxxxxxxx\n");
			printf("%s\n", strbuf);
			printf("xxxxxxxxxxxxx\n");
			i_pos++;
            break;
		}
		else
		{
			strbuf[b_pos] = instr[i_pos];
			i_pos++;
			b_pos++;
		}
		
	}

	cout << "[FIRST LINE] :" << strbuf << "\n\n";
	char *from = strchr(strbuf, '{');
	char *to = strchr(from, '}');
	*to =0x0;

	int bodyLen = atoi(&from[1]);
	cout << "[BODY LENGTH] :" << bodyLen << "\n\n";

	char dec[65536];
	IMAPClient::MimeQPDecode(&instr[i_pos], bodyLen, dec);
	string bodyString = string(dec);
	msg->body.append((string(bodyString)));
	cout << "[BODY --START--]\n";
	cout << msg->body << "\n";
	cout << "[BODY ---END---]\n\n";

	return (size * num);
}

size_t IMAPClient::parseFetchResult(char *instr, size_t size, size_t num, void *ptr) {

	int BUFFER_LENGTH = 1024;
	char strbuf[1024];
	string recvString;
	string recvUtf8String;

	cout << "parseFetchResult() called.\n";

	IMAPMessage *msg = (IMAPMessage *)ptr;

	int pos = 0;
	int bufpos = 0;
    while (instr[pos] != '\n' && pos < (BUFFER_LENGTH -1))
	{
		strbuf[bufpos] = instr[pos];
		pos++;
		bufpos++;
	}
	strbuf[bufpos] = 0x0;
	recvString = string(strbuf);

	int p1 = recvString.find("{", 0);
	int p2 = recvString.find("}", 0);
	int byteNum = atoi(recvString.substr((p1+1), (p2-p1)).c_str());
	cout << "RECV: " << recvString << "\n";

	pos++;
    int basepos = pos;
	while (pos < (basepos + byteNum)) {

		bufpos = 0;
    	while (instr[pos] != '\n' && pos < (BUFFER_LENGTH -1))
		{
			strbuf[bufpos] = instr[pos];
			pos++;
			bufpos++;
		}
		strbuf[--bufpos] = 0x0;

		recvString = string(strbuf);
		if (recvString.find(":") != string::npos) {
			cout << "RECV [" << (recvString.length() +1) << "] " << recvString << "\n";

			int colon = (int)recvString.find(":");
			string lside = recvString.substr(0, colon);

			/* rside --- trim and base64 decode --- */
			/* trim */
			string rside_bare = string(recvString.substr(colon+1));
			int s1 = 0;
			int s2 = rside_bare.length() - 1;
			for (int i=0; rside_bare.substr(i,1) == " "; i++)
				s1 = i+1;
			for (int i=(rside_bare.length()-1); rside_bare.substr(i,1) == " "; i--)
				s2 = i-1;
			string rside_trimmed = rside_bare.substr(s1, (s2-s1)+1);

			/* base64 decode */
			string rside;
			int b64_from = 0;
			int b64_to   = 0;
			int b64_end  = rside_trimmed.length() - 1;

			int encoded = 0;
			b64_from = (int)(rside_trimmed.find("=?", 0));
			if (b64_from > 0)
				rside += rside_trimmed.substr(0, b64_from - 1);

			while (b64_from < b64_end && b64_from != (int)(string::npos))
			{
				encoded = 0;
				b64_from = (int)(rside_trimmed.find("B?", b64_from + 2));
				if (b64_from == (int)(string::npos))
					break;
				b64_to = (int)(rside_trimmed.find("?=", b64_from + 2));
				if (b64_to == (int)(string::npos))
					break;

				string enc = rside_trimmed.substr(b64_from + 2, b64_to - b64_from -2);
				cout << "enc is [" << enc << "]\n";

				char dec[1024];
				for(int p = 0; p < 1024; p++)
					dec[p] = 0x0;
				IMAPClient::b64_op((char *)(enc.c_str()), (int)(enc.length()), dec, 0, OP_DECODE);
				dec[sizeof(dec)-1] = 0x0;
				rside += string(dec);
				encoded = 2;

				if ((b64_to + 2) > b64_end)
					break;

				b64_from = (int)(rside_trimmed.find("=?", b64_to + 2));
				if (b64_from == (int)(string::npos))
					break;
				if (b64_from > (b64_to + 2))
					rside += rside_trimmed.substr(b64_to + 2, b64_from - b64_to - 2);
			}
			if ((b64_from < b64_to) && (b64_to < b64_end - 1))
				rside += rside_trimmed.substr(b64_to + encoded);
			if ((b64_from > b64_to) && (b64_from < b64_end - 1))
				rside += rside_trimmed.substr(b64_from + encoded);

			if (lside.compare("Date") == 0)
				msg->sentTime = string(rside);
			if (lside.compare("From") == 0)
				msg->from = string(rside);
			if (lside.compare("Subject") == 0)
				msg->subject = string(rside);

		}
		pos++;
		bufpos = 0;
	}


	cout << "parseFetchResult() call end.\n\n";

	return (size * num);
}

size_t IMAPClient::parseSearchResult(char *instr, size_t size, size_t num, void *ptr) {

	int BUFFER_LENGTH = 1024;
	char strbuf[1024];

	cout << "parseSearchResult() called.\n";

	IMAPFolder *folder = (IMAPFolder *)ptr;

	int pos = 0;
    while (instr[pos] != '\n' && pos < (BUFFER_LENGTH -1))
	{
		strbuf[pos] = instr[pos];
		pos++;
	}
	strbuf[pos] = 0x0;
	string recvString = string(strbuf);

	cout << "RECV: " << recvString << "\n";
	if (recvString.find("SEARCH", 0) != string::npos)
	{
		int pos = 9;
		int nxt = recvString.find(" ", pos);
		while ((nxt > 0) && (nxt < (int)recvString.length()))
		{
			int uid = atoi(recvString.substr(pos, nxt - pos).c_str());

			IMAPMessage msg;
			msg.uid = uid;
			folder->messageList.push_back(msg);
			cout << "Message (uid:" << msg.uid << ") has been stored!\n";

			pos = nxt + 1;
			nxt = recvString.find(" ", pos);
		}

		int uid = atoi(recvString.substr(pos).c_str());

		IMAPMessage msg;
		msg.uid = uid;
		folder->messageList.push_back(msg);
		cout << "Message (uid:" << msg.uid << ") has been stored!\n";
	}

	cout << "parseSearchResult() call end.\n\n";

	return (size * num);
}


size_t IMAPClient::parseSelectResult(char *instr, size_t size, size_t num, void *ptr) {

	int BUFFER_LENGTH = 1024;
	char strbuf[1024];

	IMAPFolder *folder = (IMAPFolder *)ptr;

	int pos = 0;
    while (instr[pos] != '\n' && pos < (BUFFER_LENGTH -1))
	{
		strbuf[pos] = instr[pos];
		pos++;
	}
	strbuf[pos] = 0x0;
	string recvString = string(strbuf);

	int messageCount = 0;
	if (recvString.find("EXISTS", 0) != string::npos)
	{
		int endPos = recvString.find(" ", 2);
		if (endPos > 2) {
			string messageCountString = recvString.substr(2, (endPos -2));
			messageCount = atoi(messageCountString.c_str());
		}
		cout << "Folder [" << folder->getFolderName() << "] has " << messageCount << " message(s).\n";
	}

	if (messageCount > 0) {
		folder->messageNum = messageCount;
	}

	return (size * num);
}

size_t IMAPClient::parseInitResult(char *instr, size_t size, size_t num, void *ptr) {

	int BUFFER_LENGTH = 1024;
	char strbuf[1024];

	list<IMAPFolder> *fl = (list<IMAPFolder> *)ptr;

	int pos = 0;
    while (instr[pos] != '\n' && pos < (BUFFER_LENGTH -1))
	{
		strbuf[pos] = instr[pos];
		pos++;
	}
	strbuf[pos] = 0x0;
	string strLine (strbuf);

	string folderName = "";
	if (strLine.find("* LIST") != string::npos
		&& strLine.find("\".\" \"") != string::npos)
	{
		int startPos = (int)strLine.find("\".\" \"") + 5;
		int endPos = (int)strLine.length() - 2;
		int len = endPos - startPos;
		folderName = strLine.substr(startPos, len);
	}
	IMAPFolder folder = IMAPFolder(folderName);
	fl->push_back(folder);

	return (size * num);
}

IMAPClient::~IMAPClient() {
	curl_easy_cleanup(curl);
}

size_t IMAPClient::calcDecodeLength(char* b64input) { //Calculates the length of a decoded string
	size_t len = strlen((const char*)b64input),
		padding = 0;

	if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
		padding = 2;
	else if (b64input[len-1] == '=') //last char is =
		padding = 1;

	return (len*3)/4 - padding;
}

int IMAPClient::b64_op(char* in, int in_len, char *out, int out_len, int op)
{
    int ret = 0;
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO *bio = BIO_new(BIO_s_mem());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_push(b64, bio);
    if (op == 0)
    {
        ret = BIO_write(b64, in, in_len);
        BIO_flush(b64);
        if (ret > 0)
        {
            ret = BIO_read(bio, out, sizeof(out));
        }

    } else
    {
		out_len = (int)IMAPClient::calcDecodeLength(in);
        ret = BIO_write(bio, in, in_len);
        BIO_flush(bio);
        if (ret)
        {
            ret = BIO_read(b64, out, out_len);
        }
    }
    BIO_free(b64);
    return ret;
}

int IMAPClient::MimeQPDecode(char *in, int len, char *out)
{
	in[len] = 0x0;
    int p = 0;
    for (int i=0; i < len; i++)
    {
        if (in[i] == '=')
        {
            if ((i+1 < len && i+2 < len)
				&& (in[i+1] ==13 && in[i+2] == 10))
			{
				i += 2;
				continue;
			}

            if (((i+1 < len) && (i+2 < len))
				&& ((in[i+1] >= 48 && in[i+1] <= 57) || (in[i+1] >= 65 && in[i+1] <= 70))
				&& ((in[i+2] >= 48 && in[i+2] <= 57) || (in[i+2] >= 65 && in[i+2] <= 70)))
            {
                char code[3];
                code[0] = in[i+1];
                code[1] = in[i+2];
                code[2] = 0x0;
                char a = (char)strtol(code, 0, 16);
                out[p] = a;
                p++;
                i += 2;
            }
            else
            {
                out[p] = in[i];
                p++;
            }   
        }
        else
        {
            out[p] = in[i];
            p++;
        }
    }
    out[p] = 0x0;

	return 0;

}
