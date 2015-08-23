#ifndef _GERCHIK_RANGE_BREAK2_STATEMACHINE_INCLUDED
#define _GERCHIK_RANGE_BREAK2_STATEMACHINE_INCLUDED

// -------------------------
#define OLD_STATE_BEGIN(STATE)	\
	switch(STATE) {

#define OLD_STATE_ENTRY_BEGIN(STATE_CONST)	\
	case GET_ENUM_ENTRY(STATE_CONST): {

#define OLD_STATE_ENTRY_END()	\
	}	\
	break;

#define OLD_STATE_END()	\
	default: ;	\
 };	

#define OLD_STATE_ENTRY_ALL() \
	{

#define OLD_STATE_ENTRY_ALL_END()	\
	}


// --------------------

#define NEW_STATE_BEGIN(STATE)	{ \
	switch(STATE) {

#define NEW_STATE_ENTRY_BEGIN(STATE_CONST)	\
	case GET_ENUM_ENTRY(STATE_CONST): {

#define NEW_STATE_ENTRY_END()	\
	};	\
	break;

#define NEW_STATE_END()	\
	default: ;	\
	};	}; \

// -------------------

#define NEW_EVENT_BEGIN(EVENT)	{ \
	switch(EVENT) {

#define NEW_EVENT_ENTRY_BEGIN(EVENT_CONST)	\
	case GET_ENUM_ENTRY(EVENT_CONST): {

#define NEW_EVENT_ENTRY_END()	\
	};	\
	break;

#define NEW_EVENT_END()	\
	default: ;	\
};	}; \

// -----------------------------------
#define ACTION_BEGIN(ACTION)	{ \
	switch(ACTION) {

#define ACTION_ENTRY_BEGIN(ACTION_CONST)	\
	case GET_ENUM_ENTRY(ACTION_CONST): {

#define ACTION_ENTRY_END()	\
	};	\
	break;

#define ACTION_END()	\
	default: ;	\
};	}; \

// ----------------------------------

namespace AlgLib {

};

#endif