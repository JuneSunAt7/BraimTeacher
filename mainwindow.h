#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

#include "teacher.h"
#include <QList>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddTeacher();
    void onSaveReport();
    void onImportCSV();

private:
    void setupUI();
    void populateTable();
    void addTeacherToList(const Teacher &teacher);
    void loadSampleData(); // Для демонстрации
    bool importCSV(const QString &filePath);

    QTableWidget *m_table;
    QList<Teacher> m_teachers;
    int m_nextId = 1; // Простой способ генерации ID
};

#endif // MAINWINDOW_H
