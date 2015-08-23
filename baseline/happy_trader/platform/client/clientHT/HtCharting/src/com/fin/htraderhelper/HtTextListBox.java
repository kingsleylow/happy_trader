/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * HtTextListBox.java
 *
 * Created on 01.03.2010, 11:10:03
 * This contains the lits of text objects
 */
package com.fin.htraderhelper;

import com.fin.htraderhelper.utils.HtDrawableObject;
import com.fin.htraderhelper.utils.Utils;
import java.awt.Color;
import java.awt.Component;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import javax.swing.JLabel;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;
import javax.swing.UIManager;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableCellRenderer;
import javax.swing.table.TableColumn;
import org.jdesktop.application.Action;

/**
 *
 * @author DanilinS
 */
public class HtTextListBox extends javax.swing.JFrame {

	DataChart base_m = null;
	Map<Long, List<HtDrawableObject.Text>> textObjectMap_m = null;
	Set<Integer> selectedRows_m = new HashSet<Integer>();

	// ----------------------------

	// This renderer extends a component. It is used each time a
// cell must be displayed.
	private class MyTableCellRenderer extends JLabel implements TableCellRenderer {


		private HtTextListBox base_m = null;
		// This method is called each time a cell in a column
		// using this renderer needs to be rendered.

		public MyTableCellRenderer(HtTextListBox base)
		{
			base_m = base;
		}


		// The following methods override the defaults for performance reasons
		public void validate() {
		}

		public void revalidate() {
		}

		protected void firePropertyChange(String propertyName, Object oldValue, Object newValue) {
		}

		public void firePropertyChange(String propertyName, boolean oldValue, boolean newValue) {
		}

		public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
			// 'value' is value contained in the cell located at
			// (rowIndex, vColIndex)

			int idx = Integer.valueOf((String)table.getValueAt(row, 3));
			boolean our_row = base_m.selectedRows_m.contains( idx );

			if (hasFocus || isSelected) {
				// cell (and perhaps other cells) are selected
				setForeground(table.getSelectionForeground());
				setOpaque(true);
				setBackground(Color.BLACK);

			} else {
				setForeground(table.getForeground());
				//super.setBackground(table.getBackground());

				if (!our_row)
					super.setBackground(table.getBackground());
				else {
					setOpaque(true);
					super.setBackground(Color.YELLOW);
				}

			}


		
			// Configure the component with the specified value
			setText(Utils.formatDateTime(((Long) value).longValue()));



			// Set tool tip if desired
			//setToolTipText((String) value);

			// Since the renderer is a component, return itself
			return this;
		}
	}

	/** Creates new form HtTextListBox */
	public HtTextListBox(Map<Long, List<HtDrawableObject.Text>> textObjectsMap, DataChart base) {
		initComponents();
		base_m = base;
		textObjectMap_m = textObjectsMap;

		DefaultTableModel model = (DefaultTableModel) jTextListTable.getModel();
		this.clearAllRows();
		jTextListTable.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
		jTextListTable.getTableHeader().setReorderingAllowed(false);


		model.setColumnCount(0);

		model.addColumn("Time");
		model.addColumn("Short Text");
		model.addColumn("Text");
		model.addColumn("ID");

		// renderer
		TableColumn col = jTextListTable.getColumnModel().getColumn(0);
		col.setCellRenderer(new MyTableCellRenderer(this));


		int idx = 0;
		for (Iterator<Long> it = textObjectsMap.keySet().iterator(); it.hasNext();) {
			Long time_i = it.next();
			List<HtDrawableObject.Text> list_i = textObjectsMap.get(time_i);
			for (int i = 0; i < list_i.size(); i++) {
				HtDrawableObject.Text text_i = list_i.get(i);
				model.addRow(new Object[]{time_i, text_i.shortText_m.toString(), text_i.text_m.toString(), String.valueOf(idx++)});
			}
		}

		jTextListTable.getColumnModel().getColumn(0).setPreferredWidth(150);
		jTextListTable.getColumnModel().getColumn(1).setPreferredWidth(150);
		jTextListTable.getColumnModel().getColumn(2).setPreferredWidth(1100);
		jTextListTable.getColumnModel().getColumn(3).setPreferredWidth(50);


		// row selection listener
		ListSelectionModel rowSM = jTextListTable.getSelectionModel();
		rowSM.addListSelectionListener(new ListSelectionListener() {

			public void valueChanged(ListSelectionEvent e) {
				if (e.getValueIsAdjusting()) {
					return; // if you don't want to handle intermediate selections
				}
				ListSelectionModel rowSM = (ListSelectionModel) e.getSource();

				if (base_m != null) {
					int selectedIndex = rowSM.getMinSelectionIndex();

					if (selectedIndex < 0) {
						return;
					}


					long ttime = (Long) jTextListTable.getValueAt(selectedIndex, 0);
					if (ttime <= 0) {
						return;
					}

					//System.out.println("Time to draw marker: " + Utils.formatDateTime(ttime));

					base_m.drawTimeMarkerViaExtrenalCall(ttime);
				}

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

    jScrollPane1 = new javax.swing.JScrollPane();
    jTextListTable = new javax.swing.JTable();
    jBtnSearch = new javax.swing.JButton();

    setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
    setName("Form"); // NOI18N

    jScrollPane1.setName("jScrollPane1"); // NOI18N

    jTextListTable.setAutoCreateRowSorter(true);
    jTextListTable.setModel(new DefaultTableModel() {
      public boolean isCellEditable(int rowIndex, int mColIndex) {
        return false;
      }
    });
    jTextListTable.setAutoResizeMode(javax.swing.JTable.AUTO_RESIZE_OFF);
    jTextListTable.setName("jTextListTable"); // NOI18N
    jTextListTable.addMouseListener(new java.awt.event.MouseAdapter() {
      public void mouseClicked(java.awt.event.MouseEvent evt) {
        jTextListBoxMouseClicked(evt);
      }
    });
    jScrollPane1.setViewportView(jTextListTable);

    javax.swing.ActionMap actionMap = org.jdesktop.application.Application.getInstance(com.fin.htraderhelper.HtChartingApp.class).getContext().getActionMap(HtTextListBox.class, this);
    jBtnSearch.setAction(actionMap.get("doSearch")); // NOI18N
    org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(com.fin.htraderhelper.HtChartingApp.class).getContext().getResourceMap(HtTextListBox.class);
    jBtnSearch.setText(resourceMap.getString("jBtnSearch.text")); // NOI18N
    jBtnSearch.setName("jBtnSearch"); // NOI18N

    javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
    getContentPane().setLayout(layout);
    layout.setHorizontalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(layout.createSequentialGroup()
        .addContainerGap()
        .addComponent(jBtnSearch)
        .addContainerGap(454, Short.MAX_VALUE))
      .addComponent(jScrollPane1, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 531, Short.MAX_VALUE)
    );
    layout.setVerticalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
      .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
        .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 228, Short.MAX_VALUE)
        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
        .addComponent(jBtnSearch)
        .addContainerGap())
    );

    pack();
  }// </editor-fold>//GEN-END:initComponents

	private void jTextListBoxMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_jTextListBoxMouseClicked
		// TODO add your handling code here:
		if (evt.getClickCount() == 2) {
			// here we shall show the text related here
			int row = jTextListTable.getSelectedRow();

			if (row < 0) {
				return;
			}

			long ttime = (Long) jTextListTable.getValueAt(row, 0);

			if (ttime <= 0) {
				return;
			}

			// show that row data in a separate window
			if (textObjectMap_m.containsKey(ttime)) {
				List<HtDrawableObject.Text> text_obj_list = new ArrayList<HtDrawableObject.Text>();

				HtDrawableObject newobj = new HtDrawableObject();
				HtDrawableObject.Text tobj = newobj.getAsText(true);

				tobj.priority_m = 0;
				tobj.shortText_m.append(jTextListTable.getValueAt(row, 1));
				tobj.text_m.append(jTextListTable.getValueAt(row, 2));

				text_obj_list.add(tobj);

				HtTextBox tbox = new HtTextBox(text_obj_list, ttime);
				tbox.setVisible(true);
			}


		}
	}//GEN-LAST:event_jTextListBoxMouseClicked

	private void clearAllRows() {
		while (jTextListTable.getRowCount() > 0) {
			((DefaultTableModel) jTextListTable.getModel()).removeRow(0);
		}
	}

	@Action
	public void doSearch() {
		// search here
		try {

			selectedRows_m.clear();

			// search patterns
			final StringBuilder shortTextPattern = new StringBuilder();
			final StringBuilder longTextPattern = new StringBuilder();

			HSearchInput2 searchd = new HSearchInput2(this, true, shortTextPattern, longTextPattern);

			// no filter
			if (shortTextPattern.length() <= 0 && longTextPattern.length() <= 0) {
				//update
				((AbstractTableModel)jTextListTable.getModel()).fireTableDataChanged();
				return;
			}

			int idx = 0;
			for (Iterator<Long> it = textObjectMap_m.keySet().iterator(); it.hasNext();) {
				Long time_i = it.next();

				List<HtDrawableObject.Text> list_i = textObjectMap_m.get(time_i);

				for (int i = 0; i < list_i.size(); i++) {
					HtDrawableObject.Text text_i = list_i.get(i);

					if (shortTextPattern.length() > 0) {
						if (text_i.shortText_m.toString().indexOf(shortTextPattern.toString()) >= 0) {
							selectedRows_m.add( idx );
						}

					}

					if (longTextPattern.length() > 0) {
						if (text_i.text_m.toString().indexOf(longTextPattern.toString()) >= 0) {
							selectedRows_m.add( idx );
						}
					}

					idx++;

				}
			}



		} catch (Throwable e) {
			new MsgBox(this, "Exception on searching: " + e.getMessage(), false);
		}

		//update
		((AbstractTableModel)jTextListTable.getModel()).fireTableDataChanged();
	}
	// ---------------------------------
	// helpers
  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JButton jBtnSearch;
  private javax.swing.JScrollPane jScrollPane1;
  private javax.swing.JTable jTextListTable;
  // End of variables declaration//GEN-END:variables
}
