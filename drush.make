; Archimedes Server make file
core = 7.x
api = 2
; comment this out in to use on drupal.org
projects[] = drupal

; Modules
projects[archimedes][download][type] = "git"
projects[archimedes][download][url] = "http://git.drupal.org/sandbox/fiasco/1116808.git"
projects[archimedes][subdir] = "contrib"
projects[archimedes][type] = "module"

projects[ctools][version] = "1.2"
projects[ctools][subdir] = "contrib"

projects[features][version] = "1.0"
projects[features][subdir] = "contrib"

projects[entityreference][version] = "1.0-rc5"
projects[entityreference][subdir] = "contrib"

projects[link][version] = "1.0"
projects[link][subdir] = "contrib"

projects[entity][version] = "1.0-rc3"
projects[entity][subdir] = "contrib"

projects[environment][version] = "1.x-dev"
projects[environment][subdir] = "contrib"

projects[libraries][version] = "2.0"
projects[libraries][subdir] = "contrib"

projects[menu_position][version] = "1.1"
projects[menu_position][subdir] = "contrib"

projects[pathauto][version] = "1.2"
projects[pathauto][subdir] = "contrib"

projects[redirect][version] = "1.0-rc1"
projects[redirect][subdir] = "contrib"

projects[strongarm][version] = "2.0"
projects[strongarm][subdir] = "contrib"

projects[token][version] = "1.4"
projects[token][subdir] = "contrib"

projects[twitter_bootstrap_ui][version] = "2.x-dev"
projects[twitter_bootstrap_ui][subdir] = "contrib"

projects[panels][version] = "3.3"
projects[panels][subdir] = "contrib"

projects[entitycache][version] = "1.x-dev"
projects[entitycache][subdir] = "contrib"
projects[entitycache][patches][] = "http://drupal.org/files/entitycache-numeric-ids.patch"

projects[jquery_update][version] = "2.2"
projects[jquery_update][subdir] = "contrib"

projects[views][version] = "3.5"
projects[views][subdir] = "contrib"

projects[views_bulk_operations][version] = "3.0"
projects[views_bulk_operations][subdir] = "contrib"

projects[archimedes_server][download][type] = "git"
projects[archimedes_server][download][url] = "http://git.drupal.org/sandbox/fiasco/1116748.git"
projects[archimedes_server][download][revision] = "7.x-1.x"
projects[archimedes_server][type] = "module"
projects[archimedes_server][subdir] = "contrib"

; Themes
; archimedes_theme
projects[archimedes_theme][download][type] = "git"
projects[archimedes_theme][download][url] = "http://git.drupal.org/sandbox/fiasco/1871610.git"
projects[archimedes_theme][download][revision] = "master"
projects[archimedes_theme][type] = "theme"
; twitter_bootstrap
projects[twitter_bootstrap][type] = "theme"
projects[twitter_bootstrap][version] = "2.0-beta1"
projects[twitter_bootstrap][subdir] = "contrib"
; rubik
projects[rubik][type] = "theme"
projects[rubik][version] = "4.0-beta8"
projects[rubik][subdir] = "contrib"
; tao
projects[tao][type] = "theme"
projects[tao][version] = "3.0-beta4"
projects[tao][subdir] = "contrib"

; Libraries
libraries[twitter_bootstrap][directory_name] = "twitter_bootstrap"
libraries[twitter_bootstrap][type] = "library"
libraries[twitter_bootstrap][destination] = "libraries"
libraries[twitter_bootstrap][download][type] = "get"
libraries[twitter_bootstrap][download][url] = "http://twitter.github.com/bootstrap/assets/bootstrap.zip"

libraries[profiler][download][type] = "get"
libraries[profiler][download][url] = "http://ftp.drupal.org/files/projects/profiler-7.x-2.0-beta1.tar.gz"

libraries[archimedes][download][type] = "get"
libraries[archimedes][download][url] = "https://raw.github.com/fiasco/Archimedes-Library/master/php/archimedes.class.php"
