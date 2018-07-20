#include "routersvr/router_svr.h"
#include <iostream>
#include "protolib/src/routersvr_config.pb.h"
using namespace std;

int main(int argc, char* argv[]) {
	config::RouterSvrConfig cfg;
	cfg.set_log_level(9);
	std::cout << cfg.ShortDebugString() << endl;
	config::RouterSvrConfig cfg2;
	cfg2.ParseFromString(cfg.SerializeAsString());
	std::cout << cfg2.ShortDebugString() << endl;
	return 0;
	RouterAppSgl::mutable_instance().Init(argc, argv);
	RouterAppSgl::mutable_instance().Run();
	return 0;
}