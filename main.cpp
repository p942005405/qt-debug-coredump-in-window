#include "mainwindow.h"
#include <QApplication>
#include "ccrashstack.h"
#include <QDir>
#include <QFile>
#include <shlobj.h>
#include <QDebug>
#include <DbgHelp.h>
#include <QDateTime>

//如果选msvc编译 删掉crashstack类
#ifdef Q_OS_WIN
long __stdcall   callback(_EXCEPTION_POINTERS*   excp)
{
    CCrashStack crashStack(excp);
    QString sCrashInfo = crashStack.GetExceptionInfo();

//    TCHAR my_documents[MAX_PATH];
//    SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, my_documents);
//    QString file_path = QString::fromWCharArray(my_documents);
    QDir dir;
    QString file_path = dir.currentPath();

    QDir *folder_path = new QDir;
    bool exist = folder_path->exists(file_path.append("\\MyApp"));
    if(!exist)
    {
        folder_path->mkdir(file_path);
    }
    delete folder_path;
    folder_path = nullptr;

    QString sFileName = file_path + "\\crash.log";

    QFile file(sFileName);
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        file.write(sCrashInfo.toUtf8());
        file.close();
    }

    return   EXCEPTION_EXECUTE_HANDLER;
}
#endif

long ApplicationCrashHandler(EXCEPTION_POINTERS *pException){
{
        // 在程序exe的上级目录中创建dmp文件夹
        QDir *dmp = new QDir;
        bool exist = dmp->exists("../dmp/");
        if(exist == false)
            dmp->mkdir("../dmp/");
        }
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString current_date = current_date_time.toString("yyyy_MM_dd_hh_mm_ss");
        QString time =  current_date + ".dmp";
          EXCEPTION_RECORD *record = pException->ExceptionRecord;
          QString errCode(QString::number(record->ExceptionCode, 16));
          QString errAddr(QString::number((uint)record->ExceptionAddress, 16));
          QString errFlag(QString::number(record->ExceptionFlags, 16));
          QString errPara(QString::number(record->NumberParameters, 16));
          qDebug()<<"errCode: "<<errCode;
          qDebug()<<"errAddr: "<<errAddr;
          qDebug()<<"errFlag: "<<errFlag;
          qDebug()<<"errPara: "<<errPara;
          HANDLE hDumpFile = CreateFile((LPCWSTR)QString("../dmp/" + time).utf16(),
                   GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
          if(hDumpFile != INVALID_HANDLE_VALUE) {
              MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
              dumpInfo.ExceptionPointers = pException;
              dumpInfo.ThreadId = GetCurrentThreadId();
              dumpInfo.ClientPointers = TRUE;
              MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
              CloseHandle(hDumpFile);
          }
          else{
              qDebug()<<"hDumpFile == null";
          }
          return EXCEPTION_EXECUTE_HANDLER;
}

#define aaa 1
#ifdef aaa
LONG ApplicationCrashHandler1(EXCEPTION_POINTERS *pException){//程式异常捕获
    /*
      ***转储堆栈信息***
    */
    //创建 Dump 文件
    QDir dir;
    QString file_path = dir.currentPath();
    QString dumpFilename = file_path + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".dmp";
    HANDLE hDumpFile = CreateFile((LPCWSTR)(dumpFilename.toStdWString().c_str()), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if( hDumpFile != INVALID_HANDLE_VALUE){
        //Dump信息
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;
        //写入Dump文件内容
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, (MINIDUMP_TYPE)(MiniDumpWithDataSegs | MiniDumpWithProcessThreadData | MiniDumpWithUnloadedModules), &dumpInfo, NULL, NULL);
    }

    /*
      ***保存数据代码***
    */
    //这里弹出一个错误对话框并退出程序
    EXCEPTION_RECORD* record = pException->ExceptionRecord;
    QString errCode(QString::number(record->ExceptionCode,16));
    QString errAdr(QString::number((uint)record->ExceptionAddress,16));

    printf("Exe Crash And Restart[errcode = %s][erraddr = %s]", errCode.toStdString().c_str(), errAdr.toStdString().c_str());

    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    SetUnhandledExceptionFilter(callback);
#endif
    //注冊异常捕获函数
//    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);


//    TCHAR my_documents[MAX_PATH];
//    SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, my_documents);
//    QString file_path = QString::fromWCharArray(my_documents);
    QString path;
    QDir dir;
    path = dir.currentPath();
    qDebug()<< path <<endl;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    QString *p = NULL;
//    p->clear();

    return a.exec();
}
