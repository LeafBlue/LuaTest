#include "mainwindow.h"

#include "sol.hpp"

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

lua_createtable
用来初始化一个表数据结构
第二个参数：代表连续索引的数组长度
第三个参数：代表非数组部分的长度

lua_rawseti
这个函数用来把值压入表中
在此之前，需要先把值压入栈
第二个参数：表在栈中的位置，一般是-2
第三个参数，数值在表中的键，可以近似看成下标，但从1开始
每次将值压入表中，会自动弹出栈顶

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

void MainWindow::test3()
{
    lua_State *l_ = luaL_newstate();
    luaL_openlibs(l_);

    luaL_dofile(l_,"luafile/test3.lua");

    lua_getglobal(l_,"getsum");

    lua_pushnumber(l_,5);
    lua_pushnumber(l_,3);
    lua_pcall(l_,2,1,0);

    int re = lua_tonumber(l_,-1);
    qDebug()<<"得到返回值："<<re;

    lua_pop(l_,1);
    lua_close(l_);
}

/*
4.
注册C++函数给Lua调用
目标：定义一个C函数（如计算两个数的平均值），注册到Lua，让Lua脚本调用它。
要求：Lua传入两个number，C函数返回平均值。
*/
void MainWindow::test4()
{
    lua_State *l_ = luaL_newstate();
    luaL_openlibs(l_);

    lua_pushcfunction(l_,setprint);
    lua_setglobal(l_,"print");

    lua_pushcfunction(l_,cfunc1);
    lua_setglobal(l_,"cfunc1");

    luaL_dofile(l_, "luafile/test4.lua");
    lua_close(l_);
}

/*
当你写下这个函数时，你要假设lua已经在调用这个函数
lua在栈中压入了参数
然后你要取出参数，经过你的操作并把返回值压进去
lua自然会处理
*/
int MainWindow::cfunc1(lua_State *l_)
{
    int a = lua_tonumber(l_,1);
    int b = lua_tonumber(l_,2);

    double re = (a+b)/2.0;
    lua_pushnumber(l_,re);

    return 1;
}

/*
5.
C++调用Lua函数（带参数和多返回值）
目标：C++调用Lua中一个带参数的函数，并处理多返回值。
要求：Lua函数接受两个number，返回它们的和与差；C++传入参数并打印结果。
*/
void MainWindow::test5()
{
    lua_State *lua = luaL_newstate();

    luaL_openlibs(lua);

    luaL_dofile(lua,"luafile/test5.lua");
    lua_getglobal(lua,"getresult");

    lua_pushnumber(lua,8);
    lua_pushnumber(lua,6);

    lua_pcall(lua,2,2,0);

    int re1 = lua_tonumber(lua,-1);
    int re2 = lua_tonumber(lua,-2);

    qDebug()<<"re1:"<<re1<<",re2:"<<re2;
    lua_pop(lua,2);

    lua_close(lua);
}

/*
6.
操作Lua table（数组式）
目标：C++创建一个Lua table（作为数组），填充数据，传递给Lua脚本处理。
要求：C++创建table {1,4,9,16}（平方数），Lua脚本计算总和并返回给C++。
*/
void MainWindow::test6()
{
    lua_State *lua = luaL_newstate();
    luaL_openlibs(lua);

    luaL_dofile(lua,"luafile/test6.lua");
    lua_getglobal(lua,"sumtable");

    lua_createtable(lua,4,0);

    lua_pushnumber(lua,2);
    lua_rawseti(lua,-2,1);

    lua_pushnumber(lua,3);
    lua_rawseti(lua,-2,2);

    lua_pushnumber(lua,5);
    lua_rawseti(lua,-2,3);

    lua_pushnumber(lua,8);
    lua_rawseti(lua,-2,4);

    lua_pcall(lua,1,1,0);

    int re = lua_tonumber(lua,-1);
    lua_pop(lua,1);

    qDebug()<<"最终结果："<<re;

    lua_close(lua);
}


/*
操作Lua table（键值式）
目标：C++创建一个关联table（如配置表），Lua读取并修改后返回。
要求：table如{name="Player", hp=100, mp=50}，Lua增加一个key "level=1"，C++读取修改后table。
*/
void MainWindow::test7()
{
    lua_State *lua = luaL_newstate();
    luaL_openlibs(lua);

    luaL_dofile(lua,"luafile/test7.lua");
    lua_getglobal(lua,"updatetable");

    lua_createtable(lua,0,3);

    lua_createtable(lua,1,0);
    lua_pushstring(lua,"Player");
    lua_setfield(lua,-2,"name");

    lua_rawseti(lua,-2,1);

    lua_createtable(lua,1,0);
    lua_pushnumber(lua,100);
    lua_setfield(lua,-2,"hp");

    lua_rawseti(lua,-2,2);

    lua_createtable(lua,1,0);
    lua_pushnumber(lua,50);
    lua_setfield(lua,-2,"mp");

    lua_rawseti(lua,-2,3);

    lua_pcall(lua,1,1,0);

    lua_rawgeti(lua,-1,1);
    lua_getfield(lua,-1,"name");
    if(lua_isstring(lua,-1)){
        qDebug()<<"name = "<<lua_tostring(lua,-1);
    }
    lua_pop(lua,2);

    lua_rawgeti(lua,-1,2);
    lua_getfield(lua,-1,"hp");
    if(lua_isnumber(lua,-1)){
        qDebug()<<"hp = "<<lua_tonumber(lua,-1);
    }
    lua_pop(lua,2);

    lua_rawgeti(lua,-1,3);
    lua_getfield(lua,-1,"mp");
    if(lua_isnumber(lua,-1)){
        qDebug()<<"mp = "<<lua_tonumber(lua,-1);
    }
    lua_pop(lua,2);

    lua_rawgeti(lua,-1,4);
    lua_getfield(lua,-1,"level");
    if(lua_isnumber(lua,-1)){
        qDebug()<<"level = "<<lua_tonumber(lua,-1);
    }
    lua_pop(lua,2);

    lua_pop(lua,3);
    lua_close(lua);

}


/*
8.
Lua table遍历与复杂结构
目标：C++传递一个嵌套table给Lua，Lua遍历并返回统计信息。
要求：嵌套table如{pos={x=10,y=20}, items={"sword","shield"}}，Lua返回items数量和pos的总和。
*/
void MainWindow::test8()
{
    lua_State *lua = luaL_newstate();
    luaL_openlibs(lua);

    luaL_dofile(lua,"luafile/test8.lua");
    lua_getglobal(lua,"checktable");

    //创建主表
    lua_createtable(lua,0,2);
    //创建pos
    lua_createtable(lua,0,2);

    lua_pushnumber(lua,10);
    lua_setfield(lua,-2,"x");

    lua_pushnumber(lua,20);
    lua_setfield(lua,-2,"y");

    lua_setfield(lua,-2,"pos");

    lua_createtable(lua,2,0);

    lua_pushstring(lua,"sword");
    lua_rawseti(lua,-2,1);

    lua_pushstring(lua,"shield");
    lua_rawseti(lua,-2,2);

    lua_setfield(lua,-2,"items");

    lua_pcall(lua,1,2,0);

    int re1 = lua_tonumber(lua,-2);
    int re2 = lua_tonumber(lua,-1);

    qDebug()<<"re1 = "<<re1<<"re2 = "<<re2;

    lua_pop(lua,2);
    lua_close(lua);
}


/*
9.
使用userdata传递C++对象
目标：创建一个简单的C++结构体，用userdata传递到Lua，让Lua调用C++方法。
要求：定义struct Point {int x,y;}，注册add方法（返回新Point），Lua创建并操作Point。
*/
void MainWindow::test9()
{
    struct Point{
        int x,y;
    };

    sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.do_file("luafile/test9.lua");

    lua.new_usertype<Point>("Point",
                            sol::constructors<Point()>(),
                            "x",&Point::x,
                            "y",&Point::y
                            );

    Point p;
    p.x = 10;
    p.y = 20;
    Point re = lua["setpoint"](p);

    qDebug()<<"得到p:"<<p.x<<"---"<<p.y;

}


