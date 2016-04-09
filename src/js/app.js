var url = "http://api.citybik.es/v2/networks/villo";

var min_distance = 100000;
var min_name = "";
var min_angle = 0;

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
      callback(this.responseText);
    };
  xhr.open(type, url);
  xhr.send();
};

var to_degree = function (radians) {
    return radians * 180 / Math.PI;
}

var calc_distance = function (lat1, lon1, lat2, lon2) {
    var unit = "K";
    
    var radlat1 = Math.PI * lat1 / 180
    var radlat2 = Math.PI * lat2 / 180
    var radlon1 = Math.PI * lon1 / 180
    var radlon2 = Math.PI * lon2 / 180

    var theta = lon1 - lon2
    var radtheta = Math.PI * theta/180

    var dist = Math.sin(radlat1) * Math.sin(radlat2) + Math.cos(radlat1) * Math.cos(radlat2) * Math.cos(radtheta);
    dist = Math.acos(dist)
    dist = dist * 180 / Math.PI
    dist = dist * 60 * 1.1515

    if (unit=="K") { dist = dist * 1.609344 }
    if (unit=="N") { dist = dist * 0.8684 }

    console.log("DIST IS : " + dist);

    return dist;
}

var location_success = function (pos) {
    // We will request the weather here

    var my_lat = pos.coords.latitude;
    var my_long = pos.coords.longitude;

    console.log("Requesting " + url);

    xhrRequest(url, 'GET', 
        function(responseText) {
            var json = JSON.parse(responseText);
            var stations = json['network']['stations'];

            min_distance = 360;

            for (var i = 0; i < stations.length; ++i) {
                var station = stations[i];
                if (station['free_bikes'] == 0) {
                    continue;
                }
                var distance = calc_distance(my_lat, my_long, station['latitude'], station['longitude']);

               
                if (distance < min_distance) {
                    min_distance = distance;
                    min_name = stations[i]['name'];

                    var delta_lat = my_lat - station['latitude'];
                    var delta_long = my_long - station['longitude'];
                    var angle = to_degree(Math.atan(delta_long / delta_lat));

                    min_angle = angle;
                }
            }

            if (min_distance < 1) {
                min_distance = (min_distance * 1000).toString().slice(0, 6) + " M";
            } else {
                min_distance = min_distance.toString().slice(0, 6) + " KM";
            }

            var result_dict = {
                KEY_NAME : min_name, 
                KEY_DISTANCE : min_distance,
                KEY_ANGLE : min_angle
            };

            Pebble.sendAppMessage(result_dict,
                function(e) {
                    console.log('API info sent to Pebble successfully!');
                },
                function(e) {
                    console.log('Error sending API info to Pebble!');
                }
            );
        }
    );
}
    
var location_error = function (err) {
    console.log('Error requesting location!');
}

var get_villo_station = function () {
    navigator.geolocation.getCurrentPosition(
        location_success,
        location_error,
        {timeout: 15000, maximumAge: 60000}
    );
};

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
    function(e) {
        console.log('PebbleKit JS ready!');
        get_villo_station();
    }
);

Pebble.addEventListener('appmessage',
    function(e) {
        console.log('AppMessage received!');
        getAPIResult();
    }                     
);
