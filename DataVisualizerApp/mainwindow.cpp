#include "mainwindow.h"
#include "ui_mainwindow.h"

QString email;
QString pass;
MainWindow* prozor;
std::string token;

class device
{
    QString deviceName;
    QString pressure;
    QString temperature;
    QString humidity;

    device()
    {
        deviceName = "";
        pressure = "";
        temperature = "";
        humidity = "";
    }
    bool isEmpty()
    {
        return deviceName == "" &&
        pressure == "" &&
        temperature == "" &&
        humidity == "";
    }
    bool isFull()
    {
        return deviceName != "" &&
        pressure != "" &&
        temperature != "" &&
        humidity != "";
    }
};

std::string parseValue(std::string json, std::string lookingFor, int from = 0)
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

    return value;
}

int locateValue(std::string json, std::string lookingFor, int from = 0)
{
    return json.find(lookingFor, from) + lookingFor.length() + 3;
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
void  getDevice()
{
    CURL* c = curl_easy_init();
    //URL, request type
    curl_easy_setopt(c, CURLOPT_URL, "https://api-demo.wolkabout.com/api/devices/");
    curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "GET");

    //setting up the header (authorization)
    std::string authorization = "Authorization: Bearer " + token;
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, &authorization[0]);
    curl_easy_setopt(c, CURLOPT_HTTPHEADER, headers);

    //prepare for response
    std::string received;
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(c, CURLOPT_WRITEDATA, &received);

    //performing
    curl_easy_perform(c);

    //parse device name out of response
    std::string deviceName;
    int deviceLoc = 0;
    do
    {
        deviceName = parseValue(received, "deviceName", deviceLoc);
        deviceLoc = locateValue(received, "deviceName", deviceLoc) + 1;
        prozor->ui->deviceCombo->addItem(QString::fromUtf8(deviceName.c_str()));


        int presLoc = locateValue(received,"Barometer", 0);
        std::string pressure = parseValue(received, "value", presLoc);

        int humiLoc = locateValue(received,"Hygrometer", 0);
        std::string humidity = parseValue(received, "value", humiLoc);

        int tempLoc = locateValue(received,"Thermometer", 0);
        std::string temperature = parseValue(received, "value", tempLoc);
    }
    while(deviceName!="")
    curl_easy_cleanup(c);
}
void getDevices()
{

}
void loginWolk()
{
    prozor->ui->isConnected->setChecked(false);
    CURL* c = curl_easy_init();
    //URL, request type
    curl_easy_setopt(c, CURLOPT_URL, "https://api-demo.wolkabout.com/api/emailSignIn/");
    curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "POST");

    //body; string -> object -> document -> json
    QJsonObject obj;
    obj.insert("username", prozor->ui->emailLine->text());//"lcubrilo01@gmail.com");
    obj.insert("password", prozor->ui->passLine->text());//"FHp4LwCzGNXchS2");
    QJsonDocument doc(obj);
    std::string jsonString = doc.toJson(QJsonDocument::Compact).toStdString();
    curl_easy_setopt(c, CURLOPT_POSTFIELDS, jsonString.c_str());

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

    //parse token out of response
    std::string str = parseValue(received, "accessToken", 0);
    if(str!="")
     {
        prozor->ui->isConnected->setChecked(true);
        prozor->ui->sensGroup->setEnabled(true);
    }
    curl_easy_cleanup(c);
    token = str;
}
void updateEmail()
{
    email = prozor->ui->emailLine->text();
}
void updatePass()
{
    pass = prozor->ui->passLine->text();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->ui->isConnected->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->ui->isConnected->setFocusPolicy(Qt::NoFocus);
    prozor = this;
    connect(ui->pushButton, &QPushButton::clicked, loginWolk);
    connect(ui->emailLine, &QLineEdit::textChanged, updateEmail);
    connect(ui->passLine, &QLineEdit::textChanged, updatePass);
    connect(ui->deviceCombo, &QComboBox::currentTextChanged, getDevice);

}

MainWindow::~MainWindow()
{
    delete ui;
}

