let data = require('./data.js');

let calc_distance = (lat1, long1, lat2, long2) => {
    let radlat1 = Math.PI * lat1 / 180;
    let radlat2 = Math.PI * lat2 / 180;
    let radlon1 = Math.PI * long1 / 180;
    let radlon2 = Math.PI * long2 / 180;

    let theta = long1 - long2;
    let radtheta = ( Math.PI * theta ) / 180;

    let dist = ( Math.sin(radlat1) * Math.sin(radlat2) ) + ( Math.cos(radlat1) * Math.cos(radlat2) * Math.cos(radtheta) );
    dist = Math.acos(dist);
    dist = dist * 180 / Math.PI;
    dist = dist * 60 * 1.1515;

    return dist;
};

let sortByName = (a, b) => {
    let nameA = a.name.toLowerCase();
    let nameB = b.name.toLowerCase();
    if (nameA < nameB) {
        return -1;
    } else if (nameA > nameB) {
        return 1;
    } else {
        return 0;
    }
};

module.exports = {
    calc_distance : calc_distance,
        
    sortByName : sortByName,

    find_closest : () => {
        let results = [];
        navigator.geolocation.getCurrentPosition(function(pos) {
            let min = 1000000000;
            let minObj = {};
            let xhr = new XMLHttpRequest();

            // xhr.open('GET', 'http://api.citybik.es/v2/networks/');
            // xhr.onreadystatechange = function() {
            //     if (!(xhr.readyState == 4 && xhr.status == 200)) {
            //         console.log('xhr.status : ' + xhr.status);
            //         return;
            //     }

            //     let json = JSON.parse(xhr.responseText);

            //     for (let i = 0; i < json['networks'].length; ++i) {
            //         let dist = this.calc_distance(json['networks'][i]['location']['latitude'],
            //                 json['networks'][i]['location']['longitude'],
            //                 pos.coords.latitude,
            //                 pos.coords.longitude);
            //         if (dist < min) {
            //             min = dist;
            //             minObj = {
            //                 'name' : json['networks'][i]['name'],
            //                 'url' : 'http://api.citybik.es/' + json['networks'][i]['href'],
            //             };
            //         }

            //         results.push({
            //             'name' : json['networks'][i]['name'],
            //             'url' : 'http://api.citybik.es/' + json['networks'][i]['href'],
            //         });
            //     }

            //     console.log('Min is ' + JSON.stringify(minObj));

            //     results.sort(this.sortByName);

            //     let select = document.getElementById('stationList');

            //     let option = document.createElement('option');
            //     option.appendChild( document.createTextNode(minObj.name) );
            //     option.value = minObj.url;
            //     select.add(option);

            //     for (var i = 0; i < results.length; ++i) {
            //         option = document.createElement('option');
            //         option.appendChild( document.createTextNode(results[i].name) );
            //         option.value = results[i].url;
            //         select.add(option);
            //     }
            // };
            // xhr.send();

            for (let i = 0; i < data['networks'].length; ++i) {
                let dist = calc_distance(data['networks'][i]['location']['latitude'],
                        data['networks'][i]['location']['longitude'],
                        pos.coords.latitude,
                        pos.coords.longitude);
                if (dist < min) {
                    min = dist;
                    minObj = {
                        'name' : data['networks'][i]['name'],
                        'url' : 'http://api.citybik.es/' + data['networks'][i]['href'],
                    };
                }

                results.push({
                    'name' : data['networks'][i]['name'],
                    'url' : 'http://api.citybik.es/' + data['networks'][i]['href'],
                });
            }

            console.log('Min is ' + JSON.stringify(minObj));

            results.sort(sortByName);

            let select = document.getElementById('stationList');

            let option = document.createElement('option');
            option.appendChild( document.createTextNode(minObj.name) );
            option.value = minObj.url;
            select.add(option);

            for (var i = 0; i < results.length; ++i) {
                option = document.createElement('option');
                option.appendChild( document.createTextNode(results[i].name) );
                option.value = results[i].url;
                select.add(option);
            }
        }, function (err) {
            console.log(err);
        });
    },
};
