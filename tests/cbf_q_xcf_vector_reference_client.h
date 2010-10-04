#ifndef CBF_Q_XCF_VECTOR_REFERENCE_CLIENT_HH
#define CBF_Q_XCF_VECTOR_REFERENCE_CLIENT_HH

#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QCheckBox>
#include <QtGui/QScrollArea>
#include <QtCore/QObject>


#include <xcf/RemoteServer.hpp>

class Xcf_enter_remote_values_tab : public QWidget{
	Q_OBJECT

	private:
	static const int SPINBOX_DECIMALS = 9;
	static const double SPINBOX_STEP = 0.000000001;	
	static const double SPINBOX_MIN = -1000;
	static const double SPINBOX_MAX = +1000;

	QWidget *inputWindow;
	QPushButton *sendbutton;
	QCheckBox *alwaysSend;
	QSpinBox *decimalSpinBox;

	XCF::RemoteServerPtr _remoteServer;

	unsigned int dim;
	std::vector<QDoubleSpinBox*> *spinboxes;

	public:
	Xcf_enter_remote_values_tab(QWidget *parent, XCF::RemoteServerPtr _remoteServer, std::string input);

	public slots:
	void send();
	void quit();
	void changeSendMode();
	void setDecimals();
};

class Test_xcf_reference_client_gui : public QObject{
	Q_OBJECT
	
	private:
	QApplication *app;
	QTabWidget *window;
	QWidget *connectWindow;
	QLabel *label;
	QLineEdit *lineedit;
	QPushButton *okaybutton;
	QPushButton *exitbutton;
	
	std::vector<Xcf_enter_remote_values_tab*> *tabs;

	public:
	Test_xcf_reference_client_gui(int argc, char *argv[]);

	public slots:
	void connect();
	void quit();
};

#endif
