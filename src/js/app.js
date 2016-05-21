var options = JSON.parse(localStorage.getItem('options'));
if (!options || !options.api_address) {
    options = { api_address : 'http://api.citybik.es/v2/networks/villo' };
}

var min_distance = 100000;

var current = [];

/* @desc : Transform radians to degree.
 *
 * @param {radians} : La mesure en radian.
 */
var to_degree = function (radians) {
    return radians * 180 / Math.PI;
};

/* @desc : Calculate a distance beetween two (latitude, longitude) couple.
 */
var calc_distance = function (lat1, lon1, lat2, lon2) {
    var unit = 'K';
    
    var radlat1 = Math.PI * lat1 / 180;
    var radlat2 = Math.PI * lat2 / 180;
    var radlon1 = Math.PI * lon1 / 180;
    var radlon2 = Math.PI * lon2 / 180;

    var theta = lon1 - lon2;
    var radtheta = Math.PI * theta/180;

    var dist = Math.sin(radlat1) * Math.sin(radlat2) + Math.cos(radlat1) * Math.cos(radlat2) * Math.cos(radtheta);
    dist = Math.acos(dist);
    dist = dist * 180 / Math.PI;
    dist = dist * 60 * 1.1515;

    if (unit=='K') { dist = dist * 1.609344; }
    if (unit=='N') { dist = dist * 0.8684; }

    return dist;
};

var format_distance = function (distance) {
    var result = '';
    if (distance < 1) {
        result = (distance * 1000).toString().slice(0, 6) + ' M';
    } else {
        result = distance.toString().slice(0, 6) + ' KM';
    }

    return result;
};

var station = {
    name : null,
    latitude : null,
    longitude : null,

    free_bikes : null,
    empty_slots : null,
};

/* @desc : Make the response from the current position and the current list.
 *
 * @param {pos} : Current geolocation.
 */
var make_response = function (pos) {
    var ret = {
        KEY_NAME : null,
        KEY_DISTANCE : null,
        KEY_ANGLE : null,
        KEY_FREE_BIKE : null,
        KEY_PARKINGS : null,
        KEY_NUMBER_OF_STATIONS : 1
    };

    ret.KEY_NAME = current[0].name;

    ret.KEY_DISTANCE = format_distance(calc_distance(current[0].latitude, current[0].longitude, pos.coords.latitude, pos.coords.longitude));
    console.log('DISTANCE : ' + format_distance(calc_distance(current[0].latitude, current[0].longitude, pos.coords.latitude, pos.coords.longitude)));

    var delta_lat = pos.coords.latitude - current[0].latitude;
    var delta_long = pos.coords.longitude - current[0].longitude;

    ret.KEY_ANGLE = to_degree(Math.atan(delta_long / delta_lat));

    ret.KEY_FREE_BIKE = current[0].free_bikes;
    ret.KEY_PARKINGS = current[0].empty_slots;

    return ret;
};

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
      callback(this.responseText);
    };
  xhr.open(type, url);
  xhr.send();
};

/* ------------------------------------------------
 *  Get close villo stations request functions.
 * ------------------------------------------------
 */
var get_close_stations = function (pos) {
    // We will request the weather here
    var my_lat = pos.coords.latitude;
    var my_long = pos.coords.longitude;

    console.log('Requesting ' + options.api_address);

    xhrRequest(options.api_address, 'GET', 
        function(responseText) {
            var json = JSON.parse(responseText);
            var stations = json['network']['stations'];

            min_distance = 100000;

            // Calculate distance with every stations.
            for (var i = 0; i < stations.length; ++i) {
                var station = stations[i];

                if (station['free_bikes'] === 0) {
                    continue;
                }

                var distance = calc_distance(my_lat, my_long, station['latitude'], station['longitude']);
               
                if (distance < min_distance) {
                    var station_obj = {};

                    min_distance = distance;

                    station_obj.name = station['name'];
                    station_obj.latitude = station['latitude'];
                    station_obj.longitude = station['longitude'];

                    station_obj.free_bikes = station['free_bikes'];
                    station_obj.empty_slots = station['empty_slots'];

                    current[0] = station_obj;
                }
            }

            var result_dict = make_response(pos);

            console.log(JSON.stringify(result_dict));

            Pebble.sendAppMessage(result_dict,
                function(e) {
                    console.log('API info sent to Pebble successfully!');
                },
                function(e) {
                    console.log('Error sending API info to Pebble : ' + JSON.stringify(e));
                }
            );
        }
    );
};
    
var location_error = function (err) {
    console.log('Error requesting location!');
};

var get_stations = function () {
    navigator.geolocation.getCurrentPosition(
        get_close_stations,
        location_error,
        {timeout: 15000, maximumAge: 60000}
    );
};

/* ------------------------------------------------
 *  Get the position request functions.
 * ------------------------------------------------
 */
var recalculate_position = function (pos) {
    if (!current.length) {
        return; 
    }

    var result_dict = make_response(pos);

    Pebble.sendAppMessage(result_dict,
        function(e) {
            console.log('API info sent to Pebble successfully!');
        },
        function(e) {
            console.log('Error sending API info to Pebble!');
        }
    );


};

var get_location = function () {
    navigator.geolocation.getCurrentPosition(
        recalculate_position,
        location_error,
        {timeout: 15000, maximumAge: 60000}
    );
};

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
    function(e) {
        console.log('PebbleKit JS ready!');

        get_stations();
    }
);

Pebble.addEventListener('appmessage',
    function(e) {
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

Pebble.addEventListener('showConfiguration', function() {
  var url = 'https://rawgit.com/thomacer/pebble-villo/master/config/index.html';

  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  // Decode the user's preferences
  if (e.response) {
    options = JSON.parse(decodeURIComponent(e.response));
    console.log('Received : ' + JSON.stringify(options));
    localStorage.setItem('options', JSON.stringify(options)); 

    get_stations();
  }
});
