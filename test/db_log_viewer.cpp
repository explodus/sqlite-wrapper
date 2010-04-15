///	@file test/db_log_viewer.cpp
///  
///	@date 16.3.2010 19:51
///  
/// @author T. Schroeder (explodus@gmx.de)
///  
/// @brief
///  
/// This file is part of the sqlite-wrapper project
///

#include <sqlite_wrapper/config.hpp>
#include <sqlite_wrapper/db.hpp>
#include <sqlite_wrapper/log.hpp>

#include <QApplication>
#include <QDialog>
#include <QtGui>

#include "db_log_viewer.hpp"

namespace db
{
	namespace test 
	{
		std::string log_db = "log.db";
	}

	namespace detail
	{
		inline QString internal_convert( std::string const &_s )
		{  return QString( QByteArray( _s.c_str() ) ); }

		inline QString internal_convert( std::wstring const &_s )
		{ return QString::fromUtf16( reinterpret_cast<const ushort*>( _s.c_str() ), _s.length() ); }
	}

	inline QString to_qstring( db::string const &_s )
	{ return detail::internal_convert(_s); }
}


QVariant db::test::db_log_model::headerData( 
	  int section
	, Qt::Orientation orientation
	, int role ) const
{
	QVariant tmp;

	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) 
	{
		switch (section) 
		{
			case COLUMN_ID:
				tmp = tr("#");
				break;
			case COLUMN_DATE_REAL:
				tmp = tr("DateReal");
				break;
			case COLUMN_DATE_TEXT:
				tmp = tr("DateText");
				break;
			case COLUMN_LEVEL:
				tmp = tr("Level");
				break;
			case COLUMN_MSG:
				tmp = tr("Message");
				break;
			default:
				qCritical() 
					<< __FUNCTION__ 
					<< "Error: unknown column:" 
					<< section;
		}
	}

	return tmp;
}

QVariant db::test::db_log_model::data( 
	  const QModelIndex & index
	, int role /*= Qt::DisplayRole*/ ) const
{
	static QVariant empty_tmp;

	if (!index.isValid() || index.model() != this) 
		return empty_tmp;

	if (!_q)
		return empty_tmp;

	if (_q->size() == 0)
		return empty_tmp;

	int row(index.row()), column(index.column());

	if (role == Qt::DisplayRole) 
	{
		static QVariant filled_tmp;

		switch (column)
		{
		case db_log_model::COLUMN_LEVEL:
			switch (_q->getRow(row).value(column).get_int())
			{
			case db::log::log_critical:
				filled_tmp = QString("critical");
				break;
			case db::log::log_error:
				filled_tmp = QString("error");
				break;
			case db::log::log_warning:
				filled_tmp = QString("warning");
				break;
			case db::log::log_notice:
				filled_tmp = QString("notice");
				break;
			case db::log::log_info:
				filled_tmp = QString("info");
				break;
			case db::log::log_debug:
				filled_tmp = QString("debug");
				break;
			default:
				filled_tmp = QString("undef");
				break;
			}
			break;
		default :
			filled_tmp = db::to_qstring(
				_q->getRow(row).value(column).str());
			break;
		}
		return filled_tmp;
	}
	else if (role == Qt::BackgroundColorRole) 
	{
		switch (_q->getRow(row).value(db_log_model::COLUMN_LEVEL).get_int())
		{
		case db::log::log_critical:
			return qVariantFromValue(QColor(Qt::white));
		case db::log::log_error:
			return qVariantFromValue(QColor(Qt::red));
		case db::log::log_warning:
			return qVariantFromValue(QColor(Qt::darkRed));
		case db::log::log_notice:
			return qVariantFromValue(QColor(Qt::darkYellow));
		case db::log::log_info:
			return qVariantFromValue(QColor(Qt::yellow));
		case db::log::log_debug:
			return qVariantFromValue(QColor(Qt::lightGray));
		default:
			return qVariantFromValue(QColor(Qt::white));
		}
	}
	
	return empty_tmp;
}

int main( int argc, char **argv )
{
	QApplication app(argc, argv);
	QStringList arguments = app.arguments();
	QString app_path = app.applicationDirPath();

	db::test::log_db = app_path.toAscii();
	db::test::log_db += "/log.db";

	db::test::dialog dlg;
	dlg.show();
	return app.exec();
}

db::test::dialog::dialog() : 
	  proxyModel(0)
	, sourceGroupBox(0)
	, proxyGroupBox(0)
	, sourceView(0)
	, proxyView(0)
	, filterCaseSensitivityCheckBox(0)
	, sortCaseSensitivityCheckBox(0)
	, showAllCheckBox(0)
	, filterPatternLabel(0)
	, filterSyntaxLabel(0)
	, filterColumnLabel(0)
	, filterPatternLineEdit(0)
	, filterSyntaxComboBox(0)
	, filterColumnComboBox(0)
	, model(0)
{
	proxyModel = new QSortFilterProxyModel;
	proxyModel->setDynamicSortFilter(true);

	sourceGroupBox = new QGroupBox(tr("Database"));
	proxyGroupBox = new QGroupBox(tr("Sorted/Filtered Database Lines"));

	sourceView = new QTableView;
	sourceView->verticalHeader()->setDefaultSectionSize(17);
	//sourceView->verticalHeader()->setResizeMode (QHeaderView::ResizeMode::Interactive);
	//sourceView->verticalHeader()->setResizeMode (QHeaderView::ResizeMode::ResizeToContents);

	proxyView = new QTableView;
	proxyView->setModel(proxyModel);
	proxyView->setSortingEnabled(true);
	proxyView->verticalHeader()->setResizeMode (QHeaderView::ResizeMode::ResizeToContents);

	model = new db::test::db_log_model(this);
	setSourceModel(model);

	sortCaseSensitivityCheckBox = new QCheckBox(tr("Case sensitive sorting"));
	filterCaseSensitivityCheckBox = new QCheckBox(tr("Case sensitive filter"));

	showAllCheckBox = new QCheckBox(tr("Show All Lines"));

	filterPatternLineEdit = new QLineEdit;
	filterPatternLabel = new QLabel(tr("&Filter pattern:"));
	filterPatternLabel->setBuddy(filterPatternLineEdit);

	filterSyntaxComboBox = new QComboBox;
	filterSyntaxComboBox->addItem(tr("Regular expression"), QRegExp::RegExp);
	filterSyntaxComboBox->addItem(tr("Wildcard"), QRegExp::Wildcard);
	filterSyntaxComboBox->addItem(tr("Fixed string"), QRegExp::FixedString);
	filterSyntaxLabel = new QLabel(tr("Filter &syntax:"));
	filterSyntaxLabel->setBuddy(filterSyntaxComboBox);

	filterColumnComboBox = new QComboBox;
	filterColumnComboBox->addItem(tr("#"));
	filterColumnComboBox->addItem(tr("DateReal"));
	filterColumnComboBox->addItem(tr("DateText"));
	filterColumnComboBox->addItem(tr("Level"));
	filterColumnComboBox->addItem(tr("Message"));
	filterColumnLabel = new QLabel(tr("Filter &column:"));
	filterColumnLabel->setBuddy(filterColumnComboBox);

	connect(filterPatternLineEdit, SIGNAL(textChanged(QString)),
		this, SLOT(filterRegExpChanged()));
	connect(filterSyntaxComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(filterRegExpChanged()));
	connect(filterColumnComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(filterColumnChanged()));
	connect(filterCaseSensitivityCheckBox, SIGNAL(toggled(bool)),
		this, SLOT(filterRegExpChanged()));
	connect(showAllCheckBox, SIGNAL(toggled(bool)),
		this, SLOT(showAllChanged()));
	connect(sortCaseSensitivityCheckBox, SIGNAL(toggled(bool)),
		this, SLOT(sortChanged()));

	QHBoxLayout *sourceLayout = new QHBoxLayout;
	sourceLayout->addWidget(sourceView);
	sourceGroupBox->setLayout(sourceLayout);

	QGridLayout *proxyLayout = new QGridLayout;
	proxyLayout->addWidget(proxyView, 0, 0, 1, 4);
	proxyLayout->addWidget(filterPatternLabel, 1, 0);
	proxyLayout->addWidget(filterPatternLineEdit, 1, 1, 1, 3);
	proxyLayout->addWidget(filterSyntaxLabel, 2, 0);
	proxyLayout->addWidget(filterSyntaxComboBox, 2, 1, 1, 3);
	proxyLayout->addWidget(filterColumnLabel, 3, 0);
	proxyLayout->addWidget(filterColumnComboBox, 3, 1, 1, 3);
	proxyLayout->addWidget(filterCaseSensitivityCheckBox, 4, 0, 1, 3);
	proxyLayout->addWidget(sortCaseSensitivityCheckBox, 4, 1);
	proxyLayout->addWidget(showAllCheckBox, 4, 3);
	proxyGroupBox->setLayout(proxyLayout);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(sourceGroupBox);
	mainLayout->addWidget(proxyGroupBox);
	setLayout(mainLayout);

	setWindowTitle(tr("Basic Sort/Filter Model"));
	resize(600, 800);

	proxyView->sortByColumn(0, Qt::DescendingOrder);
	filterColumnComboBox->setCurrentIndex(3);

	filterPatternLineEdit->setText("error");
	filterCaseSensitivityCheckBox->setChecked(true);
	sortCaseSensitivityCheckBox->setChecked(true);
	showAllCheckBox->setChecked(false);

	refresh();
}

void db::test::dialog::setSourceModel(QAbstractItemModel *model)
{
	proxyModel->setSourceModel(model);
	sourceView->setModel(model);
}

void db::test::dialog::filterRegExpChanged()
{
	QRegExp::PatternSyntax syntax =
		QRegExp::PatternSyntax(filterSyntaxComboBox->itemData(
		filterSyntaxComboBox->currentIndex()).toInt());
	Qt::CaseSensitivity caseSensitivity =
		filterCaseSensitivityCheckBox->isChecked() 
		? Qt::CaseSensitive
		: Qt::CaseInsensitive;

	QRegExp regExp(filterPatternLineEdit->text(), caseSensitivity, syntax);
	proxyModel->setFilterRegExp(regExp);
}

void db::test::dialog::filterColumnChanged()
{
	proxyModel->setFilterKeyColumn(filterColumnComboBox->currentIndex());
}

void db::test::dialog::sortChanged()
{
	proxyModel->setSortCaseSensitivity
	(
		  sortCaseSensitivityCheckBox->isChecked() 
		? Qt::CaseSensitive
		: Qt::CaseInsensitive
	);
}

void db::test::dialog::showAllChanged()
{
	refresh();
}

void db::test::dialog::refresh()
{
	try
	{
		db::base_ptr db_; 
		db_.reset(new db::base);
		db_->connect(db::test::log_db.c_str());
		model->Q(
			db_->execute_ptr(
			(
				  db::sel(L"log")
				, L"id"
				, L"date_real"
				, L"date_text"
				, L"level"
				, L"msg")
				.limit(showAllCheckBox->isChecked() ? 0 : 1000)
				.order_by(L"id", false)
			)
		);
	}
	catch (db::exception::base& e)
	{
		db::log::singleton::basic::log_msg msg(
			db::log::singleton::basic::get_log()
			, e.what()
			, db::log::log_error);
	}
}