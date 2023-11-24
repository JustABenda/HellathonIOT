var mainData = {
    "teplota": 23.5, // degrees (Celsius)
    "vitr-smer": "NE", // degrees
    "pocitova-teplota": 19, // degrees (Celsius)
    "vitr-rychlost": 21, // m/s
    // "srazky": 3.3, // mm/m3
    "vlhkost": 2.6, // percentage
    "bourka": 37.5, // km
    "ppm": 200, // ?
    "graf-teplota": [
        "poslední čas měření - z toho výpočet do 24 hodin dozadu",
        [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24]
    ]
}

function fetchDatabase() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/database", true);
    xhr.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            //mainData = JSON.parse(xhr.responseText);
            console.log(xhr.responseText);
        }
    };
    
    xhr.send();
}

function reloadData(dataKey, textBefore, textAfter) {
    document.getElementById(dataKey).innerHTML = `<div class="text-before">${textBefore}</div>` +
        `<div class="text-content">${mainData[dataKey]} <span class="text-after">${textAfter}</span></div>`;
}

function reloadGraph() {
    let dataToInsert = "";
    for (let i = 0; i < mainData["graf-teplota"][1].length; i++) {
        dataToInsert += `<span class="graph-tag">${mainData['graf-teplota'][1][i]}</span>`;
        if (mainData['graf-teplota'][1][i] < 0) {
            dataToInsert += `<span class="graph-item" style="height: ${mainData['graf-teplota'][1][i] * -1}vh; background-color: red;"></span>`;
        } else {
            dataToInsert += `<span class="graph-item" style="height: ${mainData['graf-teplota'][1][i]}vh; background-color: blue;"></span>`;
        }

        document.getElementById("graf-teplota").innerHTML = dataToInsert;
    }
}

function reload() {
    fetchDatabase();
    reloadData("teplota", "", "°C");
    reloadData("vitr-smer", "Směr větru", "");
    reloadData("pocitova-teplota", "Pocitová teplota", "°C");
    reloadData("vitr-rychlost", "Rychlost větru", "m/s");
    // reloadData("srazky", "Srážky", "mm/m&sup2;");
    reloadData("vlhkost", "Vlhkost", "%");
    reloadData("bourka", "Vzdálenost bouřky", "km");
    reloadData("ppm", "Znečištění ovzduší", "ppm");
    reloadGraph();
}