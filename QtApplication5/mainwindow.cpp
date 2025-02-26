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
	delete ui;
}

