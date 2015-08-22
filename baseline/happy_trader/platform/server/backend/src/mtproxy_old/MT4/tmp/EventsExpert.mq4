//+------------------------------------------------------------------+
//|                                                 EventsExpert.mq4 |
//|                                      Copyright © 2006, komposter |
//|                                      mailto:komposterius@mail.ru |
//+------------------------------------------------------------------+
#property copyright "Copyright © 2006, komposter"
#property link      "mailto:komposterius@mail.ru"

extern int MagicNumber = 0;

#include <Events.mq4>

int start()
{
	CheckEvents( MagicNumber );

	if ( eventBuyClosed_SL > 0 )
		Alert( Symbol(), ": Buy-позиция закрыта по СтопЛоссу!" );

	if ( eventBuyClosed_TP > 0 )
		Alert( Symbol(), ": Buy-позиция закрыта по ТейкПрофиту!" );

	if ( eventBuyLimitOpened > 0 || eventBuyStopOpened > 0 || 
		  eventSellLimitOpened > 0 || eventSellStopOpened > 0 )
		Alert( Symbol(), ": сработал отложенный ордер!" );
return(0);
}

