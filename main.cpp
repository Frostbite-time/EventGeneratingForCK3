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
        QHBoxLayout* mainLayout = new QHBoxLayout(this);  // Change QVBoxLayout to QHBoxLayout
        QVBoxLayout* leftLayout = new QVBoxLayout();  // Layout for the left side
        QVBoxLayout* rightLayout = new QVBoxLayout();  // Layout for the right side (sidebar)
        updateButton = new QPushButton("Update Event", this);
        titleLocalisationLineEdit = new QLineEdit(this);
        descLocalisationLineEdit = new QLineEdit(this);
        addTriggerButton = new QPushButton("Add Trigger", this);

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
        leftLayout->addWidget(addTriggerButton);
        leftLayout->addWidget(addEventButton);
        leftLayout->addWidget(saveButton);
        leftLayout->addItem(new QSpacerItem(0, 20));

        rightLayout->addWidget(new QLabel("Select Event:", this));
        rightLayout->addWidget(eventSelector);
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





        setLayout(mainLayout);

        QString namespaceName = QInputDialog::getText(this, "Enter Namespace", "Namespace:");
        namespaceLineEdit->setText(namespaceName);

        QString defaultText = "Please enter localization content";
        titleLocalisationLineEdit->setText(defaultText);
        descLocalisationLineEdit->setText(defaultText);

        addEvent();
    }

public slots:

    QString generateTriggerText(QString delayType, int delayValue, int delayMinValue ,int delayMaxValue ,QString delayUnit, QString triggeredEventId) {

        QString text = "trigger_event = {\n";

        if (delayType == "Fixed") {
            text += "    " + delayUnit + " = " + QString::number(delayValue) + "\n";
        }
        else if (delayType == "Range") {
            text += "    "+ delayUnit +" = { " + QString::number(delayMinValue) + " " + QString::number(delayMaxValue) + " }\n";
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

        QString option = QString("\n        option = {\n            name = %1.%2.%3\n            # option details...\n        }\n")
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
    }

    // �������¼�
    void addEvent() {
        eventCounter++;  // �����¼�������
        QString eventId = QString("%1").arg(eventCounter, 4, 10, QChar('0'));
        QString namespaceName = namespaceLineEdit->text();
        QString eventType = eventTypeComboBox->currentText();
        QString theme = themeLineEdit->text();

        // ��ʼ�����ػ���Ϣ
        localisationMap[QString("%1.%2.t").arg(namespaceName).arg(eventId)] = "";
        localisationMap[QString("%1.%2.desc").arg(namespaceName).arg(eventId)] = "";

        QString eventStructure = QString("\n%1.%2 = {\n    type = %3\n    title = %1.%2.t\n    desc = %1.%2.desc\n    theme = %4\n    # other event details...\n}\n")
            .arg(namespaceName)
            .arg(eventId)
            .arg(eventType)
            .arg(theme);

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
                int endIndex = eventText.indexOf("}\n", startIndex) + 2;
                QString newEvent = eventViewer->toPlainText();
                eventText.replace(startIndex, endIndex - startIndex, newEvent);
                eventEditor->setText(eventText);
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










