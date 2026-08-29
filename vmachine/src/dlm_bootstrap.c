#include <arpa/inet.h>
#include <fcntl.h>
#include <libdlm.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void)
{
	struct sockaddr_storage ss = {0};
	struct sockaddr_in *sin = (struct sockaddr_in *)&ss;

	sin->sin_family = AF_INET;

	if (inet_pton(AF_INET, "10.0.2.15", &sin->sin_addr) != 1) {
		perror("inet_pton");
		return 1;
	}

	int fd = open(
		"/sys/kernel/config/dlm/cluster/comms/1/addr",
		O_WRONLY
	);

	if (fd < 0) {
		perror("open addr");
		return 1;
	}

	if (write(fd, &ss, sizeof(ss)) != sizeof(ss)) {
		perror("write addr");
		close(fd);
		return 1;
	}

	close(fd);

	dlm_lshandle_t ls = dlm_create_lockspace("test", 0666);

	if (!ls) {
		perror("dlm_create_lockspace");
		return 1;
	}

	chmod("/dev/dlm_test", 0666);

	dlm_close_lockspace(ls);

	return 0;
}
