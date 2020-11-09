#include "mainwindow.h"
#include <curl/curl.h>
#include <QApplication>
#include <iostream>
#include <QJsonObject>
#include <QJsonDocument>


std::string parseValue(std::string json, std::string lookingFor)
{
    int start = json.find(lookingFor) + lookingFor.length() + 3;
    int end = json.find("\"", start);
    int len = end - start;

    return json.substr(start, len);
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
void  getDevice(CURL *c, std::string token)
{
    //URL, request type
    curl_easy_setopt(c, CURLOPT_URL, "https://api-demo.wolkabout.com/api/devices/471d6850-8bed-4eca-9e6e-97fd9300b117");
    curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "GET");

    //setting up the header (authorization)
    struct curl_slist* header = NULL;
    header = curl_slist_append(header, "Authorization: Bearer ");
    header = curl_slist_append(header, &token[0]);
    curl_easy_setopt(c, CURLOPT_HTTPHEADER, header);

    //prepare for response
    std::string received;
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(c, CURLOPT_WRITEDATA, &received);

    //performing
    curl_easy_perform(c);

    //parse device name out of response
    std::string deviceName = "lol";
    deviceName+=parseValue(received, "name");
    std::cout<<deviceName;
}
std::string loginWolk(CURL *c)
{
    //URL, request type
    curl_easy_setopt(c, CURLOPT_URL, "https://api-demo.wolkabout.com/api/emailSignIn/");
    curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "POST");

    //body; string -> object -> document -> json
    QJsonObject obj;
    obj.insert("username","lcubrilo01@gmail.com");
    obj.insert("password","FHp4LwCzGNXchS2");
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
    return parseValue(received, "accessToken");
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    CURL* c = curl_easy_init();
    std::string accessToken = loginWolk(c);

    getDevice(c, accessToken);

    curl_easy_cleanup(c);
    return a.exec();
}
