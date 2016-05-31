let utils = require('./utils.js');
let pebble = require('./pebble.js');

let submitButton = document.getElementById('submit_button');
    submitButton.addEventListener('click', function() {
    console.log('Submit');

    // Set the return URL depending on the runtime environment
    pebble.send();
});

utils.find_closest();
