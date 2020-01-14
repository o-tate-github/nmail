#include <iostream>
#include <gtkmm.h>
#include <curl/curl.h>
#include <ui_main.h>

ModelColumns::ModelColumns()
{
    add(m_col_uid);
    add(m_col_key);
    add(m_col_value);
}

UIMain::UIMain()
{
	/* Root Window */
	set_default_size(480, 500);
	set_border_width(10);
	set_title("N-Mail");

	/* Root Pane */
	account_name_label.set_label("test@one-start.tech");
	account_name_label.set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
	account_name_label.set_visible(true);
	account_list_combo.add(account_name_label);
	account_list_combo.set_size_request(380,40);
	account_list_combo.set_visible(true);
	account_sync_button.set_label("Sync");
	account_sync_button.set_size_request(80,40);
	account_sync_button.set_visible(true);

	ref_m_folder_list = Gtk::ListStore::create(m_folder_list_column);
    folder_list_combo.set_model(ref_m_folder_list);

	Gtk::TreeModel::Row row = *(ref_m_folder_list->append());
	row[m_folder_list_column.mColumnName] = "* none *";
	folder_list_combo.set_active(row);
	folder_list_combo.pack_start(m_folder_list_column.mColumnName);

	folder_list_combo.set_size_request(380,40);
	folder_list_combo.set_visible(true);

	folder_view_button.set_label("View");
	folder_view_button.set_visible(true);
	folder_view_button.set_size_request(80,40);

	account_list_hbox.add(account_list_combo);
	account_list_hbox.add(account_sync_button);
	account_list_hbox.set_size_request(460,40);
	account_list_hbox.set_visible(true);

	folder_list_hbox.add(folder_list_combo);
	folder_list_hbox.add(folder_view_button);
	folder_list_hbox.set_size_request(460,40);
	folder_list_hbox.set_visible(true);

	ref_m_message_list = Gtk::TreeStore::create(m_cols);
    view_TreeStore.signal_row_collapsed().connect(sigc::mem_fun(*this, &UIMain::on_treeview_row_collapsed) );
    view_TreeStore.signal_row_expanded().connect(sigc::mem_fun(*this, &UIMain::on_treeview_row_expanded) );
    view_TreeStore.signal_row_activated().connect(sigc::mem_fun(*this, &UIMain::on_treeview_row_activated) );

    view_TreeStore.set_model(ref_m_message_list);
    view_TreeStore.append_column("Key", m_cols.m_col_key);
    view_TreeStore.append_column("Value", m_cols.m_col_value);

    view_TreeStore.columns_autosize();
    view_TreeStore.set_activate_on_single_click(true);
	view_TreeStore.set_visible(true);
	view_TreeStore.set_size_request(460,400);

    message_list_scrolled_window.add(view_TreeStore);
	message_list_scrolled_window.set_size_request(460,360);
	message_list_scrolled_window.set_visible(true);

	btnMessageView.set_label("View Message");
	btnMessageView.set_size_request(460,40);
	btnMessageView.set_visible(true);

	message_list_vbox.add(account_list_hbox);
	message_list_vbox.add(folder_list_hbox);
	message_list_vbox.add(message_list_scrolled_window);
	message_list_vbox.add(btnMessageView);
	message_list_vbox.set_visible(true);

	ref_m_HeaderTreeStore = Gtk::TreeStore::create(m_header_cols);
    view_HeaderTreeStore.set_model(ref_m_HeaderTreeStore);
    view_HeaderTreeStore.append_column("Key", m_header_cols.m_col_key);
    view_HeaderTreeStore.append_column("Value", m_header_cols.m_col_value);
    view_HeaderTreeStore.columns_autosize();
	view_HeaderTreeStore.set_visible(true);

    view_HeaderScrolledWindow.add(view_HeaderTreeStore);
	view_HeaderScrolledWindow.override_background_color(Gdk::RGBA("#f0f0ff"), Gtk::STATE_FLAG_NORMAL);
	view_HeaderScrolledWindow.set_size_request(460,128);
	view_HeaderScrolledWindow.set_visible(true);

    wv =  WEBKIT_WEB_VIEW(webkit_web_view_new());
    webkit_web_view_load_plain_text(wv, "Hello!");
    //webkit_web_view_load_html(wv, "Hello!", "https://osa2.me/");
    webkitWebViewWidget = Glib::wrap(GTK_WIDGET(wv));
    webkitWebViewWidget->set_size_request(460, 312);  
    webkitWebViewWidget->show();
/*
	view_BodyText.set_size_request(460,312);
	view_BodyText.override_background_color(Gdk::RGBA("#ffffe6"), Gtk::STATE_FLAG_NORMAL);
	view_BodyText.set_border_width(5);
	view_BodyText.set_wrap_mode(Gtk::WrapMode::WRAP_WORD);
	view_BodyText.set_resize_mode(Gtk::ResizeMode::RESIZE_QUEUE);
	view_BodyText.set_editable(false);
	view_BodyText.set_visible(true);
*/

    //view_BodyScrollableWindow.add(view_BodyText);
    view_BodyScrollableWindow.add(*webkitWebViewWidget);
	view_BodyScrollableWindow.set_size_request(460,312);
	view_BodyScrollableWindow.override_background_color(Gdk::RGBA("#ffffc6"), Gtk::STATE_FLAG_NORMAL);
	view_BodyScrollableWindow.set_policy (Gtk::PolicyType::POLICY_ALWAYS,
										  Gtk::PolicyType::POLICY_ALWAYS);
	view_BodyScrollableWindow.set_resize_mode(Gtk::ResizeMode::RESIZE_QUEUE);
	view_BodyScrollableWindow.set_visible(true);

	btnBackListView.set_label("Back to List");
	btnBackListView.set_size_request(460,40);
	btnBackListView.set_visible(true);

	message_view_vbox.add(view_HeaderScrolledWindow);
	message_view_vbox.add(view_BodyScrollableWindow);
	// message_view_vbox.add(*webkitWebViewWidget);
	message_view_vbox.add(btnBackListView);
	message_view_vbox.set_visible(true);

	Glib::ustring tabListViewRecv = "List";
	root_notebook.append_page(message_list_vbox);
	root_notebook.set_tab_label_text(message_list_vbox, tabListViewRecv);
	Glib::ustring tabMessageViewRecv = "Msg";
	root_notebook.append_page(message_view_vbox);
	root_notebook.set_tab_label_text(message_view_vbox, tabMessageViewRecv);
	root_notebook.set_current_page(0);
	root_notebook.set_visible(true);
	add(root_notebook);

	account_sync_button.signal_clicked().connect(sigc::mem_fun(*this, &UIMain::account_sync_button_clicked));
	folder_view_button.signal_clicked().connect(sigc::mem_fun(*this, &UIMain::folder_view_button_clicked));
	btnMessageView.signal_clicked().connect(sigc::mem_fun(*this, &UIMain::button_message_view_clicked));
	btnBackListView.signal_clicked().connect(sigc::mem_fun(*this, &UIMain::button_back_listview_clicked));
}

void UIMain::on_treeview_row_collapsed(const Gtk::TreeModel::iterator& iter, const Gtk::TreeModel::Path& path)
{
  Gtk::TreeModel::iterator itr = ref_m_message_list->get_iter(path);
  if(itr)
  {
    Gtk::TreeModel::Row m_row = *itr;
    m_row[m_cols.m_col_key] = "";
    view_TreeStore.columns_autosize();
  }
}

void UIMain::on_treeview_row_expanded(const Gtk::TreeModel::iterator& iter, const Gtk::TreeModel::Path& path)
{
  Gtk::TreeModel::iterator itr = ref_m_message_list->get_iter(path);
  if(itr)
  {
    Gtk::TreeModel::Row m_row = *itr;
    m_row[m_cols.m_col_key] = "Title";
    view_TreeStore.columns_autosize();
  }
}

void UIMain::on_treeview_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn *view_column) {
  Gtk::TreeModel::iterator itr = ref_m_message_list->get_iter(path);
  if(itr)
  {
    Gtk::TreeModel::Row m_row = *itr;
	uid_selected = m_row[m_cols.m_col_uid];
	cout << "row::uid is " << m_row[m_cols.m_col_uid] << "\n";
  }
}

void UIMain::account_sync_button_clicked() {

	cout << "The Button Sync was clicked!\n\n";

	imap.syncOfAccount();
	folderList = imap.getFolders();

	ref_m_folder_list->clear();
	Gtk::TreeModel::Row row = *(ref_m_folder_list->append());
	row[m_folder_list_column.mColumnName] = "* select a folder *";

	list<IMAPFolder>::iterator it;
	for (it = folderList.begin(); it != folderList.end(); ++it)
	{
		string folderName = (*it).getFolderName();
		Gtk::TreeModel::Row row = *(ref_m_folder_list->append());
		row[m_folder_list_column.mColumnName] = (Glib::ustring)folderName;
	}
	folder_list_combo.set_active(2);

	imap.getMail();
	folderList = imap.getFolders();
	for (it = folderList.begin(); it != folderList.end(); ++it)
	{
		cout << "Folder [" << (*it).getFolderName() << "] has " << (*it).messageNum << " message(s).\n";
	}

	cout << "The Button Sync was processed!\n\n";
}

void UIMain::folder_view_button_clicked() {
	cout << "The Button View was clicked!\n\n";

	string folderName;
	Gtk::TreeModel::iterator iter = folder_list_combo.get_active();
	if(iter)
	{
		Gtk::TreeModel::Row row = *iter;
		if(row)
		{
			Glib::ustring name = row[m_folder_list_column.mColumnName];
			folderName = (string)name;
		}
	}
	else
	{
		cout << "invalid iter.\n";
	}
	cout << "Folder [" << folderName << "] selected!\n\n";

	list<IMAPFolder>::iterator it;
	for (it = folderList.begin(); it != folderList.end(); ++it)
	{
		if (folderName.compare((*it).getFolderName()) == 0)
		{
			cout << "Folder matched!\n";
			IMAPFolder *folder = &(*it);
			printf("Message num is : %d\n", folder->messageNum);
			list<IMAPMessage>::iterator im;
			for (im = folder->messageList.begin(); im != folder->messageList.end(); ++im)
			{
				Gtk::TreeModel::iterator row_itr = ref_m_message_list->append();
				Gtk::TreeModel::Row m_row = *row_itr;

				m_row[m_cols.m_col_uid] = int2ustring((*im).uid);
				m_row[m_cols.m_col_key] = "";
				m_row[m_cols.m_col_value] = (Glib::ustring)((*im).subject);

				Gtk::TreeModel::Row m_row_child1 = *(ref_m_message_list->append(m_row.children()));
				m_row_child1[m_cols.m_col_uid]   = int2ustring((*im).uid);
				m_row_child1[m_cols.m_col_key]   = "From";
				m_row_child1[m_cols.m_col_value] = (Glib::ustring)((*im).from);

				Gtk::TreeModel::Row m_row_child2 = *(ref_m_message_list->append(m_row.children()));
				m_row_child2[m_cols.m_col_uid]   = int2ustring((*im).uid);
				m_row_child2[m_cols.m_col_key]   = "Date";
				m_row_child2[m_cols.m_col_value] = (Glib::ustring)((*im).sentTime);
			}
			view_TreeStore.show_all_children();
		}
	}

	cout << "The Button View was processed!\n\n";
}

void UIMain::button_message_view_clicked() {
	cout << "The Button Message View was clicked!\n\n";

	// an example
	std::string folderName = "INBOX";

	list<IMAPFolder>::iterator it;
	for (it = folderList.begin(); it != folderList.end(); ++it)
	{
		if (uid_selected.compare("") == 0)
			break;

		if (folderName.compare((*it).getFolderName()) == 0)
		{
			cout << "Folder matched!\n";
			IMAPFolder *folder = &(*it);
			printf("Message num is : %d\n", folder->messageNum);
			list<IMAPMessage>::iterator im;
			for (im = folder->messageList.begin(); im != folder->messageList.end(); ++im)
			{
				if (uid_selected.compare(int2ustring((*im).uid)) == 0) {
					cout << "-----\n" << (*im).subject << endl;
					cout << "-----\n" << (*im).body;
					cout << "-----\n";

                    // webkit_web_view_load_html(wv, (const gchar*)(((*im).body).c_str()), "https://osa2.me/");
                    webkit_web_view_load_plain_text(wv, (const gchar*)(((*im).body).c_str()));
                    webkitWebViewWidget = Glib::wrap(GTK_WIDGET(wv));
                    webkitWebViewWidget->set_size_request(460, 312);  
                    webkitWebViewWidget->show_now();
/*
					ref_m_TextBuffer = Gtk::TextBuffer::create();
					ref_m_TextBuffer->set_text((Glib::ustring)((*im).body));
					view_BodyText.set_buffer(ref_m_TextBuffer);
*/
					ref_m_HeaderTreeStore->clear();

					Gtk::TreeModel::iterator row_itr;
					row_itr = ref_m_HeaderTreeStore->append();
					Gtk::TreeModel::Row m_row_title = *row_itr;
					m_row_title[m_header_cols.m_col_uid] = int2ustring((*im).uid);
					m_row_title[m_header_cols.m_col_key] = "Title";
					m_row_title[m_header_cols.m_col_value] = (Glib::ustring)((*im).subject);

					row_itr = ref_m_HeaderTreeStore->append();
					Gtk::TreeModel::Row m_row_from = *row_itr;
					m_row_from[m_cols.m_col_uid]   = "";
					m_row_from[m_cols.m_col_key]   = "From";
					m_row_from[m_cols.m_col_value] = (Glib::ustring)((*im).from);

					row_itr = ref_m_HeaderTreeStore->append();
					Gtk::TreeModel::Row m_row_date = *row_itr;
					m_row_date[m_cols.m_col_uid]   = "";
					m_row_date[m_cols.m_col_key]   = "Date";
					m_row_date[m_cols.m_col_value] = (Glib::ustring)((*im).sentTime);
				}
			}
    		view_HeaderTreeStore.columns_autosize();
			view_HeaderTreeStore.show_all_children();
			root_notebook.set_current_page(1);
		}
	}
	cout << "The Button Message View was processed!\n\n";
}

void UIMain::button_back_listview_clicked() {
	cout << "The Button Back to List was clicked!\n\n";
	root_notebook.set_current_page(0);
	cout << "The Button Back to List was processed!\n\n";
}

Glib::ustring UIMain::int2ustring(int i)
{
    std::ostringstream in_stream;
    in_stream << i;
    Glib::ustring ustr = in_stream.str();

    return ustr;
}
