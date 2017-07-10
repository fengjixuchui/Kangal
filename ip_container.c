#include "ip_container.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

struct IP_entry** ip_init () {
	/* Entry 0 will not be used*/
	struct IP_entry ** ip_tmp = calloc(IP_ARR_SIZE + 1, sizeof(struct IP_entry));

	for (int i = 1; i <= IP_ARR_SIZE; ++i) {
		ip_tmp[i] = malloc(sizeof(struct IP_entry));
		ip_tmp[i]->count = 0;
		ip_tmp[i]->ts_index = 0;
		ip_tmp[i]->is_rejected = 0;
	}

	return ip_tmp;
}

void ip_update (struct IP_entry **ip_list, u_char index, char* source_ip, long int sec, long int usec, char can_drop) {
	ip_list[index]->count++;
	u_char curr_index = ip_list[index]->ts_index;
	struct IP_timestamp * curr = &(ip_list[index]->timestamps[curr_index]);
	curr->sec = sec;
	curr->usec = usec;


	struct IP_timestamp next = ip_list[index]->timestamps[(curr_index + 1) % 50];

	printf("26. satir %li\n", curr->sec);
	printf("27. satir %li\n", next.sec);
	if ( (curr->sec - next.sec) < 3 ) {
		printf("29. satir%li\n", curr->sec);
	}

	/*THis checks count > 50 and time difference < 3*/
	if ((curr->sec - next.sec) < 3) {

		if (ip_list[index]->is_rejected == 0) {
			/*
			unsigned int a, b;
	     
	    	inet_pton (AF_INET, source_ip, &a);
	    	inet_pton (AF_INET, "10.20.40.31", &b);
	     	
	     	
	    	insert_rule ("filter",
	                   "INPUT",
	                   a,
	                   0,
	                   b,
	                   1,
	                   "REJECT");
	        */


	        char iptables_systemcall[100] = "iptables -t filter -A TCPIP_REJECTED -p tcp -s ";
	        strcat(iptables_systemcall, source_ip);
	        strcat(iptables_systemcall, " -j REJECT --reject-with tcp-reset");

	    	system(iptables_systemcall);

	    	ip_list[index]->is_rejected = 1;
		}
		else if(ip_list[index]->is_rejected == 1) {
			char iptables_systemcall[90] = "iptables -t filter -A TCPIP_DROPPED -p tcp -s ";
	        strcat(iptables_systemcall, source_ip);
	        strcat(iptables_systemcall, " -j DROP ");

	    	system(iptables_systemcall);

	    	ip_list[index]->is_rejected = 2;
		}
	}
	
	(ip_list[index]->ts_index)++;
	(ip_list[index]->ts_index) %= 50;
}


void ip_free(struct IP_entry **ip_list) {
	if (ip_list) {
		for (int i = 0; i < IP_ARR_SIZE; ++i) {
			free(ip_list[i]);
		}
		free(ip_list);
	}
	
}

void ip_flush(const char *table, const char *chain) {
	flush_table(table, chain);
}

// iptables -A INPUT -s 65.55.44.100 -j DROP

/*
int main() {
	struct IP_entry** ip_list = ip_init();

	char ipsrc_tmp[16];
	strcpy(ipsrc_tmp, "10.20.40.1");
	char* token = strtok(ipsrc_tmp, ".");
	u_char count = 0;
	while (token) {
		if (count == 3) {
			u_char index = atoi(token);
			ip_list[index]->count++;
			ip_list[index]->arrival_time = (double)clock() / (double)CLOCKS_PER_SEC;
			printf("index = %d, count = %d, arrival_time = %f\n", index, ip_list[index]->count, ip_list[index]->arrival_time);
		}
		token = strtok(NULL, ".");
    	count++;
	}

	for (int i = 1; i <= IP_ARR_SIZE; ++i) {
		printf("%d %f\n", ip_list[i]->count, ip_list[i]->arrival_time);
	}


	return 0;
} */