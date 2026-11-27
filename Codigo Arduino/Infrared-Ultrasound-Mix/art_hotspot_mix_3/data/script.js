let devices = new Map();
devices.set('infrarroja',  {"status":false, "ip":"192.168.1.3",  "name":"Luz infrarroja", "on":"Activada",  "off":"Desactivada"});
devices.set('ultrasonido', {"status":false, "ip":"192.168.1.10", "name":"Ultrasonido",    "on":"Activado",  "off":"Desactivado"});

function toggleDeviceStatus(stimuliType) {
  if (devices.has(stimuliType)) {
    let device = devices.get(stimuliType);
    device.status = !device.status;
    devices.set(stimuliType, device);

    if (device.status) {
      fetch("http://" + device.ip + "/on").catch(error => console.log('Error:', error));
    } else {
      fetch("http://" + device.ip + "/off").catch(error => console.log('Error:', error));
    }

    let card = document.getElementById(stimuliType);
    if (card) {
      card.classList.toggle('on', device.status);
      card.querySelector('.row').setAttribute('aria-pressed', device.status ? 'true' : 'false');
      card.querySelector('.row-state').textContent = device.status ? device.on : device.off;
    }

    let panel = document.getElementById('statusPanel');
    let line = document.createElement('div');
    line.textContent = new Date().toLocaleTimeString() + "  " + device.name + ": " + (device.status ? device.on.toLowerCase() : device.off.toLowerCase());
    panel.appendChild(line);
    panel.scrollTop = panel.scrollHeight;
  }
}