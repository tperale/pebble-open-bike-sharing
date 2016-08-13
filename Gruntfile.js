module.exports = function(grunt) {
  var appConfig = {
      info: grunt.file.readJSON('appinfo.json')
  };

  grunt.initConfig({
        jshint : {
            files : [
                'Gruntfile.js',
                'src/js/**/*.js',
                'config/js/dev/**/*.js',
                '!src/js/**/*.tpl.js',
                '!src/js/pebble-js-app.js',
            ],
            options : {
                jshintrc : true, 
            }
        },

        config: appConfig, 

        copy: {
            main: {
                options: {
                    process: function (content, path) {
                        return grunt.template.process(content);
                    }
                },
                files: {
                    'src/appinfo.h': ['src/appinfo.tpl.h'],
                    'src/js/appinfo.js': ['src/js/appinfo.tpl.js'],
                }
            },
        },

        browserify: {
            dist: {
                options: {
                    transform: [
                        ['babelify', {
                            presets: ['es2015'],
                        }]
                    ]
                },

                files: {                                                         
                    './src/js/pebble-js-app.js' : ['src/js/**/*.js', '!src/js/**/*.tpl.js', '!src/js/pebble-js-app.js'],
                    './config/js/selectNetworksList.js' : ['./config/js/dev/selectNetworksList.js'],
                    './config/js/select.js' : ['./config/js/dev/select.js'],
                },
            },
        },
  });
  grunt.loadNpmTasks('grunt-contrib-jshint');
  grunt.loadNpmTasks('grunt-browserify');
  grunt.loadNpmTasks('grunt-contrib-copy');
  grunt.registerTask('default', ['copy', 'jshint', 'browserify']);
};
