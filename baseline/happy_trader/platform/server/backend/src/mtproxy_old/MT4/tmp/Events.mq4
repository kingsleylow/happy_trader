//+------------------------------------------------------------------+
//|                                                       Events.mq4 |
//|                                      Copyright � 2006, komposter |
//|                                      mailto:komposterius@mail.ru |
//+------------------------------------------------------------------+
#property copyright "Copyright � 2006, komposter"
#property link      "mailto:komposterius@mail.ru"

// ������ �������� ������� ���������� �� ���������� ���
int pre_OrdersArray[][2]; // [���������� �������][� ������, ��� �������]

// ���������� �������
int eventBuyClosed_SL  = 0, eventBuyClosed_TP  = 0;
int eventSellClosed_SL = 0, eventSellClosed_TP = 0;
int eventBuyLimitDeleted_Exp  = 0, eventBuyStopDeleted_Exp  = 0;
int eventSellLimitDeleted_Exp = 0, eventSellStopDeleted_Exp = 0;
int eventBuyLimitOpened  = 0, eventBuyStopOpened  = 0;
int eventSellLimitOpened = 0, eventSellStopOpened = 0;

void CheckEvents( int magic = 0 )
{
	// ���� ������� �������
	static bool first = true;
	// ��� ��������� ������
	int _GetLastError = 0;
	// ����� ���������� �������
	int _OrdersTotal = OrdersTotal();
	// ���-�� �������, ��������������� ��������� (������� ���������� � �������� MagicNumber),
	// ���������� �� ������� ���
	int now_OrdersTotal = 0;
	// ���-�� �������, ��������������� ���������, ���������� �� ���������� ���
	static int pre_OrdersTotal = 0;
	// ������ �������� ������� ���������� �� ������� ���
	int now_OrdersArray[][2]; // [� � ������][� ������, ��� �������]
	// ������� ����� ������� � ������� now_OrdersArray (��� ��������)
	int now_CurOrder = 0;
	// ������� ����� ������� � ������� pre_OrdersArray (��� ��������)
	int pre_CurOrder = 0;

	// ������ ��� �������� ���������� �������� ������� ������� ����
	int now_ClosedOrdersArray[6][3]; // [��� ������][��� ��������]
	// ������ ��� �������� ���������� ����������� ���������� �������
	int now_OpenedPendingOrders[4]; // [��� ������]

	// ��������� �����
	bool OrderClosed = true, PendingOrderOpened = false;
	// ��������� ����������
	int ticket = 0, type = -1, close_type = -1;

	//�������� ���������� �������
	eventBuyClosed_SL  = 0; eventBuyClosed_TP  = 0;
	eventSellClosed_SL = 0; eventSellClosed_TP = 0;
	eventBuyLimitDeleted_Exp  = 0; eventBuyStopDeleted_Exp  = 0;
	eventSellLimitDeleted_Exp = 0; eventSellStopDeleted_Exp = 0;
	eventBuyLimitOpened  = 0; eventBuyStopOpened  = 0;
	eventSellLimitOpened = 0; eventSellStopOpened = 0;

	// �������� ������ ������� �������� ������� ��� ������� ���-��
	ArrayResize( now_OrdersArray, MathMax( _OrdersTotal, 1 ) );
	// �������� ������
	ArrayInitialize( now_OrdersArray, 0.0 );

	// �������� ������� �������� ������� � ����������� �������
	ArrayInitialize( now_ClosedOrdersArray, 0.0 );
	ArrayInitialize( now_OpenedPendingOrders, 0.0 );

	//+------------------------------------------------------------------+
	//| ���������� ��� ������� � ���������� � ������ ������ ��, �������
	//| ������������� ���������
	//+------------------------------------------------------------------+
	for ( int z = _OrdersTotal - 1; z >= 0; z -- )
	{
		if ( !OrderSelect( z, SELECT_BY_POS ) )
		{
			_GetLastError = GetLastError();
			Print( "OrderSelect( ", z, ", SELECT_BY_POS ) - Error #", _GetLastError );
			continue;
		}
		// ������� ���������� ������� �� �������� ������� � � �������� MagicNumber
		if ( OrderMagicNumber() == magic && OrderSymbol() == Symbol() )
		{
			now_OrdersArray[now_OrdersTotal][0] = OrderTicket();
			now_OrdersArray[now_OrdersTotal][1] = OrderType();
			now_OrdersTotal ++;
		}
	}
	// �������� ������ ������� �������� ������� ��� ���-�� �������, ��������������� ���������
	ArrayResize( now_OrdersArray, MathMax( now_OrdersTotal, 1 ) );

	//+------------------------------------------------------------------+
	//| ���������� ������ ������� ����������� ����, � ������� ������� ��������� ������� �
	//| ��������� ���������� �������
	//+------------------------------------------------------------------+
	for ( pre_CurOrder = 0; pre_CurOrder < pre_OrdersTotal; pre_CurOrder ++ )
	{
		// ���������� ����� � ��� ������
		ticket = pre_OrdersArray[pre_CurOrder][0];
		type   = pre_OrdersArray[pre_CurOrder][1];
		// ������������, ��� ���� ��� �������, �� ��� ���������
		OrderClosed = true;
		// ������������, ��� ���� ��� ��� ���������� �����, �� �� �� ��������
		PendingOrderOpened = false;

		// ���������� ��� ������� �� �������� ������ �������� �������
		for ( now_CurOrder = 0; now_CurOrder < now_OrdersTotal; now_CurOrder ++ )
		{
			// ���� ������� � ����� ������� ���� � ������,
			if ( ticket == now_OrdersArray[now_CurOrder][0] )
			{
				// ������ ������� �� ���� ������� (����� �� ��� �����)
				OrderClosed = false;

				// ���� � ��� ���������,
				if ( type != now_OrdersArray[now_CurOrder][1] )
				{
					// ������ ��� ��� ���������� �����, � �� ��������
					PendingOrderOpened = true;
				}
				break;
			}
		}
		// ���� ���� ������� ������� (����� �����),
		if ( OrderClosed )
		{
			// �������� �
			if ( !OrderSelect( ticket, SELECT_BY_TICKET ) )
			{
				_GetLastError = GetLastError();
				Print( "OrderSelect( ", ticket, ", SELECT_BY_TICKET ) - Error #", _GetLastError );
				continue;
			}
			// � ����������, ��� ��������� ������� (�������� �����):
			if ( type < 2 )
			{
				// ��� � ����: 0 - �������, 1 - ��, 2 - ��
				close_type = 0;
				if ( StringFind( OrderComment(), "[sl]" ) >= 0 ) close_type = 1;
				if ( StringFind( OrderComment(), "[tp]" ) >= 0 ) close_type = 2;
			}
			else
			{
				// ���������� ������: 0 - �������, 1 - ����� ���������
				close_type = 0;
				if ( StringFind( OrderComment(), "expiration" ) >= 0 ) close_type = 1;
			}
			
			// � ���������� � ������ �������� �������, ��� ����� � ����� type 
			// �������� ��� ��������������� close_type
			now_ClosedOrdersArray[type][close_type] ++;
			continue;
		}
		// ���� �������� ���������� �����,
		if ( PendingOrderOpened )
		{
			// ���������� � ������ ����������� �������, ��� ����� � ����� type ��������
			now_OpenedPendingOrders[type-2] ++;
			continue;
		}
	}

	//+------------------------------------------------------------------+
	//| ��� ����������� ���������� ������� - ��������� ���������� ������� ������ ��������
	//+------------------------------------------------------------------+
	// ���� ��� �� ������ ������ ��������
	if ( !first )
	{
		// ���������� ��� �������� ������� ������������ ���������� �������
		for ( type = 2; type < 6; type ++ )
		{
			// � ���� ������� �� ������ (����� ������ ���� ��������), ������ �������� ����������
			if ( now_OpenedPendingOrders[type-2] > 0 )
				SetOpenEvent( type );
		}

		// ���������� ��� �������� ������� �������� �������
		for ( type = 0; type < 6; type ++ )
		{
			for ( close_type = 0; close_type < 3; close_type ++ )
			{
				// � ���� ������� �� ������ (���� ������� �������), ������ �������� ����������
				if ( now_ClosedOrdersArray[type][close_type] > 0 )
					SetCloseEvent( type, close_type );
			}
		}
	}
	else
	{
		first = false;
	}

	//---- ��������� ������ ������� ������� � ������ ���������� �������
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

