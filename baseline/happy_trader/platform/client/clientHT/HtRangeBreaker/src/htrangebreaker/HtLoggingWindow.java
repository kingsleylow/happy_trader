/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * HtLoggingWindow.java
 *
 * Created on 03.08.2010, 16:35:54
 */
package htrangebreaker;

import htrangebreaker.utils.gui.HtWorkInProgressDlg;
import htrangebreaker.utils.gui.MsgBox;
import com.fin.httrader.utils.HtDateTimeUtils;
import com.fin.httrader.utils.ThreadLocalFormats.HtThreadLocalSimpleDateFormat;
import com.fin.httrader.utils.hlpstruct.CommonLog;
import com.fin.httrader.utils.hlpstruct.HtDrawableObject;
import com.fin.httrader.utils.hlpstruct.HtRtData;
import htrangebreaker.utils.CommunicationInterface;
import htrangebreaker.utils.HtCommonEventReceiver;
import htrangebreaker.utils.HtFrameLogger;
import htrangebreaker.utils.HtKeyedPair;
import htrangebreaker.utils.HtSettingsStruct;

import htrangebreaker.utils.ProgressDialogInterface;
import htrangebreaker.utils.gui.GuiUtils;
import java.awt.Color;
import java.util.Iterator;
import java.util.Map;
import javax.swing.DefaultComboBoxModel;

import javax.swing.JTable;
import javax.swing.SwingUtilities;
import javax.swing.table.DefaultTableModel;
import org.jdesktop.application.Action;


import htrangebreaker.utils.gui.MultiLineTableRenderer;
import htrangebreaker.utils.gui.SimpleCellEditor;

import java.awt.Rectangle;
import java.io.UnsupportedEncodingException;

import java.util.Date;

import java.util.TimeZone;
import java.util.logging.Level;
import javax.swing.SpinnerNumberModel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
 *
 * @author DanilinS
 */
public class HtLoggingWindow extends javax.swing.JInternalFrame implements CommunicationInterface, ProgressDialogInterface {

	// ------------------------
	private HtCommonEventReceiver commonEventReceiver_m = null;
	private HtLoggingWindow_LogTableRenderer renderer_m = null;
	private int defaultRowHight_m = 0;
	private static HtThreadLocalSimpleDateFormat dateTimeFormat_m = new HtThreadLocalSimpleDateFormat("dd-MM-yyyy HH:mm:ss", "GMT-0");
	// const
	private static final int TAB_IDX_TIME = 0;
	private static final int TAB_IDX_LEVEL = 1;
	private static final int TAB_IDX_THREAD = 2;
	private static final int TAB_IDX_SESSION = 3;
	private static final int TAB_IDX_CONTEXT = 4;
	private static final int TAB_IDX_CONTENT = 5;

	// --------------------------------
	/** Creates new form HtLoggingWindow */
	public HtLoggingWindow() {
		initComponents();

		// init some components
		DefaultComboBoxModel cbmodel = (DefaultComboBoxModel) jLogLevelCB.getModel();

		Map<Integer, String> mp = CommonLog.getAvailableLogLevelsMap();
		for (Iterator<Integer> it = mp.keySet().iterator(); it.hasNext();) {

			Integer key = it.next();
			String value = mp.get(key);


			HtKeyedPair pair = new HtKeyedPair(key, value);
			cbmodel.addElement(pair);
		}

		cbmodel.setSelectedItem(new HtKeyedPair(CommonLog.LL_INFO, CommonLog.getLogLevelName(CommonLog.LL_INFO)));

		//java.awt.Frame frm = HtRangeBreakerApp.getApplication().getMainFrame();

		// table
		DefaultTableModel model = (DefaultTableModel) jLogTable.getModel();
		model.addColumn("Time");
		model.addColumn("Level");
		model.addColumn("Thread");
		model.addColumn("Session");
		model.addColumn("Context");
		model.addColumn("Content");

		renderer_m = new HtLoggingWindow_LogTableRenderer(new HtLoggingWindow_LogTableRenderer.RenderInterface() {

			public String onRenderValue(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
				switch (column) {
					case TAB_IDX_TIME:
						return HtDateTimeUtils.time2String_Gmt(((Long) value).longValue());

					case TAB_IDX_LEVEL:
						return CommonLog.getLogLevelName(((Integer) value).intValue());

					case TAB_IDX_THREAD:
						return String.valueOf(((Long) value).longValue());

					case TAB_IDX_SESSION:
						return (String) value;

					case TAB_IDX_CONTEXT:
						return (String) value;

					case TAB_IDX_CONTENT:
						return (String) value;

					default:
						return "";
				}

			}

			public Color getBackgroundColor(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
				int log_level = ((Integer) table.getValueAt(row, TAB_IDX_LEVEL)).intValue();
				if (log_level == CommonLog.LL_WARN) {
					if (isSelected) {
						return Color.GREEN;
					} else {
						return Color.YELLOW;
					}
				} else if (log_level <= CommonLog.LL_ERROR) {
					if (isSelected) {
						return Color.RED;
					} else {
						return Color.ORANGE;
					}
				}

				return null;
			}

			public Color getForegroundColor(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
				return null;
			}
		});

		jLogTable.getColumnModel().getColumn(TAB_IDX_TIME).setCellRenderer(renderer_m);
		jLogTable.getColumnModel().getColumn(TAB_IDX_LEVEL).setCellRenderer(renderer_m);
		jLogTable.getColumnModel().getColumn(TAB_IDX_THREAD).setCellRenderer(renderer_m);
		jLogTable.getColumnModel().getColumn(TAB_IDX_SESSION).setCellRenderer(renderer_m);
		jLogTable.getColumnModel().getColumn(TAB_IDX_CONTEXT).setCellRenderer(renderer_m);
		jLogTable.getColumnModel().getColumn(TAB_IDX_CONTENT).setCellRenderer(renderer_m);

		jLogTable.getColumnModel().getColumn(TAB_IDX_TIME).setPreferredWidth(150);
		jLogTable.getColumnModel().getColumn(TAB_IDX_LEVEL).setPreferredWidth(60);
		jLogTable.getColumnModel().getColumn(TAB_IDX_THREAD).setPreferredWidth(60);
		jLogTable.getColumnModel().getColumn(TAB_IDX_SESSION).setPreferredWidth(100);
		jLogTable.getColumnModel().getColumn(TAB_IDX_CONTEXT).setPreferredWidth(100);
		jLogTable.getColumnModel().getColumn(TAB_IDX_CONTENT).setPreferredWidth(500);


		SimpleCellEditor editor = new SimpleCellEditor();
		editor.setFont(jLogTable.getFont());
		jLogTable.getColumnModel().getColumn(TAB_IDX_CONTENT).setCellEditor(editor);



		// to/from date / times
		// add first item
		HtSettingsStruct s = HtSettings.getInstance().getSettings();
		long begin_date_hist = HtDateTimeUtils.getCurGmtTime_DayBegin() - (long) s.log_history_depth_days_m * 24 * 60 * 60 * 1000;

		jTextFromDate.setValue(new Date(begin_date_hist));

		jRowsCnt.setText(String.valueOf(jLogTable.getRowCount()));
		showConnectChannelStatus(false);

		//
		//row hights
		defaultRowHight_m = jLogTable.getRowHeight();
		jRowHightSpinner.addChangeListener(new ChangeListener() {

			public void stateChanged(ChangeEvent event) {
				SpinnerNumberModel model = ((SpinnerNumberModel) jRowHightSpinner.getModel());
				int rowsn = model.getNumber().intValue();
				int rowhight = rowsn * defaultRowHight_m + 2;

				jLogTable.setRowHeight(rowhight);

			}
		});

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
    jLabel1 = new javax.swing.JLabel();
    jContentFilter = new javax.swing.JTextField();
    jLabel2 = new javax.swing.JLabel();
    jThreadFilter = new javax.swing.JTextField();
    jLabel3 = new javax.swing.JLabel();
    jLogLevelCB = new javax.swing.JComboBox();
    jLabel4 = new javax.swing.JLabel();
    jContextFilter = new javax.swing.JTextField();
    jLabel5 = new javax.swing.JLabel();
    jLabel6 = new javax.swing.JLabel();
    jStartStop = new javax.swing.JToggleButton();
    jSeparator1 = new javax.swing.JSeparator();
    jTextFromDate = new javax.swing.JFormattedTextField( dateTimeFormat_m.getFormatThreadCopy() );
    jTextToDate = new javax.swing.JFormattedTextField( dateTimeFormat_m.getFormatThreadCopy() );
    jScrollDown = new javax.swing.JCheckBox();
    jRowsCnt = new javax.swing.JTextField();
    jConnectStatusTxt = new javax.swing.JTextField();
    jLabel7 = new javax.swing.JLabel();
    jRowHightSpinner = new javax.swing.JSpinner();
    jScrollPane1 = new javax.swing.JScrollPane();
    jLogTable = new javax.swing.JTable();
    jMenuBar1 = new javax.swing.JMenuBar();

    setClosable(true);
    setIconifiable(true);
    setMaximizable(true);
    setResizable(true);
    org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(htrangebreaker.HtRangeBreakerApp.class).getContext().getResourceMap(HtLoggingWindow.class);
    setTitle(resourceMap.getString("Form.title")); // NOI18N
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

    jLabel1.setText(resourceMap.getString("jLabel1.text")); // NOI18N
    jLabel1.setName("jLabel1"); // NOI18N

    jContentFilter.setText(resourceMap.getString("jContentFilter.text")); // NOI18N
    jContentFilter.setName("jContentFilter"); // NOI18N

    jLabel2.setText(resourceMap.getString("jLabel2.text")); // NOI18N
    jLabel2.setName("jLabel2"); // NOI18N

    jThreadFilter.setText(resourceMap.getString("jThreadFilter.text")); // NOI18N
    jThreadFilter.setName("jThreadFilter"); // NOI18N

    jLabel3.setText(resourceMap.getString("jLabel3.text")); // NOI18N
    jLabel3.setName("jLabel3"); // NOI18N

    jLogLevelCB.setModel(new DefaultComboBoxModel());
    jLogLevelCB.setName("jLogLevelCB"); // NOI18N

    jLabel4.setText(resourceMap.getString("jLabel4.text")); // NOI18N
    jLabel4.setName("jLabel4"); // NOI18N

    jContextFilter.setText(resourceMap.getString("jContextFilter.text")); // NOI18N
    jContextFilter.setName("jContextFilter"); // NOI18N

    jLabel5.setText(resourceMap.getString("jLabel5.text")); // NOI18N
    jLabel5.setName("jLabel5"); // NOI18N

    jLabel6.setText(resourceMap.getString("jLabel6.text")); // NOI18N
    jLabel6.setName("jLabel6"); // NOI18N

    javax.swing.ActionMap actionMap = org.jdesktop.application.Application.getInstance(htrangebreaker.HtRangeBreakerApp.class).getContext().getActionMap(HtLoggingWindow.class, this);
    jStartStop.setAction(actionMap.get("doStartStop")); // NOI18N
    jStartStop.setText(resourceMap.getString("jStartStop.text")); // NOI18N
    jStartStop.setName("jStartStop"); // NOI18N

    jSeparator1.setName("jSeparator1"); // NOI18N

    jTextFromDate.setText(resourceMap.getString("jTextFromDate.text")); // NOI18N
    jTextFromDate.setName("jTextFromDate"); // NOI18N

    jTextToDate.setText(resourceMap.getString("jTextToDate.text")); // NOI18N
    jTextToDate.setName("jTextToDate"); // NOI18N

    jScrollDown.setText(resourceMap.getString("jScrollDown.text")); // NOI18N
    jScrollDown.setName("jScrollDown"); // NOI18N

    jRowsCnt.setEditable(false);
    jRowsCnt.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
    jRowsCnt.setName("jRowsCnt"); // NOI18N

    jConnectStatusTxt.setEditable(false);
    jConnectStatusTxt.setText(resourceMap.getString("jConnectStatusTxt.text")); // NOI18N
    jConnectStatusTxt.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));
    jConnectStatusTxt.setName("jConnectStatusTxt"); // NOI18N

    jLabel7.setText(resourceMap.getString("jLabel7.text")); // NOI18N
    jLabel7.setName("jLabel7"); // NOI18N

    jRowHightSpinner.setModel(new javax.swing.SpinnerNumberModel(Integer.valueOf(1), Integer.valueOf(1), null, Integer.valueOf(1)));
    jRowHightSpinner.setName("jRowHightSpinner"); // NOI18N

    javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
    jPanel1.setLayout(jPanel1Layout);
    jPanel1Layout.setHorizontalGroup(
      jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(jPanel1Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
          .addGroup(jPanel1Layout.createSequentialGroup()
            .addComponent(jLabel1)
            .addGap(18, 18, 18)
            .addComponent(jContentFilter, javax.swing.GroupLayout.PREFERRED_SIZE, 101, javax.swing.GroupLayout.PREFERRED_SIZE))
          .addGroup(jPanel1Layout.createSequentialGroup()
            .addComponent(jLabel2)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addComponent(jThreadFilter, javax.swing.GroupLayout.PREFERRED_SIZE, 101, javax.swing.GroupLayout.PREFERRED_SIZE)))
        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
          .addGroup(jPanel1Layout.createSequentialGroup()
            .addGap(32, 32, 32)
            .addComponent(jLabel3)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
            .addComponent(jLogLevelCB, javax.swing.GroupLayout.PREFERRED_SIZE, 124, javax.swing.GroupLayout.PREFERRED_SIZE))
          .addGroup(jPanel1Layout.createSequentialGroup()
            .addGap(18, 18, 18)
            .addComponent(jLabel4)
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addComponent(jContextFilter)))
        .addGap(18, 18, 18)
        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addComponent(jLabel5)
          .addComponent(jLabel6))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
          .addComponent(jTextToDate)
          .addComponent(jTextFromDate, javax.swing.GroupLayout.PREFERRED_SIZE, 132, javax.swing.GroupLayout.PREFERRED_SIZE))
        .addContainerGap(86, Short.MAX_VALUE))
      .addComponent(jSeparator1, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 670, Short.MAX_VALUE)
      .addGroup(jPanel1Layout.createSequentialGroup()
        .addContainerGap()
        .addComponent(jStartStop, javax.swing.GroupLayout.PREFERRED_SIZE, 108, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
        .addComponent(jScrollDown)
        .addGap(13, 13, 13)
        .addComponent(jLabel7)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(jRowHightSpinner, javax.swing.GroupLayout.PREFERRED_SIZE, 51, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 163, Short.MAX_VALUE)
        .addComponent(jRowsCnt, javax.swing.GroupLayout.PREFERRED_SIZE, 87, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(jConnectStatusTxt, javax.swing.GroupLayout.PREFERRED_SIZE, 83, javax.swing.GroupLayout.PREFERRED_SIZE)
        .addContainerGap())
    );
    jPanel1Layout.setVerticalGroup(
      jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel1Layout.createSequentialGroup()
        .addContainerGap()
        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
          .addGroup(jPanel1Layout.createSequentialGroup()
            .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
              .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                .addComponent(jContentFilter, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addComponent(jLabel3)
                .addComponent(jLogLevelCB, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
              .addComponent(jLabel1))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(jLabel2)
              .addComponent(jThreadFilter, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
              .addComponent(jLabel4)
              .addComponent(jContextFilter, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)))
          .addGroup(jPanel1Layout.createSequentialGroup()
            .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(jLabel5)
              .addComponent(jTextFromDate, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
            .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
              .addComponent(jLabel6)
              .addComponent(jTextToDate, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))))
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addComponent(jSeparator1, javax.swing.GroupLayout.DEFAULT_SIZE, 1, Short.MAX_VALUE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
        .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
          .addComponent(jStartStop)
          .addComponent(jScrollDown)
          .addComponent(jConnectStatusTxt, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jRowsCnt, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addComponent(jLabel7)
          .addComponent(jRowHightSpinner, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
        .addContainerGap())
    );

    jScrollPane1.setName("jScrollPane1"); // NOI18N

    jLogTable.setAutoCreateRowSorter(true);
    jLogTable.setFont(new java.awt.Font("Courier New", 0, 11)); // NOI18N
    jLogTable.setModel(new DefaultTableModel() {

      public boolean isCellEditable(int rowIndex, int mColIndex) {
        switch (mColIndex) {
          case TAB_IDX_TIME: return false;
          case TAB_IDX_LEVEL: return false;
          case TAB_IDX_THREAD: return false;
          case TAB_IDX_SESSION: return false;
          case TAB_IDX_CONTEXT: return false;
        }
        return true;
      };
    }
  );
  jLogTable.setAutoResizeMode(javax.swing.JTable.AUTO_RESIZE_OFF);
  jLogTable.setName("jLogTable"); // NOI18N
  jScrollPane1.setViewportView(jLogTable);

  jMenuBar1.setName("jMenuBar1"); // NOI18N
  setJMenuBar(jMenuBar1);

  javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
  getContentPane().setLayout(layout);
  layout.setHorizontalGroup(
    layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
    .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
    .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 674, Short.MAX_VALUE)
  );
  layout.setVerticalGroup(
    layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
    .addGroup(layout.createSequentialGroup()
      .addComponent(jPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
      .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
      .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 242, Short.MAX_VALUE))
  );

  pack();
  }// </editor-fold>//GEN-END:initComponents

	private void formInternalFrameClosing(javax.swing.event.InternalFrameEvent evt) {//GEN-FIRST:event_formInternalFrameClosing
		// TODO add your handling code here:
		shutDownOperations();
		HtRangeBreakerView.getInstance().deregisterWindow(this);

	}//GEN-LAST:event_formInternalFrameClosing

	@Action
	public void doStartStop() {

		if (jStartStop.isSelected()) {
			jStartStop.setText("Stop");
		} else {
			jStartStop.setText("Start");
		}

		try {
			if (jStartStop.isSelected()) {
				// need to start


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
				commonEventReceiver_m = new HtCommonEventReceiver(
								s.host,
								s.port,
								s.user,
								s.hashed_password,
								from_date,
								to_date,
								jContextFilter.getText(),
								jContentFilter.getText(),
								jThreadFilter.getText(),
								((HtKeyedPair) jLogLevelCB.getSelectedObjects()[0]).key,
								s.isServletDebug_m,
								this);



				disableControls(false);

			} else {
				shutDownOperations();
				disableControls(true);
			}
		} catch (Throwable e) {
			HtFrameLogger.logError(e, "Exception occured: " + e.toString());
		}
	}
  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JTextField jConnectStatusTxt;
  private javax.swing.JTextField jContentFilter;
  private javax.swing.JTextField jContextFilter;
  private javax.swing.JLabel jLabel1;
  private javax.swing.JLabel jLabel2;
  private javax.swing.JLabel jLabel3;
  private javax.swing.JLabel jLabel4;
  private javax.swing.JLabel jLabel5;
  private javax.swing.JLabel jLabel6;
  private javax.swing.JLabel jLabel7;
  private javax.swing.JComboBox jLogLevelCB;
  private javax.swing.JTable jLogTable;
  private javax.swing.JMenuBar jMenuBar1;
  private javax.swing.JPanel jPanel1;
  private javax.swing.JSpinner jRowHightSpinner;
  private javax.swing.JTextField jRowsCnt;
  private javax.swing.JCheckBox jScrollDown;
  private javax.swing.JScrollPane jScrollPane1;
  private javax.swing.JSeparator jSeparator1;
  private javax.swing.JToggleButton jStartStop;
  private javax.swing.JFormattedTextField jTextFromDate;
  private javax.swing.JFormattedTextField jTextToDate;
  private javax.swing.JTextField jThreadFilter;
  // End of variables declaration//GEN-END:variables

	public void onStartLengthyOperation(String operationName) {
		HtRangeBreakerView.getInstance().showWIPDialog(this, operationName);
	}

	public void onStopLengthyOperation() {
		HtRangeBreakerView.getInstance().hideWIPDialog();
	}

	public void onStartCommunicationChannel() throws Exception {
		showConnectChannelStatus(true);
	}

	public void onStopCommunicationChannel() {
		showConnectChannelStatus(false);
	}

	public void onDrawableObjectReceived(boolean isHistory, HtDrawableObject cdraw) {
	}

	public void onHtRtDataReceived(boolean isHistory, HtRtData rtdata) {
	}

	public void onLogEntryReceived(boolean isHistory, final CommonLog clog) {


		final DefaultTableModel model = (DefaultTableModel) jLogTable.getModel();
		final boolean scroll_down = this.jScrollDown.isSelected();

		/*
		String content_s1 = null;
		try {
			content_s1 = new String(clog.getContent().getBytes("Cp1252"), "Cp1251");
		}
		catch(UnsupportedEncodingException e)
		{
		}
		*/


		//final String content_sf =content_s1;
		//jScrollPane1;
		SwingUtilities.invokeLater(new Runnable() {

			public void run() {

				model.addRow(new Object[]{
									Long.valueOf(clog.getLogDate()),
									Integer.valueOf(clog.getLogLevel()),
									Long.valueOf(clog.getLogThread()),
									new String(clog.getSession()),
									new String(clog.getContext()),
									new String(clog.getContent())
								});

				//HtFrameLogger.log( "content: " + clog.getContent());

				jRowsCnt.setText(String.valueOf(model.getRowCount()));

				// then scroll to the last
				if (scroll_down) {
					GuiUtils.scrollToVisible(jLogTable);
				}


			}
		});

	}

	public void onErrorOccured(String message) {

		final StringBuilder fmsg = new StringBuilder("Communication trade events channel error: " + message);
		HtFrameLogger.logError(fmsg.toString());


	}

	public void onHeartBeat() {
	}

	public void onCancel() {
		shutDownOperations();
		disableControls(true);
	}

	/**
	 * Helpers
	 */
	private void disableControls(boolean enabled) {
		jContentFilter.setEnabled(enabled);
		this.jContextFilter.setEnabled(enabled);
		this.jLogLevelCB.setEnabled(enabled);
		this.jTextFromDate.setEnabled(enabled);
		this.jTextToDate.setEnabled(enabled);
		this.jThreadFilter.setEnabled(enabled);

	}

	private void shutDownOperations() {
		if (commonEventReceiver_m != null) {


			// need to stop


			commonEventReceiver_m.shutdown();
			commonEventReceiver_m = null;

			sendClearTableMessage();

		}
	}

	private void sendClearTableMessage() {
		// need to stop
		SwingUtilities.invokeLater(new Runnable() {

			public void run() {

				DefaultTableModel model = (DefaultTableModel) jLogTable.getModel();
				model.setRowCount(0);
				jRowsCnt.setText(String.valueOf(model.getRowCount()));
			}
		});
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
}
