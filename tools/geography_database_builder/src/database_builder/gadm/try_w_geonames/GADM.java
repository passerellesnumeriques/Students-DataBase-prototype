package database_builder.gadm.try_w_geonames;

import java.sql.Connection;
import java.sql.Driver;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Enumeration;
import java.util.LinkedList;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.Properties;

import sql.SQLProcessor;
import database_builder.DataBase;

public class GADM {

	public static void main(String[] args) {
		try {
			System.out.println("Connecting to GADM Access DataBase...");
			gadm = connect_gadm();
			System.out.println("Connection to MySQL DataBase...");
			db = DataBase.connect_db();
			SQLProcessor.start_threads();
			Names.init();
			Map<String,String> countries_gadmid = Countries.process();
			Adm.process(countries_gadmid);
			Names.end();
			SQLProcessor.end_threads();
			db.close();
			gadm.close();
		} catch (Throwable t) {
			t.printStackTrace(System.err);
		}
	}
	
	static Connection connect_gadm() throws SQLException {
		Class<?> clazz;
		try { clazz = Class.forName("sun.jdbc.odbc.JdbcOdbcDriver"); }
		catch (ClassNotFoundException e) { throw new SQLException(e); }
		Enumeration<Driver> drivers = DriverManager.getDrivers();
		Driver driver = null;
		while (drivers.hasMoreElements()) {
			Driver d = drivers.nextElement();
			if (d.getClass().equals(clazz)) {
				driver = d;
				break;
			}
		}
		Properties props = new Properties();
		props.setProperty("user", "");
		props.setProperty("password", "");
		return driver.connect("jdbc:odbc:Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=D:/Code/content/geography/gadm.org/gadm2.mdb",props);
	}
	
	static Connection gadm, db;
	
	public static List<String> getNames(String name) {
		LinkedList<String> list = new LinkedList<String>();
		if (name != null && name.length() > 0) {
			do {
				int i = name.indexOf('|');
				if (i < 0) {
					list.add(name);
					break;
				} else {
					list.add(name.substring(0,i));
					name = name.substring(i+1);
				}
			} while (name.length() > 0);
			for (ListIterator<String> it = list.listIterator(); it.hasNext(); ) {
				String s = it.next();
				int i = s.indexOf(" or ");
				if (i > 0) {
					it.remove();
					do {
						it.add(s.substring(0,i));
						s = s.substring(i+4);
						i = s.indexOf(" or ");
						if (i < 0) { it.add(s); break; }
					} while (true);
				}
			}
			for (ListIterator<String> it = list.listIterator(); it.hasNext(); ) {
				String s = it.next();
				int i = s.indexOf(",");
				if (i > 0) {
					String s1 = s.substring(0,i).trim();
					String s2 = s.substring(i+1).trim();
					if (s1.charAt(0) == s2.charAt(0)) {
						it.remove();
						do {
							it.add(s.substring(0,i).trim());
							s = s.substring(i+1).trim();
							i = s.indexOf(",");
							if (i < 0) { it.add(s); break; }
						} while (true);
					}
				}
			}
		}
		return list;
	}
	
}
