const getQueryParam = (variable, defaultValue) => {
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

module.exports.sendObjectToPebble = (object) => {
    for (let key in object) {
        localStorage[key] = object[key];
    }
    const return_to = getQueryParam('return_to', 'pebblejs://close#');
    document.location = return_to + encodeURIComponent(JSON.stringify(object));
};
