//##########################################################################
//#                                                                        #
//#                              CLOUDCOMPARE                              #
//#                                                                        #
//#  This program is free software; you can redistribute it and/or modify  #
//#  it under the terms of the GNU General Public License as published by  #
//#  the Free Software Foundation; version 2 or later of the License.      #
//#                                                                        #
//#  This program is distributed in the hope that it will be useful,       #
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
//#  GNU General Public License for more details.                          #
//#                                                                        #
//#          COPYRIGHT: EDF R&D / TELECOM ParisTech (ENST-TSI)             #
//#                                                                        #
//##########################################################################

#ifndef CC_CONSOLE_HEADER
#define CC_CONSOLE_HEADER

//qCC_db
#include <ccLog.h>

//Qt
#include <QObject>
#include <QMutex>
#include <QStringList>
#include <QTimer>
#include <QVector>
#include <QPair>
#include <QFile>

#include <QListWidget>
#include <QClipboard>
#include <QKeyEvent>
#include <QApplication>

class QWidget;
class MainWindow;
class QTextStream;

//! Custom QListWidget to allow for the copy of all selected elements when using CTRL+C
class ccCustomQListWidget : public QListWidget
{
public:

	ccCustomQListWidget(QWidget* parent = 0) : QListWidget(parent) {}

protected:
	
	virtual void keyPressEvent(QKeyEvent *event) override
	{
		if (event->matches(QKeySequence::Copy))
		{
			int itemsCount = count();
			QStringList strings;
			for (int i = 0; i < itemsCount; ++i)
			{
				if (item(i)->isSelected())
				{
					strings << item(i)->text();
				}
			}

			QApplication::clipboard()->setText(strings.join("\n"));
		}
		else
		{
			QListWidget::keyPressEvent(event);
		}
	}
};

//! Console
class ccConsole : public QObject, public ccLog
{
	Q_OBJECT

public:

	//! Destructor
	virtual ~ccConsole();

	//! Inits console (and optionaly associates it with a text output widget)
	/** WARNING: in release mode, no message will be output if no 'textDisplay'
		widget is defined. Moreover, error messages will only appear in a
		(blocking) QMessageBox if a 'parentWidget' widget is defined.
		In debug mode, all message are sent to system console (with 'printf').
		\param textDisplay text output widget (optional)
		\param parentWidget parent widget (optional)
		\param parentWindow parent window (if any - optional)
	**/
	static void Init(	QListWidget* textDisplay = 0,
						QWidget* parentWidget = 0,
						MainWindow* parentWindow = 0);

	//! Returns the (unique) static instance
	static ccConsole* TheInstance();

	//! Releases unique instance
	static void ReleaseInstance();

	//! Sets auto-refresh state
	void setAutoRefresh(bool state);

	//! Sets log file
	bool setLogFile(QString filename);

	//! Whether to show Qt messages (qDebug / qWarning / etc.) in Console
	static void EnableQtMessages(bool state);

	//! Returns whether to show Qt messages (qDebug / qWarning / etc.) in Console or not
	static bool QtMessagesEnabled() { return s_showQtMessagesInConsole; }

	//! Returns the parent widget (if any)
	inline QWidget* parentWidget() { return m_parentWidget; }

public slots:

	//! Refreshes console (display all messages still in queue)
	void refresh();

protected:

	//! Default constructor
	/** Constructor is protected to avoid using this object as a non static class.
	**/
	ccConsole();

	//inherited from ccLog
	virtual void logMessage(const QString& message, int level);

	//! Associated text display widget
	QListWidget* m_textDisplay;

	//! Parent widget
	QWidget* m_parentWidget;

	//! Parent window (if any)
	MainWindow* m_parentWindow;

	//! Mutex for concurrent thread access to console
	QMutex m_mutex;

	//! Queue element type (message + color)
	typedef QPair<QString,int> ConsoleItemType;

	//! Queue for incoming messages
	QVector<ConsoleItemType> m_queue;

	//! Timer for auto-refresh
	QTimer m_timer;

	//! Log file
	QFile m_logFile;
	//! Log file stream
	QTextStream* m_logStream;

	//! Whether to show Qt messages (qDebug / qWarning / etc.) in Console
	static bool s_showQtMessagesInConsole;
};

#endif
