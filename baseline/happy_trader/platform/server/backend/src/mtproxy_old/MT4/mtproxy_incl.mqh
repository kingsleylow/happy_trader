#import "htmtproxy_bck.dll"

int initialize(string,string);
int deinitialize();
int getPendingCommand(string );
int getPendingCounterNumber();
int deletePendingCommand();
int pushCommandResult(string, string);
int pushSymbolMetaInfo(string, double, int, int);
int pushPositionMetaInfo(
	string ,
	int ,
	int ,
	double ,
	double ,
	double ,
	double ,
	double ,
	double ,
	double ,
	double ,
	double ,
	double ,
	int	   ,
	string ,
	double ,
	double ,
	double
);
 
int setGlobal(string, string );
int removeGlobal(string );
int removeAllGlobals();
int getGlobal(string , string );