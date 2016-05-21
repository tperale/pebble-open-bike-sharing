module.exports = {
    <% for (prop in config.info.appKeys) {
    %><%= prop %> : <%= config.info.appKeys[prop] %>,
    <% } %>
};

