const async = require('async');
const app = require('./appinfo.js');
const Stations = require('./stations.js').Stations;
const utils = require('./utils.js');

let options = JSON.parse(localStorage.getItem('options'));

const xhrRequest = (url, type, callback) => {
  let xhr = new XMLHttpRequest();
  xhr.onload = () => {
      callback(xhr.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

let stations = null;

/* @desc Find the closest stations from your location.
 */
const find_closest_stations = () => {
    console.log('Finding closest station from the options : ' + JSON.stringify(options) + ' at ' + options.api_address);
    async.parallel([
        (callback) => {
            navigator.geolocation.getCurrentPosition(
                (pos) => {
                    callback(null, pos);
                },
                (err) => {
                    console.log('Error requesting location : ' + err);
                    callback(err, null);
                }, {
                    timeout: 15000, 
                    maximumAge: 60000
                }
            );
        }, (callback) => {
            xhrRequest(options.api_address, 'GET', 
                // TODO Error cheking
                (responseText) => {
                    
                    callback(null, responseText);
                });
        },
    ], (err, results) => {
        if (err) {
            console.log(err);
            Pebble.showSimpleNotificationOnPebble('Error', err);
            return;
        }

        console.log('received : ' + JSON.stringify(results[1]));

        const pos = results[0];
        const json = JSON.parse(results[1]);

        stations = new Stations(pos.coords);

        async.map(json['network']['stations'], (item, callback) => {
            stations.add(item);
            callback();
        }, (err, results) => {
            stations.send();
        });


    });
};

/* @desc Get the closest network from your location.
 */
const find_closest_network = () => {
    console.log('Looking for closest network');
    const network_adress = 'http://api.citybik.es/v2/networks';

    async.parallel([
        (callback) => {
            navigator.geolocation.getCurrentPosition(
                (pos) => {
                    callback(null, pos);
                },
                (err) => {
                    console.log('Error requesting location : ' + err);
                    callback(err, null);
                }, {
                    timeout: 15000, 
                    maximumAge: 60000
                }
            );
        }, (callback) => {
            console.log('Requesting ' + network_adress);

            xhrRequest(network_adress, 'GET', 
                // TODO Error cheking
                (responseText) => {
                    const json = JSON.parse(responseText);
                    callback(null, json);
                });
        },
    ], (err, results) => {
        if (err) {
            console.log(err);
            Pebble.showSimpleNotificationOnPebble('Error', err);
            return;
        }
        let min = {
            distance : 10000000,
        };

        let latitude = results[0].coords.latitude;
        let longitude = results[0].coords.longitude;
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

            find_closest_stations();
        });
    });
};

const get_location = () => {
    navigator.geolocation.getCurrentPosition(
        (pos) => {
            if (stations) {
                stations.update(pos.coords);
            } else {
            
            }
            return; 
        },
        (err) => {
            console.log('Error requesting location : ' + err);
            Pebble.showSimpleNotificationOnPebble('Error', err);
        },
        {timeout: 15000, maximumAge: 60000}
    );
};

Pebble.addEventListener('ready', 
    (e) => {
        console.log('PebbleKit JS ready!');

        if (!options || !options.api_address) {
            find_closest_network();        
        } else {
            find_closest_stations();
        }
    }
);

Pebble.addEventListener('appmessage',
    (e) => {
        console.log('AppMessage received!');

        console.log('RECEIVED : ' + JSON.stringify(e));

        switch (e.payload.KEY_COMMUNICATION) {
            case app.GET_UPDATED_LOCATION:
                console.log('Getting updated location.');
                get_location();
                break;
            case app.GET_STATIONS_UPDATE:
                console.log('Getting station update.');
                find_closest_stations();
                break;
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

        find_closest_stations();
    }
});
