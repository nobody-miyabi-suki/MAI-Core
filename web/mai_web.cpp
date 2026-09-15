#include "mai_web.h"


MaiWeb web;



void MaiWeb::begin()
{

    server.on("/", [this]()
    {

        server.send(
            200,
            "text/html",
            R"rawliteral(

<!DOCTYPE html>
<html>

<head>
<title>MAI Core</title>

<style>

body{
background:#111;
color:white;
font-family:Arial;
text-align:center;
}

.box{
margin:40px;
padding:20px;
border-radius:15px;
background:#222;
}

</style>

</head>


<body>

<div class="box">

<h1>MAI Core v2</h1>

<p>Status: ONLINE</p>

<p>ESP32 Connected</p>

</div>


</body>

</html>

)rawliteral"
        );


    });



    server.begin();


    Serial.println(
        "WEB SERVER READY"
    );

}



void MaiWeb::handle()
{

    server.handleClient();

}