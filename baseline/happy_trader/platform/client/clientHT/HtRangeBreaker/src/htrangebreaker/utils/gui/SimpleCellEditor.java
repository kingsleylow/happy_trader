/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package htrangebreaker.utils.gui;

import java.awt.Font;
import java.util.EventObject;
import javax.swing.DefaultCellEditor;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

/**
 *
 * @author DanilinS
 */
public class SimpleCellEditor extends DefaultCellEditor {
		private JTextArea textArea_m = null;
		private JScrollPane scrollPane_m = null;

		public void setFont(Font f)
		{
			textArea_m.setFont(f);
		}



		public SimpleCellEditor() {
			super(new JTextField());
			textArea_m = new JTextArea();
			textArea_m.setWrapStyleWord(true);
			textArea_m.setLineWrap(true);
			scrollPane_m = new JScrollPane(textArea_m);
			scrollPane_m.setBorder(null);
			editorComponent = scrollPane_m;



			delegate = new DefaultCellEditor.EditorDelegate() {

				@Override
				public void setValue(Object value) {
					textArea_m.setText((value != null) ? value.toString() : "");
				}

				@Override
				public Object getCellEditorValue() {
					return textArea_m.getText();
				}
			};
		}

}
