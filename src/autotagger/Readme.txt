LunuxMCE-Tag ver 1.5
 - Langston Ball
   golgoj4@gmail.com

    GPLv2 Licensed
    Metadata aquisition tool that looks up television show and movie data and inserts it into the linuxmce database.
    It utilizes http://www.thetvdb.com for television show information
    It utilizes http://www.themoviedb.com for movie and some television mini series and direct to dvd movies.


Building / Compiling.
This program requires the installation of additional mysql development packages
to link against at compile time.

Steps
(1)Install neccesary mysql headers 'libmysqlclient15-dev'

Usage:

With a path
EX: linucmeTag "path to files. no escape needed when in quotes"

With a File Number
EX linuxmceTag 10220

calling it without parameters will default to /home/public/data/videos

Known Issues
*If run inside of a directory, it will index all but one file.
*Movie and tv show detection can be improved
*can consume moderate cpu with large numbers of files

-To do list
*Implement DVD tagging for television series
 specifically, tag each .iso, video_ts folder or other dvd structred media with the episodes assigned to the dvd and allow the user to bookmark them.
*Implement automatic updating of television series
*Make automatic by having it run in the background.



