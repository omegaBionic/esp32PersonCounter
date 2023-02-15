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
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "FS.h" 
#include "SD.h"
#include "SPI.h"

String stDeb; 
String stFin;
String stList;
String stHTML;

void initVarHtml()
{
  // debut du fichier index.html
  stDeb += "<!DOCTYPE html>\n";
  stDeb += "<html>\n";
  stDeb += "<meta charset='utf-8'>\n";
  stDeb += "  <head>\n";
  stDeb += "    <!-- CSS -> Style -->\n";
  stDeb += "    <style media='screen'>\n";
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
  stDeb += "    <!-- Javascript -> Logic -->\n";
  stDeb += "    <script type='text/javascript>\n";
  stDeb += "      function removeLastDirectoryPartOf(the_url) {\n";
  stDeb += "        var the_arr = the_url.split('/');\n";
  stDeb += "        the_arr.pop();\n";
  stDeb += "        return the_arr.join('/');\n";
  stDeb += "      }\n";
  stDeb += "      function download(dataurl, filename) {\n";
  stDeb += "        const link = document.createElement('a');\n";
  stDeb += "        link.href = dataurl;\n";
  stDeb += "        link.download = filename;\n";
  stDeb += "        link.click();\n";
  stDeb += "      }\n";
  stDeb += "      function checkAndUncheck() {\n";
          // Define variables
  stDeb += "        var isChecked = false;\n";
  stDeb += "        var inputs = document.getElementsByTagName('input');\n";
  stDeb += "        var buttonCheckAll = document.getElementById('buttonCheckAll');\n";
          // Get if there is at least input checked
  stDeb += "        for (var i = 0; i < inputs.length; ++i) {\n";
  stDeb += "          if (inputs[i].checked === true) {\n";
  stDeb += "            isChecked = true;\n";
  stDeb += "            break;\n";
  stDeb += "          }\n";
  stDeb += "        }\n";
          // Check or unckeck input
  stDeb += "        for (var i = 0; i < inputs.length; ++i) {\n";
  stDeb += "          inputs[i].checked = !isChecked;\n";
  stDeb += "        }\n";
          // Update buttonCheckAll label
  stDeb += "        if (isChecked) {\n";
  stDeb += "          buttonCheckAll.value = 'Tout cocher';\n";
  stDeb += "        } else {\n";
  stDeb += "          buttonCheckAll.value = 'Tout decocher';\n";
  stDeb += "        }\n";
  stDeb += "      }\n";
  stDeb += "      function downloadAllItems() {\n";
          // Define variables
  stDeb += "        var concatenateUrlForDownload = "";\n";
  stDeb += "        var inputs = document.getElementsByTagName('input');\n";
          // Define const
  stDeb += "        const hostUrl = window.location.href;\n";
          // Download all checked items
  stDeb += "        for (var i = 0; i < inputs.length; ++i) {\n";
            // if isChecked and no check button
  stDeb += "          if (inputs[i].checked && inputs[i].id != 'buttonCheckAll') {\n";
              // Debug for display inputs id
  stDeb += "            console.log(''inputs[i].id: '' + inputs[i].id + ''');\n";
              // Concatenate url from pwd
  stDeb += "            concatenateUrlForDownload =\n";
  stDeb += "              removeLastDirectoryPartOf(hostUrl) + '/' + inputs[i].id;\n";
  stDeb += "            console.log(\n";
  stDeb += "              'concatenateUrlForDownload: '' + concatenateUrlForDownload + ''\n";
  stDeb += "            );\n";
              // Download files
              // TODO
              //downloadURI(concatenateUrlForDownload, inputs[i].id);
  stDeb += "            download(concatenateUrlForDownload, 'file4.txt');\n";
  stDeb += "          }\n";
  stDeb += "        }\n";
          // Debug display full object
          //console.log(inputs);
  stDeb += "      }\n";
  stDeb += "    </script>\n";
  stDeb += "  </head>\n";
  

  stDeb += "  <!-- HTML -> Struct -->\n";
  stDeb += "  <body>\n";
  stDeb += "    <div>\n";
  stDeb += "      <div>\n";
  stDeb += "        <h1 class='center'>Serveur de fichiers</h1>\n";
  stDeb += "      </div>\n";
  stDeb += "      <div>\n";
  stDeb += "        <h3>😼 FabLab made in Iki 😼</h3>\n";
  stDeb += "      </div>\n";
  stDeb += "      <br />\n";
  stDeb += "    </div>\n";
  stDeb += "    <div>\n";
  stDeb += "      <div>\n";
  stDeb += "        <table class='center'>\n";
  stDeb += "          <thead>\n";
  stDeb += "            <tr>\n";
  stDeb += "              <th colspan='2'>Documents disponibles</th>\n";
  stDeb += "            </tr>\n";
  stDeb += "          </thead>\n";
  stDeb += "          <tbody>\n";

  stFin += "          </tbody>\n";
  stFin += "        </table>\n";
  stFin += "      </div>\n";
  stFin += "      <div class='center'>\n";
  stFin += "        <br />\n";
  stFin += "        <input\n";
  stFin += "          onclick='checkAndUncheck()'\n";
  stFin += "          type='button'\n";
  stFin += "          value='Tout cocher'\n";
  stFin += "          id='buttonCheckAll'\n";
  stFin += "        />\n";
  stFin += "        <button onclick='downloadAllItems()' type='submit'>\n";
  stFin += "          Tout télécharger\n";
  stFin += "        </button>\n";
  stFin += "      </div>\n";
  stFin += "    </div>\n";
  stFin += "  </body>\n";
  stFin += "</html>\n";

  stHTML = stDeb + stList + stFin;
Serial.println("*****************");
Serial.println(stHTML.c_str());
Serial.println("*****************");
};





// Functions
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
  String st = "";
  stList = ""; 
  Serial.println("debut listdir");
  Serial.printf("Listing directory: %s\n", dirname);

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
      st = '"'+String(file.name()).substring(1) + '"'+ "type='checkbox' /></td>";
      stList += "<tr><td><input id=" + st;
      st = String(file.name()).substring(1);
      stList += "<td>"+st+"</td></tr>\n";
    }
    file = root.openNextFile();
  }
  Serial.println("stList");
}

void createDir(fs::FS &fs, const char * path){
  Serial.printf("Creating Dir: %s\n", path);
  if(fs.mkdir(path)){
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char * path){
  Serial.printf("Removing Dir: %s\n", path);
  if(fs.rmdir(path)){
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
}

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
}

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
}

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
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char * path){
  Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

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

// Replace with your network credentials
const char* ssid = "Realme";
const char* password = "0952061100";

// Create AsyncWebServer object on port 80 (http) ou 443 (https)
AsyncWebServer server(80);

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
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void setup(){
  Serial.begin(115200);
  initWiFi();
  initSDCard();
  Serial.println("avant listDir");
  listDir(SD, "/", 0);
  Serial.println("apres listDir");
  initVarHtml();
  Serial.println("apres initVarHTML");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // index.html est dans /var/www/html/index.html
    // il contiendra toute le page css et html
    // avec avant mise à jour de la liste des fichiers obtenus avec dirlist
  Serial.println("avant WriteFile");
    writeFile(SD, "/index.html", stHTML.c_str());
  Serial.println("apres WriteFile");
    request->send(SD, "/index.html", "/text/)($html");
  });

  server.serveStatic("/", SD, "/");

  server.begin();

  
}

void loop() {
  
}
