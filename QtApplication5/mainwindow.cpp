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
	ui->transactionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void MainWindow::loadTransactions()
{
	model->removeRows(0, model->rowCount());

	QSqlQuery query("SELECT date, description, category, amount, type FROM transactions");

	while (query.next())
	{
		QList<QStandardItem*> items;

		for (int i = 0; i < 5; i++)
		{
			items.append(new QStandardItem(query.value(i).toString()));
		}
		model->appendRow(items);
	}
	updateTotalLabels();
}

void MainWindow::on_addButton_clicked()
{
	QSqlQuery query;
	query.prepare("INSERT INTO transactions (date, description, category, amount, type )"
		"VALUES (?, ?, ?, ?, ?)");

	query.addBindValue(ui->dateEdit->date().toString("yyyy-MM-dd"));
	query.addBindValue(ui->descriptionEdit->text());
	query.addBindValue(ui->categoryCombo->currentText());
	query.addBindValue(ui->amountSpin->value());
	query.addBindValue(ui->typeCombo->currentText());

	if (query.exec())
	{
		loadTransactions();
		clearInputFields();
	}
	else
	{
		QMessageBox::critical(this, "Fehler", "Eintrag konnte nicht hinzugefügt werden!");
	}
}

void MainWindow::on_editButton_clicked()
{
	QModelIndexList selection = ui->transactionTable->selectionModel()->selectedRows();

	if (selection.isEmpty())
	{
		return;
	}

	int row = selection.first().row();

	QSqlQuery query;
	query.prepare("UPDATE transactions SET date=?, description=?, category=?, amount=?, type=? "
		"WHERE rowid=?");
	query.addBindValue(ui->dateEdit->date().toString("yyyy-MM-dd"));
	query.addBindValue(ui->descriptionEdit->text());
	query.addBindValue(ui->categoryCombo->currentText());
	query.addBindValue(ui->amountSpin->value());
	query.addBindValue(ui->typeCombo->currentText());
	query.addBindValue(model->item(row, 0)->data(Qt::UserRole + 1)); // ID

	if (query.exec())
	{
		loadTransactions();
		clearInputFields();
	}


}

void MainWindow::on_deleteButton_clicked()
{
	QModelIndexList selection = ui->transactionTable->selectionModel()->selectedRows();
	if (selection.isEmpty())
	{
		return;
	}

	int row = selection.first().row();
	QString id = model->item(row, 0)->data(Qt::UserRole + 1).toString();

	QSqlQuery query;
	query.prepare("DELETE FROM transactions WHERE rowid=?");
	query.addBindValue(id);

	if (query.exec())
	{
		loadTransactions();
		clearInputFields();
	}
}

