#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// Datenbank initialisieren
	initializeDatabase();

	// Tabelle einrichten
	setupTable();

	// Transaktionen laden
	loadTransactions();

	// ComboBox-Kategorien
	ui->categoryCombo->addItems({ "Lebensmittel", "Miete", "Transport", "Unterhaltung", "Gehalt", "Sonstiges" });
	ui->typeCombo->addItems({ "Einnahme", "Ausgaben" });

	// Signal-Slot-Verbindungen
	connect(ui->transactionTable->selectionModel(), &QItemSelectionModel::selectionChanged,
		this, &MainWindow::updateTotalLabels);
	
}

MainWindow::~MainWindow()
{
	db.close();
	delete ui;
}

void MainWindow::initializeDatabase()
{
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("haushaltsbuch.db");

	if (!db.open())
	{
		QMessageBox::critical(this, "Fehler", "Datenbank konnte nicht geöffnet werden!");
		return;
	}

	// Tabelle erstellen
	QSqlQuery query;
	query.exec("CREATE TABLE IF NOT EXISTS transactions ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"date DATE NOT NULL,"
		"description TEXT,"
		"category TEXT ,"
		"amount REAL ,"
		"type TEXT)");
		
}

void MainWindow::setupTable()
{
	model = new QStandardItemModel(this);
	model->setColumnCount(5);
	model->setHorizontalHeaderLabels({ "Datum", "Beschreibung", "Kategorie", "Betrag", "Typ" });
	ui->transactionTable->setModel(model);
}

