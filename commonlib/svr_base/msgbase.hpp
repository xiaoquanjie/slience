#ifndef M_SVR_BASE_MSG_BASE_INCLUDE
#define M_SVR_BASE_MSG_BASE_INCLUDE

#define msgid_define(msg, msgid) \
enum message_msgid { \
	msg = msgid, \
};



#endif