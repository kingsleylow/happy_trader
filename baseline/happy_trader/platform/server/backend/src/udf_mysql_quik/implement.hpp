#ifndef _UDF_MYSQL_QUIK_IMPL_INCLUDED
#define _UDF_MYSQL_QUIK_IMPL_INCLUDED


#include "udf_mysql_quikdefs.hpp"
/*


DROP TABLE IF EXISTS `quik`.`quik_ticks`;
CREATE TABLE  `quik`.`quik_ticks` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `Symbol` varchar(32) NOT NULL,
  `price` double NOT NULL,
  `vol` double unsigned NOT NULL,
  `Operation` varchar(32) NOT NULL,
  `T` time NOT NULL,
  PRIMARY KEY (`id`),
  KEY `quik_ticks_i1` (`Symbol`),
  KEY `quik_ticks_i2` (`T`)
) ENGINE=MyISAM AUTO_INCREMENT=2429222 DEFAULT CHARSET=utf8;


drop function IF EXISTS send_quik_data;
CREATE FUNCTION send_quik_data RETURNS STRING SONAME 'htudf_mysql_quik_bck.dll';
DROP TRIGGER IF EXISTS quik_ticks_fire;
DELIMITER $$
CREATE  TRIGGER quik_ticks_fire BEFORE insert ON quik_ticks FOR EACH ROW begin
    
    set result = send_quik_data(new.Symbol, new.Price, new.Vol,   HOUR(new.T),MINUTE(new.T),SECOND(new.T), new.Operation);

end $$

DELIMITER ;
*/

namespace AlgLib {

	
	extern "C" UDF_MYSQL_QUIK_EXP my_bool send_quik_data_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
	extern "C" UDF_MYSQL_QUIK_EXP void send_quik_data_deinit(UDF_INIT *initid);
	extern "C" UDF_MYSQL_QUIK_EXP char* send_quik_data(UDF_INIT *initid, UDF_ARGS *args, char *result, unsigned long *length, char *is_null, char *error);
	
	// helpers
	void init_returnString(char const* str, char *result, unsigned long *length,char *is_null);
	BOOL WINAPI TransferData(HWND hServer, DWORD dwMsg, LPWM_DATASTRUCTURE dataToSend, DWORD dwBytes);


}; // end of namespace 


#endif // _UDF_MYSQL_QUIK_IMPL_INCLUDED
