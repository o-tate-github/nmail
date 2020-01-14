#include <gtkmm.h>

class ModelSingleColumn : public Gtk::TreeModel::ColumnRecord
{
	public:
		ModelSingleColumn();
		Gtk::TreeModelColumn<Glib::ustring> mColumnName;
};

class ModelMessage : public Gtk::TreeModel::ColumnRecord
{
	public:
		ModelMessage();
		Gtk::TreeModelColumn<Glib::ustring> mColumnFrom;
		Gtk::TreeModelColumn<Glib::ustring> mColumnDate;
		Gtk::TreeModelColumn<Glib::ustring> mColumnSubject;
};

using namespace std;

