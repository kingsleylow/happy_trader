/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.eventplugins.ht_mt_bridge.persistent;

import com.fin.httrader.utils.HtException;
import org.hibernate.Session;
import org.hibernate.Transaction;

/**
 *
 * @author Victor_Zoubok
 */
public class TransactionManager {
		private Session session_m = null;
		private Transaction transaction_m = null;
		
		private String getContext() {
				return getClass().getSimpleName();
		}
		
		public TransactionManager(Session session)
		{
				session_m = session;
		}
		
		public Session getSession()
		{
				return session_m;
		}
		
		public void startTransaction()
		{
			
				transaction_m = session_m.beginTransaction();
		}
		
		public void commitTransaction() throws Exception
		{
				if (transaction_m != null) {
						transaction_m.commit();
						transaction_m = null;
				}
				else
						throw new HtException(getContext(), "commitTransaction", "Transaction not started");
		}
		
		public void rollBackTransaction() throws Exception
		{
				if (transaction_m != null) {
						transaction_m.rollback();
						transaction_m = null;
				}
				else
						throw new HtException(getContext(), "commitTransaction", "Transaction not started");
		}
		
}
