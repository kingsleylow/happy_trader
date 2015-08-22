/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package com.fin.httrader.configprops.helper;

/**
 *
 * @author Victor_Zoubok
 */
public class LoadedDllsHelper {
		public static final int DETECTED_DUMMY = 0x0000;
		public static final int DETECTED_DEBUG_VER = 0x0001;
		public static final int DETECTED_RELEASE_VER = 0x0002;
		
		private int detectedLibraries_m =  DETECTED_DUMMY;
		private int loadedJNILibraries_m =  DETECTED_DUMMY;

		public LoadedDllsHelper(int detectedLibraries, int loadedJNILibraries)
		{
				detectedLibraries_m = detectedLibraries;
				loadedJNILibraries_m = loadedJNILibraries;
		}
		
		public int getDetectedLibraries() {
				return detectedLibraries_m;
		}

				
		public int getLoadedJNILibraries() {
				return loadedJNILibraries_m;
		}

		
		
		
}
