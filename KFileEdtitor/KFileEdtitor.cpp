#include "KFileEdtitor.h"
#include "qfiledialog.h"
#include <QHeaderView>
#include "qlabel.h"
#include "QTextEdit"

KFileEdtitor::KFileEdtitor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::KFileEdtitorClass())
{
    ui->setupUi(this);

    treeWidget = new TreeWidget(ui->centralWidget);
    treeWidget->setMinimumSize(250, 600);
    ui->verticalLayout->addWidget(treeWidget);

    displayWidget = new DisplayWidget(ui->centralWidget);
    displayWidget->setMinimumSize(600, 500);
	ui->horizontalLayout->addWidget(displayWidget);

    translator = nullptr;
    translator = new Translator("E:/kTranslation.json");

    fileRW = new ReadWrite();
    data = nullptr;
    itemDialog = nullptr;

    addPlot();
}

KFileEdtitor::~KFileEdtitor()
{
    delete ui;
}

void KFileEdtitor::addPlot()
{
    connect(ui->actionOpen, &QAction::triggered, this, &KFileEdtitor::getData);
    connect(treeWidget->treeItem, &QTreeWidget::doubleClicked, this, &KFileEdtitor::treeViewDoubleClick);
    connect(treeWidget->treeItem, &QTreeWidget::clicked, this, &KFileEdtitor::treeViewClick);
    connect(ui->menuDemo, &QAction::triggered, this, &KFileEdtitor::freshData);
    //connect(ui->menuDemo, &QAction::triggered, new Translator(), &Translator::TestJson);


}

void KFileEdtitor::funDemo()
{
    //translator =  new Translator("E:/kTranslation.json");
    if (translator->json != nullptr)
    {
        auto a=translator->json;
        auto b= a->value("slsfac");
        //qDebug() << b;
        auto c = b.toString();
        QString jLable = translator->json->value("slsfac").toString();
        //qDebug() << jLable;
    }
    
}

void KFileEdtitor::getData()
{
    QString filepath = QFileDialog::getOpenFileName(// 正常加载
        this, "open k file",
        ".",
        "k files (*.k);;All files (*.*)");
    if (filepath == nullptr)
        return;
    
    this->data = fileRW->readData(filepath, displayWidget->textDisplay);
    
    //displayWidget->textDisplay->append(filepath);
    displayItem();
}

void KFileEdtitor::displayItem()
{
    treeWidget->treeItem->setRootIsDecorated(false);
    for each (auto s in *(data->rootOrder))
    {
        QTreeWidgetItem* childItem1 = new QTreeWidgetItem(treeWidget->root);
        childItem1->setIcon(0, QIcon("E:/Logo/sec.png"));
        childItem1->setText(0, s.mid(1));
    }
    treeWidget->treeItem->expandAll();  // 展开所有节点
}

void KFileEdtitor::treeViewDoubleClick()
{
    showDialog();
    freshData();
}

void KFileEdtitor::showDialog()
{
    if (this->data == nullptr)
        return;
    QTreeWidgetItem* item = treeWidget->treeItem->currentItem();
    QString key = item->text(0);
    if (key == u8"激活能量数值")return;

    itemDialog = new ItemDialog(this);
    itemDialog->setWindowTitle(key);

    int numCount = 0;
    int w = 90, h = 30, px = 10, py = 40;

    QMap<QString, QString>* kv = nullptr;
    QList<QString>* attOrder = nullptr;

    kv = data->rootMap->value(key);
    attOrder = data->order->value(key);

    if (kv == nullptr || attOrder == nullptr)
        return;

    for each (auto k in *attOrder)
    {
        QLabel* label = new QLabel(itemDialog);
        //! 配置中文
        if (translator != nullptr)
        {
            QString jLable = translator->json->value(k).toString();
            if (jLable != "")
            {
                label->setText(jLable);
            }
            else
            {
                label->setText(k);
            }
        }
        else
        {
            label->setText(k);
        }

        //! 如果Lable是unused直接放在最后面
        if (k.mid(0, 6) == "unused")
        {
            label->setGeometry((w + px) * (7 % 8) + 45, (h + py) * (numCount / 8), w, h);
            numCount = numCount + (7 - (numCount % 8));
        }
        else
        {
            label->setGeometry((w + px) * (numCount % 8) + 45, (h + py) * (numCount / 8), w, h);
        }

        //! 把unused属性的值设置为不可修改的textBrowser
        if (k.mid(0, 6) == "unused")
        {
            QTextBrowser* value = new QTextBrowser(itemDialog);
            value->setText(kv->value(k));
            value->setAlignment(Qt::AlignCenter);
            numCount = numCount - (7 - (numCount % 8));
            value->setGeometry((w + px) * (7 % 8) + 45, (h + py) * (numCount / 8) + 35, w, h);
            numCount = numCount + (7 - (numCount % 8));
        }
        else
        {
            QTextEdit* value = new QTextEdit(itemDialog);
            value->setText(kv->value(k));
            value->setGeometry((w + px) * (numCount % 8) + 45, (h + py) * (numCount / 8) + 35, w, h);
        }
        numCount++;
    }

    int xx = (w + px) * 9, yy = (h + py) * (numCount / 8 + 1) + 50;
    itemDialog->resize(xx, yy);

    itemDialog->save->move(xx - 240, yy - 50);
    itemDialog->save->setVisible(true);
    itemDialog->cacel->move(xx - 140, yy - 50);
    itemDialog->cacel->setVisible(true);

    itemDialog->show();
    //freshData();
    //displayWidget->textDisplay->append("freshData()");
    
}

void KFileEdtitor::freshData()
{

    //displayWidget->textDisplay->append("freshData()");

    if (itemDialog == nullptr)
    {
        displayWidget->textDisplay->append("itemDialog = nullptr");
        return;
    }
    //! 对话框里面的数据
    auto diaData = itemDialog->dialogData;
    if (diaData == nullptr)
    {
        displayWidget->textDisplay->append("diaData = nullptr");
        return;
    }
    //! 对话框标题：Data的节点
    QString k = itemDialog->windowTitle();
    //! 得到原来的节点
    //auto oldK = data->rootMap->value(k);

    //! 修改后的节点信息
    QList<QString> kk = *(data->order->value(k));
    QList<QString>v;
    auto node = itemDialog->onlyValue;
    for (auto n : node)
    {
        v.append(n->toPlainText());
    }
    QMap<QString, QString>* newData = new QMap<QString, QString>;
    for (int i = 0; i < v.size(); i++)
    {
        newData->insert(kk[i], v[i]);
    }

    //! 更新地址
    data->rootMap->insert(k, newData);
    //! 释放原地址
    //delete oldK;


    //! 更新修改后的属性
    //treeViewClick();
}



void KFileEdtitor::treeViewClick()
{
    
    //displayWidget->textDisplay->append("treeViewClick()");
    if (this->data == nullptr)
        return;

    //! 得到当前点击的键
    QTreeWidgetItem* item = treeWidget->treeItem->currentItem();
    if (item->text(0) == u8"激活能量数值")
        return;
    freshData();

    /* 创建数据模型 */
    QStandardItemModel* model = new QStandardItemModel();
    /* 设置表格标题行(输入数据为QStringList类型) */
    model->setHorizontalHeaderLabels({ u8"属性", u8"值" });
    model->setItem(0, 0, new QStandardItem(u8"名字"));
    model->setItem(0, 1, new QStandardItem(item->text(0)));

    /* 设置表格视图数据 */
    treeWidget->itemAttr->setModel(model);
    treeWidget->itemAttr->verticalHeader()->hide();//不显示序号  
    

    //! 加载键的属性值并显示
    QMap<QString, QString >* itemValue = nullptr;
    QList<QString >* valueOrder = nullptr;

    auto a = item->text(0);
    auto b = data->rootMap;
    itemValue = data->rootMap->value(item->text(0));  // nullptr
    valueOrder = data->order->value(item->text(0));

    if (itemValue == nullptr || valueOrder == nullptr)
        return;

    int lineCount = 0;
    for each (auto k in *valueOrder)
    {
        if (k.mid(0, 6) == "unused") 
        {
            continue;
        }
        model->setItem(lineCount, 0, new QStandardItem(k));
        model->setItem(lineCount, 1, new QStandardItem(itemValue->value(k)));
        lineCount++;
    }

    /* 显示 */
    treeWidget->itemAttr->show();
}