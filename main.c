#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <sys/mount.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "usage: %s /path/to/allow someprogram someargs\n", argv[0]);
        return 3;
    }

    // get our own namespace
    int r = unshare(CLONE_NEWNS);
    if (r != 0)
        perror("unshare");


    // unshare our namespace from parent
    // now if you're confused, like me, why unshare doesnt unshare:
    // unshare(CLONE_FS) unshares the process->namespace pointer.
    // i.e. we get a new mount table. but the mount table is a copy of the parent,
    // which happens to be shared, so we share all of them with the parent.
    // oof. did that make any sense at all? no not really.
    // so basicly two level thingy:
    // process->namespace->mount
    // and we need to unshare both levels
    r = mount("/","/", 0, MS_PRIVATE | MS_REC, 0);
    if (r != 0)
        perror("make private");


    // great now let's mount root somewhere else
    r = mount("/", "/root", 0, MS_BIND |  MS_RDONLY | MS_REC | MS_PRIVATE , 0);
    if (r != 0)
        perror("bind /");

    // MS_RDONLY only works with REMOUNT. ugh
    // FIXME: that only does it for /
    r = mount("/", "/root", 0, MS_BIND |  MS_RDONLY | MS_REC | MS_PRIVATE | MS_REMOUNT, 0);
    if (r != 0)
        perror("bind /");

    //mount the allowed path rw
    char *m = malloc(strlen("/root/") + strlen(argv[1]));
    strcpy(m, "/root/");
    strcat(m, argv[1]);
    r = mount(argv[1], m, 0, MS_BIND | MS_PRIVATE, 0);
    if (r != 0)
        perror("bind ");



    // our new home
    chroot ("/root");



    // drop to the requesting user
    seteuid(getuid());
    setegid(getgid());

    execvp(argv[2] , argv + 3);
}
