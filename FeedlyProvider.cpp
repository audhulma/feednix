#include <iostream>
#include <string>
#include <curl/curl.h>
#include <stdio.h>
#include <fstream>
#include <jsoncpp/json/json.h>
//#include <json/json.h>
using namespace std;

#include "FeedlyProvider.h"

FeedlyProvider::FeedlyProvider(){

        curl_global_init(CURL_GLOBAL_DEFAULT);
}

string FeedlyProvider::getAuthCode(const string& email, const string& passwd){

        FILE* temp = fopen("temp.txt", "wb");

        getCookies(temp);

        curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, string(GOOGLE_AUTH_URL).c_str()); 
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/4.0");
        curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1 );
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookie");
        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookie");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, temp);
//        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_res = curl_easy_perform(curl);

        curl_easy_setopt(curl, CURLOPT_REFERER, string(GOOGLE_AUTH_URL).c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, string("signIn=Sign+in&_utf8=&#9731;&bgresponse=js_disabled&GALX=" + extract_galx_value() + "&pstMsg=&dnCon=&checkConnection=youtube:1141:1&checkedDomains=youtube&Email=jorgemartinezhernandez&Passwd=trueforerunner117").c_str());
        curl_res = curl_easy_perform(curl);
        char *currentURL;



        if(curl_res == CURLE_OK){
                curl_res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &currentURL);
                string tempCode(currentURL);

                string code;
                if((curl_res == CURLE_OK) && currentURL){
                        unsigned codeIndex = tempCode.find("code=") + 5;
                        code = tempCode.substr(codeIndex, (tempCode.find("&", codeIndex) - codeIndex));
                        userData["userAuthCode"] = code;
                        curl_easy_cleanup(curl);
                        return code;
                }
        }

        fprintf(stderr, "curl_easy_perform() failed : %s\n", curl_easy_strerror(curl_res));
        curl_easy_cleanup(curl);
        return "";


}
void FeedlyProvider::getTokens(){

        struct curl_httppost *formpost = NULL;
        FILE *tokenJSON = fopen("tokenFile.json", "wb");

        curl = curl_easy_init();

        feedly_url = FEEDLY_URI + string("auth/token?code=") + userData["userAuthCode"] +  CLIENT_ID +  CLIENT_SECRET +  REDIRECT_URI + SCOPE + "&grant_type=authorization_code";

        curl_easy_setopt(curl, CURLOPT_URL, feedly_url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(curl, CURLOPT_AUTOREFERER, true);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, tokenJSON);
        curl_res = curl_easy_perform(curl);
        bool parsingSuccesful;
        fclose(tokenJSON);
        Json::Value root;
        Json::Reader reader;
        ifstream tokenFile;
        tokenFile.open("tokenFile.json");
        if(tokenJSON != NULL && curl_res == CURLE_OK){
                parsingSuccesful = reader.parse(tokenFile, root);
                if(parsingSuccesful){
                        userData["userAccessToken"] = (root["access_token"]).asString();
                }
        }

        if(!parsingSuccesful || curl_res != CURLE_OK){
                cerr << "Failed to parse tokens file: " << reader.getFormatedErrorMessages() << endl;
                fprintf(stderr, "curl_easy_perform() failed : %s\n", curl_easy_strerror(curl_res));
        }
        curl_easy_cleanup(curl);


} 
void FeedlyProvider::giveAll(){
}
string FeedlyProvider::extract_galx_value(){

        string l;
        ifstream temp;
        temp.open("cookie");
        int index;

        while(getline(temp, l)){
                index = l.find("GALX");
                if(index > 0 && index != string::npos){
                        unsigned last = l.find("X") + 2;
                        temp.close();
                        return l.substr(last);

                }
        } 

        temp.close();
        return l;
}
void FeedlyProvider::getCookies(FILE* temp){

        curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, string(GOOGLE_AUTH_URL).c_str()); 
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/4.0");
        curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1 );
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookie");
        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookie");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, temp);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        curl_res = curl_easy_perform(curl);

        curl_easy_setopt(curl, CURLOPT_REFERER, string(GOOGLE_AUTH_URL).c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, string("signIn=Sign+in&_utf8=&#9731;&bgresponse=js_disabled&GALX=" + extract_galx_value() + "&pstMsg=&dnCon=&checkConnection=youtube:1141:1&checkedDomains=youtube&Email=jorgemartinezhernandez&Passwd=trueforerunner117").c_str());
        curl_res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
}
