/*
*API function headers!!
*/

int gallery_connect(char * host, in_port_t port);

uint32_t  gallery_add_photo(int peer_socket, char *file_name);

int gallery_add_keyword (int peer_socket, uint32_t id_photo, char *keyword);
