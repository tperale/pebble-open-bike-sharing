module.exports = function(grunt) {
  var appConfig = {
      info: grunt.file.readJSON('appinfo.json')
  };

  grunt.initConfig({
        jshint : {
            files : [
                'Gruntfile.js',
                'src/js/**/*.js',
                '!src/js/**/*.tpl.js',
                '!src/js/pebble-js-app.js',
            ],
            options : {
                jshintrc : true, 
            }
        },

        browserify: {
            build: {
                src: [
                    'src/js/**/*.js',
                    '!src/js/**/*.tpl.js',
                    '!src/js/pebble-js-app.js',
                ],
                dest: 'src/js/pebble-js-app.js',
            },
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
  });
  grunt.loadNpmTasks('grunt-contrib-jshint');
  grunt.loadNpmTasks('grunt-browserify');
  grunt.loadNpmTasks('grunt-contrib-copy');
  grunt.registerTask('default', ['copy', 'jshint', 'browserify']);
};
