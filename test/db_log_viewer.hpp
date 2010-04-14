///	@file test/db_log_viewer.hpp
///  
///	@date 16.3.2010 19:51
///  
/// @author T. Schroeder (explodus@gmx.de)
///  
/// @brief
///  
/// This file is part of the sqlite-wrapper project
///

#ifndef SQLITE_WRAPPER_TEST_DB_LOG_VIEWER_HPP_INCLUDED
#define SQLITE_WRAPPER_TEST_DB_LOG_VIEWER_HPP_INCLUDED

#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/detail/query.hpp>

#include <QWidget>
#include <QtCore/QAbstractItemModel>

class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QSortFilterProxyModel;
class QTreeView;

class db_log_model : public QAbstractItemModel 
{
	db::query_ptr _q;

	Q_OBJECT
public:
	enum
	{
			COLUMN_FIRST = -1
		, COLUMN_ID
		, COLUMN_DATE_REAL
		, COLUMN_DATE_TEXT
		, COLUMN_LEVEL
		, COLUMN_MSG
		, COLUMN_LAST
	};

	db_log_model(QObject *parent = 0) : QAbstractItemModel(parent)
	{

	}

	virtual ~db_log_model() { }

	db::query_ptr Q() const { return _q; }
	void Q(db::query_ptr val) { _q = val; }

	int columnCount(const QModelIndex & parent = QModelIndex()) const
	{ UNREFERENCED_PARAMETER(parent); return COLUMN_LAST; }

	int rowCount(const QModelIndex & parent = QModelIndex()) const
	{
		if (parent.isValid()) 
			return 0;

		if (!_q)
			return 0;

		return _q->size();
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

	QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const
	{
		if (parent.isValid()) 
			return QModelIndex();

		if (!hasIndex(row, column, parent)) 
			return QModelIndex();

		return createIndex(row, column);
	}
	
	QModelIndex parent(const QModelIndex & /*index*/) const
	{ return QModelIndex(); }

	Qt::ItemFlags flags(const QModelIndex & index) const
	{
		if (!index.isValid()) 
			return Qt::ItemIsDropEnabled;

		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
	}
};

class dialog : public QWidget
{
	Q_OBJECT

public:
	dialog();

	void setSourceModel(QAbstractItemModel *model);

	private slots:
		void filterRegExpChanged();
		void filterColumnChanged();
		void sortChanged();

private:
	QSortFilterProxyModel *proxyModel;

	QGroupBox *sourceGroupBox;
	QGroupBox *proxyGroupBox;
	QTreeView *sourceView;
	QTreeView *proxyView;
	QCheckBox *filterCaseSensitivityCheckBox;
	QCheckBox *sortCaseSensitivityCheckBox;
	QLabel *filterPatternLabel;
	QLabel *filterSyntaxLabel;
	QLabel *filterColumnLabel;
	QLineEdit *filterPatternLineEdit;
	QComboBox *filterSyntaxComboBox;
	QComboBox *filterColumnComboBox;
};

#endif
