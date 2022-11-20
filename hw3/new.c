typedef struct hostent
{
    char *h_name;       // official (canonical) name of host
    char **h_aliases;   // pointer to array of pointers to 			    alias names
    int h_addrtype;     // host address type: AF_INET
    int h_length;       // length of address: 4
    char **h_addr_list; // ptr to array of ptrs with IPv4 addrs
} hostent;