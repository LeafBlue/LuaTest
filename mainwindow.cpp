#include "mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{}

MainWindow::~MainWindow() {}

/*
注意：！！！这里会多次操作栈，包括压入栈，获取栈数据
这个栈不是我们理解的系统栈，而是lua提供的一个数据结构
两者并不是同一个东西，只是习惯性称其为栈！！！

luaL_openlibs
打开lua标准库，包括 base、table、string、math、io、os、debug 等
也可以选择单独打开一个，比如打开base luaopen_base(L)
需要头文件<lauxlib.h> <lua.h>

luaL_dofile
加载并执行一个lua文件
成功返回0，失败时可从栈顶获取错误信息

lua_getglobal
从lua的全局环境中获取一个值，压入栈顶。
一般来说这个值可能是一个全局变量名，也可能是个函数名

lua_isfunction
检查是否是函数

lua_pushnumber
按顺序把参数压入栈中，但每次压入后参数在栈顶

lua_pcall( lua虚拟机指针, 参数个数, 期望返回值个数, 错误处理函数的索引)
调用函数
调用函数结束后，会自动弹出函数和参数，并压入返回值
但必须手动处理返回值！


lua_pushcfunction  lua_setglobal
用来实现让lua反向调用C函数
lua_pushcfunction(lua_State *L, lua_CFunction f);
将f（C函数）压入栈顶，
该函数要求不带this指针

lua_setglobal
将栈顶的值赋值给lua的全局变量或者函数，然后弹出栈顶

lua_gettop(L)
得到栈内元素的“总数”。所谓top实际上是高度的意思，不是顶部的意思

lua_type(L, idx)
得到指定位置的类型  得到LUA_TSTRING LUA_TNUMBER等

lua_isstring(L, idx)    lua_tostring(L, idx)
判断是不是字符串，转换为C字符串

当 Lua 调用一个 C 函数时，根据约定，它会把所有参数放在栈上，并且不放任何东西
当从栈上获取数据时，存在正索引和负索引两种方式，通常我们获取栈顶用-1，用的就是负索引
但如果从底部开始，就可以 1 2 3 4这样获取，如果4是栈顶，那它等同于负索引的-1
但如果我们不知道栈的长度，而想要直接获取栈顶，这时候可以用-1

*/





/*
1.
嵌入Lua并执行简单脚本
目标：从C++创建Lua状态，加载并执行一个Lua字符串脚本。
要求：脚本打印"Hello World"，C++端捕获执行错误并打印。
*/
void MainWindow::test1()
{
    lua_State *l_ = luaL_newstate();

    luaL_openlibs(l_);

    lua_pushcfunction(l_,setprint);
    lua_setglobal(l_,"print");

    luaL_dofile(l_,"luafile/test1.lua");

    lua_getglobal(l_,"printword");

    lua_pcall(l_,0,0,0);
}

//替换lua的print函数
int MainWindow::setprint(lua_State *l_)
{
    int paramNum = lua_gettop(l_);

    QString re_str = "";
    for (int i = 1; i <= paramNum; ++i) {
        lua_getglobal(l_,"tostring");
        /*
            这里如何理解？
            实际上他们共用同一个栈，而且以函数名为分界线
            执行tostring时，会从tostring开始往上寻找函数
            此时我们把索引i位置的值再次压入
        */
        lua_pushvalue(l_,i);
        lua_pcall(l_,1,1,0);

        const char* ch = lua_tostring(l_,-1);
        if (ch) {
            re_str += QString::fromUtf8(ch);
        } else {
            re_str += "[nil]";
        }
        lua_pop(l_,1);

        re_str += "\t";
    }

    qDebug().noquote()<<re_str;
    return 0;
}

/*
2.
C++向Lua传递基本类型变量
目标：C++设置Lua全局变量（number、string、boolean），Lua脚本读取并打印它们。
要求：传递一个int、一个std::string、一个bool到Lua全局。
*/
void MainWindow::test2()
{
    int a = 10;
    QString b = "哈哈";
    bool c = false;

    lua_State *l_ = luaL_newstate();

    luaL_openlibs(l_);

    lua_pushcfunction(l_,setprint);
    lua_setglobal(l_,"print");

    luaL_dofile(l_,"luafile/test2.lua");

    lua_getglobal(l_,"printinput");


    lua_pushnumber(l_,a);
    lua_pushstring(l_,b.toStdString().c_str());
    lua_pushboolean(l_,c);

    int ret = lua_pcall(l_,3,0,0);

}


/*
3.
Lua向C++返回基本类型
目标：执行Lua脚本返回一个值，C++读取它。
要求：Lua脚本返回一个计算结果（如两个数的和），C++打印返回值。

*/
