var mainData = {
    "teplota": 23.5, // degrees (Celsius)
    "vitr-smer": "NE", // degrees
    "pocitova-teplota": 19, // degrees (Celsius)
    "vitr-rychlost": 21, // m/s
    // "srazky": 3.3, // mm/m3
    "vlhkost": 2.6, // percentage
    "bourka": 37.5, // km
    "ppm": 200, // ?
    "graf-teplota": [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24]
}

function fetchDatabase() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/database", true);
    xhr.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            let loadedDatabase = xhr.responseText.split("|");
            let parsedData = [];
            loadedDatabase.reverse();
            for (let i = 0; i < loadedDatabase.length; i++) {
                if (loadedDatabase[i] != '' && parsedData.length < 24) {
                    let getTime = loadedDatabase[i].split(" ");
                    getTime = getTime[1].split("@");
                    let temperature = getTime[1];
                    getTime = getTime[0].split(":");
                    if (getTime[1] == "00") {
                        // console.log(temperature);
                        parsedData.push(temperature);
                    }
                }
            }
            parsedData.reverse();
            mainData["graf-teplota"] = parsedData;
            reload();
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
    for (let i = 0; i < mainData["graf-teplota"].length; i++) {
        dataToInsert += `<span class="graph-tag">${mainData['graf-teplota'][i]}</span>`;
        if (mainData['graf-teplota'][i] < 0) {
            dataToInsert += `<span class="graph-item" style="height: ${mainData['graf-teplota'][i] * -1}vh; background-color: red;"></span>`;
        } else {
            dataToInsert += `<span class="graph-item" style="height: ${mainData['graf-teplota'][i]}vh; background-color: blue;"></span>`;
        }

        document.getElementById("graf-teplota").innerHTML = dataToInsert;
    }
}

function reload() {
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