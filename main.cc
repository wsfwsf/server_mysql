#include "server_sqlconnectionpool.h"

int main(){
	std::string ip="192.168.183.128";
	server s(5188,ip,10,10,10);
	s.Bind();
	s.Listen();
	s.Run();
	return 0;
}
