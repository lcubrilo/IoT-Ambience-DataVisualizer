#include "mainwindow.h"
#include "ui_mainwindow.h"
//FHp4LwCzGNXchS2

MainWindow* prozor;

//Timers
void getSensors();
QTimer* timer;
void startProgress()
{
    timer->start(500);
    prozor->ui->progressBar->setValue(10);
    prozor->ui->progressLabel->setText("Updated values!");
}
void updateProgress()
{
    timer->start(500);
    int x = prozor->ui->progressBar->value();
    if(x>10)
        //prozor->ui->progressLabel->setText("Updating values...");
        prozor->ui->progressLabel->setText("...");
    if(x>90)
    {
        timer->stop();
        getSensors();
        return;
    }
    prozor->ui->progressBar->setValue(x+10);
}

//JSON Manipulations
QString parseValue(std::string json, std::string lookingFor, int from = 0)
{
    int start = json.find(lookingFor, from) + lookingFor.length() + 3;
    int end = json.find("\"", start);
    int len = end - start;
    std::string value = json.substr(start, len);

    if(value == "") //the value may be a json
    {
        end = json.find("}", start); //therefore, the value actually ends with }
        len = end - start;
        value = json.substr(start, len); //redo with the fixed endpoint
    }

    return QString::fromUtf8(value.c_str());
}
int locateValue(std::string json, std::string lookingFor, int from = 0)
{
    return json.find(lookingFor, from) + lookingFor.length() + 3;
}
QString getSensorValue(std::string json, QString sensor, QString device, std::string thing = "value")
{
    int iterator = 0;
    QString tmpSensor, tmpDevice, value;
    do //in the proper device
    {
        do //iterate until the proper sensor
        {
            tmpSensor = parseValue(json, "name", iterator);
            iterator = locateValue(json, "name", iterator) + 1;
        }while(tmpSensor!=sensor);

        tmpDevice = parseValue(json, "deviceName", iterator);
        iterator = locateValue(json, "deviceName", iterator) + 1;

    }while(tmpDevice!=device);

    return parseValue(json, thing, iterator);
}
std::string credentials2JSON()
{
    //body; string -> object -> document -> json
    QJsonObject obj;
    obj.insert("username", prozor->ui->emailLine->text());
    obj.insert("password", prozor->ui->passLine->text());
    QJsonDocument doc(obj);
    std::string jsonString = doc.toJson(QJsonDocument::Compact).toStdString();
    return jsonString;
}

//curl helpers
QString token;
double temperature, pressure, humidity;
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
std::string get(char* url)
{
    CURL* c = curl_easy_init();
    //URL, request type
    curl_easy_setopt(c, CURLOPT_URL, url);
    curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "GET");

    //setting up the header (authorization)
    std::string authorization = token.toLocal8Bit().constData();
    authorization = "Authorization: Bearer " + authorization;
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, &authorization[0]);
    curl_easy_setopt(c, CURLOPT_HTTPHEADER, headers);

    //prepare for response
    std::string received;
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(c, CURLOPT_WRITEDATA, &received);

    //performing
    curl_easy_perform(c);
    curl_easy_cleanup(c);

    return received;
}
std::string post(char* url, std::string body)
{
    CURL* c = curl_easy_init();
    //URL, request type
    curl_easy_setopt(c, CURLOPT_URL, url);
    curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "POST");

    curl_easy_setopt(c, CURLOPT_POSTFIELDS, body.c_str());

    //header
    struct curl_slist* header = NULL;
    header = curl_slist_append(header,"Content-Type: application/json");
    curl_easy_setopt(c, CURLOPT_HTTPHEADER, header);

    //prepare for response
    std::string received;
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(c, CURLOPT_WRITEDATA, &received);

    //performing
    curl_easy_perform(c);
    curl_easy_cleanup(c);

    return received;
}

//Login stuff
bool isMail(QString maybeEmail)
{
    QRegExp mailREX("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
    mailREX.setCaseSensitivity(Qt::CaseInsensitive);
    mailREX.setPatternSyntax(QRegExp::RegExp);
    bool isOkay = mailREX.exactMatch(maybeEmail);

    if(!isOkay)
    {
        QMessageBox Msgbox;
        Msgbox.setText(maybeEmail + " is not a valid email address.");
        Msgbox.exec();
    }

    return isOkay;
}
void loginWolk()
{
    prozor->ui->isConnected->setChecked(false);
    //Check if mail is real
    if(!isMail(prozor->ui->emailLine->text()))
        return;

    //Prepare http post request
    std::string url = "https://api-demo.wolkabout.com/api/emailSignIn/";
    std::string body = credentials2JSON();

    std::string received = post(&url[0], body);

    //parse token out of response
    QString parsedToken = parseValue(received, "accessToken", 0);
    if(parsedToken=="")
        return;

    token = parsedToken;
    prozor->ui->sensGroup->setEnabled(true);
    prozor->ui->humiCombo->setEnabled(false);
    prozor->ui->presCombo->setEnabled(false);
    prozor->ui->tempCombo->setEnabled(false);
    prozor->ui->credGroup->setEnabled(false);
    prozor->ui->pushButton->setText("Log out");
}
void logoutWolk()
{
    prozor->ui->sensGroup->setEnabled(false);
    prozor->ui->credGroup->setEnabled(true);
    prozor->ui->pushButton->setText("Log in");
    prozor->ui->emailLine->setText("");
    prozor->ui->passLine->setText("");
    prozor->ui->progressLabel->setText("");
}
void logWolk()
{
    if(prozor->ui->pushButton->text() == "Log in")
        loginWolk();
    else
        logoutWolk();
}

//Devices stuff
void getDevices()
{
    startProgress();
    prozor->ui->isConnected->setEnabled(false);
    prozor->ui->lcdTemp->display("");
    prozor->ui->lcdPres->display("");
    prozor->ui->lcdHumi->display("");

    prozor->ui->deviceCombo->setPlaceholderText("Click to see devices");
    std::string url = "https://api-demo.wolkabout.com/api/devices/";
    std::string received = get(&url[0]);

    //parse device name out of response
    QString deviceName;
    int deviceLoc = 0;

    do
    {
        deviceName = parseValue(received, "deviceName", deviceLoc);
        deviceLoc = locateValue(received, "deviceName", deviceLoc) + 1; //iterate

        if(deviceName.contains(":")) //there must be none of :
            break;
        else if(prozor->ui->deviceCombo->findText(deviceName)==-1) //if its not in the list
            prozor->ui->deviceCombo->addItem(deviceName);
    }
    while(!deviceName.contains(":"));
}
bool deviceOnline(std::string json, QString sensor, QString device)
{
    return getSensorValue(json, sensor, device, "connected") == "rue";
}

//Sensors stuff
void displayValues()
{
    int adjustedTemp, adjustedPres, adjustedHumi;

    QString tempUnit = prozor->ui->tempCombo->currentText();
    qDebug() << tempUnit;
    if(tempUnit == "°C")
        adjustedTemp = (int)temperature;
    else if(tempUnit == "K")
        adjustedTemp = (int)temperature + 273;
    else if(tempUnit == "F")
        adjustedTemp =  (int)(temperature * 1.8) + 32;

    QString presUnit = prozor->ui->presCombo->currentText();
    qDebug() << presUnit;
    if(presUnit == "mbar"||presUnit=="hPa")
        adjustedPres = (int)pressure;
    else if(presUnit == "kPa")
        adjustedPres = (int)(pressure/10);
    else if(presUnit == "psi")
        adjustedPres =  (int)(pressure*0.0145037738);

    QString humiUnit = prozor->ui->humiCombo->currentText();
    qDebug() << humiUnit;
    if(humiUnit == "%")
        adjustedHumi = (int)humidity;

    prozor->ui->lcdTemp->display(adjustedTemp);
    prozor->ui->lcdPres->display(adjustedPres);
    prozor->ui->lcdHumi->display(adjustedHumi);
    prozor->ui->isConnected->setChecked(true);
}
void getSensors()
{
    startProgress();
    std::string url = "https://api-demo.wolkabout.com/api/deviceSensors/";
    std::string received = get(&url[0]);

    QString device = prozor->ui->deviceCombo->currentText();

    temperature = getSensorValue(received, "Thermometer", device).toDouble();
    pressure = getSensorValue(received, "Barometer", device).toDouble();
    humidity = getSensorValue(received, "Hygrometer", device).toDouble();

    prozor->ui->isConnected->setEnabled(deviceOnline(received, "Thermometer", device));

    qDebug() << temperature << " " << pressure << " " << humidity;
    prozor->ui->humiCombo->setEnabled(true);
    prozor->ui->presCombo->setEnabled(true);
    prozor->ui->tempCombo->setEnabled(true);

    displayValues();
}

//Qt stuff
void connections()
{
    Ui::MainWindow* ui = prozor->ui;
    prozor->connect(ui->pushButton, &QPushButton::clicked, logWolk);

    prozor->connect(ui->refreshDevicesButton, &QPushButton::clicked, getDevices);
    prozor->connect(ui->deviceCombo, &QComboBox::currentTextChanged, getSensors);
    prozor->connect(ui->refreshSensorsButton, &QPushButton::clicked, getSensors);
    prozor->connect(timer, &QTimer::timeout, updateProgress);

    prozor->connect(ui->tempCombo, &QComboBox::currentTextChanged, displayValues);
    prozor->connect(ui->presCombo, &QComboBox::currentTextChanged, displayValues);
    prozor->connect(ui->humiCombo, &QComboBox::currentTextChanged, displayValues);
}
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    timer = new QTimer(this);
    ui->setupUi(this);

    //Checkmark settings
    this->ui->isConnected->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->ui->isConnected->setFocusPolicy(Qt::NoFocus);

    prozor = this;
    connections();
}
MainWindow::~MainWindow()
{
    delete ui;
}

