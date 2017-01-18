#include <stdio.h>
#include <unistd.h>
#include <assert.h>

struct message {
	int visits;
};

void send_message ( int sendd, struct message *m ) {
	int ret = write(sendd,m,sizeof(struct message));
	assert(ret != -1);
}

void recv_message ( int recvd, struct message *m ) {
	int ret = read(recvd,m,sizeof(struct message));
	assert(ret == sizeof(struct message));
}

void ringelreihen ( int recvd, int sendd ) {
	struct message m;
	while (1) {
		recv_message(recvd,&m);
		m.visits++;
		send_message(sendd,&m);
	}
}

int main ( int ac, char **av ) {
	int ret;

	int a2b[2]; // write to 1 read from 0
	ret = pipe(a2b);
	assert(ret != -1);

	int b2c[2];
	ret = pipe(b2c);
	assert(ret != -1);

	int c2a[2]; 
	ret = pipe(c2a);
	assert(ret != -1);

	// I am a
	ret = fork();
	assert(ret != -1);
	if (ret == 0) {
		// I am b
		ringelreihen(a2b[0],b2c[1]);
	}
	else {
		// I am still a
		ret = fork();
		assert(ret != -1);
		if (ret == 0) {
			// I am c
			ringelreihen(b2c[0],c2a[1]);
		}
		else {
			// Still a again ... so do all the initialization and logging
			struct message m;
			m.visits = 0;
			send_message(a2b[1],&m);
			while (1) {
				recv_message(c2a[0],&m);
				printf("visits ist %d\n",m.visits);
				m.visits++;
				send_message(a2b[1],&m);
			}
		}
	}
}