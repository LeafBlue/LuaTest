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

    void test3();

    void test4();

    static int cfunc1(lua_State *l_);


    void test5();

    void test6();

    void test7();

    void test8();


    void test9();



};
#endif // MAINWINDOW_H
