var async = require('async');
// var app = require('./appinfo.js');
var Stations = require('./stations.js').Stations;

var options = JSON.parse(localStorage.getItem('options'));
if (!options || !options.api_address) {
    options = { api_address : 'http://api.citybik.es/v2/networks/villo' };
}

var xhrRequest = (url, type, callback) => {
  var xhr = new XMLHttpRequest();
  xhr.onload = () => {
      callback(xhr.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

var get_stations = () => {
    navigator.geolocation.getCurrentPosition(
        function (pos) {
            console.log('Requesting ' + options.api_address);

            xhrRequest(options.api_address, 'GET', 
                (responseText) => {
                    var json = JSON.parse(responseText);

                    var stations = new Stations(pos.coords.latitude,
                                                pos.coords.longitude);

                    async.map(json['network']['stations'], function (item, callback) {
                        stations.add(item);
                        callback();
                    }, function (err, results) {
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

var get_location = () => {
    navigator.geolocation.getCurrentPosition(
        (pos) => {
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
            case 100: // location.
                get_location();
                break;
            case 101:
                get_stations();
                break;
        }
    }                     
);

Pebble.addEventListener('showConfiguration', () => {
  var url = 'https://rawgit.com/thomacer/pebble-villo/master/config/index.html';

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
