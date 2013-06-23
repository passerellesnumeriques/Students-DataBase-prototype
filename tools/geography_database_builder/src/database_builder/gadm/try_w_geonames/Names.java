package database_builder.gadm.try_w_geonames;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;

import sql.SQLProcessor;

public class Names extends Thread {

	private static Names instance;
	public static void init() throws SQLException {
		System.out.println("Initializing names: reset database table");
		Statement s = GADM.db.createStatement();
		s.execute("DROP TABLE IF EXISTS `geography`.`names`");
		s.execute("CREATE TABLE `geography`.`names` (`id` INT(32) NOT NULL,`name` varchar(200) NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
		s.execute("CREATE INDEX names ON `geography`.`names` (`name`)");
		s.close();
		instance = new Names();
		instance.start();
	}
	
	public static void end() throws SQLException {
		instance.stop = true;
		synchronized (todo) {
			todo.notify();
		}
		synchronized (instance) {
			if (!instance.stopped)
				try { instance.wait(); } catch (InterruptedException e) { return; }
		}
		SQLProcessor.wait_all_processed();
		System.out.println("Optimize names table");
		long start = System.currentTimeMillis();
		Statement s = GADM.db.createStatement();
		s.execute("ALTER IGNORE TABLE `geography`.`names` ADD PRIMARY KEY (`id`)");
		s.execute("ALTER TABLE `geography`.`names` MODIFY `id` int(32) NOT NULL AUTO_INCREMENT");
		s.execute("ALTER TABLE `geography`.`names` AUTO_INCREMENT="+name_auto_id);
		s.close();
		s = GADM.db.createStatement();
		s.execute("DROP INDEX names ON `geography`.`names`");
		s.close();
		s = GADM.db.createStatement();
		s.execute("OPTIMIZE TABLE `geography`.`names`");
		s.close();
		System.out.println(" + names optimized in "+(System.currentTimeMillis()-start)/1000+"s.");
	}
	
	private static long name_auto_id = 1;
	
	public static interface Call {
		public void call(long name_id);
	}
	public static abstract class Call2 implements Call {
		public Call2(Object o1, Object o2) { this.o1 = o1; this.o2 = o2; }
		protected Object o1,o2;
	}
	
	private static class ToDo {
		String name;
		Call call;
	}
	private static LinkedList<ToDo> todo = new LinkedList<ToDo>();
	
	public static void get(String name, Call call) {
		ToDo t = new ToDo();
		t.name = name;
		t.call = call;
		synchronized (todo) {
			todo.add(t);
			todo.notify();
		}
	}
	
	private Names() {
		super("Names");
	}
	private boolean stop = false, stopped = false;
	public void run() {
		final Map<String,Long> recent = new HashMap<String,Long>(1000);
		Statement s;
		try { s = GADM.db.createStatement(); }
		catch (SQLException e) { e.printStackTrace(System.err); return; }
		ResultSet r;
		String en;
		ToDo t;
		Long id;
		String[] fields = new String[] { "id", "name" };
		Runnable clear_recent = new Runnable() {
			@Override
			public void run() {
				synchronized (recent) {
					recent.clear();
				}
			}
		};
		do {
			synchronized (todo) {
				if (todo.isEmpty()) {
					if (stop) break;
					try { todo.wait(); } catch (InterruptedException e) { break; }
					continue;
				}
				t = todo.removeFirst();
			}
			synchronized (recent) {
				id = recent.get(t.name);
			}
			if (id != null) {
				t.call.call(id);
				continue;
			}
			en = SQLProcessor.escape(t.name);
			try {
				s.execute("SELECT `id` FROM `geography`.`names` WHERE name="+en);
				r = s.getResultSet();
				if (!r.next()) {
					long i = name_auto_id++;
					recent.put(t.name, i);
					SQLProcessor.insert_delay("`geography`.`names`", fields, new String[] { Long.toString(i), en }, clear_recent);
					t.call.call(i);
				} else {
					id = r.getLong(1);
					t.call.call(id);
				}
				r.close();
			} catch (SQLException e) {
				e.printStackTrace(System.err);
			}
		} while (true);
		synchronized (this) {
			stopped = true;
			this.notify();
		}
	}
	
}
