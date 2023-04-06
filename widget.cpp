#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
//    ui->Result->setParent(this);
}

Widget::~Widget()
{
    delete ui;
}



int judge(QString grammer,set<QChar> NT,set<QString> PRO,set<PII> PRO_DICT)
{
    if (grammer[1] != '[' || grammer[3] != ']' || grammer.size() > 4)   return 1;

    if (NT.size() == 0 || PRO.size() == 0)  return 2;

    for (auto it = PRO_DICT.begin(); it != PRO_DICT.end(); it ++)
    {
        QString left = (*it).first;
        bool flag = false;  //检测产生式左侧是否有非终结符
        for (int i = 0; i < left.size(); i ++)
            if (NT.count(left[i]) > 0)  flag = true;
        if (flag == false)  return 3;
    }

    return 0;
}

void Widget::on_GeneButton_clicked()
{

    // 对所有全局变量进行初始化
    BEGIN.clear();
    NT.clear();
    T.clear();
    PRO.clear();
    PRO_DICT.clear();


    //定义输出句柄
    QString CLASSTYPE_OUT =  "错误了喵";
    QString GRAMMER_OUT =  "错误了喵";
    QString PRODUCTION_OUT =  "错误了喵";

    // 获取 Grammer 数据
    QString grammer = ui->Grammer->toPlainText().remove(' ');
    // 获取 VNot 数据
    QString vnot = ui->VNot->toPlainText().remove(' ');
    // 获取 Production 数据
    QString production = ui->Production->toPlainText().remove(' ');

    // 根据输入获取开始符、非终结符集合、终结符集合和产生式的集合

    // 获取开始符
    BEGIN = grammer[2];
    // 获取非终结符集合
    for (int i = 0; i < vnot.size(); i ++)
    {
        if (vnot[i] != ',' && vnot[i] != '\n')
            NT.insert(vnot[i]);
    }
    // 获取产生式集合和终结符集合
    for (int i = 0; i < production.size(); )
    {
        int j = i;
        while (j < production.size() && production[j] != '\n')
        {
            if (production[j] != '=' && production[j] != ':' && production[j] != '|' && NT.count(production[j]) == 0)
                T.insert(production[j]);
            j ++;
        }

        if (production[j] == "\n")
            PRO.insert(production.mid(i,j-i));
        else
            PRO.insert(production.mid(i,j-i+1));

        i = j+1;
    }

    // 获取产生式左右值之间的对应关系
    for (auto it = PRO.begin(); it != PRO.end(); it ++)
    {
        QString left,right;
        int i = 0;
        while (i < (*it).size() && (*it)[i] != ':' && (*it)[i] != '=') left += (*it)[i ++];
        while (i < (*it).size() && ((*it)[i] == ':' || (*it)[i] == '=')) i ++;
        while (i < (*it).size())    right += (*it)[i ++];
        PRO_DICT.insert({left,right});
    }


    // 检验输入是否为合法的文法

    int JUDGE_RESULT = judge(grammer,NT,PRO,PRO_DICT);
    if (JUDGE_RESULT)
    {
        CLASSTYPE_OUT = "请输入正确的文法表达式，形如：\n"
                             "---------------------------------------------\n"
                             "请输入文法: G[N]\n"
                             "请输入非终结符VN: N,D\n"
                             "请依次输入产生式规则: N::=ND|D\n"
                             "                     D::=0|1|2|3|4|5|6|7|8|9\n"
                             "---------------------------------------------\n"
                             "(注：空串使用美元符号$表示)\n"
                             "产生式左侧至少要有一个非终结符\n";
        switch(JUDGE_RESULT)
        {
            case 1: PRODUCTION_OUT = "文法输入错误（正确输入形如G[N]，N表示开始符号）\n";    break;
            case 2: PRODUCTION_OUT = "产生式为空，或者未输入终结符\n"; break;
            case 3: PRODUCTION_OUT = "产生式左侧缺少非终结符号（错误输入形如a::=b|c，其中a、b、c均为终结符号）";  break;
        }


        ui->PRODUCTION->setText(PRODUCTION_OUT);
        ui->GRAMMER->setText(GRAMMER_OUT);
        ui->CLASSTYPE->setText(CLASSTYPE_OUT);
        return;
    }

    bool isChomsky3 = true, isChomsky2 = true, isChomsky1 = true, isChomsky0 = true;

    // 判断是否为3型文法
    int isleft = -1;
    //判断是否为左线性文法
    // -1表示未经过判断
    // 0 表示已出现右线性
    // 1 表示已出现左线性
    for (auto it = PRO_DICT.begin(); it != PRO_DICT.end(); it ++)
    {
        if (isChomsky3 == false)    break;
        QString left,right;
        left = (*it).first; right = (*it).second;
        if (left.size() > 1)
        {
            isChomsky3 = false;     break;
        }

        // 逐个判断左右线性关系
        int i = 0;
        while (i < right.size())
        {
            int j = i;
            while (j < right.size() && right[j] != '|') j ++;
            j --;
            if (j - i + 1 > 2)
            {
                isChomsky3 = false;     break;
            }

            if (j == i)
            {
                if (right[j] == '$' || NT.count(right[j]) > 0)
                {
                    isChomsky3 = false;     break;
                }
            }
            else
            {
                if (isleft == -1)
                {
                    if (NT.count(right[i]) > 0 && NT.count(right[j]) > 0)
                    {
                        isChomsky3 = false;     break;
                    }
                    else if (NT.count(right[i]) > 0)    isleft = 1;
                    else if (NT.count(right[j]) > 0)    isleft = 0;
                    else
                    {
                        isChomsky3 = false;     break;
                    }
                }
                else if (isleft == 0)
                {
                    if (NT.count(right[j]) <= 0)
                    {
                        isChomsky3 = false;     break;
                    }
                }
                else
                {
                    if (NT.count(right[i]) <= 0)
                    {
                        isChomsky3 = false;     break;
                    }
                }
            }
            i = j + 2;
        }

    }


    // 判断是否为2型文法
    for (auto it = PRO_DICT.begin(); it != PRO_DICT.end(); it ++)
    {
        if (isChomsky2 == false)    break;
        QString left = (*it).first;
        for (int i = 0; i < left.size(); i ++)
            if (T.count(left[i]) > 0)
            {
                isChomsky2 = false; break;
            }
    }

    // 判断是否为1型文法
    for (auto it = PRO_DICT.begin(); it != PRO_DICT.end(); it ++)
    {
        if ((*it).first.size() > (*it).second.size())
        {
            isChomsky1 = false; break;
        }
    }

    // 判断是否为0型文法
    for (auto it = PRO_DICT.begin(); it != PRO_DICT.end(); it ++)
    {
        if (isChomsky0 == false)    break;
        QString left = (*it).first;
        bool alive = false;
        for (int i = 0; i < left.size(); i ++)
            if (NT.count(left[i]) > 0)
            {
                alive = true;   break;
            }
        isChomsky0 = alive;
    }



    if (isChomsky3) CLASSTYPE_OUT = "该文法是Chomsky3型文法（即正规文法）。";
    else if (isChomsky2) CLASSTYPE_OUT = "该文法是Chomsky2型文法（即上下文无关文法）。";
    else if (isChomsky1) CLASSTYPE_OUT = "该文法是Chomsky1型文法（即上下文有关文法）。";
    else CLASSTYPE_OUT = "该文法是Chomsky0型文法。";


    GRAMMER_OUT = grammer + "= ({";
    for (auto it  = NT.begin(); it != NT.end(); it ++)
    {
        GRAMMER_OUT.push_back(*it);
        GRAMMER_OUT.push_back(',');
    }
    GRAMMER_OUT.remove(GRAMMER_OUT.size()-1,1);
    GRAMMER_OUT.push_back("},{");

    for (auto it  = T.begin(); it != T.end(); it ++)
    {
        GRAMMER_OUT.push_back(*it);
        GRAMMER_OUT.push_back(',');
    }
    if (GRAMMER_OUT[GRAMMER_OUT.size() - 1] != '{')
        GRAMMER_OUT.remove(GRAMMER_OUT.size()-1,1);
    GRAMMER_OUT.push_back("},");
    GRAMMER_OUT.push_back("P,");
    GRAMMER_OUT.push_back(BEGIN);
    GRAMMER_OUT.push_back(")");



    PRODUCTION_OUT = production;

    ui->PRODUCTION->setText(PRODUCTION_OUT);
    ui->GRAMMER->setText(GRAMMER_OUT);
    ui->CLASSTYPE->setText(CLASSTYPE_OUT);


    // 测试代码02
    //***********************************************************
    // 合并BEGIN、NT、T、PRO中的内容，并设置到Result的文本中
//    resultText = "开始符号: " + BEGIN + "\n" + "非终结符: ";

//    for (auto it = NT.begin(); it != NT.end(); it ++)
//    {
//        resultText.push_back(*it);  resultText.push_back(",");
//    }
//    resultText.remove(resultText.size()-1,1);
//    resultText.push_back("\n终结符号: ");

//    for (auto it = T.begin(); it != T.end(); it ++)
//    {
//        resultText.push_back(*it);  resultText.push_back(",");
//    }
//    resultText.remove(resultText.size()-1,1);
//    resultText.push_back("\n产生式: \n");

//    for (auto it = PRO.begin(); it != PRO.end(); it ++)
//    {
//        resultText.push_back(*it);  resultText.push_back("\n");
//    }
//    resultText.remove(resultText.size()-1,1);
//    resultText.push_back("\n对应关系: \n");

//    for (auto it = PRO_DICT.begin(); it != PRO_DICT.end(); it ++)
//    {
//        resultText.push_back((*it).first);
//        resultText.push_back(" ---> ");
//        resultText.push_back((*it).second);
//        resultText.push_back("\n");
//    }

//    if (isChomsky0 == true) resultText.push_back("是0型文法\n");
//    else    resultText.push_back("不是0型文法\n");

//    resultText.remove(resultText.size()-1,1);
    //***********************************************************


//    //  测试代码01
//    //***********************************************************
//    // 合并文本框内容，并设置到 Result 的文本中
//    QString resultText = "Grammer: " + grammer + "\n"
//                         "VNot: " + vnot + "\n"
//                         "Production: " + production;
//    //***********************************************************

//    // 输出结果
//    ui->Result->setText(resultText);
}
