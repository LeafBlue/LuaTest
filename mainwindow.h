#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    void test1();

    static int setprint(lua_State *l_);


    void test2();






};
#endif // MAINWINDOW_H
