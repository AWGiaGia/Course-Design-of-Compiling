#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QProcess>
#include <set>
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

typedef pair<QString,QString> PII;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_GeneButton_clicked();

private:
    Ui::Widget *ui;
    QString BEGIN;      //开始符
    set<QChar> NT;    //非终结符集合
    set<QChar> T;     //终结符集合
    set<QString> PRO;   //产生式的集合
    set<PII> PRO_DICT; //产生式左右值的字典

};
#endif // WIDGET_H
