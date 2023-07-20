// main.cpp
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QInputDialog>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QShortcut>
#include <QMessageBox>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QCoreApplication>



//一个QGraphicsLineItem的子类，它的末端可以带有箭头
class ArrowLineItem : public QGraphicsLineItem {
public:
    ArrowLineItem(qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem* parent = nullptr)
        : QGraphicsLineItem(x1, y1, x2, y2, parent) {}

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override {
        // 绘制线段
        QGraphicsLineItem::paint(painter, option, widget);

        // 计算箭头的方向
        QLineF line = this->line();
        qreal angle = std::atan2(-line.dy(), line.dx());

        // 计算箭头的两个端点
        QPointF arrowP1 = line.p2() - QPointF(sin(angle + M_PI / 3) * arrowSize, cos(angle + M_PI / 3) * arrowSize);
        QPointF arrowP2 = line.p2() - QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize, cos(angle + M_PI - M_PI / 3) * arrowSize);

        // 绘制箭头
        painter->setBrush(Qt::black);
        painter->drawPolygon(QPolygonF() << line.p2() << arrowP1 << arrowP2);
    }

private:
    static constexpr qreal arrowSize = 10;  // 箭头的大小
};

//一个QGraphicsItem的子类，用于表示事件节点，用于后续绘图
class EventNode : public QGraphicsItem {
public:
    EventNode(const QString& eventName)
        : m_eventName(eventName) {}

    QRectF boundingRect() const override {
        // 这个函数应返回包含这个图形项的矩形。这里我们返回一个固定大小的矩形。
        return QRectF(0, 0, 100, 100);
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override {
        // 这个函数用于绘制这个图形项。这里我们简单地绘制一个矩形和事件名称。
        painter->drawRect(0, 0, 100, 100);
        painter->drawText(10, 50, m_eventName);
    }

    //用于获取m_eventName的公有函数
    QString eventName() const {
        return m_eventName;
    }

private:
    QString m_eventName;
};

class EventGenerator : public QWidget {
    Q_OBJECT

public:
    EventGenerator(QWidget* parent = nullptr)
        : QWidget(parent)
        , namespaceLineEdit(new QLineEdit(this))
        , eventTypeComboBox(new QComboBox(this))
        , themeLineEdit(new QLineEdit(this))
        , eventEditor(new QTextEdit(this))
        , addButton(new QPushButton("Add Option", this))
        , addEventButton(new QPushButton("Add Event", this))
        , saveButton(new QPushButton("Save Event", this))
        , eventSelector(new QComboBox(this))
        , eventViewer(new QTextEdit(this))
        , leftLayout(new QVBoxLayout())
        , currentOptionCounter(0)
        , eventCounter(0)
    {
        //我愿意将以下行代码称为原初构造，别问为什么
        QString currentDirectory = getCurrentDirectory();
        QInputDialog* namespaceInputDialog = new QInputDialog(this);
        namespaceInputDialog->setWindowTitle("Enter Namespace");
        namespaceInputDialog->setLabelText("Namespace:");
        namespaceInputDialog->setOkButtonText("OK");
        namespaceInputDialog->setCancelButtonText("Cancel");
        namespaceInputDialog->setInputMode(QInputDialog::TextInput);
        QPushButton* loadButton = new QPushButton(this);
        QSize iconSize(28, 28);  // You can adjust the size according to your icon
        loadButton->setIconSize(iconSize);
        loadButton->setFixedSize(iconSize);
        QIcon openFileIcon(currentDirectory+"/icons/openFile.png");
        loadButton->setIcon(openFileIcon);
        Indentation = "    ";

        //以下是事件链的构造函数
        // 创建一个场景
        eventChainScene = new QGraphicsScene();
        // 创建一个视图来显示场景
        eventChainview = new QGraphicsView(eventChainScene);
        QVBoxLayout* eventChainLayout = new QVBoxLayout();
        eventChainLayout->addWidget(eventChainview);
        //以上是事件链的构造函数


        QHBoxLayout* mainLayout = new QHBoxLayout(this);  // Change QVBoxLayout to QHBoxLayout
        QVBoxLayout* leftLayout = new QVBoxLayout();  // Layout for the left side
        QVBoxLayout* rightLayout = new QVBoxLayout();  // Layout for the right side (sidebar)
        updateButton = new QPushButton("Update Event", this);
        titleLocalisationLineEdit = new QLineEdit(this);
        descLocalisationLineEdit = new QTextEdit(this);
        addTriggerButton = new QPushButton("Add Trigger", this);
        refreshButton = new QPushButton(this);
        refreshButton->setIconSize(iconSize);
        refreshButton->setFixedSize(iconSize);
        eventSelectorLayout = new QHBoxLayout(this);
        editOptionLocalisationButton = new QPushButton("Edit Option Localisation", this);
        QIcon refreshIcon(currentDirectory+"/icons/refreshButton.png");  // Replace with the path to your icon file
        refreshButton->setIcon(refreshIcon);
        namespaceInputDialog->layout()->addWidget(loadButton);
        connect(loadButton, &QPushButton::clicked, this, &EventGenerator::loadEvents);

        eventCounter = 0;

        leftLayout->addWidget(new QLabel("Title Localisation:", this));
        leftLayout->addWidget(titleLocalisationLineEdit);
        leftLayout->addWidget(new QLabel("Description Localisation:", this));
        leftLayout->addWidget(descLocalisationLineEdit);

        leftLayout->addWidget(new QLabel("Event Type:", this));
        eventTypeComboBox->addItems({ "character_event", "letter_event", "court_event", "activity_event" });
        leftLayout->addWidget(eventTypeComboBox);
        leftLayout->addItem(new QSpacerItem(0, 10));

        leftLayout->addWidget(new QLabel("Theme:", this));
        leftLayout->addWidget(themeLineEdit);
        leftLayout->addItem(new QSpacerItem(0, 10));

        leftLayout->addWidget(new QLabel("Event Structure:", this));
        leftLayout->addWidget(eventEditor);
        leftLayout->addItem(new QSpacerItem(0, 10));

        leftLayout->addWidget(addButton);
        leftLayout->addWidget(editOptionLocalisationButton);
        leftLayout->addWidget(addTriggerButton);
        leftLayout->addWidget(addEventButton);
        leftLayout->addWidget(saveButton);
        leftLayout->addItem(new QSpacerItem(0, 20));

        //下拉框的刷新按钮
        eventSelectorLayout->addWidget(refreshButton);

        rightLayout->addWidget(new QLabel("Select Event:", this));
        eventSelectorLayout->addWidget(eventSelector);
        rightLayout->addLayout(eventSelectorLayout);
        rightLayout->addWidget(new QLabel("Event Details:", this));
        rightLayout->addWidget(eventViewer);
        rightLayout->addWidget(updateButton);

        mainLayout->addLayout(leftLayout);
        mainLayout->addLayout(rightLayout);
        mainLayout->addLayout(eventChainLayout);

        connect(addButton, &QPushButton::clicked, this, &EventGenerator::addOption);
        connect(addEventButton, &QPushButton::clicked, this, &EventGenerator::addEvent);
        connect(saveButton, &QPushButton::clicked, this, &EventGenerator::saveEvent);
        connect(eventTypeComboBox, &QComboBox::currentTextChanged, this, &EventGenerator::updateEventStructure);
        connect(eventSelector, &QComboBox::currentTextChanged, this, &EventGenerator::loadEvent);
        connect(updateButton, &QPushButton::clicked, this, &EventGenerator::updateEvent);
        connect(titleLocalisationLineEdit, &QLineEdit::textEdited, this, &EventGenerator::updateLocalisation);
        connect(descLocalisationLineEdit, &QTextEdit::textChanged, this, &EventGenerator::updateLocalisation);
        connect(addTriggerButton, &QPushButton::clicked, this, &EventGenerator::addTrigger);
        connect(eventSelector, SIGNAL(activated(int)), this, SLOT(updateEventSelector(int)));
        connect(refreshButton, &QPushButton::clicked, this, &EventGenerator::refreshEventSelector);
        connect(namespaceInputDialog, &QInputDialog::finished, this, &EventGenerator::hideNamespaceLineEdit);
        connect(editOptionLocalisationButton, &QPushButton::clicked, this, &EventGenerator::editOptionLocalisation);



        //以下为测试用
        // 创建一个新的快捷键
        QShortcut* shortcut = new QShortcut(QKeySequence("Ctrl+Shift+L"), this);
        connect(shortcut, &QShortcut::activated, this, &EventGenerator::saveEventGraph);
        //以上为测试用


        setLayout(mainLayout);

        int result = namespaceInputDialog->exec();  // 显示输入对话框，等待用户点击 "确定 "或 "取消"。
        if (result == QDialog::Accepted) {
            // Only set the namespace if it hasn't been set yet
            if (namespaceLineEdit->text().isEmpty()) {
                QString namespaceName = namespaceInputDialog->textValue();
                namespaceLineEdit->setText(namespaceName);
            }
        }


        QString defaultText = "Please enter localization content";
        titleLocalisationLineEdit->setText(defaultText);
        // 在更改文本之前阻断信号
        descLocalisationLineEdit->blockSignals(true);
        descLocalisationLineEdit->setText(defaultText);
        // 更改完后重新打开信号
        descLocalisationLineEdit->blockSignals(false);

        addEvent();
        parseEventText(eventEditor->toPlainText());
        createEventNodes();
        drawNodes();
    }

public slots:

    QString getCurrentDirectory() {
        return QCoreApplication::applicationDirPath();
    }

    QString findShortestIndentation(const QString& text) {
        QString shortestIndentation;
        QStringList lines = text.split('\n');
        for (const QString& line : lines) {
            QRegularExpression re("^(\\s*)");
            QRegularExpressionMatch match = re.match(line);
            if (match.hasMatch()) {
                QString indentation = match.captured(1);
                if (!indentation.isEmpty() &&
                    (shortestIndentation.isEmpty() || indentation.length() < shortestIndentation.length())) {
                    shortestIndentation = indentation;
                }
            }
        }
        return shortestIndentation;
    }

    void drawNodes() {
        // 清除旧的图形项
        eventChainScene->clear();

        // 创建两个映射，将事件名称映射到对应的图形项和文本项
        QMap<QString, QGraphicsEllipseItem*> itemMap;
        QMap<QString, QGraphicsTextItem*> textItemMap;

        // 计算最大的事件名称长度
        int maxEventNameLength = 0;
        for (EventNode* node : m_eventNodes) {
            maxEventNameLength = qMax(maxEventNameLength, node->eventName().length());
        }

        // 计算节点间的距离
        int distance = maxEventNameLength * 7;  // 一个字符的大小按照6个像素的大小计算
        distance += 25;  // 额外提供一定距离作为空档

        // 计算网格的大小
        int gridSize = qCeil(qSqrt(m_eventNodes.size()));

        // 遍历所有的节点
        for (int i = 0; i < m_eventNodes.size(); ++i) {
            EventNode* node = m_eventNodes[i];

            // 计算节点的坐标
            int x = (i % gridSize) * distance;
            int y = (i / gridSize) * distance;

            // 创建一个新的椭圆形项来表示节点
            QGraphicsEllipseItem* item = new QGraphicsEllipseItem(x, y, 10, 10);
            // 将项添加到映射中
            itemMap.insert(node->eventName(), item);

            // 将项添加到场景中
            eventChainScene->addItem(item);

            // 创建一个新的文本项来显示节点的名称
            QGraphicsTextItem* textItem = new QGraphicsTextItem(node->eventName());
            // 设置文本项的位置
            textItem->setPos(x, y - textItem->boundingRect().height());
            // 将文本项添加到映射和场景中
            textItemMap.insert(node->eventName(), textItem);
            eventChainScene->addItem(textItem);
        }

        // 遍历事件图，为每个触发关系添加一条线
        for (auto it = eventGraph.begin(); it != eventGraph.end(); ++it) {
            QString eventId = it.key();
            QList<QString> triggeredEvents = it.value();
            for (const QString& triggeredEventId : triggeredEvents) {
                QGraphicsEllipseItem* startItem = itemMap.value(eventId);
                QGraphicsEllipseItem* endItem = itemMap.value(triggeredEventId);
                if (startItem && endItem) {
                    if (startItem == endItem) {
                        // 如果起点和终点相同，将事件节点名称的颜色改为红色
                        QGraphicsTextItem* textItem = textItemMap.value(eventId);
                        if (textItem) {
                            textItem->setDefaultTextColor(Qt::red);
                        }
                    }
                    else {
                        // 创建一条线，将 startItem 和 endItem 连接起来
                        ArrowLineItem* line = new ArrowLineItem(
                            startItem->pos().x() + startItem->boundingRect().center().x(),  // 线的起点 x 坐标
                            startItem->pos().y() + startItem->boundingRect().center().y(),  // 线的起点 y 坐标
                            endItem->pos().x() + endItem->boundingRect().center().x(),  // 线的终点 x 坐标
                            endItem->pos().y() + endItem->boundingRect().center().y()  // 线的终点 y 坐标
                        );

                        // 创建一个 QPen 对象并设置颜色和宽度
                        QPen pen;
                        pen.setColor(Qt::blue);  // 设置颜色为红色
                        pen.setWidth(2);  // 设置宽度为2像素

                        // 将这个 QPen 对象应用到线上
                        line->setPen(pen);

                        // 将线添加到场景中
                        eventChainScene->addItem(line);
                    }

                }
            }
        }
        qDebug() << "Event graph:";
        for (auto it = eventGraph.begin(); it != eventGraph.end(); ++it) {
            qDebug() << "  " << it.key() << " triggers " << it.value();
        }
        qDebug() << "Item map:";
        for (auto it = itemMap.begin(); it != itemMap.end(); ++it) {
            qDebug() << "  " << it.key() << " at (" << it.value()->pos().x() << "," << it.value()->pos().y() << ")";
        }
        // 更新视图
        eventChainview->update();
    }

    void createEventNodes() {
        // 清除旧的事件节点
        qDeleteAll(m_eventNodes.begin(), m_eventNodes.end());
        m_eventNodes.clear();

        // 创建一个字符串列表，包含所有的事件名称
        QStringList allEventNames = eventGraph.keys();
        for (const QList<QString>& triggeredEvents : eventGraph.values()) {
            allEventNames += triggeredEvents;
        }

        // 去除重复的事件名称
        allEventNames.removeDuplicates();

        // 为每个事件创建一个 EventNode
        for (const QString& eventName : allEventNames) {
            m_eventNodes.append(new EventNode(eventName));
        }
    }

    void saveEventGraph() {   //测试用函数，正式版需要去除
        // 打开一个文件对话框，让用户选择保存的位置
        QString filePath = QFileDialog::getSaveFileName(this, "Save Event Graph", "", "Text Files (*.txt)");
        if (filePath.isEmpty()) {
            return;
        }

        // 解析事件文本
        parseEventText(eventEditor->toPlainText());

        // 打开文件
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "Error", "Failed to open file for writing");
            return;
        }

        // 创建一个文本流，用于写入文件
        QTextStream out(&file);

        // 遍历事件图，将它的内容写入到文件中
        for (auto it = eventGraph.begin(); it != eventGraph.end(); ++it) {
            QString eventId = it.key();
            QList<QString> triggeredEvents = it.value();
            out << "Event ID: " << eventId << "\n";
            for (const QString& triggeredEventId : triggeredEvents) {
                out << "  Triggers: " << triggeredEventId << "\n";
                
            }
            qDebug() << "Event ID:" << eventId << "triggers:" << triggeredEvents;  // 打印每个事件及其触发的事件
        }

        // 关闭文件
        file.close();
    }


    void parseEventText(const QString& eventText) {
        // 清除旧的数据
        eventGraph.clear();
        eventInfo.clear();

        // 创建正则表达式对象
        QRegularExpression eventRegex("(\\w+\\.\\d+)\\s*=\\s*\\{");
        QRegularExpression triggerRegex1("trigger_event\\s*=\\s*(\\w+\\.\\d+)");
        QRegularExpression triggerRegex2("trigger_event\\s*=\\s*\\{[^}]*id\\s*=\\s*(\\w+\\.\\d+)[^}]*\\}");

        // 查找所有的事件
        QRegularExpressionMatchIterator eventIt = eventRegex.globalMatch(eventText);
        while (eventIt.hasNext()) {
            QRegularExpressionMatch eventMatch = eventIt.next();
            int startEvent = eventMatch.capturedStart();

            // 查找下一个事件的位置
            int endEvent;
            if (eventIt.hasNext()) {
                QRegularExpressionMatch nextEventMatch = eventIt.peekNext();
                endEvent = nextEventMatch.capturedStart();
            }
            else {
                endEvent = eventText.size();
            }

            QString eventBlock = eventText.mid(startEvent, endEvent - startEvent);
            QString eventId = eventMatch.captured(1);

            // 添加事件到图
            if (!eventGraph.contains(eventId)) {
                eventGraph.insert(eventId, QList<QString>());
            }

            // 查找当前事件内容中的触发关系
            QRegularExpressionMatchIterator triggerIt1 = triggerRegex1.globalMatch(eventBlock);
            while (triggerIt1.hasNext()) {
                QRegularExpressionMatch triggerMatch = triggerIt1.next();
                QString triggeredEventId = triggerMatch.captured(1);
                // 添加触发关系到图
                eventGraph[eventId].append(triggeredEventId);
            }

            QRegularExpressionMatchIterator triggerIt2 = triggerRegex2.globalMatch(eventBlock);
            while (triggerIt2.hasNext()) {
                QRegularExpressionMatch triggerMatch = triggerIt2.next();
                QString triggeredEventId = triggerMatch.captured(1);
                // 添加触发关系到图
                eventGraph[eventId].append(triggeredEventId);
            }
        }
    }





    void loadLocalisationFile(const QString& fileName) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        QTextStream in(&file);
        bool isFirstLine = true;
        while (!in.atEnd()) {
            QString line = in.readLine();
            // Skip the first line
            if (isFirstLine) {
                isFirstLine = false;
                continue;
            }
            int colonIndex = line.indexOf(':');
            if (colonIndex != -1) {
                QString key = line.left(colonIndex).trimmed();
                QString value = line.mid(colonIndex + 1).trimmed();
                // remove the double quotes around the value
                if (value.startsWith('\"') && value.endsWith('\"')) {
                    value = value.mid(1, value.length() - 2);
                }
                localisationMap.insert(key, value);
            }
        }
        file.close();
    }



    void loadEvents() {
        // Open the event file
        QString eventFileName = QFileDialog::getOpenFileName(this, "Load Event", "", "Text Files (*.txt)");
        if (!eventFileName.isEmpty()) {
            QFile eventFile(eventFileName);
            if (eventFile.open(QIODevice::ReadOnly)) {
                QTextStream stream(&eventFile);
                QString namespaceLine;
                stream.readLineInto(&namespaceLine);
                // Extract the namespace from the line "namespace = <namespace>"
                QString namespaceName = namespaceLine.mid(namespaceLine.indexOf('=') + 1).trimmed();
                namespaceLineEdit->setText(namespaceName);
                // Load the rest of the event file into the event editor
                eventEditor->setText(stream.readAll());
                eventFile.close();
                //设置一个flag，标志着我们使用读取文件的方式进入了程序
                eventLoadedFromFile = true;
                // 刷新事件选择器
                refreshEventSelector();
            }
        }

        // Open the localisation file
        QString localisationFileName = QFileDialog::getOpenFileName(this, "Load Localisation (Optional)", "", "YAML Files (*.yml)");
        if (!localisationFileName.isEmpty()) {
            if (!localisationFileName.isEmpty()) {
                loadLocalisationFile(localisationFileName);
                // 如果读取了本地化文件，显示第一个事件的本地化标题和描述
                if (!localisationMap.isEmpty()) {
                    QString namespaceName = namespaceLineEdit->text();
                    QString titleKey = namespaceName + ".0001.t";
                    QString descKey = namespaceName + ".0001.desc";
                    if (localisationMap.contains(titleKey) && localisationMap.contains(descKey)) {
                        titleLocalisationLineEdit->setText(localisationMap.value(titleKey));
                        // 在更改文本之前阻断信号
                        descLocalisationLineEdit->blockSignals(true);
                        descLocalisationLineEdit->setText(localisationMap.value(descKey));
                        descLocalisationLineEdit->blockSignals(false);
                    }
                }
            }
        }

        // Close the namespace input dialog and enter the main interface
        QInputDialog* namespaceInputDialog = qobject_cast<QInputDialog*>(sender()->parent());
        if (namespaceInputDialog) {
            namespaceInputDialog->accept();
        }
    }

    void editOptionLocalisation() {
        QDialog* dialog = new QDialog(this);
        QVBoxLayout* layout = new QVBoxLayout(dialog);
        QComboBox* optionSelectorComboBox = new QComboBox(dialog);
        QLineEdit* optionLocalisationLineEdit = new QLineEdit(dialog);
        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog);

        layout->addWidget(optionSelectorComboBox);
        layout->addWidget(optionLocalisationLineEdit);
        layout->addWidget(buttonBox);

        // Get the current selected event
        QString eventName = eventSelector->currentText();

        // 在事件编辑器中查找事件
        QString eventText = eventEditor->toPlainText();
        int startIndex = eventText.indexOf(QString("\n%1 = {").arg(eventName));
        if (startIndex != -1) {
            int braceCount = 1;  // 计算开括号的数量
            int endIndex = startIndex + QString("\n%1 = {").arg(eventName).length();

            // 找到事件的终点
            while (endIndex < eventText.length()) {
                QChar ch = eventText[endIndex];
                if (ch == '{') {
                    braceCount++;
                }
                else if (ch == '}') {
                    braceCount--;
                    if (braceCount == 0) {  // 发现事件结束
                        break;
                    }
                }
                endIndex++;
            }

            QString eventDetails = eventText.mid(startIndex, endIndex - startIndex);

            // 在事件详情中查找所有的选项，并添加到 optionSelectorComboBox 中
            QRegularExpression optionRegex("option = \\{\\n\\s*name = ([\\w\\.]+)");
            QRegularExpressionMatchIterator i = optionRegex.globalMatch(eventDetails);
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                optionSelectorComboBox->addItem(match.captured(1));
            }
        }

        // 更新选项本地化文本
        connect(optionSelectorComboBox, &QComboBox::currentTextChanged, [=](const QString& optionName) {
            QString optionLocalisationKey = QString("%2").arg(optionName);
            QString optionLocalisation = localisationMap.value(optionLocalisationKey, "Please enter localisation content");
            optionLocalisationLineEdit->setText(optionLocalisation);
            });

        // 更新本地化映射
        connect(buttonBox, &QDialogButtonBox::accepted, [=]() {
            QString optionName = optionSelectorComboBox->currentText();
            QString optionLocalisationKey = QString("%1").arg(optionName);
            QString optionLocalisation = optionLocalisationLineEdit->text();
            localisationMap.insert(optionLocalisationKey, optionLocalisation);
            dialog->accept();
            });

        // 取消对话框
        connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

        dialog->setLayout(layout);
        dialog->show();

        // 触发 currentTextChanged 信号以更新选项本地化文本
        QString currentOption = optionSelectorComboBox->currentText();
        if (!currentOption.isEmpty()) {
            emit optionSelectorComboBox->currentTextChanged(currentOption);
        }
    }

    void hideNamespaceLineEdit() {
        namespaceLineEdit->hide();
    }

    void refreshEventSelector() {
        // Clear the event selector
        eventSelector->clear();

        // Get the text from the event editor
        QString eventText = eventEditor->toPlainText();

        // Use a regular expression to find all events
        QRegularExpression re(QString("\\b%1\\.\\d{4} = \\{").arg(namespaceLineEdit->text()));
        QRegularExpressionMatchIterator i = re.globalMatch(eventText);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString eventName = match.captured().left(match.captured().length() - 4);  // Remove " = {" from the end

            // Check if the event is already in the event selector
            if (eventSelector->findText(eventName) == -1) {
                eventSelector->addItem(eventName);
            }
        }
        parseEventText(eventEditor->toPlainText());
        createEventNodes();
        drawNodes();
    }

    void updateEventSelector(int index) {
        // Get the selected event name
        QString eventName = eventSelector->itemText(index);

        // Load the event
        loadEvent(eventName);
    }

    QString generateTriggerText(QString delayType, int delayValue, int delayMinValue ,int delayMaxValue ,QString delayUnit, QString triggeredEventId) {

        QString text = "trigger_event = {\n";

        if (delayType == "Fixed") {
            text += Indentation+ Indentation+ Indentation + delayUnit + " = " + QString::number(delayValue) + "\n";
        }
        else if (delayType == "Range") {
            text += Indentation + Indentation + Indentation + delayUnit +" = { " + QString::number(delayMinValue) + " " + QString::number(delayMaxValue) + " }\n";
        }

        text += Indentation+ Indentation+ Indentation+"id = " + triggeredEventId + "\n";
        text += Indentation+ Indentation+"}";

        return text;
    }

    bool verifyEventId(const QString& eventId) {
        if (eventId.length() == 0) {
            return false;
        }

        // 其他验证逻辑

        return true;
    }

    //一个一键隐藏的功能，专门用于一整个布局内的所有组件
    void setWidgetsVisibility(QLayout* layout, bool visible) {
        for (int i = 0; i < layout->count(); ++i) {
            QWidget* widget = layout->itemAt(i)->widget();
            if (widget) {
                widget->setVisible(visible);
            }
        }
    }

    //用于更新布局的显示状态的一个函数  用于动态更新延迟显示这一部分
    void updateDelayType(const QString& delayType) {
        bool isFixedDelay = (delayType == "Fixed");
        bool isRangeDelay = (delayType == "Range");
 
        setWidgetsVisibility(delayMinLayout, isRangeDelay);
        setWidgetsVisibility(delayMaxLayout, isRangeDelay);
        setWidgetsVisibility(delayFixedLayout, isFixedDelay);
        setWidgetsVisibility(delayUnitLayout, isFixedDelay || isRangeDelay);
    }

    //用于更新布局的显示状态的一个函数  用于动态更新选项这一部分
    void updateOptionSelectorVisibility(const QString& triggerType) {
        bool isVisible = (triggerType == "option");
        setWidgetsVisibility(optionLayout, isVisible);
    }

    void addTrigger() {
        addTriggerDialog = new QDialog(this);
        QHBoxLayout* dialogLayout = new QHBoxLayout(addTriggerDialog);
        QVBoxLayout* triggerTypeLayout = new QVBoxLayout(addTriggerDialog);
        QVBoxLayout* DelayTypeLayout = new QVBoxLayout(addTriggerDialog);
        eventIdLineEdit = new QLineEdit(addTriggerDialog);
        QVBoxLayout* eventIdLineEditLayout = new QVBoxLayout(addTriggerDialog);


        triggerTypeComboBox = new QComboBox(addTriggerDialog);
        triggerTypeComboBox->addItems({ "immediate", "after", "option" });
        triggerTypeLayout->addWidget(new QLabel("Trigger Type:", addTriggerDialog));
        triggerTypeLayout->addWidget(triggerTypeComboBox);
        dialogLayout->addLayout(triggerTypeLayout);

        //这一小段代码将实现事件的选项的动态
        optionLayout = new QVBoxLayout(addTriggerDialog);
        optionSelectorComboBox = new QComboBox(addTriggerDialog);
        dialogLayout->addLayout(optionLayout);
        optionLayout->addWidget(new QLabel("Option:", addTriggerDialog));
        optionLayout->addWidget(optionSelectorComboBox);

        QString eventName = eventSelector->currentText();

        //这一小段代码实现了事件下拉框的布局
        delayTypeComboBox = new QComboBox(addTriggerDialog);
        delayTypeComboBox->addItems({ "None", "Fixed", "Range" });
        DelayTypeLayout->addWidget(new QLabel("Delay Type:", addTriggerDialog));
        DelayTypeLayout->addWidget(delayTypeComboBox);
        dialogLayout->addLayout(DelayTypeLayout);

        //接下来一段实现了动态的延迟范围的布局
        delayMinLayout = new QVBoxLayout(addTriggerDialog);
        delayMinLineEdit = new QLineEdit(addTriggerDialog);
        delayMinLayout->addWidget(new QLabel("Delay Min:", addTriggerDialog));
        delayMinLayout->addWidget(delayMinLineEdit);
        dialogLayout->addLayout(delayMinLayout);

        delayMaxLayout = new QVBoxLayout(addTriggerDialog);
        delayMaxLineEdit = new QLineEdit(addTriggerDialog);
        delayMaxLayout->addWidget(new QLabel("Delay Max:", addTriggerDialog));
        delayMaxLayout->addWidget(delayMaxLineEdit);
        dialogLayout->addLayout(delayMaxLayout);

        //接下来一段实现了固定延迟的下拉框的动态布局
        delayFixedLayout = new QVBoxLayout(addTriggerDialog);
        fixedDelayLineEdit = new QLineEdit(addTriggerDialog);
        delayFixedLayout->addWidget(new QLabel("Fixed Delay:", addTriggerDialog));
        delayFixedLayout->addWidget(fixedDelayLineEdit);
        dialogLayout->addLayout(delayFixedLayout);

        //接下来一段实现了选择时间单位的下拉框的动态布局
        delayUnitLayout = new QVBoxLayout(addTriggerDialog);
        delayUnitComboBox = new QComboBox(addTriggerDialog);
        delayUnitComboBox->addItems({ "days", "months", "years" });
        delayUnitLayout->addWidget(new QLabel("Delay Unit:", addTriggerDialog));
        delayUnitLayout->addWidget(delayUnitComboBox);
        dialogLayout->addLayout(delayUnitLayout);

        // 在弹框布局中  添加了一个输入框 用于输入被触发的事件id
        eventIdLineEditLayout->addWidget(new QLabel("Event ID:", addTriggerDialog));
        eventIdLineEditLayout->addWidget(eventIdLineEdit);
        dialogLayout->addLayout(eventIdLineEditLayout);

        // 在事件编辑器中查找事件
        QString eventText = eventEditor->toPlainText();
        int startIndex = eventText.indexOf(QString("\n%1 = {").arg(eventName));
        if (startIndex != -1) {
            int braceCount = 1;  // 计算开括号的数量
            int endIndex = startIndex + QString("\n%1 = {").arg(eventName).length();

            // 找到事件的终点
            while (endIndex < eventText.length()) {
                QChar ch = eventText[endIndex];
                if (ch == '{') {
                    braceCount++;
                }
                else if (ch == '}') {
                    braceCount--;
                    if (braceCount == 0) {  // 发现事件结束
                        break;
                    }
                }
                endIndex++;
            }

            QString eventDetails = eventText.mid(startIndex, endIndex - startIndex);

            // 在事件详情中查找所有的选项，并添加到 optionSelectorComboBox 中
            QRegularExpression optionRegex("option = \\{\\n\\s*name = ([\\w\\.]+)");
            QRegularExpressionMatchIterator i = optionRegex.globalMatch(eventDetails);
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                optionSelectorComboBox->addItem(match.captured(1));
            }
        }

        // 其他组件的初始化...

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, addTriggerDialog);
        buttonBox->setOrientation(Qt::Vertical);
        connect(buttonBox, &QDialogButtonBox::accepted, this, &EventGenerator::confirmAddTrigger);
        connect(buttonBox, &QDialogButtonBox::rejected, addTriggerDialog, &QDialog::reject);
        connect(triggerTypeComboBox, &QComboBox::currentTextChanged, this, &EventGenerator::updateOptionSelectorVisibility);
        connect(delayTypeComboBox, &QComboBox::currentTextChanged, this, &EventGenerator::updateDelayType);
        dialogLayout->addWidget(buttonBox);

        addTriggerDialog->setLayout(dialogLayout);
        addTriggerDialog->show();
        updateOptionSelectorVisibility(triggerTypeComboBox->currentText());
        updateDelayType(delayTypeComboBox->currentText());
    }

    void confirmAddTrigger() {
        QString triggerType = triggerTypeComboBox->currentText();

        QString delayType = delayTypeComboBox->currentText();

        QString delayUnit = delayUnitComboBox->currentText();

        int delayValue = fixedDelayLineEdit->text().toInt();

        int delayMinValue = delayMinLineEdit->text().toInt();

        int delayMaxValue = delayMaxLineEdit->text().toInt();

        QString namespaceName = namespaceLineEdit->text();

        // 依次获取其他输入值
        QString Id = eventIdLineEdit->text();
        QString eventId = namespaceName + "." + Id;
        QString triggerText = generateTriggerText(delayType,delayValue,delayMinValue,delayMaxValue,delayUnit, eventId);
        // 添加验证
        if (verifyEventId(eventId)) {
            // 保存事件ID到触发器数据结构
            // 获取触发器文本
            QString triggerText = "\n"+ Indentation+ Indentation + generateTriggerText(delayType, delayValue, delayMinValue, delayMaxValue, delayUnit, eventId) + "\n"+ Indentation;

            // 获取当前选中的事件
            QString eventName = eventSelector->currentText();

            // 获取事件的详细信息
            QString eventDetails = eventViewer->toPlainText();

            // 确定插入的位置
            int insertPos;
            if (triggerType == "immediate") {
                QString immediateBlockStart = Indentation+"immediate = {";
                int immediateBlockStartPos = eventDetails.indexOf(immediateBlockStart);

                if (immediateBlockStartPos == -1) {
                    // 如果没有 immediate 块，添加一个
                    insertPos = eventDetails.indexOf("\n", eventDetails.indexOf("theme = ")) + 1;
                    eventDetails.insert(insertPos, immediateBlockStart + "\n" + triggerText + "}\n");
                }
                else {
                    // 如果有 immediate 块，插入到这个块中
                    insertPos = eventDetails.indexOf("\n", immediateBlockStartPos + immediateBlockStart.length() - 1) + 1;
                    eventDetails.insert(insertPos, triggerText);
                }
            }
            else if (triggerType == "option") {
                QString optionName = optionSelectorComboBox->currentText();
                insertPos = eventDetails.indexOf("\n", eventDetails.indexOf(QString("name = %1").arg(optionName))) + 1;
                eventDetails.insert(insertPos, triggerText);
            }
            else if (triggerType == "after") {
                int lastBracePos = eventDetails.lastIndexOf("}");
                int afterBlockPos = eventDetails.indexOf("\n"+ Indentation +"after = {");
                QString afterStart = Indentation+"after = {";
                if (lastBracePos != -1) {
                    if (afterBlockPos == -1) {  // If the event does not already contain an "after" block
                        eventDetails.insert(lastBracePos, "\n"+ Indentation +"after = {\n" + triggerText + "}\n");
                    }
                    else {  // If the event already contains an "after" block
                        int afterBlockEndPos = eventDetails.indexOf("\n", afterBlockPos + afterStart.length() - 1) + 1;
                        if (afterBlockEndPos != -1) {
                            eventDetails.insert(afterBlockEndPos, triggerText);
                        }
                    }
                }
            }
            else {
                // 其他触发器类型...
            }

            // 更新事件详情
            eventViewer->setText(eventDetails);

        }
        else {
            // 错误提示
        }


        addTriggerDialog->accept();
        //添加后同步更新到主编辑框中
        updateEvent();

        parseEventText(eventEditor->toPlainText());
        createEventNodes();
        drawNodes();
    }

    void updateLocalisation() {
        QString selectedEvent = eventSelector->currentText();
        if (selectedEvent.isEmpty()) {
            // No event selected, so do not update the localisation
            return;
        }
        QString eventId = selectedEvent.mid(selectedEvent.lastIndexOf('.') + 1);
        if (eventId.length() != 4) {
            // Invalid event ID, so do not update the localisation
            return;
        }
        QString namespaceName = namespaceLineEdit->text();

        QString titleLocalisation = titleLocalisationLineEdit->text();
        QString descLocalisation = descLocalisationLineEdit->toPlainText();

        localisationMap[QString("%1.%2.t").arg(namespaceName).arg(eventId)] = titleLocalisation;
        localisationMap[QString("%1.%2.desc").arg(namespaceName).arg(eventId)] = descLocalisation;
    }

    void addOption() {
        currentOptionCounter++;
        QString eventName = eventSelector->currentText();  // 获取当前选中的事件
        QString eventId = eventName.mid(eventName.lastIndexOf('.') + 1);

        if (!optionCounters.contains(eventName)) {
            // 在第一次为这个事件添加选项时初始化选项计数器
            optionCounters[eventName] = 1;
        }
        else {
            optionCounters[eventName]++;
        }
        int optionId = optionCounters[eventName];

        // 在事件编辑器中查找事件
        QString eventText = eventEditor->toPlainText();
        int startIndex = eventText.indexOf(QString("\n%1.%2 = {").arg(namespaceLineEdit->text()).arg(eventId));
        if (startIndex == -1) {
            // 未找到事件，不添加选项
            return;
        }
        int braceCount = 1;  // 计算开括号的数量
        int endIndex = startIndex + QString("\n%1.%2 = {").arg(namespaceLineEdit->text()).arg(eventId).length();

        // 找到事件的终点
        while (endIndex < eventText.length()) {
            QChar ch = eventText[endIndex];
            if (ch == '{') {
                braceCount++;
            }
            else if (ch == '}') {
                braceCount--;
                if (braceCount == 0) {  // 发现事件结束
                    break;
                }
            }
            endIndex++;
        }

        if (endIndex >= eventText.length()) {
            // 未找到事件结束，不添加选项
            return;
        }

        QString option = QString("\n"+ Indentation +"option = {\n"+ Indentation + Indentation+"name = %1.%2.%3\n"+ Indentation + Indentation+"# option details...\n"+ Indentation +"}\n")
            .arg(namespaceLineEdit->text())
            .arg(eventId)
            .arg(optionId);
        eventText.insert(endIndex, option);
        eventEditor->setText(eventText);

        QLineEdit* optionLocalisationLineEdit = new QLineEdit(this);
        optionLocalisationLineEdits.append(optionLocalisationLineEdit);
        leftLayout->addWidget(optionLocalisationLineEdit);

        // 添加本地化文本
        QString optionLocalisation = QInputDialog::getText(this, "Enter Option Localisation", "Option Localisation:");
        optionLocalisationLineEdit->setText(optionLocalisation);
        localisationMap.insert(QString("%1.%2.%3").arg(namespaceLineEdit->text()).arg(eventId).arg(optionId), optionLocalisation);
        // 更新eventViewer中的文本内容
        loadEvent(eventName);
    }

    // 增加新事件
    void addEvent() {
        eventCounter++;  // 递增事件计数器
        QString eventId = QString("%1").arg(eventCounter, 4, 10, QChar('0'));
        QString namespaceName = namespaceLineEdit->text();
        QString eventType = eventTypeComboBox->currentText();
        QString theme = themeLineEdit->text();


        QString eventStructure = QString("\n%1.%2 = {\n"+ Indentation +"type = %3\n"+ Indentation +"title = %1.%2.t\n"+ Indentation +"desc = %1.%2.desc\n"+ Indentation +"theme = %4\n"+ Indentation +"# other event details...\n}\n")
            .arg(namespaceName)
            .arg(eventId)
            .arg(eventType)
            .arg(theme);

        // 如果已经从文件中加载了事件，就不添加新的事件
        if (eventLoadedFromFile) {
            Indentation = findShortestIndentation(eventEditor->toPlainText());
            eventLoadedFromFile = false;
            return;
        }

        eventEditor->append(eventStructure);
        currentOptionCounter = 0;  // 重置选项计数器

        // 直接添加新事件到 eventSelector 中，无需先清空
        eventSelector->addItem(QString("%1.%2").arg(namespaceLineEdit->text()).arg(eventId));

        // 将eventSelector的当前索引设置为新事件。
        int newIndex = eventSelector->count() - 1;  // 新事件的索引
        eventSelector->setCurrentIndex(newIndex);

        // 保存当前本地化文本
        QString defaultText = "Please enter localization content";

        // 将新建事件的默认本地化文本添加到 localisationMap 中。
        localisationMap[QString("%1.%2.t").arg(namespaceName).arg(eventId)] = defaultText;
        localisationMap[QString("%1.%2.desc").arg(namespaceName).arg(eventId)] = defaultText;

        // 最后加载事件
        loadEvent(QString("%1.%2").arg(namespaceName).arg(eventId));

        parseEventText(eventEditor->toPlainText());
        createEventNodes();
        drawNodes();
    }




    void saveEvent() {
        QString fileName = QFileDialog::getSaveFileName(this, "Save Event", "", "Text Files (*.txt)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly)) {
                QTextStream stream(&file);
                stream.setGenerateByteOrderMark(true);  // 设置生成字节序标记（BOM）
                stream << "namespace = " << namespaceLineEdit->text() << "\n";
                stream << eventEditor->toPlainText();
                file.close();
            }
        }
        QString namespaceName = namespaceLineEdit->text();
        QString eventId = QString("%1").arg(eventCounter, 4, 10, QChar('0'));
        localisationMap.insert(QString("%1.%2.t").arg(namespaceName).arg(eventId), titleLocalisationLineEdit->text());
        localisationMap.insert(QString("%1.%2.desc").arg(namespaceName).arg(eventId), descLocalisationLineEdit->toPlainText());
        // 保存事件的本地化信息到文件
        QString localisationFilePath = QFileDialog::getSaveFileName(this, "Save Localisation", "", "YAML Files (*.yml)");
        QFile localisationFile(localisationFilePath);
        if (localisationFile.open(QIODevice::WriteOnly)) {
            QTextStream out(&localisationFile);
            out.setGenerateByteOrderMark(true);  // 设置生成字节序标记（BOM）
            out << "l_simp_chinese:\n";
            for (auto it = localisationMap.begin(); it != localisationMap.end(); ++it) {
                out << " " << it.key() << ": \"" << it.value() << "\"\n";
            }
        }
    }

        void updateEventStructure(const QString & eventType) {
            // 清除事件编辑器并重新添加所有事件  用于将副编辑框的内容重新同步到主编辑框中
            eventEditor->clear();
            for (int i = 1; i <= eventCounter; ++i) {
                QString eventId = QString("%1").arg(i, 4, 10, QChar('0'));
                QString namespaceName = namespaceLineEdit->text();
                QString theme = themeLineEdit->text();

                QString eventStructure = QString("\n%1.%2 = {\n"+ Indentation +"type = %3\n"+ Indentation +"title = %1.%2.t\n"+ Indentation +"desc = %1.%2.desc\n"+ Indentation +"theme = %4\n"+ Indentation +"# other event details...\n}\n")
                    .arg(namespaceName)
                    .arg(eventId)
                    .arg(eventType)
                    .arg(theme);

                eventEditor->append(eventStructure);
            }
            currentOptionCounter = 0;  // 重置选项计数器   
        }

        void updateEvent() {
            QString eventName = eventSelector->currentText();
            QString eventId = eventName.mid(eventName.lastIndexOf('.') + 1);
            int id = eventId.toInt();

            QString eventText = eventEditor->toPlainText();
            int startIndex = eventText.indexOf(QString("\n%1.%2 = {").arg(namespaceLineEdit->text()).arg(eventId));
            if (startIndex != -1) {
                int braceCount = 1;  // 计算开括号的数量
                int endIndex = startIndex + QString("\n%1.%2 = {").arg(namespaceLineEdit->text()).arg(eventId).length();

                // 找到事件的终点
                while (endIndex < eventText.length()) {
                    QChar ch = eventText[endIndex];
                    if (ch == '{') {
                        braceCount++;
                    }
                    else if (ch == '}') {
                        braceCount--;
                        if (braceCount == 0) {  // 发现事件结束
                            endIndex++;
                            break;
                        }
                    }
                    endIndex++;
                }

                if (endIndex < eventText.length()) {
                    QString newEvent = eventViewer->toPlainText();
                    eventText.replace(startIndex, endIndex - startIndex, newEvent);
                    eventEditor->setText(eventText);
                }
            }
        }

        void loadEvent(const QString& eventName) {

            if (optionCounters.contains(eventName)) {
                currentOptionCounter = optionCounters[eventName];
            }
            else {
                currentOptionCounter = 0;
            }

            // 从事件名称中提取事件ID
            QString eventId = eventName.mid(eventName.lastIndexOf('.') + 1);
            int id = eventId.toInt();

            // 在事件编辑器中查找事件
            QString eventText = eventEditor->toPlainText();
            int startIndex = eventText.indexOf(QString("\n%1.%2 = {").arg(namespaceLineEdit->text()).arg(eventId));
            if (startIndex != -1) {
                int braceCount = 0;  // 计算开括号的数量
                int endIndex = startIndex;

                // 找到事件的终点
                while (endIndex < eventText.length()) {
                    QChar ch = eventText[endIndex];
                    if (ch == '{') {
                        braceCount++;
                    }
                    else if (ch == '}') {
                        braceCount--;
                        if (braceCount == 0) {  // 发现事件结束
                            endIndex++;
                            break;
                        }
                    }
                    endIndex++;
                }

                QString eventDetails = eventText.mid(startIndex, endIndex - startIndex);
                eventViewer->setText(eventDetails);

                // 提取事件ID
                QString eventId = eventName.mid(eventName.lastIndexOf('.') + 1);
                // 获取命名空间
                QString namespaceName = namespaceLineEdit->text();
                // 更新本地化文本框
                titleLocalisationLineEdit->setText(localisationMap.value(QString("%1.%2.t").arg(namespaceName).arg(eventId)));
                descLocalisationLineEdit->blockSignals(true);
                descLocalisationLineEdit->setText(localisationMap.value(QString("%1.%2.desc").arg(namespaceName).arg(eventId)));
                descLocalisationLineEdit->blockSignals(false);
                
            }
        }
        




private:
    //这里原本有一个函数，但是它并没有被用到，所以删除了，保留此部分

private:
    QLineEdit* namespaceLineEdit;
    QComboBox* eventTypeComboBox;
    QLineEdit* themeLineEdit;
    QTextEdit* eventEditor;
    QPushButton* addButton;
    QPushButton* addEventButton;
    QPushButton* saveButton;
    QComboBox* eventSelector;
    QTextEdit* eventViewer;
    QPushButton* updateButton;
    QLineEdit* titleLocalisationLineEdit;
    QTextEdit* descLocalisationLineEdit;
    QList<QLineEdit*> optionLocalisationLineEdits;
    QMap<QString, QString> localisationMap;  // 保存事件与本地化信息的映射关系
    QVBoxLayout* leftLayout;
    QMap<QString, int> optionCounters;  // 保存每个事件的选项计数器
    QPushButton* addTriggerButton;
    QDialog* addTriggerDialog;
    QComboBox* triggerTypeComboBox;
    QComboBox* optionSelectorComboBox;
    QComboBox* delayTypeComboBox;
    QLineEdit* delayMinLineEdit;
    QLineEdit* delayMaxLineEdit;
    QComboBox* delayUnitComboBox;
    QLineEdit* eventIdLineEdit;
    QLineEdit* fixedDelayLineEdit;
    QVBoxLayout* optionLayout;
    QVBoxLayout* delayMinLayout;
    QVBoxLayout* delayMaxLayout;
    QVBoxLayout* delayFixedLayout;
    QVBoxLayout* delayUnitLayout;
    QPushButton* refreshButton;
    QHBoxLayout* eventSelectorLayout;
    QPushButton* editOptionLocalisationButton;
    bool eventLoadedFromFile = false;  // 没有从文件中加载事件
    QMap<QString, QList<QString>> eventGraph;
    QMap<QString, QString> eventInfo; 
    QList<EventNode*> m_eventNodes;
    QGraphicsScene* eventChainScene;
    QGraphicsView* eventChainview;
    QString Indentation;

    int currentOptionCounter;
    int eventCounter;
};

    int main(int argc, char* argv[]) {
        QApplication app(argc, argv);

        EventGenerator window;
        window.show();

        return app.exec();
    }

#include "main.moc"










