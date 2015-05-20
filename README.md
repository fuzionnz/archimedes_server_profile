Archimedes Server Profile
=========================

Archimedes is a passive application monitoring server built on Drupal but designed to facilitate any web based application. Archimedes achieves passive application monitoring by installing an Archimedes client on your web hosted application. In Drupal's case, it's installing the Archimedes Client module.

The Client collects up information about the application and sends it back to the server where the server reads it in and creates or updates the site node that represents that client on the server.

Archimedes currently collects these metrics

* Hostname
* Servername
* Site Data size
* Site Root size
* Database details
* Database size
* Site instance Metrics
* Environment (Production, Staging, Development)
* Associated users
* Git hash
* Git remotes
* Unique Identifier
* Software specific
 * Modules, themes, etc
 * Features

Some of the features of Archimedes
----------------------------------

 * HTTP or SMTP communication with the server. Permits Archimedes to monitor sites behind firewalls without requiring firewall exemptions.
 * RSA key encryption. All data collected by clients is encrypted in transit with the server's public RSA key.

Installation
------------

Clone this profile repo, then run `drush make` with the file `build-archimedes.make` from within it.

    drush make path/to/archimedes_server_profile/build-archimedes.make /path/to/archimedes.example.org

_Help wanted ... it should be possible to do this by pointing `drush make` at the URL for a raw .make file?_

This will build your Archimedes Server codebase.

Now visit the site and install Archimedes!
