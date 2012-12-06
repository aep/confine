confine
----------

trivial sandbox, restricting where an application can write

not really secure. only good for preventing accidential side effects from things like buildscripts


for example:

    $ cd ~/crap/buid/
    $ confine /home/me/crap/build ./build.sh



