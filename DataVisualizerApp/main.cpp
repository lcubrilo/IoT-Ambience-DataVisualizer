#include "mainwindow.h"
#include <curl/curl.h>
#include <QApplication>
#include <iostream>

struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    //prebaceno u c++
    char *ptr = new char(mem->size + realsize + 1);
    ptr = mem->memory;

  /*char *ptr = realloc(mem->memory, mem->size + realsize + 1); //ovo je C
    if(ptr == NULL) {
        //out of memory!
        printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }*/
    mem->memory = ptr;

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

char *readToken(CURL *c)
{
    MemoryStruct chunk;
    chunk.memory = new char(0);
    chunk.size = 0;
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(c, CURLOPT_WRITEDATA, (void *)&chunk);
    std::cout<<curl_easy_perform(c);
    return chunk.memory;
}

char *loginWolk(CURL *c)
{
    c = curl_easy_init();
    curl_easy_setopt(c, CURLOPT_URL, "https://api-demo.wolkabout.com/api/emailSignIn/");
    curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(c, CURLOPT_POSTFIELDS, "{\"username\": \"lcubrilo01@gmail.com\",\"password\": \"FHp4LwCzGNXchS2\"}");
    std::cout<<curl_easy_perform(c);
    //return readToken(c);
}

void getDevice(CURL *c, char* token)
{
    curl_easy_setopt(c, CURLOPT_URL, "https://api-demo.wolkabout.com/api/devices/471d6850-8bed-4eca-9e6e-97fd9300b117");
    curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "GET");

    struct curl_slist* header = NULL;
    header = curl_slist_append(header, "Authorization: Bearer");
    header = curl_slist_append(header, token);

    curl_easy_setopt(c, CURLOPT_HTTPHEADER, header);
    curl_easy_perform(c);
}

int main(int argc, char *argv[])
{
    CURL* c;
   //char* token =
            loginWolk(c);
    //getDevice(c, token);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
