/* @desc : Construct a "station" object from the JSON of the api.
 *
 * @param {obj} : Station JSON from the API, example
 *      {
 *          "empty_slots": 11, 
 *          "extra": {
 *              "address": "CHATEAU-KASTEEL - AVENUE DU CH\u00c2TEAU/KASTEELLAAN", 
 *              "banking": false, 
 *              "bonus": false, 
 *              "last_update": 1463856622000, 
 *              "slots": 24, 
 *              "status": "OPEN", 
 *              "uid": 133
 *          }, 
 *          "free_bikes": 13, 
 *          "id": "9e07346cff37b86357d3f68af29713df", 
 *          "latitude": 50.866795, 
 *          "longitude": 4.3092224, 
 *          "name": "133 - CH\u00c2TEAU/KASTEEL", 
 *          "timestamp": "2016-05-21T18:57:11.225000Z"
 *      }, 
 */
var Station = function (obj) {
    this.name = obj.name;
    this.empty_slots = obj.empty_slots;
    this.free_bikes = obj.free_bikes;
    this.latitude = obj.latitude;
    this.longitude = obj.longitude;
    this.distance = null;
    this.angle = null;
};

/* @desc : Transform the object into a formated JSON to send to the Pebble.
 */
Station.prototype.getPlain = function () {
    return {
        KEY_NAME : this.name,
        KEY_DISTANCE : this.distance,
        KEY_ANGLE : this.angle,
        KEY_FREE_BIKE : this.free_bikes,
        KEY_PARKINGS : this.empty_slots,
    };
};

/* @desc : Calculate the distance of the station from the geolocalisation
 *
 * @param {latitude} : Geoloc. latitude.
 * @param {longitude} : Geoloc. longitude.
 *
 * @return {Number} : Distance in kilometers.
 */
Station.prototype.distanceFrom = function (latitude, longitude) {
    var unit = 'K';
    
    var radlat1 = Math.PI * this.latitude / 180;
    var radlat2 = Math.PI * latitude / 180;
    var radlon1 = Math.PI * this.longitude / 180;
    var radlon2 = Math.PI * longitude / 180;

    var theta = this.longitude - longitude;
    var radtheta = ( Math.PI * theta ) / 180;

    var dist = ( Math.sin(radlat1) * Math.sin(radlat2) ) + ( Math.cos(radlat1) * Math.cos(radlat2) * Math.cos(radtheta) );
    dist = Math.acos(dist);
    dist = dist * 180 / Math.PI;
    dist = dist * 60 * 1.1515;

    if (unit=='K') { dist = dist * 1.609344; }
    if (unit=='N') { dist = dist * 0.8684; }

    this.distance = dist;

    return dist;
};

/* @desc : Calculate the offset angle from the north.
 *
 * @param {latitude} : Geoloc. latitude.
 * @param {longitude} : Geoloc. longitude.
 *
 * @return {Number} : Angle in degree.
 */
Station.prototype.calcAngle = function (latitude, longitude) {
    var delta_lat = latitude - this.latitude;
    var delta_long = longitude - this.longitude;

    this.angle = (Math.atan(delta_long / delta_lat)) * 180 / Math.PI;

    return this.angle;
};

exports.Station = Station;
