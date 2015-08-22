/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package htrangebreaker.utils.gui;

import java.awt.Color;
import java.awt.Component;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import javax.swing.JTable;
import javax.swing.JTextArea;
import javax.swing.UIManager;
import javax.swing.border.Border;
import javax.swing.border.EmptyBorder;
import javax.swing.table.TableCellRenderer;
import javax.swing.table.TableColumn;
import javax.swing.table.TableColumnModel;

/**
 *
 * @author DanilinS
 */
public class MultiLineTableRenderer extends JTextArea implements TableCellRenderer {

		public static interface RenderInterface
		{
			public String onRenderValue(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column);
			public Color getBackgroundColor(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column);
			public Color getForegroundColor(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column);

			
		};
		// ----------------------

		private final Map cellSizes = new HashMap();
		private Border border_m = null;
		private RenderInterface rint_m = null;

		public MultiLineTableRenderer(RenderInterface  rint) {
			rint_m = rint;
			border_m = UIManager.getBorder("Table.focusCellHighlightBorder");
			setLineWrap(true);
			setWrapStyleWord(true);
			setOpaque(true);

		}

		@Override
		public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {

			//Component cell = super.getTableCellRendererComponent(table, value, isSelected, hasFocus, row, column);


			setText(rint_m.onRenderValue(table, value, isSelected, hasFocus, row, column));
			

			// This line was very important to get it working with JDK1.4
			TableColumnModel columnModel = table.getColumnModel();
			setSize(columnModel.getColumn(column).getWidth(), 100000);
			int height_wanted = (int) getPreferredSize().getHeight();
			addSize(table, row, column, height_wanted);
			height_wanted = findTotalMaximumRowSize(table, row);
			if (height_wanted != table.getRowHeight(row)) {
				table.setRowHeight(row, height_wanted);
			}

			if (isSelected) {
				super.setForeground(table.getSelectionForeground());
				super.setBackground(table.getSelectionBackground());
			} else {
				super.setForeground(table.getForeground());
				super.setBackground(table.getBackground());
			}

			Color c_bckg = rint_m.getBackgroundColor(table, value, isSelected, hasFocus, row, column);
			Color c_frgr = rint_m.getForegroundColor(table, value, isSelected, hasFocus, row, column);

			if (c_bckg != null) {
				super.setBackground(c_bckg);
			}

			if (c_frgr != null) {
				super.setForeground(c_frgr);
			}

			setFont(table.getFont());


			if (hasFocus) {
				setBorder(border_m);
				//if (table.isCellEditable(row, column)) {
				//	setForeground(UIManager.getColor("Table.focusCellForeground"));
				//	setBackground(UIManager.getColor("Table.focusCellBackground"));
				//}
			} else {
				setBorder(new EmptyBorder(1, 2, 1, 2));
			}



			return this;
		}

		private void addSize(JTable table, int row, int column,
						int height) {
			Map rows = (Map) cellSizes.get(table);
			if (rows == null) {
				cellSizes.put(table, rows = new HashMap());
			}
			Map rowheights = (Map) rows.get(new Integer(row));
			if (rowheights == null) {
				rows.put(new Integer(row), rowheights = new HashMap());
			}
			rowheights.put(new Integer(column), new Integer(height));
		}

		/**
		 * Look through all columns and get the renderer.  If it is
		 * also a TextAreaRenderer, we look at the maximum height in
		 * its hash table for this row.
		 */
		private int findTotalMaximumRowSize(JTable table, int row) {
			int maximum_height = 0;
			Enumeration columns = table.getColumnModel().getColumns();
			while (columns.hasMoreElements()) {
				TableColumn tc = (TableColumn) columns.nextElement();
				TableCellRenderer cellRenderer = tc.getCellRenderer();
				if (cellRenderer instanceof MultiLineTableRenderer) {
					MultiLineTableRenderer tar = (MultiLineTableRenderer) cellRenderer;
					maximum_height = Math.max(maximum_height,
									tar.findMaximumRowSize(table, row));
				}
			}
			return maximum_height;
		}

		private int findMaximumRowSize(JTable table, int row) {
			Map rows = (Map) cellSizes.get(table);
			if (rows == null) {
				return 0;
			}
			Map rowheights = (Map) rows.get(new Integer(row));
			if (rowheights == null) {
				return 0;
			}
			int maximum_height = 0;
			for (Iterator it = rowheights.entrySet().iterator();it.hasNext();) {
				Map.Entry entry = (Map.Entry) it.next();
				int cellHeight = ((Integer) entry.getValue()).intValue();
				maximum_height = Math.max(maximum_height, cellHeight);
			}
			return maximum_height;
		}
}
