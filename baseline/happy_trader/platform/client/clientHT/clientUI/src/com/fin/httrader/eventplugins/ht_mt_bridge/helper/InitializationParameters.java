/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins.ht_mt_bridge.helper;

import com.fin.httrader.utils.IntParam;
import com.fin.httrader.utils.LongParam;
import java.util.Map;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

/**
 *
 * @author Victor_Zoubok
 */
public class InitializationParameters {

		private final StringBuilder currentServerName_m = new StringBuilder();
		private final StringBuilder currentAlgLibName_m = new StringBuilder();
		private final StringBuilder eventPluginName_m = new StringBuilder();
		private int currentThread_m = -1;
		final ReentrantReadWriteLock lock_m = new ReentrantReadWriteLock();

		public void getAllData(StringBuilder currentServerName, StringBuilder currentAlgLibName, StringBuilder eventPluginName, IntParam tid) {
				try {
						lock_m.readLock().lock();
						if (currentServerName != null) {
								currentServerName.setLength(0);
								currentServerName.append(currentServerName_m);
						}

						if (currentAlgLibName != null) {
								currentAlgLibName.setLength(0);
								currentAlgLibName.append(currentAlgLibName_m);
						}

						if (eventPluginName != null) {
								eventPluginName.setLength(0);
								eventPluginName.append(eventPluginName_m);
						}

						if (tid != null) {
								tid.setInt(currentThread_m);
						}

				} finally {
						lock_m.readLock().unlock();
				}
		}

		public String getCurrentServerName() {
				try {
						lock_m.readLock().lock();
						return currentServerName_m.toString();
				} finally {
						lock_m.readLock().unlock();
				}

		}

		public String getCurrentAlgLibName() {

				try {
						lock_m.readLock().lock();
						return currentAlgLibName_m.toString();
				} finally {
						lock_m.readLock().unlock();
				}
		}

		public String getEventPluginName() {

				try {
						lock_m.readLock().lock();
						return eventPluginName_m.toString();
				} finally {
						lock_m.readLock().unlock();
				}
		}

		public int getCurrentThread() {
				try {
						lock_m.readLock().lock();
						return currentThread_m;
				} finally {
						lock_m.readLock().unlock();
				}
		}

		public void initialize(String currentServerName, String currentAlgLibName, String eventPluginName, int currentThread) {

				try {
						lock_m.writeLock().lock();

						currentServerName_m.append(currentServerName);
						currentAlgLibName_m.append(currentAlgLibName);
						eventPluginName_m.append(eventPluginName);
						currentThread_m = currentThread;
				} finally {
						lock_m.writeLock().unlock();
				}
		}

		public void clear() {
				try {
						lock_m.writeLock().lock();

						currentServerName_m.setLength(0);
						currentAlgLibName_m.setLength(0);
						eventPluginName_m.setLength(0);
						currentThread_m = -1;

				} finally {
						lock_m.writeLock().unlock();
				}
		}

}
