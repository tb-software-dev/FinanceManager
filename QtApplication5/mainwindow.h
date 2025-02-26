#pragma once

#include <QMainWindow>
#include <QtSql>
#include <QMessageBox>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE

namespace Ui
{
	class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();


private slots:
	void on_addButton_clicked();
	void on_editButton_clicked();
	void on_deleteButton_clicked();
	void on_generateReportButton_clicked();
	void updateTotalLabels();

private:
	Ui::MainWindow* ui;
	QSqlDatabase db;
	QStandardItemModel* model;

	void initializeDatabase();
	void setupTable();
	void loadTransactions();
	void clearInputFields();

};