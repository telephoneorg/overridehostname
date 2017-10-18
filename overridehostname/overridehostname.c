#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>


typedef int (*gethostname_t)(char *name, size_t len);
typedef int (*sethostname_t)(const char *name, size_t len);

gethostname_t old_gethostname;
sethostname_t old_sethostname;


int fh_get_hostname(char *name, size_t len){
    int rc = 0;
    FILE *fd = fopen("/etc/hostname", "r");
    if (fd == NULL) {
        rc = 1;
    } else {
        fgets(name, len, fd);
        fclose(fd);
    }
    return rc;
}

int fh_set_hostname(char *name, size_t len){
    int rc = 0;
    FILE *fd = fopen("/etc/hostname", "w");
    if(fd == NULL) {
        rc = 1;
    } else {
        if (fputs(name, fd) != EOF) {
            rc = 1;
        }
        fclose(fd);
    }
    return rc;
}

void do_redirect() {
	void *libc_library;
	char *error;

	if (!(libc_library = dlopen("libc.so.6", RTLD_LAZY))) {
		fprintf(stderr, "Cannot open libc.so.6: %s\n", dlerror());
		exit(1);
	}

	old_gethostname = dlsym(libc_library, "gethostname");
    old_sethostname = dlsym(libc_library, "sethostname");

	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(1);
	}
}

int gethostname(char *name, size_t len) {
    int rc = 0;

	if (!old_gethostname)
		do_redirect();

	if (getenv("OVERRIDE_HOSTNAME")) {
        rc = fh_get_hostname(name, len);
    } else {
        rc = old_gethostname(name, len);
    }
	return rc;
}

int sethostname(const char *name, size_t len) {
    int rc = 0;

    if (!old_sethostname)
		do_redirect();

	if (getenv("OVERRIDE_HOSTNAME")) {
        rc = fh_set_hostname(name, len);
    } else {
        rc = old_sethostname(name, len);
    }
	return rc;
}
