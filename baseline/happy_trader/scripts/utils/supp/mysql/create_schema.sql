-- phpMyAdmin SQL Dump
-- version 2.9.2
-- http://www.phpmyadmin.net
-- 
-- Хост: localhost
-- Время создания: Июл 29 2009 г., 19:16
-- Версия сервера: 5.0.45
-- Версия PHP: 5.2.6
-- 
-- База данных: `htrader`
-- 

-- --------------------------------------------------------

-- 
-- Структура таблицы `alerts`
-- 

DROP TABLE IF EXISTS `alerts`;
CREATE TABLE IF NOT EXISTS `alerts` (
  `id` varchar(32) NOT NULL,
  `source` varchar(32) default NULL,
  `alg_brk_pair` varchar(64) default NULL,
  `thread` int(11) default NULL,
  `priority` smallint(6) default NULL,
  `alert_time` double default NULL,
  `data` varchar(16384) default NULL,
  `runname_id` int(10) unsigned NOT NULL,
  `small_data` varchar(256) default NULL,
  `tid` int(10) unsigned NOT NULL auto_increment,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `ALERTS_U0` USING BTREE (`tid`),
  KEY `ALERTS_I0` (`alert_time`,`source`),
  KEY `ALERTS_I2` (`small_data`),
  KEY `ALERTS_I1` USING BTREE (`runname_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1250 COMMENT='InnoDB free: 947200 kB' AUTO_INCREMENT=184292 ;

-- --------------------------------------------------------

-- 
-- Структура таблицы `broker_runnames`
-- 

DROP TABLE IF EXISTS `broker_runnames`;
CREATE TABLE IF NOT EXISTS `broker_runnames` (
  `id` int(11) NOT NULL auto_increment,
  `run_name` varchar(256) NOT NULL,
  `comment` varchar(4096) default NULL,
  `op_first_time` double NOT NULL,
  `user_data` varchar(512) default NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `Index_2` (`run_name`),
  KEY `Index_3` (`op_first_time`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1250 AUTO_INCREMENT=359 ;

-- --------------------------------------------------------

-- 
-- Структура таблицы `broker_sessions`
-- 

DROP TABLE IF EXISTS `broker_sessions`;
CREATE TABLE IF NOT EXISTS `broker_sessions` (
  `session_id` varchar(32) NOT NULL,
  `create_time` double default NULL,
  `is_simulation` smallint(6) default NULL,
  `connect_string` varchar(256) default NULL,
  PRIMARY KEY  (`session_id`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1250;

-- --------------------------------------------------------

-- 
-- Структура таблицы `broker_sessions_oper`
-- 

DROP TABLE IF EXISTS `broker_sessions_oper`;
CREATE TABLE IF NOT EXISTS `broker_sessions_oper` (
  `id` int(11) NOT NULL auto_increment,
  `session_id` varchar(32) default NULL,
  `alg_brk_pair` varchar(64) default NULL,
  `thread` int(11) default NULL,
  `op_time` double default NULL,
  `operation` smallint(6) default NULL,
  `runname_id` int(11) NOT NULL,
  PRIMARY KEY  (`id`),
  KEY `BROKER_SESSIONS_OPER_I0` (`session_id`),
  KEY `BROKER_SESSIONS_OPER_I2` (`op_time`,`thread`,`alg_brk_pair`),
  KEY `BROKER_SESSIONS_OPER_I3` (`runname_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1250 AUTO_INCREMENT=299 ;

-- --------------------------------------------------------

-- 
-- Структура таблицы `broker_symbols_metainfo`
-- 

DROP TABLE IF EXISTS `broker_symbols_metainfo`;
CREATE TABLE IF NOT EXISTS `broker_symbols_metainfo` (
  `session_id` varchar(32) default NULL,
  `symbol_num` varchar(32) default NULL,
  `symbol_denom` varchar(32) default NULL,
  `sign_digits` smallint(6) default NULL,
  `point_value` double default NULL,
  `spread` double default NULL,
  KEY `BROKER_SYMBOLS_METAINFO_U1` USING BTREE (`session_id`,`symbol_num`,`symbol_denom`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1250;

-- --------------------------------------------------------

-- 
-- Структура таблицы `commonlog`
-- 

DROP TABLE IF EXISTS `commonlog`;
CREATE TABLE IF NOT EXISTS `commonlog` (
  `id` int(11) NOT NULL auto_increment,
  `tlast_created` bigint(20) NOT NULL,
  `logtype` int(11) NOT NULL,
  `level` int(11) NOT NULL,
  `thread` int(11) default NULL,
  `session` varchar(64) default NULL,
  `context` varchar(64) default NULL,
  `content` mediumblob,
  PRIMARY KEY  (`id`),
  KEY `COMMONLOG_I1` (`tlast_created`,`logtype`),
  KEY `COMMONLOG_I2` (`context`),
  KEY `COMMONLOG_I3` (`thread`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1250 AUTO_INCREMENT=7945419 ;

-- --------------------------------------------------------

-- 
-- Структура таблицы `exportimportlog`
-- 

DROP TABLE IF EXISTS `exportimportlog`;
CREATE TABLE IF NOT EXISTS `exportimportlog` (
  `id` int(11) NOT NULL auto_increment,
  `groupid` varchar(32) NOT NULL,
  `tlast_created` bigint(20) NOT NULL,
  `opertype` smallint(6) default NULL,
  `operstatus` smallint(6) default NULL,
  `opermessage` varchar(4096) default NULL,
  `symbol_id` smallint(6) default NULL,
  `tdata` varchar(4096) default NULL,
  PRIMARY KEY  (`id`),
  KEY `EXPORTIMPORTLOG_I1` (`opertype`,`groupid`),
  KEY `EXPORTIMPORTLOG_I2` (`tlast_created`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1250 AUTO_INCREMENT=201388 ;

-- --------------------------------------------------------

-- 
-- Структура таблицы `order_responses`
-- 

DROP TABLE IF EXISTS `order_responses`;
CREATE TABLE IF NOT EXISTS `order_responses` (
  `source` varchar(32) default NULL,
  `id` varchar(32) NOT NULL,
  `op_issue_time` double default NULL,
  `parent_id` varchar(32) default NULL,
  `broker_comment` varchar(512) default NULL,
  `broker_internal_code` bigint(20) default NULL,
  `result_code` bigint(20) default NULL,
  `result_detail_code` bigint(20) default NULL,
  `position_type` smallint(6) default NULL,
  `broker_position_id` varchar(32) default NULL,
  `op_open_price` double default NULL,
  `op_close_price` double default NULL,
  `op_tp_price` double default NULL,
  `op_sl_price` double default NULL,
  `op_volume` double default NULL,
  `sign_digits` smallint(6) default NULL,
  `provider` varchar(32) default NULL,
  `symbol_num` varchar(32) default NULL,
  `symbol_denom` varchar(32) default NULL,
  `op_expiration_time` double default NULL,
  `op_open_time` double default NULL,
  `op_close_time` double default NULL,
  `op_close_reason` double default NULL,
  `op_comission` double default NULL,
  `point_value` double default NULL,
  PRIMARY KEY  (`id`),
  KEY `ORDER_RESPONSES_I0` (`op_issue_time`),
  KEY `ORDER_RESPONSES_I1` (`parent_id`),
  KEY `ORDER_RESPONSES_I2` (`result_code`,`result_detail_code`),
  KEY `ORDER_RESPONSES_I3` (`broker_internal_code`),
  KEY `ORDER_RESPONSES_I4` (`broker_position_id`),
  KEY `ORDER_RESPONSES_I5` (`source`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1250;

-- --------------------------------------------------------

-- 
-- Структура таблицы `orders`
-- 

DROP TABLE IF EXISTS `orders`;
CREATE TABLE IF NOT EXISTS `orders` (
  `source` varchar(32) default NULL,
  `id` varchar(32) NOT NULL,
  `broker_position_id` varchar(32) default NULL,
  `order_type` bigint(20) default NULL,
  `provider` varchar(32) default NULL,
  `symbol_num` varchar(32) default NULL,
  `symbol_denom` varchar(32) default NULL,
  `report_symbol` varchar(32) default NULL,
  `comment` varchar(512) default NULL,
  `or_issue_time` double default NULL,
  `or_time` double default NULL,
  `or_price` double default NULL,
  `or_sl_price` double default NULL,
  `or_tp_price` double default NULL,
  `or_volume` double default NULL,
  `sign_digits` smallint(6) default NULL,
  `or_expiration_time` double default NULL,
  PRIMARY KEY  (`id`),
  KEY `ORDERS_I0` (`or_issue_time`),
  KEY `ORDERS_I1` (`broker_position_id`),
  KEY `ORDERS_I2` (`order_type`),
  KEY `ORDERS_I3` (`source`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1250;

-- --------------------------------------------------------

-- 
-- Структура таблицы `positions`
-- 

DROP TABLE IF EXISTS `positions`;
CREATE TABLE IF NOT EXISTS `positions` (
  `id` int(11) NOT NULL auto_increment,
  `tlast_created` datetime NOT NULL,
  `tlast_updated` datetime NOT NULL,
  `broker_position_id` varchar(32) default NULL,
  `symbol_id` smallint(6) default NULL,
  `open_time` double default NULL,
  `close_time` double default NULL,
  `volume` double default NULL,
  `state` smallint(6) default NULL,
  `broker_id` varchar(64) default NULL,
  `open_price` double default NULL,
  `close_price` double default NULL,
  `profit_trade_currency` double default NULL,
  `trade_currency` varchar(6) default NULL,
  PRIMARY KEY  (`id`),
  KEY `POSITIONS_I1` (`tlast_created`,`symbol_id`),
  KEY `POSITIONS_I2` (`broker_position_id`,`symbol_id`)
) ENGINE=InnoDB DEFAULT CHARSET=cp1250 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

-- 
-- Структура таблицы `symbols`
-- 

DROP TABLE IF EXISTS `symbols`;
CREATE TABLE IF NOT EXISTS `symbols` (
  `tprovider` varchar(32) default NULL,
  `tsymbol` varchar(32) default NULL,
  `tsymbol_id` smallint(6) NOT NULL auto_increment,
  PRIMARY KEY  (`tsymbol_id`),
  UNIQUE KEY `SYMBOLS_U1` (`tprovider`,`tsymbol`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1250 AUTO_INCREMENT=38 ;

-- --------------------------------------------------------

-- 
-- Структура таблицы `user_sessions`
-- 

DROP TABLE IF EXISTS `user_sessions`;
CREATE TABLE IF NOT EXISTS `user_sessions` (
  `id` int(11) NOT NULL auto_increment,
  `user_id` int(11) NOT NULL,
  `session_id` varchar(32) NOT NULL,
  `last_login` double NOT NULL,
  `disabled` smallint(6) NOT NULL,
  `user_ip` varchar(32) default NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `USER_SESSIONS_I2` (`session_id`),
  KEY `USER_SESSIONS_I1` (`user_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1250 AUTO_INCREMENT=152 ;

-- --------------------------------------------------------

-- 
-- Структура таблицы `users`
-- 

DROP TABLE IF EXISTS `users`;
CREATE TABLE IF NOT EXISTS `users` (
  `id` int(11) NOT NULL auto_increment,
  `username` varchar(64) NOT NULL,
  `password` varchar(64) NOT NULL,
  `disabled` smallint(6) NOT NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `USERS_I1` (`username`)
) ENGINE=InnoDB  DEFAULT CHARSET=cp1250 AUTO_INCREMENT=4 ;
