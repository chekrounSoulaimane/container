#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>


static char child_stack[1048576];
char *cgroup_name = "irisi_2_cgroup";

// executer le nouveau bash '/bin/sh'
void spawn_bash() {
        char *new_args[] = {"/bin/sh", NULL};
        execv("/bin/sh", new_args);
        perror("exec :");
        exit(-1);
}

// cette fonction permet de creer un cgroup soit memoiry, pids et cpu
// il permet aussi de limiter la memoire, limiter le cpu et limiter le nombre de processus
void set_cgroup(char *type, char *file, char *value, int pid) {
        char echo_value[255];
        char dir_name[255];
        char mkdir[255];
        char buf[255];

        sprintf(dir_name, "/sys/fs/cgroup/%s/%s", type, cgroup_name);
        sprintf(mkdir, "mkdir %s", dir_name);
        sprintf(echo_value, "echo %s > /sys/fs/cgroup/%s/%s/%s", value, type, cgroup_name, file);
        sprintf(buf, "echo %d > /sys/fs/cgroup/%s/%s/cgroup.procs", pid, type, cgroup_name);
        
        DIR* dir = opendir(dir_name);
        printf("%s cgroup : %s\n",type, dir_name);

        if (dir) {
                closedir(dir);
        } else {
                system(mkdir);
        }
        system(echo_value);
        // ajouter le pid au fichier cgroup.procs
        system(buf);
}

int child() {
        printf("pid as seen in the child : %d\n", (int) getpid());

        sethostname("irisi2", 6);
        chroot("./newfs");
        chdir("/");
        mount("proc", "/proc", "proc", 0, NULL);
        spawn_bash();
        return 1;
}


int main(int argc , char * argv[]) {
        char buf[255];
        int i = 1;
        char *memory_size;
        char *pids_max;
        char *cpu_shares;
        int flags = CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNS | CLONE_NEWNET;

        while (argv[i]) {
                if (strstr(argv[i], "--memory-size") != NULL) {
                        char *ptr = strtok(argv[i], "=");
                        memory_size = strtok(NULL, " ");
                        set_cgroup("memory", "memory.limit_in_bytes", memory_size, getpid());
                } else if (strstr(argv[i], "--pids-max") != NULL) {
                        char *ptr = strtok(argv[i], "=");
                        pids_max = strtok(NULL, " ");
                        set_cgroup("pids", "pids.max", pids_max, getpid());
                } else if (strstr(argv[i], "--cpu-shares") != NULL) {
                        char *ptr = strtok(argv[i], "=");
                        cpu_shares = strtok(NULL, " ");
                        set_cgroup("cpu", "cpu.shares", cpu_shares, getpid());
                }
                i++;
        }


        pid_t pid = clone(child, child_stack+1048576, flags | SIGCHLD, NULL);

        if (memory_size != NULL) {
                sprintf(buf, "echo %d > /sys/fs/cgroup/memory/%s/cgroup.procs", pid, cgroup_name);
                system(buf);
        } else if (pids_max != NULL) {
                sprintf(buf, "echo %d > /sys/fs/cgroup/pids/%s/cgroup.procs", pid, cgroup_name);
                system(buf);
        } else if (cpu_shares != NULL) {
                sprintf(buf, "echo %d > /sys/fs/cgroup/cpu/%s/cgroup.procs", pid, cgroup_name);
                system(buf);
        }     

        printf("\npid of the parent : %d\n", (int) getpid());
        printf("pid as seen in the parent : %d\n", (int) pid);

        // le parent doit attendre le fils jusqu a ce qu il termine son execution
        waitpid(pid, NULL, 0);
        return 0;
}
