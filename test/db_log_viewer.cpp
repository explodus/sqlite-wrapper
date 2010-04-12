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
#include <QApplication>
#include <QtGui>

int main( int argc, char **argv )
{
	QApplication app(argc, argv);

	QWindow window;

	window.resize(320, 240);
	window.show();
	window.setWindowTitle(
	QApplication::translate("toplevel", "Top-level widget"));

	return app.exec();
}

