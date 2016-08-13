const Station = require('./station.js').Station;
const async = require('async');
const app = require('./appinfo.js');
const utils = require('./utils.js');

/* @desc Stations class to abstract a bike network, in a way to send easily
 *      the stations from the API.
 *
 * @param Current latitude of self.
 * @param Current longitude of self.
 * @param Number of station sent to the Pebble.
 * @param Sorted list by distance from self of reference to a specific Station.
 * @param Translation from a station name to its reference.
 * @param Stations sent to the Pebble.
 */
class Stations  {
    constructor (coords) {
        this.latitude = coords.latitude;
        this.longitude = coords.longitude;
        this.number = app.NUMBER_OF_STATIONS_MIN;
        this.stations = [];
        this.stationsDict = {};
        this.currentStation = [];
    }

    _remove (station) {
        const i = this.stations.indexOf(station);

        if (i != -1) {
            this.stations.splice(i, 1);
        }
    }

    _sendToPebble (type, list, callback) {
        const self = this;
        const number = Math.min(self.stations.length, self.number);
        utils.sendListToPebble(list, (obj, index) => {
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

    _send (type, callback) {
        this.currentStation = this.stations.slice(0, this.number);

        this._sendToPebble(type, this.currentStation, callback);
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

        this.stationsDict[station.name] = station;
        this._sort(station);
    }

    clear () {
        this.stations = [];    
    }

    send (callback) {
        this._send(app.RESPONSE_CLOSE_STATIONS, callback);
    }

    addLess (callback) {
        // Using the same "algorithm" as in the C code.
        if ((this.number / 2) > app.NUMBER_OF_STATIONS_MIN) {
            this.number = Math.floor(this.number / 2);
        } else {
            this.number = app.NUMBER_OF_STATIONS_MIN;
        }
    }

    addMore (callback) {
        const old = this.number;
        this.number = Math.min(this.number * 2, this.stations.length);

        this._sendToPebble(app.RESPONSE_ADD_STATIONS,
            this.stations.slice(
                Math.min(old, this.stations.length), 
                this.number
            ),
            callback
        );
    }

    /* @desc Update the stations stored in this class with the data from
     *      the API.
     * @param The array of "stations" from the API.
     */
    updateWithAPI (apiStations, callback) {
        const self = this;

        async.map(apiStations, (station, callback) => {
            self.stationsDict[station.name].updateWithAPI(station);
            callback();
        }, () => {
            self._send(app.RESPONSE_UPDATED_STATIONS, callback); 
        });
    }

    /* @desc Update the stations stored in this class with a new location.
     *
     * @param New location object.
     */
    updateWithLocation (coords, callback) {
        const self = this;

        this.latitude = coords.latitude;
        this.longitude = coords.longitude;

        async.map(self.stations, (station, callback) => {
            station.updateWithLocation(coords);
            // self._remove(station);
            // self._sort(station); // Put the station in place to be still sorted.
            callback();
        }, (err, results) => {
            if (err) {
                console.log('Error updating localisation : ' + err); 
            }
            self._send(app.RESPONSE_UPDATED_LOCATION, callback);
        });
    }
}

exports.Stations = Stations;
