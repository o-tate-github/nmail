#include <iostream>
#include <webkit2/webkit2.h>
#include <ui_model.h>
#include <imap_client.h>

using namespace std;

class ModelColumns : public Gtk::TreeModelColumnRecord
{
public:
    ModelColumns();
    Gtk::TreeModelColumn<Glib::ustring> m_col_uid;
    Gtk::TreeModelColumn<Glib::ustring> m_col_key;
    Gtk::TreeModelColumn<Glib::ustring> m_col_value;
};

class UIMain : public Gtk::Window
{
	public:
		UIMain();

	protected:
		IMAPClient imap;
		list<IMAPFolder> folderList;

		Gtk::Notebook root_notebook;
		Gtk::VBox message_list_vbox;
		Gtk::VBox message_view_vbox;

		Gtk::HBox account_list_hbox;
		Gtk::Label account_name_label;
		Gtk::ComboBox account_list_combo;
		Gtk::Button account_sync_button;
		void account_sync_button_clicked();

		Gtk::HBox folder_list_hbox;
		Gtk::ComboBox folder_list_combo;
		Gtk::Button folder_view_button;
		void folder_view_button_clicked();

		ModelSingleColumn m_folder_list_column;
		Glib::RefPtr<Gtk::ListStore> ref_m_folder_list;

        Glib::RefPtr<Gtk::TreeStore> ref_m_message_list;
        Gtk::ScrolledWindow message_list_scrolled_window;
        Gtk::TreeView view_TreeStore;
        ModelColumns m_cols;
		Glib::ustring uid_selected = "";
        void on_treeview_row_collapsed(const Gtk::TreeModel::iterator&, const Gtk::TreeModel::Path&);
        void on_treeview_row_expanded(const Gtk::TreeModel::iterator&, const Gtk::TreeModel::Path&);
        void on_treeview_row_activated(const Gtk::TreeModel::Path&, Gtk::TreeViewColumn*);

        Glib::RefPtr<Gtk::TreeStore> ref_m_HeaderTreeStore;
        Gtk::ScrolledWindow view_HeaderScrolledWindow;
        Gtk::TreeView view_HeaderTreeStore;
        ModelColumns m_header_cols;

        WebKitWebView *wv;
        Gtk::Widget *webkitWebViewWidget;
        Glib::RefPtr<Gtk::TextBuffer> ref_m_TextBuffer;
		Gtk::TextView view_BodyText;
        Gtk::ScrolledWindow view_BodyScrollableWindow;
		Gtk::Button btnBackListView;
		void button_back_listview_clicked();

		Gtk::Button btnMessageView;
		void button_message_view_clicked();

		Glib::ustring int2ustring(int);
};
