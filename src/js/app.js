const async = require('async');
const app = require('./appinfo.js');
const Stations = require('./stations.js').Stations;
const Station = require('./station.js').Station;
const utils = require('./utils.js');

const request = (address, callback) => {
    const xhr = new XMLHttpRequest();
    xhr.onload = () => {
        callback(null, null, xhr.responseText);
    };
    xhr.open('GET', address);
    xhr.send();
};

let options = JSON.parse(localStorage.getItem('options'));

let stations = null;

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

const getNetworks = (callback) => {
    const networks_adress = 'http://api.citybik.es/v2/networks';
    request(networks_adress, (err, response, body) => {
        if (err) {
            Pebble.showSimpleNotificationOnPebble('Error : Getting networks.', err);
            callback(err);
            return;
        }
    
        callback(null, JSON.parse(body));
    });
};

const getApiNetwork = (callback) => {
    request(options.api_address, (err, response, body) => {
        if (err) {
            Pebble.showSimpleNotificationOnPebble('Error : Getting API network.', err);
            callback(err);
            return;
        }
    
        callback(null, JSON.parse(body)['network']['stations']);
    });
};

/* @desc Find the closest stations from your location.
 */
const findClosestStations = () => {
    console.log('Finding closest station from the options : ' + JSON.stringify(options) + ' at ' + options.api_address);
    async.parallel([
        getLocation, 
        getApiNetwork,
    ], (err, results) => {
        if (err) {
            console.log(err);
            return;
        }

        const location_result = results[0];
        const tmp_stations = new Stations(location_result);

        console.log('API result : ' + JSON.stringify(results[1]));
        const api_result = results[1];
        async.map(api_result, (item, callback) => {
            tmp_stations.add(new Station(item));
            callback();
        }, (err, results) => {
          tmp_stations.send(() => {
            stations = tmp_stations;
          });
        });
    });
};

/* @desc Get the closest network from your location.
 */
const findClosestNetwork = () => {
    console.log('Looking for closest network');
    const network_adress = 'http://api.citybik.es/v2/networks';

    async.parallel([
        getLocation,
        getNetworks,
    ], (err, results) => {
        if (err) {
            console.log(err);
            Pebble.showSimpleNotificationOnPebble('Error', err);
            return;
        }

        let min = {
            distance : Infinity,
        };

        const latitude = results[0].latitude;
        const longitude = results[0].longitude;
        async.map(results[1].networks, (network, callback) => {
            network.distance = utils.calc_distance(latitude, longitude, 
                network.location.latitude, network.location.longitude);
            if (network.distance < min.distance) {
                min = network; 
            }
            callback(null, null);
        }, (err, results) => {
            if (err) {
                console.log(err);            
                Pebble.showSimpleNotificationOnPebble('Error', err);
                return;
            }
            console.log('The closest network is : ' + min.name + ' and href=' + min.href);
            const new_options = { api_address : 'http://api.citybik.es' + min.href, };
            localStorage.setItem('options', JSON.stringify(new_options));
            options = new_options;

            findClosestStations();
        });
    });
};

Pebble.addEventListener('ready', 
    (e) => {
        console.log('PebbleKit JS ready!');

        if (!options || !options.api_address) {
            findClosestNetwork();        
        } else {
            findClosestStations();
        }
    }
);

Pebble.addEventListener('appmessage',
    (e) => {
        switch (e.payload.KEY_COMMUNICATION) {
            case app.GET_UPDATED_LOCATION: {
                console.log('RECEIVED : Getting updated location.');
                getLocation((err, coords) => {
                  if (stations) {
                    stations.updateWithLocation(coords);
                  }
                });
                break;
            }
            case app.GET_STATIONS_UPDATE: {
                console.log('RECEIVED : Getting station update.');
                getApiNetwork((err, result) => {
                    if (stations) {
                        stations.updateWithAPI(result);
                    }
                });
                break;
            }
            case app.GET_ADD_STATIONS: {
                console.log('RECEIVED : Adding more station to the response.');
                if (stations) {
                    stations.addMore(); 
                }
                break;
            }
        }
    }                     
);

Pebble.addEventListener('showConfiguration', () => {
    const url = 'https://rawgit.com/thomacer/pebble-villo/master/config/index.html';

    Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', (e) => {
    // Decode the user's preferences
    if (e.response) {
        options = JSON.parse(decodeURIComponent(e.response));
        console.log('Received : ' + JSON.stringify(options));
        localStorage.setItem('options', JSON.stringify(options)); 

        findClosestStations();
    }
});
