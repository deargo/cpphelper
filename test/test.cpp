#include "test.h"
#include <thread>
#include "library.hpp"
#include "command.hpp"
#include "threadpool.hpp"
#include "vector.hpp"

using namespace std;

#   ifdef WIN32
#       define __FUNC__ __FUNCTION__
#   else
#       define __FUNC__ __func__
#   endif

#define TEST_FUNC_BEGIN cout << endl << "=============================begin " << __FUNC__ << endl
#define TEST_FUNC_ENDED cout << endl << "=============================ended " << __FUNC__ << endl <<endl

namespace CppHelperTest
{

void CLibraryTest()
{
    TEST_FUNC_BEGIN;
    //记得动态库路径为绝对路径，或者当前路径。linux动态库需-ldl和LD_LIBRARY_PATH.
#ifdef _WIN32
    CppHelper::CLibrary lib("mydll.dll");
#else
    CppHelper::CLibrary lib("libmydll.so");
#endif
    cout << "dll add exsit: " << lib.exist("add") << endl;

    auto fmax = lib.getFunction<int(int, int)>("Max");
    cout << "dll fMax: " << fmax(1, 5) << endl;

    cout << "dll fAdd: " << lib.execute<int(int, int)>("Add", 5, 8) << endl;

    TEST_FUNC_ENDED;
}


struct STA
{
    int m_a;
    int operator()(){ return m_a; }
    int operator()(int n){ return m_a + n; }
    int triple0(){ return m_a * 3; }
    int triple(int a){ return m_a * 3 + a; }
    int triple1() const { return m_a * 3; }
    const int triple2(int a) const { return m_a * 3+a; }
    void triple3(){ cout << "" <<endl; }
};

int add_one(int n)
{
    return n + 1;
}
void CCommandTest()
{
    TEST_FUNC_BEGIN;

    CppHelper::CCommand<int> cmd;
    //普通函数
    cmd.Wrap(add_one, 0);
    cout << cmd.Excecute()<< endl;

    //lambda表达式
    cmd.Wrap([](int n){return n + 1; }, 1);
    cout << cmd.Excecute()<< endl;

    //函数对象
    cmd.Wrap(STA());
    cout << cmd.Excecute()<< endl;
    cmd.Wrap(STA(), 4);
    cout << cmd.Excecute()<< endl;

    STA t = { 10 };
    int x = 3;
    //成员函数
    cmd.Wrap(&STA::triple0, &t);
    cout << cmd.Excecute()<< endl;
    cmd.Wrap(&STA::triple, &t, x);
    cout << cmd.Excecute()<< endl;
    cmd.Wrap(&STA::triple, &t, 3);
    cout << cmd.Excecute()<< endl;

    //常量成员
    CppHelper::CCommand<const int> cmd1;
    cmd1.Wrap(&STA::triple2, &t, 3);
    cout << cmd1.Excecute()<< endl;

    CppHelper::CCommand<> cmd2;
    cmd2.Wrap(&STA::triple3, &t);
    cmd2.Excecute();

    TEST_FUNC_ENDED;
}


void CThreadPoolTest()
{
    TEST_FUNC_BEGIN;
    CppHelper::CThreadPool pool(2);

    std::thread thd1([&pool]
    {
        auto thdid = this_thread::get_id();
        for (int i = 0; i < 10; i++)
        {
            pool.addTask([](std::thread::id thdid, int value)
            {
                cout << "synchronous thread1[" << thdid << "] value[" << value << "], handle thread: " << this_thread::get_id() << endl;
            },thdid,i+100);
        }
    });
    std::thread thd2([&pool]
    {
        auto thdid = this_thread::get_id();
        for (int i = 0; i < 10; i++)
        {
            pool.addTask([&](std::thread::id thdid, int value)
            {
                cout << "synchronous thread2[" << thdid << "] value[" << value << "], handle thread: " << this_thread::get_id() << endl;
            },thdid,i+200);
        }
    });

    cout << "sleep seconds(2) ..." << endl << endl;
    this_thread::sleep_for(std::chrono::seconds(2));

    cout << endl << "input any words to stop: ";
    getchar();
    pool.stop();
    thd1.join();
    thd2.join();

    TEST_FUNC_ENDED;
}

void CVectorTest()
{
    TEST_FUNC_BEGIN;

    typedef std::set<int> StdSet;
    typedef std::list<int> StdList;
    typedef std::vector<int> StdVec;
    typedef CppHelper::CVector<int> NewVec;


    StdSet stdSet{11,12,13,14,15,16,17,18,19};
    StdList stdList{21,22,23,24,25,26,27,28,29};
    StdVec stdVec1{31,32,33,34,35,36,37,38,39};
    StdVec stdVec2{41,42,43,44,45,46,47,48,49};

    NewVec newVec1;
    NewVec newVec2(20);
    NewVec newVec3(stdSet);
    NewVec newVec4(stdList);
    NewVec newVec5(stdVec1);
    newVec1.append(555);
    newVec1.append(stdSet);
    newVec1.insert(8,18);
    bool b = newVec1.contains(555);
    auto idx = newVec1.indexof(13);
    auto val = newVec1.value(idx);
    newVec1.remove(18);
    newVec5.clear();
    newVec5 += newVec4;
    newVec5 << val;

    stdSet = newVec1.toSet();

    stdVec2 = newVec3;


    TEST_FUNC_ENDED;
}

}
