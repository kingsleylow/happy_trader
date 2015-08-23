//+------------------------------------------------------------------+
//|                                                       Events.mq4 |
//|                                      Copyright © 2006, komposter |
//|                                      mailto:komposterius@mail.ru |
//+------------------------------------------------------------------+
#property copyright "Copyright © 2006, komposter"
#property link      "mailto:komposterius@mail.ru"

// массив открытых позиций состоянием на предыдущий тик
int pre_OrdersArray[][2]; // [количество позиций][№ тикета, тип позиции]

// переменные событий
int eventBuyClosed_SL  = 0, eventBuyClosed_TP  = 0;
int eventSellClosed_SL = 0, eventSellClosed_TP = 0;
int eventBuyLimitDeleted_Exp  = 0, eventBuyStopDeleted_Exp  = 0;
int eventSellLimitDeleted_Exp = 0, eventSellStopDeleted_Exp = 0;
int eventBuyLimitOpened  = 0, eventBuyStopOpened  = 0;
int eventSellLimitOpened = 0, eventSellStopOpened = 0;

void CheckEvents( int magic = 0 )
{
	// флаг первого запуска
	static bool first = true;
	// код последней ошибки
	int _GetLastError = 0;
	// общее количество позиций
	int _OrdersTotal = OrdersTotal();
	// кол-во позиций, соответствующих критериям (текущий инструмент и заданный MagicNumber),
	// состоянием на текущий тик
	int now_OrdersTotal = 0;
	// кол-во позиций, соответствующих критериям, состоянием на предыдущий тик
	static int pre_OrdersTotal = 0;
	// массив открытых позиций состоянием на текущий тик
	int now_OrdersArray[][2]; // [№ в списке][№ тикета, тип позиции]
	// текущий номер позиции в массиве now_OrdersArray (для перебора)
	int now_CurOrder = 0;
	// текущий номер позиции в массиве pre_OrdersArray (для перебора)
	int pre_CurOrder = 0;

	// массив для хранения количества закрытых позиций каждого типа
	int now_ClosedOrdersArray[6][3]; // [тип ордера][тип закрытия]
	// массив для хранения количества сработавших отложенных ордеров
	int now_OpenedPendingOrders[4]; // [тип ордера]

	// временные флаги
	bool OrderClosed = true, PendingOrderOpened = false;
	// временные переменные
	int ticket = 0, type = -1, close_type = -1;

	//обнуляем переменные событий
	eventBuyClosed_SL  = 0; eventBuyClosed_TP  = 0;
	eventSellClosed_SL = 0; eventSellClosed_TP = 0;
	eventBuyLimitDeleted_Exp  = 0; eventBuyStopDeleted_Exp  = 0;
	eventSellLimitDeleted_Exp = 0; eventSellStopDeleted_Exp = 0;
	eventBuyLimitOpened  = 0; eventBuyStopOpened  = 0;
	eventSellLimitOpened = 0; eventSellStopOpened = 0;

	// изменяем размер массива открытых позиций под текущее кол-во
	ArrayResize( now_OrdersArray, MathMax( _OrdersTotal, 1 ) );
	// обнуляем массив
	ArrayInitialize( now_OrdersArray, 0.0 );

	// обнуляем массивы закрытых позиций и сработавших ордеров
	ArrayInitialize( now_ClosedOrdersArray, 0.0 );
	ArrayInitialize( now_OpenedPendingOrders, 0.0 );

	//+------------------------------------------------------------------+
	//| Перебираем все позиции и записываем в массив только те, которые
	//| соответствуют критериям
	//+------------------------------------------------------------------+
	for ( int z = _OrdersTotal - 1; z >= 0; z -- )
	{
		if ( !OrderSelect( z, SELECT_BY_POS ) )
		{
			_GetLastError = GetLastError();
			Print( "OrderSelect( ", z, ", SELECT_BY_POS ) - Error #", _GetLastError );
			continue;
		}
		// Считаем количество ордеров по текущему символу и с заданным MagicNumber
		if ( OrderMagicNumber() == magic && OrderSymbol() == Symbol() )
		{
			now_OrdersArray[now_OrdersTotal][0] = OrderTicket();
			now_OrdersArray[now_OrdersTotal][1] = OrderType();
			now_OrdersTotal ++;
		}
	}
	// изменяем размер массива открытых позиций под кол-во позиций, соответствующих критериям
	ArrayResize( now_OrdersArray, MathMax( now_OrdersTotal, 1 ) );

	//+------------------------------------------------------------------+
	//| Перебираем список позиций предыдущего тика, и считаем сколько закрылось позиций и
	//| сработало отложенных ордеров
	//+------------------------------------------------------------------+
	for ( pre_CurOrder = 0; pre_CurOrder < pre_OrdersTotal; pre_CurOrder ++ )
	{
		// запоминаем тикет и тип ордера
		ticket = pre_OrdersArray[pre_CurOrder][0];
		type   = pre_OrdersArray[pre_CurOrder][1];
		// предпологаем, что если это позиция, то она закрылась
		OrderClosed = true;
		// предполагаем, что если это был отложенный ордер, то он не сработал
		PendingOrderOpened = false;

		// перебираем все позиции из текущего списка открытых позиций
		for ( now_CurOrder = 0; now_CurOrder < now_OrdersTotal; now_CurOrder ++ )
		{
			// если позиция с таким тикетом есть в списке,
			if ( ticket == now_OrdersArray[now_CurOrder][0] )
			{
				// значит позиция не была закрыта (ордер не был удалён)
				OrderClosed = false;

				// если её тип поменялся,
				if ( type != now_OrdersArray[now_CurOrder][1] )
				{
					// значит это был отложенный ордер, и он сработал
					PendingOrderOpened = true;
				}
				break;
			}
		}
		// если была закрыта позиция (удалён ордер),
		if ( OrderClosed )
		{
			// выбираем её
			if ( !OrderSelect( ticket, SELECT_BY_TICKET ) )
			{
				_GetLastError = GetLastError();
				Print( "OrderSelect( ", ticket, ", SELECT_BY_TICKET ) - Error #", _GetLastError );
				continue;
			}
			// и определяем, КАК закрылась позиция (удалился ордер):
			if ( type < 2 )
			{
				// Бай и Селл: 0 - вручную, 1 - СЛ, 2 - ТП
				close_type = 0;
				if ( StringFind( OrderComment(), "[sl]" ) >= 0 ) close_type = 1;
				if ( StringFind( OrderComment(), "[tp]" ) >= 0 ) close_type = 2;
			}
			else
			{
				// Отложенные ордера: 0 - вручную, 1 - время истечения
				close_type = 0;
				if ( StringFind( OrderComment(), "expiration" ) >= 0 ) close_type = 1;
			}
			
			// и записываем в массив закрытых ордеров, что ордер с типом type 
			// закрылся при обстоятельствах close_type
			now_ClosedOrdersArray[type][close_type] ++;
			continue;
		}
		// если сработал отложенный ордер,
		if ( PendingOrderOpened )
		{
			// записываем в массив сработавших ордеров, что ордер с типом type сработал
			now_OpenedPendingOrders[type-2] ++;
			continue;
		}
	}

	//+------------------------------------------------------------------+
	//| Всю необходимую информацию собрали - назначаем переменным событий нужные значения
	//+------------------------------------------------------------------+
	// если это не первый запуск эксперта
	if ( !first )
	{
		// перебираем все элементы массива срабатывания отложенных ордеров
		for ( type = 2; type < 6; type ++ )
		{
			// и если элемент не пустой (ордер такого типа сработал), меняем значение переменной
			if ( now_OpenedPendingOrders[type-2] > 0 )
				SetOpenEvent( type );
		}

		// перебираем все элементы массива закрытых позиций
		for ( type = 0; type < 6; type ++ )
		{
			for ( close_type = 0; close_type < 3; close_type ++ )
			{
				// и если элемент не пустой (была закрыта позиция), меняем значение переменной
				if ( now_ClosedOrdersArray[type][close_type] > 0 )
					SetCloseEvent( type, close_type );
			}
		}
	}
	else
	{
		first = false;
	}

	//---- сохраняем массив текущих позиций в массив предыдущих позиций
	ArrayResize( pre_OrdersArray, MathMax( now_OrdersTotal, 1 ) );
	for ( now_CurOrder = 0; now_CurOrder < now_OrdersTotal; now_CurOrder ++ )
	{
		pre_OrdersArray[now_CurOrder][0] = now_OrdersArray[now_CurOrder][0];
		pre_OrdersArray[now_CurOrder][1] = now_OrdersArray[now_CurOrder][1];
	}
	pre_OrdersTotal = now_OrdersTotal;
}
void SetOpenEvent( int SetOpenEvent_type )
{
	switch ( SetOpenEvent_type )
	{
		case OP_BUYLIMIT: eventBuyLimitOpened ++; return(0);
		case OP_BUYSTOP: eventBuyStopOpened ++; return(0);
		case OP_SELLLIMIT: eventSellLimitOpened ++; return(0);
		case OP_SELLSTOP: eventSellStopOpened ++; return(0);
	}
}
void SetCloseEvent( int SetCloseEvent_type, int SetCloseEvent_close_type )
{
	switch ( SetCloseEvent_type )
	{
		case OP_BUY:
		{
			if ( SetCloseEvent_close_type == 1 ) eventBuyClosed_SL ++;
			if ( SetCloseEvent_close_type == 2 ) eventBuyClosed_TP ++;
			return(0);
		}
		case OP_SELL:
		{
			if ( SetCloseEvent_close_type == 1 ) eventSellClosed_SL ++;
			if ( SetCloseEvent_close_type == 2 ) eventSellClosed_TP ++;
			return(0);
		}
		case OP_BUYLIMIT:
		{
			if ( SetCloseEvent_close_type == 1 ) eventBuyLimitDeleted_Exp ++;
			return(0);
		}
		case OP_BUYSTOP:
		{
			if ( SetCloseEvent_close_type == 1 ) eventBuyStopDeleted_Exp ++;
			return(0);
		}
		case OP_SELLLIMIT:
		{
			if ( SetCloseEvent_close_type == 1 ) eventSellLimitDeleted_Exp ++;
			return(0);
		}
		case OP_SELLSTOP:
		{
			if ( SetCloseEvent_close_type == 1 ) eventSellStopDeleted_Exp ++;
			return(0);
		}
	}
}

