/*
https://randomnerdtutorials.com/esp32-web-server-microsd-card/
Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-web-server-microsd-card/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h> // dvarrel 1.1.4
#include <ESPAsyncWebServer.h> // dvarrel 1.2.6
#include "FS.h" 
#include "SD.h"
#include "SPI.h"
// pour rtc
#include <Wire.h>
#include <DS3231.h> // DS32231 de Andrew Wickert  1.1.2
// lidar
#include <SoftwareSerial.h> // !!!!!! version 7.0.0 maxi
#include "TFMini.h" //Peter Jansen 0.1.0

boolean DEBUG = true;
String stDeb; 
String stFin;
String stList;
String stHTML;

const char* ssid     = "Base_Desnes";
const char* password = "";

// Create AsyncWebServer object on port 80 (http) ou 443 (https)
AsyncWebServer server(80);
// lidar un seul fil RX + masse et 5v
SoftwareSerial mySerial(16, 17); //   16: RX, (TX sur 17 inutilise)
TFMini tfmini;

// rtc
RTClib rtc;


// leds
int ledpassage = 27;
int ledpower = 26;
//
int dist = 0;
int dist0 = 0;
int mini = 0;
int maxi = 65000;
int seuil = 20;
int ecart = 10;
int precedent = 0;
int personne = 0;
int nb = 0;
///////////////////////////////////////////////////////////

void initVarHtml()
{
  // debut du fichier index.html
  stDeb += "<!DOCTYPE html>\n";
  stDeb += "<html>\n";
  stDeb += "<meta charset="+String('"')+"utf-8"+String('"')+">\n";
  stDeb += "  <head>\n";
  stDeb += "    <!-- CSS -> Style -->\n";
  stDeb += "    <style media="+String('"')+"screen"+String('"')+">\n";
  stDeb += "      h1,\n";
  stDeb += "      h2,\n";
  stDeb += "      h3 {\n";
  stDeb += "        text-align: center;\n";
  stDeb += "      }\n";
  stDeb += "      table,\n";
  stDeb += "      td,\n";
  stDeb += "      tr {\n";
  stDeb += "        border: 1px solid #333;\n";
  stDeb += "        align: center;\n";
  stDeb += "      }\n";
  stDeb += "      table {\n";
  stDeb += "        width: 50%;\n";
  stDeb += "      }\n";
  stDeb += "      thead,\n";
  stDeb += "      tfoot {\n";
  stDeb += "        background-color: #333;\n";
  stDeb += "        color: #fff;\n";
  stDeb += "      }\n";
  stDeb += "      .center {\n";
  stDeb += "        margin-left: auto;\n";
  stDeb += "        margin-right: auto;\n";
  stDeb += "        text-align: center;\n";
  stDeb += "        vertical-align: middle;\n";
  stDeb += "      }\n";
  stDeb += "    </style>\n";
//
  stDeb += "    <!-- Javascript -> Logic -->\n";
  stDeb += "    <script type="+String('"')+"text/javascript"+String('"')+">\n";
//
  stDeb += "      function removeLastDirectoryPartOf(the_url) {\n";
  stDeb += "        var the_arr = the_url.split("+String('"')+"/"+'"'+");\n";
  stDeb += "        the_arr.pop();\n";
  stDeb += "        return the_arr.join("+String('"')+"/"+String('"')+");\n";
  stDeb += "      }\n";
/*
  stDeb += "      function downloadURI(uri, name){ \n";
  stDeb += "        var link = document.createElement("+String('"')+"a"+'"'+");\n";
  stDeb += "        link.download = name;\n";
  stDeb += "        link.href = uri;\n";
  stDeb += "        document.body.appendChild(link);\n";
  stDeb += "        link.click();\n";
  stDeb += "        document.body.removeChild(link);\n";
  stDeb += "        delete link;\n";
  stDeb += "      }\n";
*/
  stDeb += "      function download(dataurl, filename) {\n";
//  stDeb += "alert(dataurl);\n";  //   affiche : "content://medias/external_primary/downloads/20220519"
  stDeb += "        const link = document.createElement("+String('"')+"a"+'"'+");\n";
  stDeb += "        link.href = dataurl;\n";
//  stDeb += "alert(filename);\n";  //   afiche : "20220519"
//  stDeb += "alert(link.href);\n";
  stDeb += "        link.download = filename;\n";
  stDeb += "        link.click();\n";
  stDeb += "      }\n";
//
  stDeb += "      function checkAndUncheck() {\n";
  stDeb += "        var isChecked = false;\n";
  stDeb += "        var inputs = document.getElementsByTagName("+String('"')+"input"+'"'+");\n";
  stDeb += "        var buttonCheckAll = document.getElementById("+String('"')+"buttonCheckAll"+'"'+");\n";
  stDeb += "        for (var i = 0; i < inputs.length; ++i) {\n";
  stDeb += "          if (inputs[i].checked === true) {\n";
  stDeb += "            isChecked = true;\n";
  stDeb += "            break;\n";
  stDeb += "          }\n";
  stDeb += "        }\n";
  stDeb += "        for (var i = 0; i < inputs.length; ++i) {\n";
  stDeb += "          inputs[i].checked = !isChecked;\n";
  stDeb += "        }\n";
  stDeb += "        if (isChecked) {\n";
  stDeb += "          buttonCheckAll.value = "+String('"')+"Tout cocher"+String('"')+";\n";
  stDeb += "        } else {\n";
  stDeb += "          buttonCheckAll.value = "+String('"') +"Tout decocher"+String('"')+";\n";
  stDeb += "        }\n";
  stDeb += "      }\n";
//  
  stDeb += "      function downloadAllItems() {\n";
  stDeb += "        var concatenateUrlForDownload = "+String('"')+String('"')+";\n";
  stDeb += "        var inputs = document.getElementsByTagName("+String('"')+"input"+'"'+");\n";
  stDeb += "        const HOST_URL = window.location.href;\n";
  stDeb += "        for (var i = 0; i < inputs.length; ++i) {\n";
  stDeb += "          if (inputs[i].checked && inputs[i].id != "+String('"')+"buttonCheckAll"+'"'+") {\n";
  stDeb += "            console.log("+String('"')+"inputs[i].id: '"+String('"')+" + inputs[i].id +"+String('"')+"'"+String('"')+");\n";
  stDeb += "            concatenateUrlForDownload =\n";
  stDeb += "              removeLastDirectoryPartOf(HOST_URL) +" + String('"')+'/'+String('"') + "+ inputs[i].id;\n";
//  stDeb += "            concatenateUrlForDownload ="+String('"')+"SD" + String('/') + String('"')+ "+ inputs[i].id;\n";
  stDeb += "            console.log(\n";
  stDeb += "              "+String('"')+"concatenateUrlForDownload: '"+String('"')+" + concatenateUrlForDownload +" + String('"')+"'"+String('"')+"\n";
  stDeb += "            );\n";
            // Download files
            // TODO
  stDeb += "            download(concatenateUrlForDownload, inputs[i].id);\n";
//  stDeb += "            downloadURI(concatenateUrlForDownload, inputs[i].id);\n";//plante
  stDeb += "          }\n";
//  stDeb += " deleteFile(SD, "+String('"')+"/index.html"+String('"')+");\n";
  stDeb += "        }\n";
            // Debug display full object
            //console.log(inputs);
  stDeb += "      }\n";
//
  stDeb += "    </script>\n";
  stDeb += "  </head>\n";
  

  stDeb += "  <!-- HTML -> Struct -->\n";
  stDeb += "  <body>\n";
  stDeb += "    <div>\n";
  stDeb += "      <div>\n";
  stDeb += "        <h1 class="+String('"')+"center"+String('"')+">Serveur de fichiers</h1>\n";
  stDeb += "      </div>\n";
  stDeb += "      <div>\n";
  stDeb += "        <h3>😼 FabLab made in Iki 😼</h3>\n";
  stDeb += "      </div>\n";
  stDeb += "    </div>\n";
  stDeb += "      <div class="+String('"')+"center"+String('"')+">\n";
//  stDeb += "        <br />\n";
  stDeb += "        <input\n";
  stDeb += "          onclick="+String('"')+"checkAndUncheck()"+'"'+"\n";
  stDeb += "          type='button'\n";
  stDeb += "          value='Tout cocher'\n";
  stDeb += "          id='buttonCheckAll'\n";
  stDeb += "        />\n";
  stDeb += "        <button onclick="+String('"')+"downloadAllItems()"+'"'+" type="+'"'+"submit"+'"'+">\n";
  stDeb += "          Tout télécharger\n";
  stDeb += "        </button>\n";
  stDeb += "      </div>\n";
  stDeb += "      <br />\n";
  stDeb += "    <div>\n";
  stDeb += "      <div>\n";
  stDeb += "        <table class="+String('"')+"center"+String('"')+">\n";
  stDeb += "          <thead>\n";
  stDeb += "            <tr>\n";
  stDeb += "              <th colspan="+String('"')+"2"+String('"')+">Documents disponibles</th>\n";
  stDeb += "            </tr>\n";
  stDeb += "          </thead>\n";
  stDeb += "          <tbody>\n";

  stFin += "          </tbody>\n";
  stFin += "        </table>\n";
  stFin += "      </div>\n";
  stFin += "    </div>\n";
  stFin += "  </body>\n";
  stFin += "</html>\n";

  stHTML = stDeb + stList + stFin;
  Serial.println(stHTML.c_str());
};

//////////////////

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
  String st = "";
  String st0 = "";
  stList = ""; 
  //Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if(!root){
    Serial.println("Failed to open directory");
    return;
  }
  if(!root.isDirectory()){
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      if(levels)
        listDir(fs, file.name(), levels -1);
    } else {
        st0 = String(file.name());
        if (st0 != String("index.html")){
          st = '"' + st0 + '"'+ "type="+String('"')+"checkbox"+String('"')+" /></td>";
          stList += "<tr><td><input id=" + st;
          stList += "<td>"+st0+"</td></tr>\n";
        };
    }
    file = root.openNextFile();
  }
};

//////////////////

void createDir(fs::FS &fs, const char * path){
  Serial.printf("Creating Dir: %s\n", path);
  if(fs.mkdir(path)){
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
};

//////////////////

void removeDir(fs::FS &fs, const char * path){
  Serial.printf("Removing Dir: %s\n", path);
  if(fs.rmdir(path)){
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
};

//////////////////

void readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
};

//////////////////

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
};

//////////////////

void appendFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
      Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
};

//////////////////

void renameFile(fs::FS &fs, const char * path1, const char * path2){
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
};

//////////////////

void deleteFile(fs::FS &fs, const char * path){
  Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
};

//////////////////

void testFileIO(fs::FS &fs, const char * path){
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if(file){
    len = file.size();
    size_t flen = len;
    start = millis();
    while(len){
      size_t toRead = len;
      if(toRead > 512){
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }

  file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for(i=0; i<2048; i++){
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}

//////////////////

void initSDCard(){
  if(!SD.begin(5)){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
};

///////////////////////////////////////////////////////////

void EcritFichier()
{
  DateTime t = rtc.now();
  String nomfic;
  String heure ;
  
  nomfic = "/" + String(t.year())+ String(t.month()) + String(t.day());
//  heure = String(t.hour()) + String(t.minute()) + String(t.second()) + char(13) + char(10);
  heure = String(t.hour()) + String(t.minute()) + String(personne) + char(13) + char(10);
  Serial.println(heure);
  if (SD.exists(nomfic.c_str()))
    appendFile(SD, nomfic.c_str(), heure.c_str());
  else
    writeFile(SD, nomfic.c_str(), heure.c_str());
};

///////////////////////////////////////////////////////////

void setup(){
  Serial.begin(115200);

  // led de controle power et passage
  pinMode(ledpower, OUTPUT);
  pinMode(ledpassage, OUTPUT);

  // rtc
  Wire.begin(); 
  DateTime t = rtc.now();
  Serial.println(String(t.year())+ "-"+String(t.month()) +"-"+ String(t.day())+"    "+ String(t.hour()) +":"+ String(t.minute()) +":"+ String(t.second()));

  //lidar
  tfmini.begin(&mySerial);    

  // carte sd et liste des fichiers sur la carte
  initSDCard();  //carte formatée en FAT32
  listDir(SD, "/", 0);  
  initVarHtml();  // preparation  page web

  // wifi
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP(); // generalement 192.168.4.1
  Serial.print("AP IP address: ");
  Serial.println(IP);
  // serveur
  //server.begin();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // index.html est dans /var/www/html/index.html
    // il contiendra toute le page css et html
    // avec mise à jour de la liste des fichiers obtenus avec dirlist
    //http://192.168.4.1/index.html
    writeFile(SD, "/index.html", stHTML.c_str());
    request->send(SD, "/index.html", "/text/)($html");
  });
  server.serveStatic("/", SD, "/"); 
  server.begin();
  
  digitalWrite(ledpower, HIGH);
  delay(500);
  digitalWrite(ledpassage, LOW);
  delay(200);
personne = 0;

};

///////////////////////////////////////////////////////////

void loop() {
  while (true) {
/*    //uint16_t dist = tfmini.getDistance();
    uint16_t dist = 0;
    
    if (dist < mini)
      mini = dist;
    if ((dist - mini >  seuil) && ((dist + seuil < maxi) || (dist0 + seuil < maxi)) && (nb > precedent + ecart))
    {  // on etait sur un mini
      // incremente le nombre de passage
      digitalWrite(ledpassage, HIGH);
      EcritFichier(); 
      personne++;
      //Serial.print("Nb Personnes :", personne, "  ecart distance :", dist - mini)
      mini= dist + seuil / 2;  //stoke le nouveau mini
      dist0 = dist; //memorise la distance
      precedent = nb; // memorise la derniere mesure significative
    };
    digitalWrite(ledpassage, LOW);
*/
    delay(1000);
/*
    personne++;
    Serial.print(" personne=");
    Serial.print(personne);
    if (DEBUG) EcritFichier();
    Serial.print(" dist=");
    Serial.println(dist);
*/
  }
}

