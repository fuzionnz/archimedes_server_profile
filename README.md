Archimedes Server
=================

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

Create a make "stub" file in an empty directory that will soon be your Archimedes root with the following contents:

    ; make file for archimedes_server_profile
    core = 7.x
    api = 2
    projects[] = drupal
    ; Profile.
    projects[archimedes_server_profile][type] = profile
    projects[archimedes_server_profile][download][type] = git
    projects[archimedes_server_profile][download][url] = https://github.com/xurizaemon/archimedes_server_profile.git

Run the stub file with drush make:

    drush make <stub_filename>.make
