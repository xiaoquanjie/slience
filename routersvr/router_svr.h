#ifndef M_ROUTERSVR_ROUTER_SVR_INCLUDE
#define M_ROUTERSVR_ROUTER_SVR_INCLUDE

#include "commonlib/svr_base/ApplicationBase.h"
#include "slience/base/singletion.hpp"

class RouterApplication : public ApplicationBase {
protected:
	int OnInit() override;

	int OnReload() override;

	int OnProc() override;
};

typedef base::singletion<RouterApplication> RouterAppSgl;

#endif