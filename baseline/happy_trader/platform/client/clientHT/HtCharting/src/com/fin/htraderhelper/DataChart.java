/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package com.fin.htraderhelper;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.text.DecimalFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.List;
import java.util.Map;
import javax.swing.JFrame;
import javax.swing.JPanel;
import org.jfree.ui.ApplicationFrame;
import org.jfree.chart.*;
import org.jfree.chart.axis.AxisLocation;
import org.jfree.chart.axis.DateAxis;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.entity.ChartEntity;
import org.jfree.chart.entity.XYItemEntity;
import org.jfree.chart.labels.StandardXYToolTipGenerator;
import org.jfree.chart.plot.CombinedDomainXYPlot;
import org.jfree.chart.plot.Plot;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.ValueMarker;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.*;
import org.jfree.data.statistics.DefaultMultiValueCategoryDataset;
import org.jfree.data.time.*;
import org.jfree.data.time.ohlc.OHLCSeries;
import org.jfree.data.time.ohlc.OHLCSeriesCollection;
import org.jfree.data.xy.DefaultXYZDataset;
import org.jfree.data.xy.IntervalXYDataset;
import org.jfree.data.xy.XYBarDataset;
import org.jfree.data.xy.XYDataset;

import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;
import org.jfree.ui.RectangleAnchor;
import org.jfree.ui.RefineryUtilities;
import org.jfree.ui.TextAnchor;

import com.fin.htraderhelper.utils.*;
import java.awt.FlowLayout;
import java.awt.event.ActionListener;
import javax.swing.AbstractButton;
import javax.swing.JButton;
import org.jfree.chart.labels.XYToolTipGenerator;

/**
 *
 * @author DanilinS
 */
public class DataChart extends JFrame {

	private HtDataProvider dataProvider_m = null;
	private SymbolKey symbol_m = null;
	

	private Map<Long, List<HtDrawableObject.Text>> textObjectsMap_m = new HashMap<Long, List<HtDrawableObject.Text>>();
	private long curMarkerTime_m = -1;

	CombinedDomainXYPlot combinedPlot_m = null;


	// -----------------------
	public DataChart(SymbolKey symbol, HtDataProvider provider) throws Exception {


		super(symbol.toString());
		dataProvider_m = provider;
		symbol_m = symbol;

		JFreeChart jfreechart = createChart();
		ChartPanel chartpanel = new ChartPanel(jfreechart, true, true, true, false, true);
		chartpanel.setPreferredSize(new Dimension(500, 270));
		setContentPane(chartpanel);

		// add mouse listener
		ChartMouseListener listener = new ChartMouseListener() {

			public void chartMouseClicked(ChartMouseEvent evt) {
				showAllTextObjects();
			}

			public void chartMouseMoved(ChartMouseEvent evt) {
				drawTimeMarkerViaMouseMove(evt);
			}
		};

		chartpanel.addChartMouseListener(listener);

		// create jbutton
		final DataChart base = this;

		//JPanel jpanel = new JPanel(new FlowLayout());
		


		JButton btn = new JButton("Text Objects");
		//jpanel.add(btn);
		//this.add(jpanel, "South");
		chartpanel.add(btn);


		btn.setActionCommand("show_texts_objects");
		btn.setVerticalTextPosition(AbstractButton.CENTER);
    btn.setHorizontalTextPosition(AbstractButton.RIGHT); //aka LEFT, for left-to-right locales
		btn.setVisible(true);

		btn.addActionListener(
				new ActionListener() {

					public void actionPerformed(ActionEvent e) {
						if (e.getActionCommand().equals("show_texts_objects")) {
							// our click
							try {
								HtTextListBox tbox = new HtTextListBox(textObjectsMap_m, base);
								tbox.setVisible(true);
							}
							catch(Throwable ex)
							{
								new MsgBox(base, "Exception happend: " + ex.getMessage(), false);
							}
							
						}
					}
				}
		);

		

	}

	public void drawTimeMarkerViaExtrenalCall(long ttime)
	{
		try {
			if (ttime > 0) {
				addMarkersToAllPlots(ttime);
			}
		} catch (Throwable e) {
			new MsgBox(this, "Exception happend: " + e.getMessage(), false);
		}
	}
	

	// -------------------
	private void showAllTextObjects() {
		if (curMarkerTime_m < 0) {
			return;
		}

		try {
			// extract text object
			List<HtDrawableObject.Text> text_obj_list = getTextObjectsForTime(curMarkerTime_m);

			// if we have it we need to


			// show
			HtTextBox tbox = new HtTextBox(text_obj_list, curMarkerTime_m);
			tbox.setVisible(true);
		} catch (Throwable e2) {
			//System.out.println("Exception: " + e2.getMessage());
		}
	}

	private void drawTimeMarkerViaMouseMove(ChartMouseEvent evt) {
		ChartEntity entity = evt.getEntity();


		if (entity != null) {



			// cast to this entity
			try {
				XYItemEntity entityXY = (XYItemEntity) entity;
				XYDataset ds = entityXY.getDataset();

				curMarkerTime_m = (long) ds.getXValue(entityXY.getSeriesIndex(), entityXY.getItem());

				// set up a marker
				//System.out.println("Found time: " + formatDateTime(tval));


				// try to cast
				//CombinedDomainXYPlot combinedplot = (CombinedDomainXYPlot) evt.getChart().getPlot();
				addMarkersToAllPlots(curMarkerTime_m);



			} catch (ClassCastException e) {
				//System.out.println("Class cast exception: " + e.getMessage());
			} catch (Throwable e2) {
				//System.out.println("Exception: " + e2.getMessage());
			}


		}
	}

	// helpers
	// add marker
	private void addMarkersToAllPlots(long ttime) throws Exception {
		// remove markers

		if (combinedPlot_m==null) {
			return;
		}


		for (int i = 0; i < combinedPlot_m.getSubplots().size(); i++) {

			XYPlot plot_i = (XYPlot) combinedPlot_m.getSubplots().get(i);
			plot_i.clearDomainMarkers();

			ValueMarker valuemarker2 = new ValueMarker(ttime);
			valuemarker2.setPaint(Color.orange);

			if (i == 0) {
				valuemarker2.setPaint(Color.orange);
				valuemarker2.setLabel(Utils.formatDateTime(ttime));
				valuemarker2.setLabelAnchor(RectangleAnchor.TOP_LEFT);
				valuemarker2.setLabelTextAnchor(TextAnchor.TOP_RIGHT);
			}

			plot_i.addDomainMarker(valuemarker2);
		}

	//xyplot.addDomainMarker(valuemarker2);

	}

	private List<HtDrawableObject.Text> getTextObjectsForTime(long ttime) throws Exception {
		if (textObjectsMap_m.containsKey(ttime)) {
			return textObjectsMap_m.get(ttime);
		} else {
			throw new Exception("Text object is not registered for the time: " + (new Date(ttime)).toGMTString());
		}
	}

	private JFreeChart createChart() throws Exception {
		//XYDataset xydataset = createPriceDataset();

		// level1
		String s = "Level 1 of " + symbol_m;

		XYSeriesCollection timeseries_all_price_collection = new XYSeriesCollection();
		XYSeriesCollection timeseries_level1_volume_collection = new XYSeriesCollection();
		XYSeriesCollection timeseries_print_volume_collection = new XYSeriesCollection();
		OHLCSeriesCollection ohlc_collection = new OHLCSeriesCollection();
		XYSeriesCollection timeseries_ohlc_volume_collection = new XYSeriesCollection();

		XYSeriesCollection text_data_collection = new XYSeriesCollection();



		populateData(
						timeseries_all_price_collection,
						timeseries_level1_volume_collection,
						timeseries_print_volume_collection,
						ohlc_collection,
						timeseries_ohlc_volume_collection,
						text_data_collection);





		// commin date axis
		DateAxis daxis = new DateAxis("Time");
		daxis.setDateFormatOverride(new SimpleDateFormat("d-MM-yyyy HH:mm:ss.S"));

		// best bid/ask prices and print prices
		XYLineAndShapeRenderer render1 = new XYLineAndShapeRenderer();
		render1.setBaseShapesFilled(true);
		render1.setBaseToolTipGenerator(new StandardXYToolTipGenerator("{0}: ({1}, {2})", new SimpleDateFormat("d-MM-yyyy HH:mm:ss.S"), new DecimalFormat("0.0000")));


		NumberAxis numberaxis_prices = new NumberAxis("Best Bid/Best Ask/Print");
		DecimalFormat decimalformat = new DecimalFormat("00.0000");
		numberaxis_prices.setNumberFormatOverride(decimalformat);
		numberaxis_prices.setAutoRangeIncludesZero(false);

		XYPlot xyplot_prices = new XYPlot(timeseries_all_price_collection, daxis, numberaxis_prices, render1);
		xyplot_prices.setRangeAxisLocation(AxisLocation.BOTTOM_OR_LEFT);

		// print volumes - add to the above prices

		NumberAxis numberaxis_volume_print = new NumberAxis("Print Volume");
		numberaxis_volume_print.setUpperMargin(3.0D);



		// width
		xyplot_prices.setDataset(1, new XYBarDataset(timeseries_print_volume_collection, 100));
		xyplot_prices.setRangeAxis(1, numberaxis_volume_print);
		xyplot_prices.mapDatasetToRangeAxis(1, 1);
		XYBarRenderer xybarrenderer = new XYBarRenderer();

		xybarrenderer.setBaseToolTipGenerator(new StandardXYToolTipGenerator("{0}: ({1}, {2})", new SimpleDateFormat("d-MM-yyyy HH:mm:ss.S"), new DecimalFormat("0.0000")));

		xyplot_prices.setRenderer(1, xybarrenderer);



		//XYTextAnnotation xytextannotation = new XYTextAnnotation("Hello!", 50D, 10000D);
		//xytextannotation.setFont(new Font("SansSerif", 0, 9));
		//xytextannotation.setRotationAngle(0.78539816339744828D);
		//xyplot.addAnnotation(xytextannotation);



		XYLineAndShapeRenderer render2 = new XYLineAndShapeRenderer();
		render2.setBaseShapesFilled(true);
		render2.setBaseToolTipGenerator(new StandardXYToolTipGenerator("{0}: ({1}, {2})", new SimpleDateFormat("d-MM-yyyy HH:mm:ss.S"), new DecimalFormat("0.0000")));


		NumberAxis numberaxis_volume = new NumberAxis("Bid/Ask Volume");
		numberaxis_volume.setAutoRangeIncludesZero(false);

		XYPlot xyplot_volumes = new XYPlot(timeseries_level1_volume_collection, daxis, numberaxis_volume, render2);
		xyplot_volumes.setRangeAxisLocation(AxisLocation.BOTTOM_OR_LEFT);


		// the 3-d - OHCL
		CandlestickRenderer render_ohlc = new CandlestickRenderer();
		NumberAxis numberaxis_ohlc = new NumberAxis("OHLC");
		numberaxis_ohlc.setAutoRangeIncludesZero(false);
		XYPlot xyplot_ohlc = new XYPlot(ohlc_collection, daxis, numberaxis_ohlc, render_ohlc);

		// add OHCL volume
		NumberAxis numberaxis_volume_ohlc = new NumberAxis("OHLC Volume");
		numberaxis_volume_ohlc.setUpperMargin(1.0D);

		xyplot_ohlc.setDataset(1, new XYBarDataset(timeseries_ohlc_volume_collection, 100));
		xyplot_ohlc.setRangeAxis(1, numberaxis_volume_ohlc);
		xyplot_ohlc.mapDatasetToRangeAxis(1, 1);
		XYBarRenderer xybarrenderer2 = new XYBarRenderer();

		xybarrenderer2.setBaseToolTipGenerator(new StandardXYToolTipGenerator("{0}: ({1}, {2})", new SimpleDateFormat("d-MM-yyyy HH:mm:ss.S"), new DecimalFormat("0.0000")));

		xyplot_ohlc.setRenderer(1, xybarrenderer2);


		// the last are text objects
		// -------------------
		XYShapeRenderer text_bar_render = new XYShapeRenderer();

		// our tooltip
		XYToolTipGenerator text_tooltip = new XYToolTipGenerator(){

			public String generateToolTip(XYDataset dataset, int series, int item) {

				StringBuilder out = new StringBuilder();
				try {
					long curtime = (long) dataset.getXValue(series, item);

					if (curtime > 0) {
						List<HtDrawableObject.Text> text_obj_list = getTextObjectsForTime(curtime);

						return Utils.createTextFromAllTextObjects(text_obj_list, curtime);
						
					}
				}
				catch(Throwable e)
				{
				}

				return "N/A";
			}
		};

		text_bar_render.setBaseToolTipGenerator( text_tooltip );


		NumberAxis numberaxis_bubble = new NumberAxis("Text");
		numberaxis_bubble.setUpperMargin(1.0D);

		XYPlot xyplot_bubble = new XYPlot(text_data_collection, daxis, numberaxis_bubble, text_bar_render);
		xyplot_bubble.mapDatasetToRangeAxis(1, 1);


		// -------------------

		combinedPlot_m = new CombinedDomainXYPlot(daxis);

		combinedPlot_m.setGap(2D);
		combinedPlot_m.add(xyplot_ohlc, 5);
		combinedPlot_m.add(xyplot_prices, 5);
		combinedPlot_m.add(xyplot_volumes, 3);
		combinedPlot_m.add(xyplot_bubble, 1);

		combinedPlot_m.setOrientation(PlotOrientation.VERTICAL);

		JFreeChart jfreechart = new JFreeChart(symbol_m.toString(), JFreeChart.DEFAULT_TITLE_FONT, combinedPlot_m, true);



		// show
		ChartUtilities.applyCurrentTheme(jfreechart);


		// set up properties later
		//xybarrenderer2.setDrawBarOutline(false);
		//xybarrenderer.setDrawBarOutline(false);
		//text_bar_render.setDrawBarOutline(false);

		xybarrenderer.setShadowVisible(false);
		xybarrenderer2.setShadowVisible(false);
		//text_bar_render.setShadowVisible(false);

		xybarrenderer.setBarPainter(new StandardXYBarPainter());
		xybarrenderer2.setBarPainter(new StandardXYBarPainter());
		//text_bar_render.setBarPainter(new StandardXYBarPainter());



		return jfreechart;
	}

	private void populateData(
					XYSeriesCollection timeseries_all_price_collection,
					XYSeriesCollection timeseries_level1_volume_collection,
					XYSeriesCollection timeseries_print_volume_collection,
					OHLCSeriesCollection ohlc_collection,
					XYSeriesCollection timeseries_ohlc_volume_collection,
					XYSeriesCollection text_data_collection) throws Exception {

		/*
		timeseries_all_price_collection.setAutoWidth(true);
		timeseries_level1_volume_collection.setAutoWidth(true);
		timeseries_print_volume_collection.setAutoWidth(true);
		timeseries_ohlc_volume_collection.setAutoWidth(true);
		text_data_collection.setAutoWidth(true);
		 */



		XYSeries bid_prices = new XYSeries("Best Bid Prices", true, true);
		XYSeries ask_prices = new XYSeries("Best Ask Prices", true, true);
		XYSeries ask_volumes = new XYSeries("Best Ask Volumes", true, true);
		XYSeries bid_volumes = new XYSeries("Best Bid Volumes", true, true);

		XYSeries print_volumes = new XYSeries("Print Volumes", true, true);
		XYSeries print_prices = new XYSeries("Print Prices", true, true);

		OHLCSeries ohlcseries = new OHLCSeries("OHLC");
		XYSeries ohlc_volumes = new XYSeries("OHLC Volumes", true, true);
		XYSeries text_coordinates = new XYSeries("Text", true, true);





		Map<Long, HtDrawableObject> objects = dataProvider_m.getSymbolProperty(symbol_m).dobjects_m;

		for (Iterator<Long> it2 = objects.keySet().iterator(); it2.hasNext(); ) {
			Long time_i = it2.next();
			HtDrawableObject obj_i = objects.get(time_i);
			

			if (obj_i.getType() == HtDrawableObject.DO_LEVEL1) {
				HtDrawableObject.Level1 ldobj_i = obj_i.getAsLevel1();


				bid_prices.add(obj_i.getTime(), ldobj_i.best_bid_price_m);
				ask_prices.add(obj_i.getTime(), ldobj_i.best_ask_price_m);

				ask_volumes.add(obj_i.getTime(), ldobj_i.best_ask_volume_m);
				bid_volumes.add(obj_i.getTime(), ldobj_i.best_bid_volume_m);

			} else if (obj_i.getType() == HtDrawableObject.DO_PRINT) {
				HtDrawableObject.Print pobj_i = obj_i.getAsPrint();

				print_prices.add(obj_i.getTime(), pobj_i.bid_m);
				print_volumes.add(obj_i.getTime(), pobj_i.volume_m);

			} else if (obj_i.getType() == HtDrawableObject.DO_OHLC) {
				FixedMillisecond tmill = new FixedMillisecond(obj_i.getTime());

				HtDrawableObject.OHLC ohlc_obj = obj_i.getAsOHLC();
				ohlc_volumes.add(obj_i.getTime(), ohlc_obj.volume_m);

				try {
					ohlcseries.add(tmill, ohlc_obj.open_m, ohlc_obj.high_m, ohlc_obj.low_m, ohlc_obj.close_m);
				} catch (Throwable e) {
					// ignore duplicates
					System.out.println("Exception on inserting this time to series: " + Utils.formatDateTime(obj_i.getTime()) + " - " + e.getMessage());
				}
			} else if (obj_i.getType() == HtDrawableObject.DO_TEXT) {
				HtDrawableObject.Text text_obj = obj_i.getAsText();




				List<HtDrawableObject.Text> textList = null;
				if (textObjectsMap_m.containsKey(obj_i.getTime())) {
					textList = textObjectsMap_m.get(obj_i.getTime());
				} else {
					textList = new ArrayList<HtDrawableObject.Text>();
					textObjectsMap_m.put(obj_i.getTime(), textList);

					text_coordinates.add(obj_i.getTime(), 1);
				}

				textList.add(text_obj);

			}
		}

		// level1
		timeseries_all_price_collection.addSeries(bid_prices);
		timeseries_all_price_collection.addSeries(ask_prices);
		timeseries_all_price_collection.addSeries(print_prices);

		timeseries_level1_volume_collection.addSeries(bid_volumes);
		timeseries_level1_volume_collection.addSeries(ask_volumes);

		timeseries_print_volume_collection.addSeries(print_volumes);

		ohlc_collection.addSeries(ohlcseries);
		timeseries_ohlc_volume_collection.addSeries(ohlc_volumes);

		// process the text type

		text_data_collection.addSeries(text_coordinates);



	}
	/*
	public JPanel createDemoPanel() throws Exception {
	JFreeChart jfreechart = createChart();
	return new ChartPanel(jfreechart);
	}
	 */
}
