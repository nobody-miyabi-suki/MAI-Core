// ==================== script.js ====================
// SYSTEM
async function updateSystem() {
  try {
    const res = await fetch('/api/system');
    const sys = await res.json();
    document.getElementById('system').innerHTML = `
      ⚡ CPU: ${sys.cpu} MHz<br>
      🧠 RAM: ${sys.ram} KB<br>
      💾 Flash: ${sys.flash} KB<br>
      ⏱ Uptime: ${sys.uptime} sec
    `;
  } catch (e) {
    console.log(e);
  }
}

// NETWORK
async function updateNetwork() {
  try {
    const res = await fetch('/api/network');
    const net = await res.json();
    document.getElementById('network').innerHTML = `
      📶 SSID: ${net.ssid}<br>
      🌐 WiFi: ${net.wifi_ip}<br>
      📡 AP: ${net.hotspot_ip}
    `;
  } catch (e) {
    console.log(e);
  }
}

// MEDIA FILES
async function updateFiles() {
  try {
    const res = await fetch('/api/files');
    const files = await res.json();
    let html = '';
    files.forEach(file => {
      html += `
        <div class="file">
          🎞 <b>${file.name}</b><br>
          💾 ${formatSize(file.size)}
          <button onclick="playMedia('${file.name}')">▶ Play</button>
          <button onclick="deleteFile('${file.name}')">🗑 Delete</button>
        </div>
      `;
    });
    document.getElementById('files').innerHTML = html;
  } catch (e) {
    console.log(e);
  }
}

function formatSize(size) {
  if (size > 1024 * 1024) return (size / 1024 / 1024).toFixed(2) + ' MB';
  if (size > 1024) return (size / 1024).toFixed(1) + ' KB';
  return size + ' B';
}

async function playMedia(file) {
  const path = file.includes('gifs')
    ? '/media/gifs/' + file.split('/').pop()
    : '/media/images/' + file.split('/').pop();
  await fetch('/api/play?file=' + encodeURIComponent(path));
  console.log('Playing:', path);
}

// UPLOAD
async function uploadFile() {
  const input = document.getElementById('uploadFile');
  if (!input.files.length) return alert('Choose file');
  const form = new FormData();
  form.append('file', input.files[0]);
  const res = await fetch('/api/upload', { method: 'POST', body: form });
  if (res.ok) {
    alert('Uploaded');
    updateFiles();
  }
}

// DELETE
async function deleteFile(name) {
  if (!confirm('Delete ' + name)) return;
  await fetch('/api/delete?file=' + encodeURIComponent(name));
  updateFiles();
}

// LED
function led() {
  fetch('/api/led');
}

// RESTART
function restart() {
  if (confirm('Restart ESP32?')) fetch('/api/restart');
}

// PAUSE / RESUME (برای GIF)
function pauseGIF() {
  fetch('/api/pause');
}

function resumeGIF() {
  fetch('/api/resume');
}

// LOOP
function update() {
  updateSystem();
  updateNetwork();
  updateFiles();
}

setInterval(update, 3000);
update();