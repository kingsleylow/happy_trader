/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * HtDrawableWindow.java
 *
 * Created on 22.08.2010, 14:10:05
 */
package htrangebreaker;

import com.fin.httrader.hlpstruct.AlertPriorities;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.HtFileUtils;
import htrangebreaker.utils.gui.MsgBox;
import com.fin.httrader.utils.ThreadLocalFormats.HtThreadLocalSimpleDateFormat;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtDrawableObject;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import com.fin.httrader.utils.hlpstruct.XmlEvent;
import htrangebreaker.utils.CommunicationInterface;
import htrangebreaker.utils.EventBerkleyDbBuffer;
import htrangebreaker.utils.EventsFileBuffer;
import htrangebreaker.utils.HtFrameLogger;
import htrangebreaker.utils.HtSettingsStruct;
import htrangebreaker.utils.ProgressDialogInterface;
import htrangebreaker.utils.TradeEventReceiver;
import htrangebreaker.utils.gui.*;


import java.awt.Color;
import java.io.BufferedWriter;
import java.io.File;

import java.io.FileWriter;
import java.io.IOException;
import java.util.Date;
import java.util.Iterator;
import java.util.Map;
import java.util.TimeZone;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JFileChooser;
import javax.swing.JTable;
import javax.swing.SpinnerNumberModel;
import javax.swing.SwingUtilities;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.filechooser.FileFilter;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableColumn;
import org.jdesktop.application.Action;

/**
 *
 * @author DanilinS
 */
public class HtDrawableWindow extends javax.swing.JInternalFrame implements CommunicationInterface, ProgressDialogInterface {

	private static final int IDX_ID = 0;
	private static final int IDX_TIME_COL = 1;
	private static final int IDX_PROVIDER_COL = 2;
	private static final int IDX_SYMBOL_COL = 3;
	private static final int IDX_PRIORITY_COL = 4;
	private static final int IDX_SHORTTEXT_COL = 5;
	private static final int IDX_TEXT_COL = 6;
	private static final String SIGNAL_STRING_ID = "SIGNAL";
	private static HtThreadLocalSimpleDateFormat dateTimeFormat_m = new HtThreadLocalSimpleDateFormat("dd-MM-yyyy HH:mm:ss", "GMT-0");
	//private MultiLineTableRenderer renderer_m = null;
	private TradeEventReceiver tradeEventReceiver_m = null;
	//private EventsFileBuffer eventsCache_m = new EventsFileBuffer();
	private EventBerkleyDbBuffer eventsCache_m = new EventBerkleyDbBuffer();
	private boolean historyReadFinished_m = false;
	private int defaultRowHight_m = 0;
	private String symbolPattern_m = null;
	private boolean isAndPaterns_m = false;
	HtDrawableWindow_SignalTableRenderer strenderer_m = null;




	/** Creates new form HtDrawableWindow */
	public HtDrawableWindow() {
		initComponents();

		// 1 day
		long begin_date_hist = HtDateTimeUtils.getCurGmtTime_DayBegin() - (long) 1 * 24 * 60 * 60 * 1000;
		jTextFromDate.setValue(new Date(begin_date_hist));

		java.awt.Frame frm = HtRangeBreakerApp.getApplication().getMainFrame();

		DiskCachedTableModel dcachedmodel = new DiskCachedTableModel(eventsCache_m, new Integer[]{IDX_TEXT_COL});
		//DefaultTableModel dcachedmodel = new DefaultTableModel();
		jDataTab.setModel(dcachedmodel);


		jDataTab.getColumnModel().addColumn(new TableColumn(IDX_ID));
		jDataTab.getColumnModel().addColumn(new TableColumn(IDX_TIME_COL));
		jDataTab.getColumnModel().addColumn(new TableColumn(IDX_PROVIDER_COL));
		jDataTab.getColumnModel().addColumn(new TableColumn(IDX_SYMBOL_COL));
		jDataTab.getColumnModel().addColumn(new TableColumn(IDX_PRIORITY_COL));
		jDataTab.getColumnModel().addColumn(new TableColumn(IDX_SHORTTEXT_COL));
		jDataTab.getColumnModel().addColumn(new TableColumn(IDX_TEXT_COL));
		dcachedmodel.setColumnCount(7);

		jDataTab.getColumnModel().getColumn(IDX_ID).setHeaderValue("ID");
		jDataTab.getColumnModel().getColumn(IDX_TIME_COL).setHeaderValue("Time");
		jDataTab.getColumnModel().getColumn(IDX_PROVIDER_COL).setHeaderValue("Provider");
		jDataTab.getColumnModel().getColumn(IDX_SYMBOL_COL).setHeaderValue("Symbol");
		jDataTab.getColumnModel().getColumn(IDX_PRIORITY_COL).setHeaderValue("Priority");
		jDataTab.getColumnModel().getColumn(IDX_SHORTTEXT_COL).setHeaderValue("Short text");
		jDataTab.getColumnModel().getColumn(IDX_TEXT_COL).setHeaderValue("Text");



		jRowsCnt.setText(String.valueOf(dcachedmodel.getRowCount()));


		HtDrawableWindow_DataTableRenderer apr = new HtDrawableWindow_DataTableRenderer(new HtDrawableWindow_DataTableRenderer.RenderInterface() {

			public Color getBackgroundColor(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
				String apriority = ((String) table.getValueAt(row, IDX_PRIORITY_COL));

				if (apriority.equalsIgnoreCase("HIGH")) {
					if (isSelected) {
						return Color.GREEN;
					} else {
						return Color.YELLOW;
					}
				}

				if (apriority.equalsIgnoreCase("CRITICAL")) {
					if (isSelected) {
						return new Color(255, 0, 102);
					} else {
						return new Color(255, 51, 0);
					}
				}


				return null;
			}

			public Color getForegroundColor(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
				return null;
			}
		},
						new HtDrawableWindow_DataTableRenderer.RowSelector() {

							public boolean isRowSelected(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {

								
								if (jSwitchFilter.isSelected()) {
									String asymbol = ((String) table.getValueAt(row, IDX_SYMBOL_COL));
									if (symbolPattern_m != null)
										return symbolPattern_m.equalsIgnoreCase(asymbol);
									else
										return true;
								}

								return false;
								

							}
						},
						IDX_ID);



		jDataTab.getColumnModel().getColumn(IDX_ID).setCellRenderer(apr);
		jDataTab.getColumnModel().getColumn(IDX_TIME_COL).setCellRenderer(apr);
		jDataTab.getColumnModel().getColumn(IDX_PROVIDER_COL).setCellRenderer(apr);
		jDataTab.getColumnModel().getColumn(IDX_SYMBOL_COL).setCellRenderer(apr);
		jDataTab.getColumnModel().getColumn(IDX_PRIORITY_COL).setCellRenderer(apr);
		jDataTab.getColumnModel().getColumn(IDX_SHORTTEXT_COL).setCellRenderer(apr);
		jDataTab.getColumnModel().getColumn(IDX_TEXT_COL).setCellRenderer(apr);


		//
		jDataTab.getColumnModel().getColumn(IDX_ID).setPreferredWidth(70);
		jDataTab.getColumnModel().getColumn(IDX_TIME_COL).setPreferredWidth(150);
		jDataTab.getColumnModel().getColumn(IDX_PROVIDER_COL).setPreferredWidth(80);
		jDataTab.getColumnModel().getColumn(IDX_SYMBOL_COL).setPreferredWidth(80);
		jDataTab.getColumnModel().getColumn(IDX_PRIORITY_COL).setPreferredWidth(60);
		jDataTab.getColumnModel().getColumn(IDX_SHORTTEXT_COL).setPreferredWidth(100);
		jDataTab.getColumnModel().getColumn(IDX_TEXT_COL).setPreferredWidth(500);

		SimpleCellEditor editor = new SimpleCellEditor();
		editor.setFont(jDataTab.getFont());
		jDataTab.getColumnModel().getColumn(IDX_TEXT_COL).setCellEditor(editor);

		defaultRowHight_m = jDataTab.getRowHeight();


		// ((SpinnerNumberModel)jRowHightSpinner.getModel()).setValue(Double.valueOf(jDataTab.getPreferredSize().getHeight()));
		//((SpinnerNumberModel)jRowHightSpinner.getModel()).setValue(Integer.valueOf(1));

		// jspinner on change
		jRowHightSpinner.addChangeListener(new ChangeListener() {

			public void stateChanged(ChangeEvent event) {
				SpinnerNumberModel model = ((SpinnerNumberModel) jRowHightSpinner.getModel());
				int rowsn = model.getNumber().intValue();
				int rowhight = rowsn * defaultRowHight_m + 2;

				jDataTab.setRowHeight(rowhight);
				jSignalTab.setRowHeight(rowhight);
			}
		});

		// symbol filter listener
		// Listen for changes in the text

		final HtDrawableWindow pThis = this;
		jSymbolText.getDocument().addDocumentListener(new DocumentListener() {

			public void changedUpdate(DocumentEvent e) {
				setupSymbolBackground();
			}

			public void insertUpdate(DocumentEvent e) {
				setupSymbolBackground();
			}

			public void removeUpdate(DocumentEvent e) {
				setupSymbolBackground();
			}
		});

		
		//
		enableSelectionNavigationButtons(false);
		showConnectChannelStatus(false);

		// ------------------------------
		// SIGNAL TAB
		DefaultTableModel sigtabmodel = (DefaultTableModel) jSignalTab.getModel();
		sigtabmodel.addColumn("ID");
		sigtabmodel.addColumn("Time");
		sigtabmodel.addColumn("Symbol");
		sigtabmodel.addColumn("Text");

		jSignalTab.getColumnModel().getColumn(0).setPreferredWidth(70);
		jSignalTab.getColumnModel().getColumn(1).setPreferredWidth(150);
		jSignalTab.getColumnModel().getColumn(2).setPreferredWidth(80);
		jSignalTab.getColumnModel().getColumn(3).setPreferredWidth(200);

		// add renderer
		HtSettingsStruct s = HtSettings.getInstance().getSettings();

		strenderer_m = new HtDrawableWindow_SignalTableRenderer(jSignalTab);
		strenderer_m.setSignalLengthSec(s.alertSignalLengthSec_m);
		strenderer_m.enableSound(jSoundCB.isSelected());

		jSignalTab.getColumnModel().getColumn(0).setCellRenderer(strenderer_m);
		jSignalTab.getColumnModel().getColumn(1).setCellRenderer(strenderer_m);
		jSignalTab.getColumnModel().getColumn(2).setCellRenderer(strenderer_m);
		jSignalTab.getColumnModel().getColumn(3).setCellRenderer(strenderer_m);


	}

	/** This method is called from within the constructor to
	 * initialize the form.
	 * WARNING: Do NOT modify this code. The content of this method is
	 * always regenerated by the Form Editor.
	 */
	@SuppressWarnings("unchecked")
  // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
  private void initComponents() {

    jPanel1 = new javax.swing.JPanel();
    jLabel5 = new javax.swing.JLabel();
    jTextFromDate = new javax.swing.JFormattedTextField( dateTimeFormat_m.getFormatThreadCopy() );
    jLabel6 = new javax.swing.JLabel();
    jTextToDate = new javax.swing.JFormattedTextField( dateTimeFormat_m.getFormatThreadCopy() );
    jSeparator1 = new javax.swing.JSeparator();
    jStartStop = new javax.swing.JToggleButton();
    jScrollDown = new javax.swing.JCheckBox();
    jLabel1 = new javax.swing.JLabel();
    jShortTextFilter = new javax.swing.JTextField();
    jLabel2 = new javax.swing.JLabel();
    jTextFilter = new javax.swing.JTextField();
    jLabel3 = new javax.swing.JLabel();
    jProfileNameTxt = new javax.swing.JTextField();
    jRowsCnt = new javax.swing.JTextField();
    jLabel4 = new javax.swing.JLabel();
    jRowHightSpinner = new javax.swing.JSpinner();
    jSymbolText = new javax.swing.JTextField();
    jSeparator2 = new javax.swing.JSeparator();
    jLabel7 = new javax.swing.JLabel();
    jSwitchFilter = new javax.swing.JToggleButton();
    jSelectedNext = new javax.swing.JButton();
    jSelectedLast = new javax.swing.JButton();
    jSelectedPrev = new javax.swing.JButton();
    jSelectedFirst = new javax.swing.JButton();
    jConnectStatusTxt = new javax.swing.JTextField();
    jSoundCB = new javax.swing.JCheckBox();
    jExportBtn = new javax.swing.JButton();
    jSplitPane1 = new javax.swing.JSplitPane();
    jPanel2 = new javax.swing.JPanel();
    jScrollPane2 = new javax.swing.JScrollPane();
    jSignalTab = new javax.swing.JTable();
    jPanel5 = new javax.swing.JPanel();
    jLocEventsCB = new javax.swing.JCheckBox();
    jLabel9 = new javax.swing.JLabel();
    jPanel3 = new javax.swing.JPanel();
    jScrollPane1 = new javax.swing.JScrollPane();
    jDataTab = new javax.swing.JTable();
    jPanel6 = new javax.swing.JPanel();
    jLabel10 = new javax.swing.JLabel();

    setClosable(true);
    org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(htrangebreaker.HtRangeBreakerApp.class).getContext().getResourceMap(HtDrawableWindow.class);
    setForeground(resourceMap.getColor("Form.foreground")); // NOI18N
    setIconifiable(true);
    setMaximizable(true);
    setResizable(true);
    setTitle(resourceMap.getString("Form.title")); // NOI18N
    setToolTipText(resourceMap.getString("Form.toolTipText")); // NOI18N
    setName("Form"); // NOI18N
    addInternalFrameListener(new javax.swing.event.InternalFrameListener() {
      public void internalFrameActivated(javax.swing.event.InternalFrameEvent evt) {
      }
      public void internalFrameClosed(javax.swing.event.InternalFrameEvent evt) {
      }
      public void internalFrameClosing(javax.swing.event.InternalFrameEvent evt) {
        formInternalFrameClosing(evt);
      }
      public void internalFrameDeactivated(javax.swing.event.InternalFrameEvent evt) {
      }
      public void internalFrameDeiconified(javax.swing.event.InternalFrameEvent evt) {
      }
      public void internalFrameIconified(javax.swing.event.InternalFrameEvent evt) {
      }
      public void internalFrameOpened(javax.swing.event.InternalFrameEvent evt) {
      }
    });

    jPanel1.setBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED));
    jPanel1.setName("jPanel1"); // NOI18N
    jPanel1.setPreferredSize(new java.awt.Dimension(801, 180));

    jLabel5.setText(resourceMap.getString("jLabel5.text")); // NOI18N
    jLabel5.setName("jLabel5"); // NOI18N

    jTextFromDate.setName("jTextFromDate"); // NOI18N

    jLabel6.setText(resourceMap.getString("jLabel6.text")); // NOI18N
    jLabel6.setName("jLabel6"); // NOI18N

    jTextToDate.setName("jTextToDate"); // NOI18N

    jSeparator1.setName("jSeparator1"); // NOI18N

    javax.swing.ActionMap actionMap = org.jdesktop.application.Application.getInstance(htrangebreaker.HtRangeBreakerApp.class).getContext().getActionMap(HtDrawableWindow.class, this);
    jStartStop.setAction(actionMap.get("doStartStopEvents")); // NOI18N
    jStartStop.setText(resourceMap.getString("jStartStop.text")); // NOI18N
    jStartStop.setName("jStartStop"); // NOI18N

    jScrollDown.setSelected(true);
    jScrollDown.setText(resourceMap.getString("jScrollDown.text")); // NOI18N
    jScrollDown.setName("jScrollDown"); // NOI18N

    jLabel1.setText(resourceMap.getString("jLabel1.text")); // NOI18N
    jLabel1.setName("jLabel1"); // NOI18N

    jShortTextFilter.setText(resourceMap.getString("jShortTextFilter.text")); // NOI18N
    jShortTextFilter.setName("jShortTextFilter"); // NOI18N

    jLabel2.setText(resourceMap.getString("jLabel2.text")); // NOI18N
    jLabel2.setName("jLabel2"); // NOI18N

    jTextFilter.setText(resourceMap.getString("jTextFilter.text")); // NOI18N
    jTextFilter.setName("jTextFilter"); // NOI18N

    jLabel3.setText(resourceMap.getString("jLabel3.text")); // NOI18N
    jLabel3.setName("jLabel3"); // NOI18N

    jProfileNameTxt.setText(resourceMap.getString("jProfileNameTxt.text")); // NOI18N
    jProfileNameTxt.setName("jProfileNameTxt"); // NOI18N

    jRowsCnt.setEditable(false);
    jRowsCnt.setText(resourceMap.getString("jRowsCnt.text")); // NOI18N
    jRowsCnt.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
    jRowsCnt.setName("jRowsCnt"); // NOI18N

    jLabel4.setText(resourceMap.getString("jLabel4.text")); // NOI18N
    jLabel4.setName("jLabel4"); // NOI18N

    jRowHightSpinner.setModel(new javax.swing.SpinnerNumberModel(Integer.valueOf(1), Integer.valueOf(1), null, Integer.valueOf(1)));
    jRowHightSpinner.setName("jRowHightSpinner"); // NOI18N

    jSymbolText.setBackground(resourceMap.getColor("jSymbolText.background")); // NOI18N
    jSymbolText.setText(resourceMap.getString("jSymbolText.text")); // NOI18N
    jSymbolText.setName("jSymbolText"); // NOI18N

    jSeparator2.setName("jSeparator2"); // NOI18N

    jLabel7.setText(resourceMap.getString("jLabel7.text")); // NOI18N
    jLabel7.setName("jLabel7"); // NOI18N

    jSwitchFilter.setAction(actionMap.get("doFilterOnOff")); // NOI18N
    jSwitchFilter.setText(resourceMap.getString("jSwitchFilter.text")); // NOI18N
    jSwitchFilter.setName("jSwitchFilter"); // NOI18N

    jSelectedNext.setAction(actionMap.get("onNextFind")); // NOI18N
    jSelectedNext.setText(resourceMap.getString("jSelectedNext.text")); // NOI18N
    jSelectedNext.setName("jSelectedNext"); // NOI18N

    jSelectedLast.setAction(actionMap.get("onLastFind")); // NOI18N
    jSelectedLast.setText(resourceMap.getString("jSelectedLast.text")); // NOI18N
    jSelectedLast.setName("jSelectedLast"); // NOI18N

    jSelectedPrev.setAction(actionMap.get("onPrevFind")); // NOI18N
    jSelectedPrev.setText(resourceMap.getString("jSelectedPrev.text")); // NOI18N
    jSelectedPrev.setName("jSelectedPrev"); // NOI18N

    jSelectedFirst.setAction(actionMap.get("onFirstFind")); // NOI18N
    jSelectedFirst.setText(resourceMap.getString("jSelectedFirst.text")); // NOI18N
    jSelectedFirst.setName("jSelectedFirst"); // NOI18N

    jConnectStatusTxt.setEditable(false);
    jConnectStatusTxt.setText(resourceMap.getString("jConnectStatusTxt.text")); // NOI18N
    jConnectStatusTxt.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
    jConnectStatusTxt.setName("jConnectStatusTxt"); // NOI18N

    jSoundCB.setSelected(true);
    jSoundCB.setText(resourceMap.getString("jSoundCB.text")); // NOI18N
    jSoundCB.setName("jSoundCB"); // NOI18N
    jSoundCB.addItemListener(new java.awt.event.ItemListener() {
      public void itemStateChanged(java.awt.event.ItemEvent evt) {
        jSoundCBItemStateChanged(evt);
      }
    });

    jExportBtn.setAction(actionMap.get("onExport")); // NOI18N
    jExportBtn.setText(resourceMap.getString("jExportBtn.text")); // NOI18N
    jExportBtn.setName("jExportBtn"); // NOI18N

    javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
    jPanel1.setLayout(jPanel1Layout);
    jPanel1Layout.setHorizontalGroup(
      jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel1Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(jPanel1Layout.createSequentialGroup()
            .addComponent(jStartStop, javax.swing.GroupLayout.PREFERRED_SIZE, 108, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addGap(18, 18, 18)
            .addComponent(jScrollDown)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
            .addComponent(jLabel4)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(jRowHightSpinner, javax.swing.GroupLayout.PREFERRED_SIZE, 51, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
            .addComponent(jSoundCB)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 262, Short.MAX_VALUE)
            .addComponent(jRowsCnt, javax.swing.GroupLayout.PREFERRED_SIZE, 87, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
            .addComponent(jConnectStatusTxt, javax.swing.GroupLayout.PREFERRED_SIZE, 83, javax.swing.GroupLayout.PREFERRED_SIZE))
          .addGroup(jPanel1Layout.createSequentialGroup()
            .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
              .addGroup(javax.swing.GroupLayout.Alignment.LEADING, jPanel1Layout.createSequentialGroup()
                .addComponent(jLabel1)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jShortTextFilter))
              .addGroup(jPanel1Layout.createSequentialGroup()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                  .addComponent(jLabel2)
                  .addComponent(jLabel3))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 13, Short.MAX_VALUE)
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                  .addComponent(jProfileNameTxt, javax.swing.GroupLayout.PREFERRED_SIZE, 124, javax.swing.GroupLayout.PREFERRED_SIZE)
                  .addComponent(jTextFilter, javax.swing.GroupLayout.PREFERRED_SIZE, 176, javax.swing.GroupLayout.PREFERRED_SIZE))))
            .addGap(18, 18, 18)
            .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addComponent(jLabel5)
              .addComponent(jLabel6))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addComponent(jTextFromDate, javax.swing.GroupLayout.PREFERRED_SIZE, 132, javax.swing.GroupLayout.PREFERRED_SIZE)
              .addComponent(jTextToDate, javax.swing.GroupLayout.PREFERRED_SIZE, 132, javax.swing.GroupLayout.PREFERRED_SIZE))))
        .addContainerGap())
      .addComponent(jSeparator1, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 871, Short.MAX_VALUE)
      .addGroup(jPanel1Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(jPanel1Layout.createSequentialGroup()
            .addComponent(jSymbolText, javax.swing.GroupLayout.PREFERRED_SIZE, 62, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addGap(18, 18, 18)
            .addComponent(jSelectedFirst)
            .addGap(1, 1, 1)
            .addComponent(jSelectedPrev)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(jSwitchFilter)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(jSelectedNext)
            .addGap(1, 1, 1)
            .addComponent(jSelectedLast)
            .addGap(10, 10, 10)
            .addComponent(jExportBtn))
          .addComponent(jLabel7))
        .addGap(413, 413, 413))
      .addComponent(jSeparator2, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 871, Short.MAX_VALUE)
    );

    jPanel1Layout.linkSize(javax.swing.SwingConstants.HORIZONTAL, new java.awt.Component[] {jShortTextFilter, jTextFilter});

    jPanel1Layout.linkSize(javax.swing.SwingConstants.HORIZONTAL, new java.awt.Component[] {jSelectedFirst, jSelectedLast, jSelectedNext, jSelectedPrev});

    jPanel1Layout.setVerticalGroup(
      jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel1Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(jLabel1)
          .addComponent(jShortTextFilter, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jLabel5)
          .addComponent(jTextFromDate, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(jLabel2)
          .addComponent(jTextFilter, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jLabel6)
          .addComponent(jTextToDate, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
        .addGap(8, 8, 8)
        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(jProfileNameTxt, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jLabel3))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(jSeparator1, javax.swing.GroupLayout.PREFERRED_SIZE, 7, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(jLabel7)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(jSymbolText, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jSwitchFilter)
          .addComponent(jSelectedFirst)
          .addComponent(jSelectedLast)
          .addComponent(jSelectedPrev)
          .addComponent(jSelectedNext)
          .addComponent(jExportBtn))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
        .addComponent(jSeparator2, javax.swing.GroupLayout.PREFERRED_SIZE, 7, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(jStartStop)
          .addComponent(jScrollDown)
          .addComponent(jLabel4)
          .addComponent(jRowHightSpinner, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jConnectStatusTxt, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jRowsCnt, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jSoundCB))
        .addContainerGap())
    );

    jSplitPane1.setDividerLocation(201);
    jSplitPane1.setName("jSplitPane1"); // NOI18N

    jPanel2.setName("jPanel2"); // NOI18N

    jScrollPane2.setName("jScrollPane2"); // NOI18N

    jSignalTab.setModel(new DefaultTableModel() {
      public boolean isCellEditable(int rowIndex, int mColIndex) {
        return false;
      }
    });
    jSignalTab.setAutoResizeMode(javax.swing.JTable.AUTO_RESIZE_OFF);
    jSignalTab.setName("jSignalTab"); // NOI18N
    jSignalTab.setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
    jSignalTab.addMouseListener(new java.awt.event.MouseAdapter() {
      public void mouseClicked(java.awt.event.MouseEvent evt) {
        jSignalTabMouseClicked(evt);
      }
    });
    jScrollPane2.setViewportView(jSignalTab);

    jPanel5.setBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED));
    jPanel5.setName("jPanel5"); // NOI18N
    jPanel5.setPreferredSize(new java.awt.Dimension(200, 34));

    jLocEventsCB.setText(resourceMap.getString("jLocEventsCB.text")); // NOI18N
    jLocEventsCB.setName("jLocEventsCB"); // NOI18N

    jLabel9.setText(resourceMap.getString("jLabel9.text")); // NOI18N
    jLabel9.setName("jLabel9"); // NOI18N

    javax.swing.GroupLayout jPanel5Layout = new javax.swing.GroupLayout(jPanel5);
    jPanel5.setLayout(jPanel5Layout);
    jPanel5Layout.setHorizontalGroup(
      jPanel5Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel5Layout.createSequentialGroup()
        .addContainerGap()
        .addComponent(jLabel9)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 33, Short.MAX_VALUE)
        .addComponent(jLocEventsCB)
        .addGap(18, 18, 18))
    );
    jPanel5Layout.setVerticalGroup(
      jPanel5Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel5Layout.createSequentialGroup()
        .addGroup(jPanel5Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addComponent(jLocEventsCB)
          .addComponent(jLabel9))
        .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
    );

    javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
    jPanel2.setLayout(jPanel2Layout);
    jPanel2Layout.setHorizontalGroup(
      jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addComponent(jPanel5, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
      .addComponent(jScrollPane2, javax.swing.GroupLayout.DEFAULT_SIZE, 200, Short.MAX_VALUE)
    );
    jPanel2Layout.setVerticalGroup(
      jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel2Layout.createSequentialGroup()
        .addComponent(jPanel5, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(jScrollPane2, javax.swing.GroupLayout.DEFAULT_SIZE, 193, Short.MAX_VALUE))
    );

    jSplitPane1.setLeftComponent(jPanel2);

    jPanel3.setName("jPanel3"); // NOI18N

    jScrollPane1.setName("jScrollPane1"); // NOI18N

    jDataTab.setAutoCreateColumnsFromModel(false);
    jDataTab.setModel(new javax.swing.table.DefaultTableModel(
      new Object [][] {

      },
      new String [] {
        "Title 1", "Title 2", "Title 3", "Title 4", "Title 5", "Title 6"
      }
    ));
    jDataTab.setAutoResizeMode(javax.swing.JTable.AUTO_RESIZE_OFF);
    jDataTab.setName("jDataTab"); // NOI18N
    jDataTab.setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
    jScrollPane1.setViewportView(jDataTab);

    jPanel6.setBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED));
    jPanel6.setName("jPanel6"); // NOI18N
    jPanel6.setPreferredSize(new java.awt.Dimension(568, 34));

    jLabel10.setText(resourceMap.getString("jLabel10.text")); // NOI18N
    jLabel10.setName("jLabel10"); // NOI18N

    javax.swing.GroupLayout jPanel6Layout = new javax.swing.GroupLayout(jPanel6);
    jPanel6.setLayout(jPanel6Layout);
    jPanel6Layout.setHorizontalGroup(
      jPanel6Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel6Layout.createSequentialGroup()
        .addContainerGap()
        .addComponent(jLabel10)
        .addContainerGap(617, Short.MAX_VALUE))
    );
    jPanel6Layout.setVerticalGroup(
      jPanel6Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel6Layout.createSequentialGroup()
        .addComponent(jLabel10)
        .addContainerGap(16, Short.MAX_VALUE))
    );

    javax.swing.GroupLayout jPanel3Layout = new javax.swing.GroupLayout(jPanel3);
    jPanel3.setLayout(jPanel3Layout);
    jPanel3Layout.setHorizontalGroup(
      jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addComponent(jPanel6, javax.swing.GroupLayout.DEFAULT_SIZE, 668, Short.MAX_VALUE)
      .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 668, Short.MAX_VALUE)
    );
    jPanel3Layout.setVerticalGroup(
      jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel3Layout.createSequentialGroup()
        .addComponent(jPanel6, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 193, Short.MAX_VALUE))
    );

    jSplitPane1.setRightComponent(jPanel3);

    javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
    getContentPane().setLayout(layout);
    layout.setHorizontalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, 875, Short.MAX_VALUE)
      .addComponent(jSplitPane1, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 875, Short.MAX_VALUE)
    );
    layout.setVerticalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(layout.createSequentialGroup()
        .addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, 200, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(jSplitPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 235, Short.MAX_VALUE))
    );

    pack();
  }// </editor-fold>//GEN-END:initComponents

		private void formInternalFrameClosing(javax.swing.event.InternalFrameEvent evt) {//GEN-FIRST:event_formInternalFrameClosing
			// TODO add your handling code here:
			shutDownOperations();
			HtRangeBreakerView.getInstance().deregisterWindow(this);
		}//GEN-LAST:event_formInternalFrameClosing

		private void jSignalTabMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jSignalTabMouseClicked
			// TODO add your handling code here:
			if (jLocEventsCB.isSelected()) {
				int row_idx = jSignalTab.getSelectedRow();

				if (row_idx >= 0) {
					int event_id = Integer.valueOf((String) jSignalTab.getValueAt(row_idx, 0));
					navigateSignalInMainDataTable(event_id);
				}
			}
		}//GEN-LAST:event_jSignalTabMouseClicked

		private void jSoundCBItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_jSoundCBItemStateChanged
			// TODO add your handling code here:
			if (strenderer_m != null) {
				strenderer_m.enableSound(jSoundCB.isSelected());
			}
		}//GEN-LAST:event_jSoundCBItemStateChanged

  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JTextField jConnectStatusTxt;
  private javax.swing.JTable jDataTab;
  private javax.swing.JButton jExportBtn;
  private javax.swing.JLabel jLabel1;
  private javax.swing.JLabel jLabel10;
  private javax.swing.JLabel jLabel2;
  private javax.swing.JLabel jLabel3;
  private javax.swing.JLabel jLabel4;
  private javax.swing.JLabel jLabel5;
  private javax.swing.JLabel jLabel6;
  private javax.swing.JLabel jLabel7;
  private javax.swing.JLabel jLabel9;
  private javax.swing.JCheckBox jLocEventsCB;
  private javax.swing.JPanel jPanel1;
  private javax.swing.JPanel jPanel2;
  private javax.swing.JPanel jPanel3;
  private javax.swing.JPanel jPanel5;
  private javax.swing.JPanel jPanel6;
  private javax.swing.JTextField jProfileNameTxt;
  private javax.swing.JSpinner jRowHightSpinner;
  private javax.swing.JTextField jRowsCnt;
  private javax.swing.JCheckBox jScrollDown;
  private javax.swing.JScrollPane jScrollPane1;
  private javax.swing.JScrollPane jScrollPane2;
  private javax.swing.JButton jSelectedFirst;
  private javax.swing.JButton jSelectedLast;
  private javax.swing.JButton jSelectedNext;
  private javax.swing.JButton jSelectedPrev;
  private javax.swing.JSeparator jSeparator1;
  private javax.swing.JSeparator jSeparator2;
  private javax.swing.JTextField jShortTextFilter;
  private javax.swing.JTable jSignalTab;
  private javax.swing.JCheckBox jSoundCB;
  private javax.swing.JSplitPane jSplitPane1;
  private javax.swing.JToggleButton jStartStop;
  private javax.swing.JToggleButton jSwitchFilter;
  private javax.swing.JTextField jSymbolText;
  private javax.swing.JTextField jTextFilter;
  private javax.swing.JFormattedTextField jTextFromDate;
  private javax.swing.JFormattedTextField jTextToDate;
  // End of variables declaration//GEN-END:variables

	public void onStartLengthyOperation(String operationName) {

		HtRangeBreakerView.getInstance().showWIPDialog(this, operationName);
		historyReadFinished_m = false;

	}

	public void onStopLengthyOperation() {
		HtRangeBreakerView.getInstance().hideWIPDialog();

		// notifies that we inserted rows

		final DiskCachedTableModel model = (DiskCachedTableModel) jDataTab.getModel();


		SwingUtilities.invokeLater(new Runnable() {

			public void run() {
				historyReadFinished_m = true;
				model.fireTableDataChanged();
			}
		});

	}

	public void onStartCommunicationChannel() throws Exception {
		showConnectChannelStatus(true);
		eventsCache_m.open();
	}

	public void onStopCommunicationChannel() {

		showConnectChannelStatus(false);
		eventsCache_m.close();
		HtFrameLogger.log("Event communication channel closed");

	}

	public void onLogEntryReceived(boolean isHistory, CommonLog clog) {
	}

	public void onDrawableObjectReceived(boolean isHistory, HtDrawableObject cdraw) {
		if (cdraw.getType() == HtDrawableObject.DO_TEXT) {
			final HtDrawableObject cdrawf = cdraw;
			final HtDrawableObject.Text txtobj = cdraw.getAsText();
			final DiskCachedTableModel model = (DiskCachedTableModel) jDataTab.getModel();
			//final DefaultTableModel model = (DefaultTableModel) jDataTab.getModel();



			long cur_counter = eventsCache_m.getCurCounter();
			final Object[] row = new Object[]{
				String.valueOf(cur_counter),
				HtDateTimeUtils.time2String_Gmt(cdrawf.getTime()),
				cdrawf.getProvider(),
				cdrawf.getSymbol(),
				AlertPriorities.getAlertpriorityByName(txtobj.priority_m),
				txtobj.shortText_m.toString(),
				txtobj.text_m.toString()
			};

			Object[] signalRow = null;
			if (txtobj.shortText_m.toString().equals(SIGNAL_STRING_ID)) {
				signalRow = new Object[]{
									String.valueOf(cur_counter),
									HtDateTimeUtils.time2String_Gmt(cdrawf.getTime()),
									cdrawf.getSymbol(),
									txtobj.text_m.toString()
								};
			}

			final Object[] signalRow_f = signalRow;

			try {
				eventsCache_m.addEntry(row);
			} catch (Throwable e) {
				// do not proceed further
				HtFrameLogger.logError(e, "Exception on adding entry to cache: " + e.toString());
			}


			SwingUtilities.invokeLater(new Runnable() {

				public void run() {

					model.appendNewEntry(row);
					//model.incrementRowCount();
					jRowsCnt.setText(String.valueOf(model.getRowCount()));


					if (historyReadFinished_m) {
						model.fireTableRowsInserted(model.getRowCount() - 1, model.getRowCount() - 1);
					}
					//model.addRow(row);


					if (signalRow_f != null) {
						// need to add to the list of signals
						DefaultTableModel sigtabmodel = (DefaultTableModel) jSignalTab.getModel();
						int row_id = sigtabmodel.getRowCount();

						sigtabmodel.addRow(signalRow_f);

						// blink on in RT mode
						if (historyReadFinished_m) {
							strenderer_m.setUpBlinkingRow(row_id);
						}


					}

					if (jScrollDown.isSelected()) {
						GuiUtils.scrollToVisible(jDataTab);
						if (signalRow_f != null) {
							GuiUtils.scrollToVisible(jSignalTab);
						}
					}


				}
			});

		}
	}

	public void onHtRtDataReceived(boolean isHistory, HtRtData rtdata) {
		// N/A
	}

	public void onErrorOccured(String message) {
		final StringBuilder fmsg = new StringBuilder("Communication log channel error: " + message);
		HtFrameLogger.logError(fmsg.toString());


	}

	public void onHeartBeat() {
	}

	@Action
	public void doFilterOnOff() {
		if (jSwitchFilter.isSelected()) {
			jSwitchFilter.setText("Filter Off");
			symbolPattern_m = null;
			

			if (jSymbolText.getText().length() > 0) {
				symbolPattern_m = jSymbolText.getText();
			}

		

			enableSelectionNavigationButtons(true);



		} else {
			jSwitchFilter.setText("Filter On");
			symbolPattern_m = null;
		

			enableSelectionNavigationButtons(false);

		}

		DiskCachedTableModel model = (DiskCachedTableModel) jDataTab.getModel();
		model.fireTableDataChanged();
	}

	@Action
	public void doStartStopEvents() {
		if (jStartStop.isSelected()) {
			jStartStop.setText("Stop");
		} else {
			jStartStop.setText("Start");
		}

		//
		try {
			if (jStartStop.isSelected()) {
				// need to start

				if (jProfileNameTxt.getText().length() <= 0) {
					MsgBox.showMessageBox("You need to enter a valid profile name!");
					jStartStop.setText("Start");
					jStartStop.setSelected(false);
					return;
				}

				//DefaultTableModel model = (DefaultTableModel)jDataTab.getModel();
				//model.setRowCount(0);

				sendClearTableMessage();



				long from_date = -1, to_date = -1;
				Date from_date_d = (Date) jTextFromDate.getValue();
				Date to_date_d = (Date) jTextToDate.getValue();

				if (from_date_d != null) {
					from_date = from_date_d.getTime();
				}

				if (to_date_d != null) {
					to_date = to_date_d.getTime();
				}

				HtSettingsStruct s = HtSettings.getInstance().getSettings();
				tradeEventReceiver_m = new TradeEventReceiver(
								s,
								this,
								from_date,
								to_date,
								jProfileNameTxt.getText(),
								jTextFilter.getText(),
								jShortTextFilter.getText(),
								new Integer[]{XmlEvent.ET_DrawableObject},
								null,
								s.isServletDebug_m);



				disableControls(false);

			} else {
				shutDownOperations();
				disableControls(true);
			}
		} catch (Throwable e) {
			HtFrameLogger.logError(e, "Exception occured: " + e.toString());
		}

	}

	public void onCancel() {
		shutDownOperations();
		disableControls(true);
	}

	@Action
	public void onFirstFind() {

		if (!checkIfCanNavigate()) {
			return;
		}

		int selectedIndex = jDataTab.getSelectedRow();
		if (selectedIndex == -1) {
			// if no row selected - select first
			selectedIndex = 0;

		}


		jScrollDown.setSelected(false);
		long rowIdx = -1;

		try {
			// include in the search first entry
			rowIdx = (int) eventsCache_m.findNextRow(symbolPattern_m, IDX_SYMBOL_COL,  0, true);
		} catch (Throwable e) {
			HtFrameLogger.logError(e, "Exception when looking the first entry: " + e.getMessage());

		}

		mesageUpdateOnNavigate(rowIdx, selectedIndex);
	}

	@Action
	public void onPrevFind() {
		if (!checkIfCanNavigate()) {
			return;
		}

		
		int selectedIndex = jDataTab.getSelectedRow();
		if (selectedIndex == -1) {
			// if no row selected - select first
			selectedIndex = 0;
		}

		jScrollDown.setSelected(false);
		long rowIdx = -1;
	
		try {
			rowIdx = (int) eventsCache_m.findPrevRow(symbolPattern_m, IDX_SYMBOL_COL,  selectedIndex, false);
		} catch (Throwable e) {
			HtFrameLogger.logError(e, "Exception when looking next entry: " + e.getMessage());

		}

		mesageUpdateOnNavigate(rowIdx, selectedIndex);
	}

	@Action
	public void onNextFind() {
		if (!checkIfCanNavigate()) {
			return;
		}

		int selectedIndex = jDataTab.getSelectedRow();
		if (selectedIndex == -1) {
			// if no row selected - select first
			selectedIndex = 0;

		}

		jScrollDown.setSelected(false);
		long rowIdx = -1;
		

		try {
			// begin serach from selectedIndex
			rowIdx = (int) eventsCache_m.findNextRow(symbolPattern_m, IDX_SYMBOL_COL,  selectedIndex, false);
		} catch (Throwable e) {
			HtFrameLogger.logError(e, "Exception when looking next entry: " + e.getMessage());

		}

		mesageUpdateOnNavigate(rowIdx, selectedIndex);

	}

	@Action
	public void onLastFind() {
		if (!checkIfCanNavigate()) {
			return;
		}

		int selectedIndex = jDataTab.getSelectedRow();
		if (selectedIndex == -1) {
			// if no row selected - select first
			selectedIndex = 0;

		}
		
		jScrollDown.setSelected(false);
		long rowIdx = -1;

		try {
			rowIdx = (int) eventsCache_m.findPrevRow(symbolPattern_m, IDX_SYMBOL_COL,  -1, true);
		} catch (Throwable e) {
			HtFrameLogger.logError(e, "Exception when looking the last entry: " + e.getMessage());
		}

		mesageUpdateOnNavigate(rowIdx, selectedIndex);

	}

	@Action
	public void onExport() {


		JFileChooser fc = new JFileChooser();
		FileFilter filter1 = new ExtentionFileFilter("CSV", new String[]{"csv"});
		fc.setFileFilter(filter1);

		int returnVal = fc.showSaveDialog(HtRangeBreakerApp.getApplication().getMainFrame());



		if (returnVal == JFileChooser.APPROVE_OPTION) {
			File f = fc.getSelectedFile();
			if (!f.isDirectory()) {
				exportSelectdToFile(f);
			}
		}


	}


	/*
	 * Helpers
	 */
	private void exportSelectdToFile(final File f) {


		BufferedWriter br = null;
		try {

			br = new BufferedWriter(new FileWriter(f));

			final BufferedWriter fbr = br;
			EventBerkleyDbBuffer.RowExportReceiver ei = new EventBerkleyDbBuffer.RowExportReceiver() {

				private long cnt_m = 0;
				private boolean cancelled_m = false;
				public long getExportedRowsCount()
				{
					return cnt_m;
				};

				ProgressDialogInterface pint_m = new ProgressDialogInterface() {

					public void onCancel() {
						cancelled_m = true;
					}
				};

				public void onExportBegin() {
					
				}

				public void onExportFinish() {
					
				}

				public boolean onMatchingRowArrived(Object[] row) {

					try {
						for(int i=0; i < row.length; i++) {

							fbr.write("\"");
							fbr.write((String)row[i]);
							fbr.write("\"");
							fbr.write(";");

						}
						fbr.write("\r\n");
						cnt_m++;
					}
					catch(IOException e)
					{
						HtFrameLogger.logError(e, "Exception on writing: " + e.getMessage());
					}
					return cancelled_m;
				}
			};

			eventsCache_m.exportUtility(this.symbolPattern_m, this.IDX_SYMBOL_COL, ei);

			MsgBox.showMessageBox("Finished, exported " + ei.getExportedRowsCount() + " row(s)" );
			
		} catch (Throwable e) {
			HtFrameLogger.logError(e, "Exception on exporting data to file: " + f.getAbsolutePath());
		} finally {
			if (br != null) {
				try {
					br.close();
				} catch (Throwable e2) {
				}
				
			}
		}
	}

	private boolean checkIfCanNavigate() {

		DiskCachedTableModel model = (DiskCachedTableModel) jDataTab.getModel();
		if (model.getRowCount() == 0) {
			return false;
		}

		return true;
	}

	private void mesageUpdateOnNavigate(long rowIdx, long oldIndex) {
		if (rowIdx >= 0) {
			jDataTab.setRowSelectionInterval((int)rowIdx, (int)rowIdx);
			GuiUtils.scrollToVisible(jDataTab, (int)rowIdx);
		} else {
			if (oldIndex >=0) {
				jDataTab.setRowSelectionInterval((int)oldIndex, (int)oldIndex);
				GuiUtils.scrollToVisible(jDataTab, (int)oldIndex);
			}
			
		}
	}

	private void enableSelectionNavigationButtons(boolean enabled) {
		jSelectedFirst.setEnabled(enabled);
		jSelectedPrev.setEnabled(enabled);
		jSelectedNext.setEnabled(enabled);
		jSelectedLast.setEnabled(enabled);
		jSymbolText.setEnabled(!enabled);
		
		jExportBtn.setEnabled(enabled);
	}

	private void sendClearTableMessage() {
		// need to stop
		SwingUtilities.invokeLater(new Runnable() {

			public void run() {

				final DiskCachedTableModel model = (DiskCachedTableModel) jDataTab.getModel();
				model.clearRowCount();
				model.fireTableDataChanged();

				DefaultTableModel signalmodel = (DefaultTableModel) jSignalTab.getModel();
				signalmodel.setRowCount(0);

			}
		});
	}

	private void disableControls(boolean enabled) {
		this.jTextFromDate.setEnabled(enabled);
		this.jTextToDate.setEnabled(enabled);
		this.jShortTextFilter.setEnabled(enabled);
		this.jTextFilter.setEnabled(enabled);
		this.jProfileNameTxt.setEnabled(enabled);
	}

	private void shutDownOperations() {
		if (tradeEventReceiver_m != null) {


			//DefaultTableModel model = (DefaultTableModel)jDataTab.getModel();
			//model.setRowCount(0);

			tradeEventReceiver_m.shutdown();
			tradeEventReceiver_m = null;
			eventsCache_m.close();

			sendClearTableMessage();

		}
	}

	private void setupSymbolBackground() {


		Color c = null;
		if (jSymbolText.getText().length() == 0) {
			c = new Color(204, 255, 153);

		} else {
			c = Color.white;

		}

		jSymbolText.setBackground(c);
	}

	

	private void showConnectChannelStatus(final boolean is_connected) {
		SwingUtilities.invokeLater(new Runnable() {

			public void run() {

				if (is_connected) {
					jConnectStatusTxt.setText("CONNECTED");
					jConnectStatusTxt.setBackground(Color.GREEN);
				} else {
					jConnectStatusTxt.setText("DISCONNECTED");
					jConnectStatusTxt.setBackground(Color.RED);
				}
			}
		});

	}

	private void navigateSignalInMainDataTable(int row_id_val) {
		jDataTab.setRowSelectionInterval(row_id_val, row_id_val);
		GuiUtils.scrollToVisible(jDataTab, row_id_val);
	}
}
