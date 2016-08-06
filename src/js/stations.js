const Station = require('./station.js').Station;
const async = require('async');
const app = require('./appinfo.js');
const utils = require('./utils.js');

class Stations  {
    constructor (coords) {
        this.latitude = coords.latitude;
        this.longitude = coords.longitude;
        this.number = 5;
        this.stations = [];
    }

    _send (type, callback) {
        const self = this;

        const number = Math.min(self.stations.length, 5);

        utils.sendListToPebble(self.stations.slice(0, self.number), (obj, index) => {
            let result = obj.getPlain();
            result.KEY_TYPE = type ? type : app.RESPONSE_CLOSE_STATIONS;
            result.KEY_INDEX = index;
            result.KEY_NUMBER_OF_STATIONS = number;
            console.log('Sending : ' + JSON.stringify(result));

            return result;
        }, (err) => {
            utils.sendToPebble({
                KEY_TYPE : app.RESPONSE_END, 
            }, callback); 
        });
    }

    /* @brief : Sort a station in the this.stations array in such way it's still
     *      sorted by distance.
     */
    _sort (station) {
        let index = Math.floor(this.stations.length / 2);
        let low = 0;
        let high = this.stations.length;
        while (low < high) {
            console.log('---------\nLow : ' + low + '\nHigh : ' + high + '\nIndex : ' + index + '\nCurrentDistance : ' + station.distance + '\nIndexDistance : ' + this.stations[index].distance);
            if ((this.stations[index].distance == station.distance) ||
                (index > 0 &&
                this.stations[index].distance < station.distance &&
                this.stations[(index - 1) % this.stations.length].distance >= station.distance
                )
            )
            {
                break;
            }
            if (station.distance < this.stations[index].distance) {
                high = index;
            } else {
                low = index + 1;
            }
            index = Math.floor( (low + high) / 2);
        }
        this.stations.splice(index, 0, station);
    }

    /* @brief : Add a new Station to the this.stations array.
     */
    add (station) {
        const self = this;
        console.log('Adding station : ' + JSON.stringify(station));
        
        station.calcAngle(self.latitude, self.longitude);
        station.distanceFrom(self.latitude, self.longitude);

        this._sort(station);
    }

    clear () {
        this.stations = [];    
    }

    send (callback) {
        this._send(app.RESPONSE_CLOSE_STATIONS, callback);
    }

    update (coords, callback) {
        this.latitude = coords.latitude;
        this.longitude = coords.longitude;

        const self = this;
        async.map(self.stations, function (item, callback) {
            item.distanceFrom(self.latitude, self.longitude);
            item.calcAngle(self.latitude, self.longitude);
            self._sort(item); // Put the station in place to be still sorted.
            callback(null, item);
        }, function (err, results) {
            if (err) {
                console.log('Error updating localisation : ' + err); 
            }
            self._send(app.RESPONSE_UPDATED_LOCATION, callback);
        });
    }
}

exports.Stations = Stations;
