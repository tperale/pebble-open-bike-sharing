const pebble = require('./pebble.js');
const select = require('./select.js');

document.getElementById('apiSubmitButton').addEventListener('click', function () {
    console.log('Submiting the current address');

    pebble.sendObjectToPebble({
        'api_address' : select.getCurrentValue('networksList'),
    });                                                                      
});

