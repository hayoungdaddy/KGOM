#include "common.h"
#include "writelog.h"

WriteLog::WriteLog()
{

}

WriteLog::~WriteLog()
{
}

void WriteLog::write(QString dir, QString str)
{
    QDateTime nowUTC = QDateTime::currentDateTimeUtc();
    QDateTime nowKST;
    nowKST = convertKST(nowUTC);
    QFile file(dir + "/KGOM." + nowKST.toString("yyyyMMdd") + ".log");
    if(file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        QTextStream stream( &file );

        stream << nowKST.toString("[hh:mm:ss] ") << str << "\n";

        file.close();
    }
}
