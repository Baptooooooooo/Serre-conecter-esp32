// Définition de la fonction fetchData()
const socket = new WebSocket("ws://45.145.167.182:81");

// Fonction exécutée lorsque la connexion WebSocket est établie avec succès
socket.onopen = function (event) {
  console.log("WebSocket connected");
};

// Fonction exécutée lorsqu'un message est reçu du serveur WebSocket
socket.onmessage = function (event) {
  // Convertir la chaîne JSON en un objet JavaScript
  const data = JSON.parse(event.data);
  if (
    data.arosage == "on" ||
    data.arosage == "off" ||
    data.feunètre == "on" ||
    data.feunètre == "off"
  ) {
  } else {
    const temperatureString = String(data.temperature);

    // Récupérer uniquement les chiffres et le point décimal de la température
    const temperatureMatches = temperatureString.match(/[0-9.]+/);

    // Limiter la longueur à 4 caractères
    const limitedTemperature = temperatureMatches
      ? temperatureMatches[0].substring(0, 4)
      : "";

    console.log(limitedTemperature); // Affiche les 4 premiers caractères de la température

    const humidity = data.humidity;
    const light = data.light;

    const temperatureDiv = document.getElementById("temperatureDiv");
    const humidityDiv = document.getElementById("humidityDiv");
    const lightDiv = document.getElementById("lightDiv");

    // Mise à jour des valeurs sur la page HTML
    temperatureDiv.textContent = `${limitedTemperature}°C`;
    humidityDiv.textContent = `${humidity}%`;
    lightDiv.textContent = `${light}%`;

    Light(light);
    updateWater(humidity);
    updateTemperature(limitedTemperature);
  }
};

// Fonction exécutée lorsqu'une erreur se produit lors de la connexion WebSocket
socket.onerror = function (error) {
  console.error("WebSocket error:", error);
};

// Fonction exécutée lorsque la connexion WebSocket est fermée
socket.onclose = function (event) {
  console.log("WebSocket closed");
};

socket.onopen = function (event) {
  tuesla();
  console.log("oui ");
};

function onarosage() {
  // Création de l'objet avec les valeurs à envoyer
  const message = {
    arosage: "on",
  };
  sendmeasage(message);
}
function offarosage() {
  // Création de l'objet avec les valeurs à envoyer
  const message = {
    arosage: "off",
  };
  sendmeasage(message);
}
function onfeunètre() {
  // Création de l'objet avec les valeurs à envoyer
  const message = {
    feunètre: "on",
  };
  sendmeasage(message);
}
function offfeunètre() {
  // Création de l'objet avec les valeurs à envoyer
  const message = {
    feunètre: "off",
  };
  sendmeasage(message);
}
function tuesla() {
  const message = {
    tuesla: true,
  };
  console.warn("pas de conextion");
  sendmeasage(message);
}
function setcolor() {
  var pickerColor = document.getElementById("piker").value;

  // Convertir la valeur hexadécimale en RGB
  var rgbColor = hexToRgb(pickerColor);

  // Afficher la couleur en RGB dans la console
  console.log(rgbColor[0], rgbColor[1], rgbColor[2]);
  const message = {
    color: [rgbColor[0], rgbColor[1], rgbColor[2]],
  };
  sendmeasage(message);
}

function hexToRgb(hex) {
  // Supprimer le caractère # s'il est présent
  hex = hex.replace(/^#/, "");

  // Convertir les parties rouge, verte et bleue
  var bigint = parseInt(hex, 16);
  var r = (bigint >> 16) & 255;
  var g = (bigint >> 8) & 255;
  var b = bigint & 255;

  r = 255 - r;
  g = 255 - g;
  b = 255 - b;

  return [r, g, b];
}

function sendmeasage(message) {
  // Convertir l'objet en chaîne JSON
  const messageJSON = JSON.stringify(message);
  console.log("Données à envoyer :", messageJSON); // Message de débogage
  if (socket.readyState === WebSocket.OPEN) {
    // Envoi de données sur la connexion WebSocket
    socket.send(messageJSON);
  } else {
    console.error("La connexion WebSocket n'est pas ouverte");
  }
}

// Exemple d'utilisation de la fonction pour envoyer un message avec deux valeurs

// Fonction pour mettre à jour l'état de la lumière
function Light(lightValue) {
  const sun = document.querySelector(".lumiere"); // Correction ici
  if (lightValue <= 20) {
    sun.classList.add("active");
  } else {
    sun.classList.remove("active");
  }
}

// Fonction pour mettre à jour l'état de la température
function updateTemperature(temperatureValue) {
  const thermometer = document.querySelector(".temperature");
  if (temperatureValue >= 25) {
    thermometer.classList.remove("froit");
    thermometer.classList.add("chaut");
  } else {
    if (temperatureValue <= 5) {
      thermometer.classList.remove("chaut");
      thermometer.classList.add("froid");
    } else {
      thermometer.classList.remove("chaut");
      thermometer.classList.remove("froid");
    }
  }
}
// Fonction pour mettre à jour l'état de l'eau
function updateWater(humidityValue) {
  const water = document.querySelector(".humiditer");
  if (humidityValue >= 50) {
    water.classList.add("active");
  } else {
    water.classList.remove("active");
  }
}

document.addEventListener("DOMContentLoaded", function () {
  function duplicateGoute() {
    const eau = document.querySelector(".eau");
    const gouteOriginal = eau.querySelector(".goute");
    const nombreDeClones = 5; // Nombre de clones à créer

    for (let i = 0; i < nombreDeClones; i++) {
      const gouteCopie = gouteOriginal.cloneNode(true);
      gouteCopie.style.left = Math.random() * 100 + "vw";
      eau.appendChild(gouteCopie);
    }
  }

  setInterval(duplicateGoute, 1000); // Appel de la fonction pour dupliquer les gouttes à intervalles réguliers
});
