

#include "mainwindow.h"
#include "teacher.h" // <-- ВОТ ЭТА СТРОКА БЫЛА ПРОПУЩЕНА

#include <QApplication>
#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QAbstractItemView>
#include <QLineEdit>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_table(nullptr)
{
    setupUI();
    loadSampleData(); // Загрузим немного данных для демонстрации
    populateTable();
}

MainWindow::~MainWindow()
{
    // Деструктор, если нужен
}

void MainWindow::setupUI()
{
    setWindowTitle("MVP Учебная Нагрузка");
    resize(800, 600);

    // Центральный виджет
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Макет
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // Таблица
    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"ID", "Имя", "Кафедра", "Часы Нагрузки"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed); // Позволяет редактировать

    // Подключаем сигнал изменения ячейки для обновления данных
    connect(m_table, &QTableWidget::cellChanged, this, [this](int row, int column){
        if (column == 3) { // Если редактируется колонка "Часы Нагрузки"
            bool ok;
            int newHours = m_table->item(row, column)->text().toInt(&ok);
            if (ok && row >= 0 && row < m_teachers.size()) {
                m_teachers[row].totalHours = newHours;
                qDebug() << "Обновлены часы для преподавателя ID:" << m_teachers[row].id << "на" << newHours;
            }
        }
    });

    layout->addWidget(m_table);

    // Кнопки
    QPushButton *btnAdd = new QPushButton("Добавить Преподавателя", this);
    QPushButton *btnSave = new QPushButton("Сохранить Отчет", this);
    QPushButton *btnImport = new QPushButton("Импорт CSV", this);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(btnAdd);
    buttonLayout->addWidget(btnSave);
    buttonLayout->addWidget(btnImport);

    layout->addLayout(buttonLayout);

    // Подключение сигналов к слотам
    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAddTeacher);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::onSaveReport);
    connect(btnImport, &QPushButton::clicked, this, &MainWindow::onImportCSV);

    // Меню
    QMenu *fileMenu = menuBar()->addMenu("Файл");
    QAction *importAction = new QAction("Импорт CSV", this);
    fileMenu->addAction(importAction);
    connect(importAction, &QAction::triggered, this, &MainWindow::onImportCSV);
}

void MainWindow::populateTable()
{
    m_table->setRowCount(m_teachers.size());
    for (int i = 0; i < m_teachers.size(); ++i) {
        const Teacher &t = m_teachers[i];
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(t.id)));
        m_table->setItem(i, 1, new QTableWidgetItem(t.name));
        m_table->setItem(i, 2, new QTableWidgetItem(t.department));
        m_table->setItem(i, 3, new QTableWidgetItem(QString::number(t.totalHours)));
    }
    // Растягиваем последнюю колонку
    m_table->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::addTeacherToList(const Teacher &teacher)
{
    m_teachers.append(teacher);
    int row = m_table->rowCount();
    m_table->setRowCount(row + 1);

    m_table->setItem(row, 0, new QTableWidgetItem(QString::number(teacher.id)));
    m_table->setItem(row, 1, new QTableWidgetItem(teacher.name));
    m_table->setItem(row, 2, new QTableWidgetItem(teacher.department));
    m_table->setItem(row, 3, new QTableWidgetItem(QString::number(teacher.totalHours)));
    m_table->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::onAddTeacher()
{
    bool ok;
    QString name = QInputDialog::getText(this, "Добавить Преподавателя", "Имя:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    QString department = QInputDialog::getText(this, "Добавить Преподавателя", "Кафедра:", QLineEdit::Normal, "", &ok);
    if (!ok) return; // Если нажали Cancel на кафедре, отменяем добавление

    bool ok_hours;
    int hours = QInputDialog::getInt(this, "Добавить Преподавателя", "Часы Нагрузки:", 0, 0, 1000, 1, &ok_hours);
    if (!ok_hours) return;

    Teacher newTeacher(m_nextId++, name, department, hours);
    addTeacherToList(newTeacher);
    qDebug() << "Добавлен преподаватель:" << newTeacher.name << "ID:" << newTeacher.id;
}

void MainWindow::onSaveReport()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить Отчет", "", "Текстовые файлы (*.txt);;Все файлы (*)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для записи: " + file.errorString());
        return;
    }

    QTextStream out(&file);
    out << "Отчет о Нагрузке Преподавателей\n";
    out << "Дата: " << QDateTime::currentDateTime().toString() << "\n\n";

    for (const Teacher &t : m_teachers) {
        out << "ID: " << t.id << "\t";
        out << "Имя: " << t.name << "\t";
        out << "Кафедра: " << t.department << "\t";
        out << "Часы: " << t.totalHours << "\n";
    }

    file.close();
    QMessageBox::information(this, "Успех", "Отчет успешно сохранен в " + fileName);
}

void MainWindow::onImportCSV()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Импорт CSV", "", "CSV файлы (*.csv);;Все файлы (*)");
    if (fileName.isEmpty()) return;

    if (importCSV(fileName)) {
        populateTable(); // Обновляем таблицу после импорта
        QMessageBox::information(this, "Успех", "Данные успешно импортированы из " + fileName);
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось импортировать данные из " + fileName);
    }
}

bool MainWindow::importCSV(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Ошибка открытия файла CSV:" << file.errorString();
        return false;
    }

    QTextStream in(&file);
    bool isFirstLine = true;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',', Qt::SkipEmptyParts);

        if (fields.size() < 3) continue; // Пропускаем некорректные строки

        if (isFirstLine) {
            // Проверяем, возможно, это заголовок
            if (fields[0] == "ID" || fields[0] == "Имя") {
                isFirstLine = false;
                continue; // Пропускаем заголовок
            }
            isFirstLine = false;
        }

        // Ожидаемый формат: Имя, Кафедра, Часы
        QString name = fields[0].trimmed();
        QString dept = fields[1].trimmed();
        bool ok;
        int hours = fields[2].trimmed().toInt(&ok);
        if (!ok) hours = 0; // Если не число, ставим 0

        // Импортируем как нового преподавателя, присваивая новый ID
        Teacher importedTeacher(m_nextId++, name, dept, hours);
        m_teachers.append(importedTeacher);
        qDebug() << "Импортирован преподаватель:" << importedTeacher.name;
    }
    file.close();
    return true;
}

void MainWindow::loadSampleData()
{
    m_teachers.append(Teacher(m_nextId++, "Иванов И.И.", "Математика", 120));
    m_teachers.append(Teacher(m_nextId++, "Петрова П.П.", "Физика", 100));
    m_teachers.append(Teacher(m_nextId++, "Сидоров С.С.", "Информатика", 150));
}
