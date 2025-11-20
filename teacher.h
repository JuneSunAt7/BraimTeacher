#ifndef TEACHER_H
#define TEACHER_H

#include <QString>

struct Teacher {
    int id;
    QString name;
    QString department;
    int totalHours; // Упрощённо: общее количество часов нагрузки

    // Конструктор
    Teacher(int id = 0, const QString &name = "", const QString &department = "", int hours = 0)
        : id(id), name(name), department(department), totalHours(hours) {}
};

#endif // TEACHER_H
