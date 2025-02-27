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
	ui->typeCombo->addItems({ "Einnahme", "Ausgabe" });

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
		"rowid INTEGER PRIMARY KEY AUTOINCREMENT, "
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

	QSqlQuery query("SELECT rowid, date, description, category, amount, type FROM transactions");

	while (query.next())
	{
		QList<QStandardItem*> items;

		// Item für das Datum (Column 0)
		QStandardItem* dateItem = new QStandardItem(query.value(1).toString());

		// Speichern der rowid in Userrole + 1
		dateItem->setData(query.value(0).toInt(), Qt::UserRole + 1);

		// Column 1 : Beschreibung
		QStandardItem* descItem = new QStandardItem(query.value(2).toString());

		// Column 2 : Kategorie
		QStandardItem* catItem = new QStandardItem(query.value(3).toString());

		// Column 3: Betrag ( & euro-char)
		double amount = query.value(4).toDouble();
		QString amountText = QString("%1 %2").arg(QString::number(amount, 'f', 2))
							.arg(QChar(0x20AC));
		QStandardItem* amountItem = new QStandardItem(amountText);

		// Column 4: (Einnahmen/Ausgaben)
		QStandardItem* typeItem = new QStandardItem(query.value(5).toString());

		// Tabellenreihe zusammensetzen
		items << dateItem << descItem << catItem << amountItem << typeItem;
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
	QStandardItem* dateItem = model->item(row, 0);

	if (!dateItem)
	{
		return;
	}

	// rowid holen
	QVariant rowid = dateItem->data(Qt::UserRole + 1);

	QSqlQuery query;
	query.prepare("UPDATE transactions SET date=?, description=?, category=?, amount=?, type=? "
		"WHERE rowid=?");
	query.addBindValue(ui->dateEdit->date().toString("yyyy-MM-dd"));
	query.addBindValue(ui->descriptionEdit->text());
	query.addBindValue(ui->categoryCombo->currentText());
	query.addBindValue(ui->amountSpin->value());
	query.addBindValue(ui->typeCombo->currentText());
	query.addBindValue(rowid.toInt());

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
	// Hole das Item aus Spalte 0 (Datum-Spalte)
	QStandardItem* dateItem = model->item(row, 0);
	if (!dateItem)
	{
		return;
	}

	// rowid aus dem UserRole+1
	QVariant rowid = dateItem->data(Qt::UserRole + 1);

	QSqlQuery query;
	query.prepare("DELETE FROM transactions WHERE rowid=?");
	query.addBindValue(rowid.toInt());

	if (query.exec())
	{
		loadTransactions();
		clearInputFields();
	}
}

void MainWindow::updateTotalLabels()
{
	double income = 0;
	double expenses = 0;

	QString euroSymbol = QChar(0x20AC);

	for (int row = 0; row < model->rowCount(); row++)
	{
		
		QString rawAmount = model->item(row, 3)->text();

		rawAmount.remove(" " + euroSymbol);

		double amount = rawAmount.toDouble();


		// Einnahme oder Ausgabe bestimmen
		if (model->item(row, 4)->text() == "Einnahme") 
		{
			income += amount;
		}
		else 
		{
			expenses += amount;
		}


	}

	ui->incomeLabel ->setText(QString("Einnahmen: %1 %2 ").arg(income, 0, 'f', 2).arg(euroSymbol));
	ui->expensesLabel->setText(QString("Ausgaben: %1 %2 ").arg(expenses, 0, 'f', 2).arg(euroSymbol));
	ui->balanceLabel->setText(QString("Saldo: %1 %2 ").arg(income - expenses, 0, 'f', 2).arg(euroSymbol));
}

void MainWindow::clearInputFields()
{
	ui->dateEdit->setDate(QDate::currentDate());
	ui->descriptionEdit->clear();
	ui->categoryCombo->setCurrentIndex(0);
	ui->amountSpin->setValue(0.0);
	ui->typeCombo->setCurrentIndex(0);
}

void MainWindow::on_generateReportButton_clicked()
{
	QSqlQuery query;

	query.exec("SELECT strftime('%Y-%m', date) AS month, "
		"SUM(CASE WHEN type= 'Einnahme' THEN amount ELSE 0 END) as income, "
		"SUM(CASE WHEN type='Ausgabe' THEN amount ELSE 0 END) as expenses "
		"FROM transactions GROUP BY month");

	QString report = "Monatliche Übersicht:\n\n";

	while (query.next())
	{
		report += QString("%1\n Einnahmen: %2 €\n Ausgaben: %3 €\n Saldo: %4 €\n\n")
			.arg(query.value("month").toString())
			.arg(query.value("income").toDouble(), 0, 'f', 2)
			.arg(query.value("expenses").toDouble(), 0, 'f', 2)
			.arg(query.value("income").toDouble() - query.value("expenses").toDouble(), 0, 'f', 2);
	}

	QMessageBox::information(this, "Monatsbericht", report);
}

