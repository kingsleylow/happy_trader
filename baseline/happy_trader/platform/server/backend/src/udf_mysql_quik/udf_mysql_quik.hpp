#ifndef _UDF_MYSQL_QUIK_INCLUDED
#define _UDF_MYSQL_QUIK_INCLUDED
// Include all externally visible headers
//
#define TARGET_WINDOW_CLASS "{E5E98898-90D3-4ff8-8954-EA6BD7270850}"

// structures
typedef struct _WM_DATASTRUCTURE{
		
		double price_m;
		double volume_m;
		unsigned int hour_m;
		unsigned int minute_m;
		unsigned int second_m;
		char symbol_m[32+1];
		char operation_m[32+1];

}WM_DATASTRUCTURE,*LPWM_DATASTRUCTURE;

#endif // _UDF_MYSQL_QUIK_INCLUDED
