# Qt程序crash信息捕捉和跟踪(mingw)

1. 项目配置

在.pro文件中添加：
QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_CFLAGS_RELEASE = $$QMAKE_CLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

2. 修改代码支持异常搜集

```
#ifdef Q_OS_WIN
long __stdcall   callback(_EXCEPTION_POINTERS*   excp)
{
    CCrashStack crashStack(excp);
    QString sCrashInfo = crashStack.GetExceptionInfo();

    TCHAR my_documents[MAX_PATH];
    SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, my_documents);
    QString file_path = QString::fromWCharArray(my_documents);

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

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
    SetUnhandledExceptionFilter(callback);
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QString *p = NULL;
    p->clear();

    return a.exec();
}
```

3. 生成汇编信息

使用cmd命令 "objdump -S app.exe > app.asm" 生成汇编信息文件

4. 定位异常

查看crash.log里的 "Exception Addr: 004021D0" 在app.asm中查找相应地址

5. 缺点

程序文件中包含了调试信息和源代码信息，用户可以查看到。