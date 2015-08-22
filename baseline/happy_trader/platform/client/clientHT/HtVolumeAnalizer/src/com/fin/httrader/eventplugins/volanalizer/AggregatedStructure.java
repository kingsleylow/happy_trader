/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.httrader.eventplugins.volanalizer;

import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtException;
import com.fin.httrader.utils.HtLog;
import com.fin.httrader.utils.HtTickCalculator;
import com.fin.httrader.utils.HtUtils;
import java.util.Iterator;
import java.util.SortedSet;
import java.util.TreeMap;
import java.util.logging.Level;

/**
 *
 * @author DanilinS
 */
public class AggregatedStructure {

	public int qty_m = 0;
	public double time_m = -1;
	public String symbol_m = "";
	double open_m = -1;
	double high_m = -1;
	double low_m = -1;
	double close_m = -1;
	// histograms
	public TreeMap<Double, HistogramEntry> histogram_m = new TreeMap<Double, HistogramEntry>();

	public String getContext() {
		return getClass().getSimpleName();
	}

	public AggregatedStructure(String symbol, double stime, int qty, double price) {
		symbol_m = symbol;
		time_m = stime;
		qty_m = qty;

		open_m = price;
		high_m = price;
		low_m = price;
		close_m = price;

	}

	public void aggregate(double price, int qty) {
		qty_m += qty;
		if (price > high_m) {
			high_m = price;
		} else if (price < low_m) {
			low_m = price;
		}

		close_m = price;
	}

	public String toString() {
		StringBuilder out = new StringBuilder();

		out.append("Symbol: ");
		out.append(symbol_m);
		out.append("Time: ");
		out.append(HtDateTimeUtils.time2String_Gmt((long) time_m));
		out.append("Qty: ");
		out.append(qty_m);

		return out.toString();
	}

	public TreeMap<Double, HistogramEntry> createHistogramPerBeans(int beansnum) {


		TreeMap<Double, HistogramEntry> result = new TreeMap<Double, HistogramEntry>();
		if (beansnum < 1) {
			return result;
		}

		//HtTickCalculator tc = new HtTickCalculator();


		double first_e = histogram_m.firstKey();
		double last_e = histogram_m.lastKey();
		double dist_e = last_e - first_e;
		double step_e = dist_e / beansnum;


		SortedSet<Double> subset = null;

		int lastidx = beansnum - 1;
		for (int i = 0; i <= lastidx; i++) {
			double begin_range = first_e + i * step_e;
			double end_range = begin_range + step_e;
			double medi_range = (begin_range + end_range) / 2.0;

			HistogramEntry entry_r = new HistogramEntry();
			for (Iterator<Double> it = histogram_m.keySet().iterator(); it.hasNext();) {
				Double key_t = it.next();
				HistogramEntry entry_t = histogram_m.get(key_t);

				if (i == lastidx) {
					if (key_t>= begin_range && key_t <= end_range) {
						entry_r.qty_m += entry_t.qty_m;
					}
				} else {
					if (key_t>= begin_range && key_t < end_range) {
						entry_r.qty_m += entry_t.qty_m;
					}
				}
			}

			/*
			if (i == lastidx) {
				subset = histogram_m.navigableKeySet().subSet(begin_range, true, end_range, true);

			} else {
				subset = histogram_m.navigableKeySet().subSet(begin_range, true, end_range, false);
			}

			for (Iterator<Double> it = subset.iterator(); it.hasNext();) {
				HistogramEntry entry_i = histogram_m.get(it.next());
				entry_r.qty_m += entry_i.qty_m;
			}
			 */

			result.put(medi_range, entry_r);

		}

		//HtLog.log(Level.INFO, "AggregatedStructure", "createHistogramPerBeans", "Elapsed: " + beansnum + " - " + tc.elapsed());



		return result;
	}
}
