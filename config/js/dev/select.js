const async = require('async');

const request = (address, callback) => {
    const xhr = new XMLHttpRequest();
    xhr.onload = () => {
        callback(null, null, xhr.responseText);
    };
    xhr.open('GET', address);
    xhr.send();
};

const getNetworks = (callback) => {
    const networks_adress = 'https://api.citybik.es/v2/networks';
    request(networks_adress, (err, response, body) => {                                                                                                                        
        if (err) {
            callback(err);
            return;
        }

        callback(null, JSON.parse(body));
    });
};

const getLocation = (callback) => {
    navigator.geolocation.getCurrentPosition(
        (pos) => {                                                                                                                                                             
            callback(null, pos.coords);
        }, (err) => {
            console.log('Error requesting location : ' + err);
            Pebble.showSimpleNotificationOnPebble('Error : Geolocation.', err);
            callback(err);
        }, {
          timeout: 15000,
          maximumAge: 60000
        }
    );
};

const calc_distance = (lat1, long1, lat2, long2) => {
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

/* @desc : Sort objects from the networks list of the API by name.
 */
const sortByName = (a, b) => {
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

/* @desc 
 *
 * @param {select} : Reference to the "select" tag.
 * @param {networks} : 
 */
const populateSelect = (select, networks) => {
    async.map(networks, (item, callback) => {
        const option = document.createElement('option');
        option.appendChild( document.createTextNode(item.name + ' (' + item.location.city + ')') );
        option.value = 'https://api.citybik.es' + item.url;
        select.add(option);
    }, (err, res) => {
        if (res) {
            res[0].selected = true;
        }
    });
};

const calcDistanceForEachNetworks = (networks, coords, callback) => {
    async.map(networks, (item, callback) => {
        item.distance = calc_distance(coords.latitude, coords.longitude,
            item.location.latitude, item.location.longitude);
        callback();
    }, callback);
};

module.exports.populateSelectByDistance = (selectID) => {
    const results = [];
    async.parallel([
        getLocation,
        getNetworks,
    ], (err, res) => {
        const coords = res[0];
        const networks = res[1].networks;

        calcDistanceForEachNetworks(networks, coords, (err, res) => {
            networks.sort((a, b) => {
                if (a.distance < b.distance) {
                    return -1;
                } else if (a.distance > b.distance) {
                    return 1;
                } else {
                    return 0;
                }
            });

            populateSelect(document.getElementById(selectID), networks);
        });
    });
};

module.exports.getCurrentValue = (selectID) => {
    const select = document.getElementById(selectID);
    return select.options[select.selectedIndex].value;
};
