const async = require('async');
const app = require('./appinfo.js');
const Stations = require('./stations.js').Stations;

let options = JSON.parse(localStorage.getItem('options'));
if (!options || !options.api_address) {
    options = { api_address : 'http://api.citybik.es/v2/networks/villo' };
}

const xhrRequest = (url, type, callback) => {
  let xhr = new XMLHttpRequest();
  xhr.onload = () => {
      callback(xhr.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

let stations = null;

const get_stations = () => {
    navigator.geolocation.getCurrentPosition(
        (pos) => {
            console.log('Requesting ' + options.api_address);

            xhrRequest(options.api_address, 'GET', 
                (responseText) => {
                    const json = JSON.parse(responseText);

                    stations = new Stations(pos.coords.latitude,
                                            pos.coords.longitude);

                    async.map(json['network']['stations'], (item, callback) => {
                        stations.add(item);
                        callback();
                    }, (err, results) => {
                        stations.send();
                    });
                });
        },
        (err) => {
            console.log('Error requesting location : ' + err);
        },
        {timeout: 15000, maximumAge: 60000}
    );
};

const get_location = () => {
    navigator.geolocation.getCurrentPosition(
        (pos) => {
            if (stations) {
                stations.update(pos.coords.latitude, pos.coords.longitude);
            } else {
            
            }
            return; 
        },
        (err) => {
            console.log('Error requesting location : ' + err);
        },
        {timeout: 15000, maximumAge: 60000}
    );
};

Pebble.addEventListener('ready', 
    (e) => {
        console.log('PebbleKit JS ready!');

        get_stations();
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
                get_stations();
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

        get_stations();
    }
});
