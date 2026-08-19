// ========================================================
//   SYSTEM
// ========================================================

async function updateSystem() {

    try {

        const res = await fetch('/api/system');
        const sys = await res.json();


        document.getElementById("system").innerHTML =
        `
        ⚡ CPU: ${sys.cpu} MHz<br>
        🧠 RAM: ${sys.ram} KB<br>
        💾 Flash: ${sys.flash} KB<br>
        ⏱ Uptime: ${sys.uptime} sec
        `;


    }
    catch(e)
    {
        console.log(e);
    }

}



// ========================================================
//   NETWORK
// ========================================================

async function updateNetwork(){

    try{

        const res = await fetch('/api/network');
        const net = await res.json();


        document.getElementById("network").innerHTML =
        `
        📶 SSID: ${net.ssid}<br>
        🌐 IP: ${net.ip}<br>
        📡 Signal: ${net.rssi} dBm
        `;


    }
    catch(e)
    {
        console.log(e);
    }

}



// ========================================================
//   FILES
// ========================================================

async function updateFiles(){

try{

    const res = await fetch('/api/files');
    const files = await res.json();



    let mediaHTML = "";



    files.forEach(file=>{


        if(file.type=="image" || file.type=="gif")
        {


            let icon =
            file.type=="gif" ? "🎞" : "🖼";



            mediaHTML +=
            `
            <div class="file">

            ${icon}
            <b>${file.name}</b>

            <br>

            💾 ${formatSize(file.size)}

            <br>


            <button onclick="playMedia('${file.path}')">
            ▶ Play
            </button>


            <button onclick="deleteFile('${file.path}')">
            🗑 Delete
            </button>


            </div>
            `;

        }


    });



    document.getElementById("mediaList").innerHTML =
    mediaHTML;




    let systemHTML="";


    files.forEach(file=>{


        if(file.type=="other")
        {


            systemHTML +=
            `
            <div class="file">

            📄 ${file.name}

            <br>

            💾 ${formatSize(file.size)}

            </div>
            `;

        }


    });



    document.getElementById("files").innerHTML =
    systemHTML;



    const storage =
    document.getElementById("storage");


    if(storage)
    {

        let total =
        files.reduce(
            (sum,f)=>sum+f.size,
            0
        );


        storage.textContent =
        formatSize(total);

    }



}
catch(e)
{
    console.log(e);
}


}




// ========================================================
//   FORMAT SIZE
// ========================================================

function formatSize(size)
{

    if(size > 1024*1024)
        return (size/1024/1024).toFixed(2)+" MB";


    if(size > 1024)
        return (size/1024).toFixed(1)+" KB";


    return size+" B";

}



// ========================================================
//   MEDIA CONTROL
// ========================================================


async function playMedia(path)
{


    console.log(
        "PLAY:",
        path
    );



    try
    {

        const res =
        await fetch(
            "/api/media/play?file="
            +
            encodeURIComponent(path)
        );



        const data =
        await res.json();



        console.log(data);


    }
    catch(e)
    {

        console.log(e);

    }


}



async function stopMedia()
{

    await fetch(
        "/api/media/stop"
    );

}



// ========================================================
//   UPLOAD
// ========================================================

async function uploadFile()
{

    const input =
    document.getElementById(
        "uploadFile"
    );


    if(!input.files.length)
    {

        alert(
            "Choose a file first"
        );

        return;

    }



    const form =
    new FormData();


    form.append(
        "file",
        input.files[0]
    );



    const res =
    await fetch(
        "/api/upload",
        {
            method:"POST",
            body:form
        }
    );



    if(res.ok)
    {

        alert(
            "Upload successful!"
        );


        updateFiles();

    }
    else
    {

        alert(
            "Upload failed!"
        );

    }

}



// ========================================================
//   DELETE
// ========================================================

async function deleteFile(path)
{


    if(!confirm("Delete this file?"))
        return;



    await fetch(
        "/api/delete?file="
        +
        encodeURIComponent(path)
    );



    updateFiles();

}



// ========================================================
//   CONTROL
// ========================================================


function led()
{
    fetch("/api/led");
}



function restart()
{

    if(confirm("Restart ESP32?"))
    {

        fetch(
            "/api/restart"
        );

    }

}



// ========================================================
//   UPDATE LOOP
// ========================================================


function updateAll()
{

    updateSystem();

    updateNetwork();

    updateFiles();

}



setInterval(
    updateAll,
    5000
);



updateAll();