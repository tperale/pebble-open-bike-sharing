var Station = require('./station.js').Station;
var async = require('async');

var format_distance = function (distance) {
    var result = '';
    if (distance < 1) {
        result = (distance * 1000).toString().slice(0, 6) + ' M';
    } else {
        result = distance.toString().slice(0, 6) + ' KM';
    }

    return result;
};

var Stations = function (latitude, longitude) {
    this.latitude = latitude;
    this.longitude = longitude;
    this.stations = [];
};

Stations.prototype.add = function (station) {
    console.log('Adding station : ' + JSON.stringify(station));
    var current = new Station(station);
    current.calcAngle(this.latitude, this.longitude);
    var distance = current.distanceFrom(this.latitude, this.longitude);

    if (this.stations.length) {
        var high = this.stations.length;
        var index = Math.floor(this.stations.length / 2);
        var low = 0;
        while (low < high && index != low && index != high) {
            console.log('---------\nLow : ' + low + '\nHigh : ' + high + '\nIndex : ' + index + '\nCurrentDistance : ' + current.distance + '\nIndexDistance : ' + this.stations[index].distance);
            if (current.distance < this.stations[index].distance) {
                high = index;
            } else {
                low = index;
            }
            index = Math.floor( (low + high) / 2);
        }
        this.stations.splice(index, 0, current);
    } else {
        this.stations.push(current);
    }
};

Stations.prototype.send = function () {
    console.log('Sending results');
    var stations_number = this.stations.length;
    // async.forEachOf(this.stations, function (item, index, callback) {
    //     var result = item.getPlain();
    //     console.log('Sending : ' + JSON.stringify(item));
    //     result.KEY_DISTANCE = format_distance(result.KEY_DISTANCE); // TODO CHANGE THIS SHIT
    //     result.KEY_INDEX = index;
    //     result.KEY_NUMBER_OF_STATIONS = stations_number;
    //     Pebble.sendAppMessage(result,
    //         function(e) {
    //             console.log('Station info sent to Pebble successfully');
    //             callback();
    //         },
    //         function(e) {
    //             console.log('Error sending API info to Pebble');
    //             callback(e);
    //         }
    //     );
    // }, function (err, results) {
    //     if (err) {
    //         console.error('Error sending the information to the pebble : ' + JSON.stringify(err));
    //     }
    // });
    var result = this.stations[0].getPlain();
    console.log('Sending : ' + JSON.stringify(result));
    // result.KEY_DISTANCE = format_distance(result.KEY_DISTANCE); // TODO CHANGE THIS SHIT
    result.KEY_DISTANCE = result.KEY_DISTANCE;
    result.KEY_INDEX = 0;
    result.KEY_NUMBER_OF_STATIONS = this.stations.length;
    Pebble.sendAppMessage(result,
        function(e) {
            console.log('Station info sent to Pebble successfully');
        },
        function(e) {
            console.log('Error sending API info to Pebble');
        }
    );

};

exports.Stations = Stations;
