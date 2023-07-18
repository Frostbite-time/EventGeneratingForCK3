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
        //��Ը�⽫�����д����Ϊԭ�����죬����Ϊʲô
        QInputDialog* namespaceInputDialog = new QInputDialog(this);
        namespaceInputDialog->setWindowTitle("Enter Namespace");
        namespaceInputDialog->setLabelText("Namespace:");
        namespaceInputDialog->setOkButtonText("OK");
        namespaceInputDialog->setCancelButtonText("Cancel");
        namespaceInputDialog->setInputMode(QInputDialog::TextInput);
        QPushButton* loadButton = new QPushButton(this);
        QSize iconSize(24, 24);  // You can adjust the size according to your icon
        loadButton->setIconSize(iconSize);
        loadButton->setFixedSize(iconSize);


        QHBoxLayout* mainLayout = new QHBoxLayout(this);  // Change QVBoxLayout to QHBoxLayout
        QVBoxLayout* leftLayout = new QVBoxLayout();  // Layout for the left side
        QVBoxLayout* rightLayout = new QVBoxLayout();  // Layout for the right side (sidebar)
        updateButton = new QPushButton("Update Event", this);
        titleLocalisationLineEdit = new QLineEdit(this);
        descLocalisationLineEdit = new QLineEdit(this);
        addTriggerButton = new QPushButton("Add Trigger", this);
        refreshButton = new QPushButton(this);
        refreshButton->setIconSize(iconSize);
        refreshButton->setFixedSize(iconSize);
        eventSelectorLayout = new QHBoxLayout(this);
        editOptionLocalisationButton = new QPushButton("Edit Option Localisation", this);
        //���������б�ע�͵��Ĵ�����������ˢ�°�ť���ͼ��� ͼ���СӦΪ24x24���ȴ������ɺ���ʹ��
        // QIcon refreshIcon("path/to/your/icon.png");  // Replace with the path to your icon file
        //refreshButton->setIcon(refreshIcon);
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

        //�������ˢ�°�ť
        eventSelectorLayout->addWidget(refreshButton);

        rightLayout->addWidget(new QLabel("Select Event:", this));
        eventSelectorLayout->addWidget(eventSelector);
        rightLayout->addLayout(eventSelectorLayout);
        rightLayout->addWidget(new QLabel("Event Details:", this));
        rightLayout->addWidget(eventViewer);
        rightLayout->addWidget(updateButton);

        mainLayout->addLayout(leftLayout);
        mainLayout->addLayout(rightLayout);

        connect(addButton, &QPushButton::clicked, this, &EventGenerator::addOption);
        connect(addEventButton, &QPushButton::clicked, this, &EventGenerator::addEvent);
        connect(saveButton, &QPushButton::clicked, this, &EventGenerator::saveEvent);
        connect(eventTypeComboBox, &QComboBox::currentTextChanged, this, &EventGenerator::updateEventStructure);
        connect(eventSelector, &QComboBox::currentTextChanged, this, &EventGenerator::loadEvent);
        connect(updateButton, &QPushButton::clicked, this, &EventGenerator::updateEvent);
        connect(titleLocalisationLineEdit, &QLineEdit::textEdited, this, &EventGenerator::updateLocalisation);
        connect(descLocalisationLineEdit, &QLineEdit::textEdited, this, &EventGenerator::updateLocalisation);
        connect(addTriggerButton, &QPushButton::clicked, this, &EventGenerator::addTrigger);
        connect(eventSelector, SIGNAL(activated(int)), this, SLOT(updateEventSelector(int)));
        connect(refreshButton, &QPushButton::clicked, this, &EventGenerator::refreshEventSelector);
        connect(namespaceInputDialog, &QInputDialog::finished, this, &EventGenerator::hideNamespaceLineEdit);
        connect(editOptionLocalisationButton, &QPushButton::clicked, this, &EventGenerator::editOptionLocalisation);



        setLayout(mainLayout);

        int result = namespaceInputDialog->exec();  // ��ʾ����Ի��򣬵ȴ��û���� "ȷ�� "�� "ȡ��"��
        if (result == QDialog::Accepted) {
            // Only set the namespace if it hasn't been set yet
            if (namespaceLineEdit->text().isEmpty()) {
                QString namespaceName = namespaceInputDialog->textValue();
                namespaceLineEdit->setText(namespaceName);
            }
        }


        QString defaultText = "Please enter localization content";
        titleLocalisationLineEdit->setText(defaultText);
        descLocalisationLineEdit->setText(defaultText);

        addEvent();
    }

public slots:

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
                //����һ��flag����־������ʹ�ö�ȡ�ļ��ķ�ʽ�����˳���
                eventLoadedFromFile = true;
                // ˢ���¼�ѡ����
                refreshEventSelector();
            }
        }

        // Open the localisation file
        QString localisationFileName = QFileDialog::getOpenFileName(this, "Load Localisation (Optional)", "", "YAML Files (*.yml)");
        if (!localisationFileName.isEmpty()) {
            if (!localisationFileName.isEmpty()) {
                loadLocalisationFile(localisationFileName);
                // �����ȡ�˱��ػ��ļ�����ʾ��һ���¼��ı��ػ����������
                if (!localisationMap.isEmpty()) {
                    QString namespaceName = namespaceLineEdit->text();
                    QString titleKey = namespaceName + ".0001.t";
                    QString descKey = namespaceName + ".0001.desc";
                    if (localisationMap.contains(titleKey) && localisationMap.contains(descKey)) {
                        titleLocalisationLineEdit->setText(localisationMap.value(titleKey));
                        descLocalisationLineEdit->setText(localisationMap.value(descKey));
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

        // ���¼��༭���в����¼�
        QString eventText = eventEditor->toPlainText();
        int startIndex = eventText.indexOf(QString("\n%1 = {").arg(eventName));
        if (startIndex != -1) {
            int braceCount = 1;  // ���㿪���ŵ�����
            int endIndex = startIndex + QString("\n%1 = {").arg(eventName).length();

            // �ҵ��¼����յ�
            while (endIndex < eventText.length()) {
                QChar ch = eventText[endIndex];
                if (ch == '{') {
                    braceCount++;
                }
                else if (ch == '}') {
                    braceCount--;
                    if (braceCount == 0) {  // �����¼�����
                        break;
                    }
                }
                endIndex++;
            }

            QString eventDetails = eventText.mid(startIndex, endIndex - startIndex);

            // ���¼������в������е�ѡ�����ӵ� optionSelectorComboBox ��
            QRegularExpression optionRegex("option = \\{\\n\\s*name = ([\\w\\.]+)");
            QRegularExpressionMatchIterator i = optionRegex.globalMatch(eventDetails);
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                optionSelectorComboBox->addItem(match.captured(1));
            }
        }

        // ����ѡ��ػ��ı�
        connect(optionSelectorComboBox, &QComboBox::currentTextChanged, [=](const QString& optionName) {
            QString optionLocalisationKey = QString("%2").arg(optionName);
            QString optionLocalisation = localisationMap.value(optionLocalisationKey, "Please enter localisation content");
            optionLocalisationLineEdit->setText(optionLocalisation);
            });

        // ���±��ػ�ӳ��
        connect(buttonBox, &QDialogButtonBox::accepted, [=]() {
            QString optionName = optionSelectorComboBox->currentText();
            QString optionLocalisationKey = QString("%1").arg(optionName);
            QString optionLocalisation = optionLocalisationLineEdit->text();
            localisationMap.insert(optionLocalisationKey, optionLocalisation);
            dialog->accept();
            });

        // ȡ���Ի���
        connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

        dialog->setLayout(layout);
        dialog->show();

        // ���� currentTextChanged �ź��Ը���ѡ��ػ��ı�
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
            text += "            " + delayUnit + " = " + QString::number(delayValue) + "\n";
        }
        else if (delayType == "Range") {
            text += "            "+ delayUnit +" = { " + QString::number(delayMinValue) + " " + QString::number(delayMaxValue) + " }\n";
        }

        text += "            id = " + triggeredEventId + "\n";
        text += "        }";

        return text;
    }

    bool verifyEventId(const QString& eventId) {
        if (eventId.length() == 0) {
            return false;
        }

        // ������֤�߼�

        return true;
    }

    //һ��һ�����صĹ��ܣ�ר������һ���������ڵ��������
    void setWidgetsVisibility(QLayout* layout, bool visible) {
        for (int i = 0; i < layout->count(); ++i) {
            QWidget* widget = layout->itemAt(i)->widget();
            if (widget) {
                widget->setVisible(visible);
            }
        }
    }

    //���ڸ��²��ֵ���ʾ״̬��һ������  ���ڶ�̬�����ӳ���ʾ��һ����
    void updateDelayType(const QString& delayType) {
        bool isFixedDelay = (delayType == "Fixed");
        bool isRangeDelay = (delayType == "Range");
 
        setWidgetsVisibility(delayMinLayout, isRangeDelay);
        setWidgetsVisibility(delayMaxLayout, isRangeDelay);
        setWidgetsVisibility(delayFixedLayout, isFixedDelay);
        setWidgetsVisibility(delayUnitLayout, isFixedDelay || isRangeDelay);
    }

    //���ڸ��²��ֵ���ʾ״̬��һ������  ���ڶ�̬����ѡ����һ����
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

        //��һС�δ��뽫ʵ���¼���ѡ��Ķ�̬
        optionLayout = new QVBoxLayout(addTriggerDialog);
        optionSelectorComboBox = new QComboBox(addTriggerDialog);
        dialogLayout->addLayout(optionLayout);
        optionLayout->addWidget(new QLabel("Option:", addTriggerDialog));
        optionLayout->addWidget(optionSelectorComboBox);

        QString eventName = eventSelector->currentText();

        //��һС�δ���ʵ�����¼�������Ĳ���
        delayTypeComboBox = new QComboBox(addTriggerDialog);
        delayTypeComboBox->addItems({ "None", "Fixed", "Range" });
        DelayTypeLayout->addWidget(new QLabel("Delay Type:", addTriggerDialog));
        DelayTypeLayout->addWidget(delayTypeComboBox);
        dialogLayout->addLayout(DelayTypeLayout);

        //������һ��ʵ���˶�̬���ӳٷ�Χ�Ĳ���
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

        //������һ��ʵ���˹̶��ӳٵ�������Ķ�̬����
        delayFixedLayout = new QVBoxLayout(addTriggerDialog);
        fixedDelayLineEdit = new QLineEdit(addTriggerDialog);
        delayFixedLayout->addWidget(new QLabel("Fixed Delay:", addTriggerDialog));
        delayFixedLayout->addWidget(fixedDelayLineEdit);
        dialogLayout->addLayout(delayFixedLayout);

        //������һ��ʵ����ѡ��ʱ�䵥λ��������Ķ�̬����
        delayUnitLayout = new QVBoxLayout(addTriggerDialog);
        delayUnitComboBox = new QComboBox(addTriggerDialog);
        delayUnitComboBox->addItems({ "days", "months", "years" });
        delayUnitLayout->addWidget(new QLabel("Delay Unit:", addTriggerDialog));
        delayUnitLayout->addWidget(delayUnitComboBox);
        dialogLayout->addLayout(delayUnitLayout);

        // �ڵ��򲼾���  �����һ������� �������뱻�������¼�id
        eventIdLineEditLayout->addWidget(new QLabel("Event ID:", addTriggerDialog));
        eventIdLineEditLayout->addWidget(eventIdLineEdit);
        dialogLayout->addLayout(eventIdLineEditLayout);

        // ���¼��༭���в����¼�
        QString eventText = eventEditor->toPlainText();
        int startIndex = eventText.indexOf(QString("\n%1 = {").arg(eventName));
        if (startIndex != -1) {
            int braceCount = 1;  // ���㿪���ŵ�����
            int endIndex = startIndex + QString("\n%1 = {").arg(eventName).length();

            // �ҵ��¼����յ�
            while (endIndex < eventText.length()) {
                QChar ch = eventText[endIndex];
                if (ch == '{') {
                    braceCount++;
                }
                else if (ch == '}') {
                    braceCount--;
                    if (braceCount == 0) {  // �����¼�����
                        break;
                    }
                }
                endIndex++;
            }

            QString eventDetails = eventText.mid(startIndex, endIndex - startIndex);

            // ���¼������в������е�ѡ�����ӵ� optionSelectorComboBox ��
            QRegularExpression optionRegex("option = \\{\\n\\s*name = ([\\w\\.]+)");
            QRegularExpressionMatchIterator i = optionRegex.globalMatch(eventDetails);
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                optionSelectorComboBox->addItem(match.captured(1));
            }
        }

        // ��������ĳ�ʼ��...

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

        // ���λ�ȡ��������ֵ
        QString Id = eventIdLineEdit->text();
        QString eventId = namespaceName + "." + Id;
        QString triggerText = generateTriggerText(delayType,delayValue,delayMinValue,delayMaxValue,delayUnit, eventId);
        // �����֤
        if (verifyEventId(eventId)) {
            // �����¼�ID�����������ݽṹ
            // ��ȡ�������ı�
            QString triggerText = "\n        " + generateTriggerText(delayType, delayValue, delayMinValue, delayMaxValue, delayUnit, eventId) + "\n    ";

            // ��ȡ��ǰѡ�е��¼�
            QString eventName = eventSelector->currentText();

            // ��ȡ�¼�����ϸ��Ϣ
            QString eventDetails = eventViewer->toPlainText();

            // ȷ�������λ��
            int insertPos;
            if (triggerType == "immediate") {
                QString immediateBlockStart = "    immediate = {";
                int immediateBlockStartPos = eventDetails.indexOf(immediateBlockStart);

                if (immediateBlockStartPos == -1) {
                    // ���û�� immediate �飬���һ��
                    insertPos = eventDetails.indexOf("\n", eventDetails.indexOf("theme = ")) + 1;
                    eventDetails.insert(insertPos, immediateBlockStart + triggerText + "}\n");
                }
                else {
                    // ����� immediate �飬���뵽�������
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
                int afterBlockPos = eventDetails.indexOf("\n    after = {");
                QString afterStart = "    after = {";
                if (lastBracePos != -1) {
                    if (afterBlockPos == -1) {  // If the event does not already contain an "after" block
                        eventDetails.insert(lastBracePos, "\n    after = {" + triggerText + "}\n");
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
                // ��������������...
            }

            // �����¼�����
            eventViewer->setText(eventDetails);

        }
        else {
            // ������ʾ
        }


        addTriggerDialog->accept();
        //��Ӻ�ͬ�����µ����༭����
        updateEvent();
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
        QString descLocalisation = descLocalisationLineEdit->text();

        localisationMap[QString("%1.%2.t").arg(namespaceName).arg(eventId)] = titleLocalisation;
        localisationMap[QString("%1.%2.desc").arg(namespaceName).arg(eventId)] = descLocalisation;
    }

    void addOption() {
        currentOptionCounter++;
        QString eventName = eventSelector->currentText();  // ��ȡ��ǰѡ�е��¼�
        QString eventId = eventName.mid(eventName.lastIndexOf('.') + 1);

        if (!optionCounters.contains(eventName)) {
            // �ڵ�һ��Ϊ����¼����ѡ��ʱ��ʼ��ѡ�������
            optionCounters[eventName] = 1;
        }
        else {
            optionCounters[eventName]++;
        }
        int optionId = optionCounters[eventName];

        // ���¼��༭���в����¼�
        QString eventText = eventEditor->toPlainText();
        int startIndex = eventText.indexOf(QString("\n%1.%2 = {").arg(namespaceLineEdit->text()).arg(eventId));
        if (startIndex == -1) {
            // δ�ҵ��¼��������ѡ��
            return;
        }
        int braceCount = 1;  // ���㿪���ŵ�����
        int endIndex = startIndex + QString("\n%1.%2 = {").arg(namespaceLineEdit->text()).arg(eventId).length();

        // �ҵ��¼����յ�
        while (endIndex < eventText.length()) {
            QChar ch = eventText[endIndex];
            if (ch == '{') {
                braceCount++;
            }
            else if (ch == '}') {
                braceCount--;
                if (braceCount == 0) {  // �����¼�����
                    break;
                }
            }
            endIndex++;
        }

        if (endIndex >= eventText.length()) {
            // δ�ҵ��¼������������ѡ��
            return;
        }

        QString option = QString("\n    option = {\n        name = %1.%2.%3\n        # option details...\n    }\n")
            .arg(namespaceLineEdit->text())
            .arg(eventId)
            .arg(optionId);
        eventText.insert(endIndex, option);
        eventEditor->setText(eventText);

        QLineEdit* optionLocalisationLineEdit = new QLineEdit(this);
        optionLocalisationLineEdits.append(optionLocalisationLineEdit);
        leftLayout->addWidget(optionLocalisationLineEdit);

        // ��ӱ��ػ��ı�
        QString optionLocalisation = QInputDialog::getText(this, "Enter Option Localisation", "Option Localisation:");
        optionLocalisationLineEdit->setText(optionLocalisation);
        localisationMap.insert(QString("%1.%2.%3").arg(namespaceLineEdit->text()).arg(eventId).arg(optionId), optionLocalisation);
        // ����eventViewer�е��ı�����
        loadEvent(eventName);
    }

    // �������¼�
    void addEvent() {
        eventCounter++;  // �����¼�������
        QString eventId = QString("%1").arg(eventCounter, 4, 10, QChar('0'));
        QString namespaceName = namespaceLineEdit->text();
        QString eventType = eventTypeComboBox->currentText();
        QString theme = themeLineEdit->text();


        QString eventStructure = QString("\n%1.%2 = {\n    type = %3\n    title = %1.%2.t\n    desc = %1.%2.desc\n    theme = %4\n    # other event details...\n}\n")
            .arg(namespaceName)
            .arg(eventId)
            .arg(eventType)
            .arg(theme);

        // ����Ѿ����ļ��м������¼����Ͳ�����µ��¼�
        if (eventLoadedFromFile) {
            eventLoadedFromFile = false;
            return;
        }

        eventEditor->append(eventStructure);
        currentOptionCounter = 0;  // ����ѡ�������

        // ֱ��������¼��� eventSelector �У����������
        eventSelector->addItem(QString("%1.%2").arg(namespaceLineEdit->text()).arg(eventId));

        // ��eventSelector�ĵ�ǰ��������Ϊ���¼���
        int newIndex = eventSelector->count() - 1;  // ���¼�������
        eventSelector->setCurrentIndex(newIndex);

        // ���浱ǰ���ػ��ı�
        QString defaultText = "Please enter localization content";

        // ���½��¼���Ĭ�ϱ��ػ��ı���ӵ� localisationMap �С�
        localisationMap[QString("%1.%2.t").arg(namespaceName).arg(eventId)] = defaultText;
        localisationMap[QString("%1.%2.desc").arg(namespaceName).arg(eventId)] = defaultText;

        // �������¼�
        loadEvent(QString("%1.%2").arg(namespaceName).arg(eventId));

    }




    void saveEvent() {
        QString fileName = QFileDialog::getSaveFileName(this, "Save Event", "", "Text Files (*.txt)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly)) {
                QTextStream stream(&file);
                stream.setGenerateByteOrderMark(true);  // ���������ֽ����ǣ�BOM��
                stream << "namespace = " << namespaceLineEdit->text() << "\n";
                stream << eventEditor->toPlainText();
                file.close();
            }
        }
        QString namespaceName = namespaceLineEdit->text();
        QString eventId = QString("%1").arg(eventCounter, 4, 10, QChar('0'));
        localisationMap.insert(QString("%1.%2.t").arg(namespaceName).arg(eventId), titleLocalisationLineEdit->text());
        localisationMap.insert(QString("%1.%2.desc").arg(namespaceName).arg(eventId), descLocalisationLineEdit->text());
        // �����¼��ı��ػ���Ϣ���ļ�
        QString localisationFilePath = QFileDialog::getSaveFileName(this, "Save Localisation", "", "YAML Files (*.yml)");
        QFile localisationFile(localisationFilePath);
        if (localisationFile.open(QIODevice::WriteOnly)) {
            QTextStream out(&localisationFile);
            out.setGenerateByteOrderMark(true);  // ���������ֽ����ǣ�BOM��
            out << "l_simp_chinese:\n";
            for (auto it = localisationMap.begin(); it != localisationMap.end(); ++it) {
                out << " " << it.key() << ": \"" << it.value() << "\"\n";
            }
        }
    }

        void updateEventStructure(const QString & eventType) {
            // ����¼��༭����������������¼�  ���ڽ����༭�����������ͬ�������༭����
            eventEditor->clear();
            for (int i = 1; i <= eventCounter; ++i) {
                QString eventId = QString("%1").arg(i, 4, 10, QChar('0'));
                QString namespaceName = namespaceLineEdit->text();
                QString theme = themeLineEdit->text();

                QString eventStructure = QString("\n%1.%2 = {\n    type = %3\n    title = %1.%2.t\n    desc = %1.%2.desc\n    theme = %4\n    # other event details...\n}\n")
                    .arg(namespaceName)
                    .arg(eventId)
                    .arg(eventType)
                    .arg(theme);

                eventEditor->append(eventStructure);
            }
            currentOptionCounter = 0;  // ����ѡ�������   
        }

        void updateEvent() {
            QString eventName = eventSelector->currentText();
            QString eventId = eventName.mid(eventName.lastIndexOf('.') + 1);
            int id = eventId.toInt();

            QString eventText = eventEditor->toPlainText();
            int startIndex = eventText.indexOf(QString("\n%1.%2 = {").arg(namespaceLineEdit->text()).arg(eventId));
            if (startIndex != -1) {
                int braceCount = 1;  // ���㿪���ŵ�����
                int endIndex = startIndex + QString("\n%1.%2 = {").arg(namespaceLineEdit->text()).arg(eventId).length();

                // �ҵ��¼����յ�
                while (endIndex < eventText.length()) {
                    QChar ch = eventText[endIndex];
                    if (ch == '{') {
                        braceCount++;
                    }
                    else if (ch == '}') {
                        braceCount--;
                        if (braceCount == 0) {  // �����¼�����
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

            // ���¼���������ȡ�¼�ID
            QString eventId = eventName.mid(eventName.lastIndexOf('.') + 1);
            int id = eventId.toInt();

            // ���¼��༭���в����¼�
            QString eventText = eventEditor->toPlainText();
            int startIndex = eventText.indexOf(QString("\n%1.%2 = {").arg(namespaceLineEdit->text()).arg(eventId));
            if (startIndex != -1) {
                int braceCount = 0;  // ���㿪���ŵ�����
                int endIndex = startIndex;

                // �ҵ��¼����յ�
                while (endIndex < eventText.length()) {
                    QChar ch = eventText[endIndex];
                    if (ch == '{') {
                        braceCount++;
                    }
                    else if (ch == '}') {
                        braceCount--;
                        if (braceCount == 0) {  // �����¼�����
                            endIndex++;
                            break;
                        }
                    }
                    endIndex++;
                }

                QString eventDetails = eventText.mid(startIndex, endIndex - startIndex);
                eventViewer->setText(eventDetails);

                // ��ȡ�¼�ID
                QString eventId = eventName.mid(eventName.lastIndexOf('.') + 1);
                // ��ȡ�����ռ�
                QString namespaceName = namespaceLineEdit->text();
                // ���±��ػ��ı���
                titleLocalisationLineEdit->setText(localisationMap.value(QString("%1.%2.t").arg(namespaceName).arg(eventId)));
                descLocalisationLineEdit->setText(localisationMap.value(QString("%1.%2.desc").arg(namespaceName).arg(eventId)));
                
            }
        }
        




private:
    //����ԭ����һ����������������û�б��õ�������ɾ���ˣ������˲���

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
    QLineEdit* descLocalisationLineEdit;
    QList<QLineEdit*> optionLocalisationLineEdits;
    QMap<QString, QString> localisationMap;  // �����¼��뱾�ػ���Ϣ��ӳ���ϵ
    QVBoxLayout* leftLayout;
    QMap<QString, int> optionCounters;  // ����ÿ���¼���ѡ�������
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
    bool eventLoadedFromFile = false;  // û�д��ļ��м����¼�

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










