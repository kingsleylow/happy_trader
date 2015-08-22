CREATE DATABASE IF NOT EXISTS `mtrunner` DEFAULT CHARACTER SET utf8;

CREATE TABLE IF NOT EXISTS `mtrunner`.`account_metainfo` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `account_name` varchar(10) DEFAULT NULL,
  `holder_name` varchar(45) DEFAULT NULL,
  `currency` varchar(6) DEFAULT NULL,
  `leverage` int(11) DEFAULT NULL,
  `company_minfo_id` bigint(20) NOT NULL,
  `server` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `company_minfo_id_idx` (`account_name`,`company_minfo_id`)
) ENGINE=InnoDB AUTO_INCREMENT=16 DEFAULT CHARSET=utf8;


CREATE TABLE IF NOT EXISTS `mtrunner`.`account_update` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `account_minfo_id` bigint(20) NOT NULL,
  `equity` bigint(20) DEFAULT NULL,
  `free_margin` bigint(20) DEFAULT NULL,
  `free_margin_mode` smallint(6) DEFAULT NULL,
  `stop_out` bigint(20) DEFAULT NULL,
  `stop_out_mode` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `acc_minfo_id_idx` (`account_minfo_id`)
) ENGINE=InnoDB AUTO_INCREMENT=553 DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `mtrunner`.`balance_update` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `account_minfo_id` bigint(20) NOT NULL,
  `balance` bigint(20) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `acc_minfo_id_idx` (`account_minfo_id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `mtrunner`.`company_metainfo` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `company_name` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `company_name_idx` (`company_name`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=16 DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `mtrunner`.`position` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `account_minfo_id` bigint(20) NOT NULL,
  `trade_symbol` varchar(10) NOT NULL,
  `ticket` varchar(15) DEFAULT NULL,
  `comment` varchar(70) DEFAULT NULL,
  `open_time` bigint(20) NOT NULL,
  `close_time` bigint(20) NOT NULL,
  `open_price` bigint(20) DEFAULT NULL,
  `close_price` bigint(20) DEFAULT NULL,
  `init_sl` bigint(20) DEFAULT NULL,
  `init_tp` bigint(20) DEFAULT NULL,
  `profit` bigint(20) DEFAULT NULL,
  `expiration` bigint(20) DEFAULT NULL,
  `direction` smallint(6) DEFAULT NULL,
  `pos_state` smallint(6) DEFAULT NULL,
  `pos_type` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`id`,`account_minfo_id`),
  UNIQUE KEY `account_minfo_id_idx_2` (`account_minfo_id`,`ticket`),
  KEY `account_minfo_id_idx_1` (`account_minfo_id`,`trade_symbol`),
  KEY `open_time_idx` (`open_time`),
  KEY `close_time_idx` (`close_time`)
) ENGINE=InnoDB AUTO_INCREMENT=64 DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `mtrunner`.`position_update` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `position_id` bigint(20) NOT NULL,
  `sl` bigint(20) DEFAULT NULL,
  `tp` bigint(20) DEFAULT NULL,
  `profit` bigint(20) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `position_id_idx` (`position_id`)
) ENGINE=InnoDB AUTO_INCREMENT=306 DEFAULT CHARSET=utf8;

