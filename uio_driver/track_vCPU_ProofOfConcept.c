#include <pthread.h>
#include <math.h>
#include <string.h>
#include <sched.h>
#include <sys/types.h>
#include <linux/module.h>
#include <unistd.h>
#include <stdlib.h>
#include <syscall.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t got_tid = PTHREAD_COND_INITIALIZER;

void *thread1(void *id) {
	pid_t *tid = (pid_t *) id;

	pthread_mutex_lock(&mutex);
	
	*tid = syscall(SYS_gettid);

	pthread_cond_signal(&got_tid);
	pthread_mutex_unlock(&mutex);

	for (int i = 0; i < 120; i++) {
		sleep(1);
	}
	return id;
}

void *thread2(void *id) {
	pid_t *tid = (pid_t *) id;

	pthread_mutex_lock(&mutex);
	
	*tid = syscall(SYS_gettid);

	pthread_cond_signal(&got_tid);
	pthread_mutex_unlock(&mutex);

	for (int i = 0; i < 120; i++) {
		sleep(1);
	}
	return id;
}

int main() {
	int ret, fd;
	pid_t threads[2] = {-1, -1};
	char *options;
	pthread_t t1, t2;
	void *t1status, *t2status;
	
	pthread_create(&t1, NULL, thread1, (void *) &threads[0]);
	pthread_mutex_lock(&mutex);
	while (threads[0] == -1) {
		pthread_cond_wait(&got_tid, &mutex);
	}
	pthread_mutex_unlock(&mutex);

	pthread_create(&t2, NULL, thread2, (void *) &threads[1]);
	pthread_mutex_lock(&mutex);
	while (threads[1] == -1) {
		pthread_cond_wait(&got_tid, &mutex);
	}
	pthread_mutex_unlock(&mutex);
	
	printf("%d %d\n", threads[0], threads[1]);
	options = malloc(sizeof(char) * 23);
	sprintf(options, "pidarray=%d,%d", threads[0], threads[1]);
	printf("%s\n", options);

	fd = open("uio_device2.ko", O_RDONLY, 0);
	ret = syscall(SYS_finit_module, fd, options, 0);
	close(fd);

	if (ret == -1) {
		switch (errno) {
			case EBADMSG:
				printf("EBADMSG\n");
				break;

			case EBUSY:
				printf("EBUSY\n");
				break;

			case EFAULT:
				printf("EFAULT\n");
				break;

			case ENOKEY:
				printf("ENOKEY\n");
				break;

			case ENOMEM:
				printf("ENOMEM\n");
				break;

			case EPERM:
				printf("EPERM\n");
				break;

			default:
				printf("%d\n", errno);
		}
	} else
		printf("Module inserted\n");

	pthread_join(t1, &t1status);
	pthread_join(t2, &t2status);

	ret = syscall(SYS_delete_module, "uio_device2", O_NONBLOCK);
	
	if (ret == -1)
		printf("%d\n", errno);
	else
		printf("Module removed\n");

	return 0;
}
