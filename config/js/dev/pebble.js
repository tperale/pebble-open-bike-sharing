let utils = require('./utils.js');

let getConfigData = () => {
    let select = document.getElementById('stationList');
    let apiAddress = select.options[select.selectedIndex].value;

    console.log('Sending API address : ' + apiAddress + ' from ' + JSON.stringify(select.options));

    let options = {
        'api_address': apiAddress,
    };

    // Save for next launch
    localStorage['api_address'] = options['api_address'];

    //console.log('Got options: ' + JSON.stringify(options));
    return options;
};


let getQueryParam = (variable, defaultValue) => {
    let query = location.search.substring(1);
    let vars = query.split('&');
    for (let i = 0; i < vars.length; i++) {
        let pair = vars[i].split('=');
        if (pair[0] === variable) {
            return decodeURIComponent(pair[1]);
        }
    }
    return defaultValue || false;
};

module.exports = {
    getConfigData : getConfigData,
    getQueryParam : getQueryParam,
    send : () => {
        let return_to = getQueryParam('return_to', 'pebblejs://close#');
        getConfigData();
        // document.location = return_to + encodeURIComponent(JSON.stringify(getConfigData()));
    },
};
