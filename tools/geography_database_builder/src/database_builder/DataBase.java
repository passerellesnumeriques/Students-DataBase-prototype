package database_builder;

import java.sql.Connection;
import java.sql.Driver;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Enumeration;
import java.util.Properties;

public class DataBase {

	public static Connection connect_db() throws SQLException {
		String server = "localhost";
		String username = "root";
		String password = "";
		// connect to db
		Class<?> clazz;
		try { clazz = Class.forName("com.mysql.jdbc.Driver"); }
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
		props.setProperty("user", username);
		props.setProperty("password", password);
		return driver.connect("jdbc:mysql://"+server, props);
	}
	
	
}
