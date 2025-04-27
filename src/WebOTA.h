#ifndef WEBOTA_H
#define WEBOTA_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>

class WebOTAUpdater {
private:
    WebServer *server;
    String hostname;
    bool debug;
    const char* updateIndex = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
   "<input type='submit' value='Update'>"
"</form>"
"<div id='prg'>progress: 0%</div>"
"<script>"
   "$('form').submit(function(e){"
      "e.preventDefault();"
      "var form = $('#upload_form')[0];"
      "var data = new FormData(form);"
      "$.ajax({"
         "url: '/update',"
         "type: 'POST',"
         "data: data,"
         "contentType: false,"
         "processData:false,"
         "xhr: function() {"
            "var xhr = new window.XMLHttpRequest();"
            "xhr.upload.addEventListener('progress', function(evt) {"
               "if (evt.lengthComputable) {"
                  "var per = evt.loaded / evt.total;"
                  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
               "}"
            "}, false);"
            "return xhr;"
         "},"
         "success:function(d, s) {"
            "console.log('success!')" 
            "$('#prg').html('Upload complete. Rebooting...');"
         "},"
         "error: function (a, b, c) {"
            "console.log('error!')"
         "}"
      "});"
   "});"
"</script>";
    
public:
    WebOTAUpdater(const String &hostname = "esp32-ota", bool debug = false);
    ~WebOTAUpdater();
    void begin();
    void handle();
};

#endif // WEBOTA_H 