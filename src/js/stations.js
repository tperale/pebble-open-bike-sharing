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
        while (low < high) {
            console.log('---------\nLow : ' + low + '\nHigh : ' + high + '\nIndex : ' + index + '\nCurrentDistance : ' + current.distance + '\nIndexDistance : ' + this.stations[index].distance);
            if ((this.stations[index].distance == current.distance) ||
                (index > 0 &&
                 this.stations[index].distance < current.distance &&
                 this.stations[(index - 1) % this.stations.length].distance >= current.distance
                )
               )
            {
                break;
            }
            if (current.distance < this.stations[index].distance) {
                high = index;
            } else {
                low = index + 1;
            }
            index = Math.floor( (low + high) / 2);
        }
        this.stations.splice(index, 0, current);
    } else {
        this.stations.push(current);
    }
};

Stations.prototype._send = function (index) {
    if (index < Math.min(this.stations.length, 5)) {
        var result = this.stations[index].getPlain();
        // result.KEY_DISTANCE = format_distance(result.KEY_DISTANCE); // TODO CHANGE THIS SHIT
        result.KEY_DISTANCE = result.KEY_DISTANCE;
        result.KEY_INDEX = index;
        result.KEY_NUMBER_OF_STATIONS = Math.min(this.stations.length, 5);
        console.log('Sending : ' + JSON.stringify(result));
        var s = this;
        Pebble.sendAppMessage(result,
            function(e) {
                console.log('Station info sent to Pebble successfully in index ' + index);
                s._send(index + 1);
            },
            function(e) {
                console.log('Error sending API info to Pebble in index ' + index);
            }
        );
    } else {
        return; 
    }

};

Stations.prototype.send = function () {
    console.log('Sending results');

    this._send(0);
};

exports.Stations = Stations;
