package database_builder.gadm;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import sql.SQLProcessor;

public class Adm {

	public static void init() throws SQLException {
		System.out.println("Reset tables for administrative divisions");
		reset_db();
		System.out.println("Launch threads");
		threads = new AdmThread[Runtime.getRuntime().availableProcessors()];
		for (int i = 0; i < threads.length; ++i) {
			threads[i] = new AdmThread();
			threads[i].start();
		}
	}
	public static void end() {
		for (int i = 0; i < threads.length; ++i) {
			threads[i].stop = true;
			synchronized (to_process) { to_process.notifyAll(); }
			synchronized (threads[i]) {
				if (!threads[i].stopped)
					try { threads[i].wait(); } catch (InterruptedException e) { break; }
			}
		}
	}
	
	public static void process_country(String code, String gadmid) {
		synchronized (to_process) {
			to_process.add(new String[] { code, gadmid });
			to_process.notify();
		}
	}
	
	private static void reset_db() throws SQLException {
		Statement s = GADM.db.createStatement();
		s.execute("DROP TABLE IF EXISTS `geography`.`adm1`");
		s.execute("DROP TABLE IF EXISTS `geography`.`adm2`");
		s.execute("DROP TABLE IF EXISTS `geography`.`adm3`");
		s.execute("DROP TABLE IF EXISTS `geography`.`adm4`");
		s.execute("DROP TABLE IF EXISTS `geography`.`adm5`");
		s.execute("CREATE TABLE `geography`.`adm1` (`id` int(32) NOT NULL,`country` varchar(2) NOT NULL,`name` varchar(200) NOT NULL,`type` varchar(200) NOT NULL,`type_en` varchar(200)) ENGINE=InnoDB");
		s.execute("CREATE TABLE `geography`.`adm2` (`id` int(32) NOT NULL,`parent_id` int(32) NOT NULL,`country` varchar(2) NOT NULL,`name` varchar(200) NOT NULL,`type` varchar(200) NOT NULL,`type_en` varchar(200)) ENGINE=InnoDB");
		s.execute("CREATE TABLE `geography`.`adm3` (`id` int(32) NOT NULL,`parent_id` int(32) NOT NULL,`country` varchar(2) NOT NULL,`name` varchar(200) NOT NULL,`type` varchar(200) NOT NULL,`type_en` varchar(200)) ENGINE=InnoDB");
		s.execute("CREATE TABLE `geography`.`adm4` (`id` int(32) NOT NULL,`parent_id` int(32) NOT NULL,`country` varchar(2) NOT NULL,`name` varchar(200) NOT NULL,`type` varchar(200) NOT NULL,`type_en` varchar(200)) ENGINE=InnoDB");
		s.execute("CREATE TABLE `geography`.`adm5` (`id` int(32) NOT NULL,`parent_id` int(32) NOT NULL,`country` varchar(2) NOT NULL,`name` varchar(200) NOT NULL,`type` varchar(200) NOT NULL,`type_en` varchar(200)) ENGINE=InnoDB");
		s.close();
	}
	
	public static void optimize_db() throws SQLException {
		System.out.println("Optimizing tables");
		long start = System.currentTimeMillis();
		Statement s = GADM.db.createStatement();
		s.execute("ALTER TABLE `geography`.`adm1` ADD PRIMARY KEY (`id`)");
		s.execute("ALTER TABLE `geography`.`adm1` MODIFY `id` int(32) NOT NULL AUTO_INCREMENT");
		s.execute("ALTER TABLE `geography`.`adm1` AUTO_INCREMENT="+adm_id[0]);
		s.execute("CREATE INDEX country ON `geography`.`adm1` (country)");
		s.execute("OPTIMIZE TABLE `geography`.`adm1`");
		s.execute("ALTER TABLE `geography`.`adm2` ADD PRIMARY KEY (`id`)");
		s.execute("ALTER TABLE `geography`.`adm2` MODIFY `id` int(32) NOT NULL AUTO_INCREMENT");
		s.execute("ALTER TABLE `geography`.`adm2` AUTO_INCREMENT="+adm_id[1]);
		s.execute("CREATE INDEX parent ON `geography`.`adm2` (parent_id)");
		s.execute("OPTIMIZE TABLE `geography`.`adm2`");
		s.execute("ALTER TABLE `geography`.`adm3` ADD PRIMARY KEY (`id`)");
		s.execute("ALTER TABLE `geography`.`adm3` MODIFY `id` int(32) NOT NULL AUTO_INCREMENT");
		s.execute("ALTER TABLE `geography`.`adm3` AUTO_INCREMENT="+adm_id[2]);
		s.execute("CREATE INDEX parent ON `geography`.`adm3` (parent_id)");
		s.execute("OPTIMIZE TABLE `geography`.`adm3`");
		s.execute("ALTER TABLE `geography`.`adm4` ADD PRIMARY KEY (`id`)");
		s.execute("ALTER TABLE `geography`.`adm4` MODIFY `id` int(32) NOT NULL AUTO_INCREMENT");
		s.execute("ALTER TABLE `geography`.`adm4` AUTO_INCREMENT="+adm_id[3]);
		s.execute("CREATE INDEX parent ON `geography`.`adm4` (parent_id)");
		s.execute("OPTIMIZE TABLE `geography`.`adm4`");
		s.execute("ALTER TABLE `geography`.`adm5` ADD PRIMARY KEY (`id`)");
		s.execute("ALTER TABLE `geography`.`adm5` MODIFY `id` int(32) NOT NULL AUTO_INCREMENT");
		s.execute("ALTER TABLE `geography`.`adm5` AUTO_INCREMENT="+adm_id[4]);
		s.execute("CREATE INDEX parent ON `geography`.`adm5` (parent_id)");
		s.execute("OPTIMIZE TABLE `geography`.`adm5`");
		s.close();
		System.out.println("=> optimization done in "+((System.currentTimeMillis()-start)/1000)+"s.");
	}
	
	private static AdmThread[] threads;
	private static LinkedList<String[]> to_process = new LinkedList<String[]>();
	
	private static class AdmThread extends Thread {
		AdmThread() {
			super("Administrative division");
		}
		boolean stop = false, stopped = false;
		@Override
		public void run() {
			System.out.println("Connection to the GADM database...");
			Connection conn;
			try { conn = GADM.connect_gadm(); }
			catch (SQLException e) { e.printStackTrace(System.err); return; }
			System.out.println("Connected, start processing");
			String[] c;
			do {
				synchronized (to_process) {
					if (to_process.isEmpty()) {
						if (stop) break;
						try { to_process.wait(); } catch (InterruptedException e) { break; }
						continue;
					}
					c = to_process.removeFirst();
				}
				try {
					process(conn, c[0], c[1]);
				} catch (SQLException e) {
					System.err.println("Error processing country "+c[0]);
					e.printStackTrace(System.err);
				}
			} while (true);
			System.out.println("Close GADM connection");
			try { conn.close(); }
			catch (SQLException e) { e.printStackTrace(System.err); }
			synchronized (this) {
				stopped = true;
				this.notify();
			}
		}
	}
	
	private static long[] adm_id = new long[] { 1, 1, 1, 1, 1 };
	
	private static void process(Connection conn, String country_code, String gadmid) throws SQLException {
		System.out.println("Start processing country "+country_code);
		long start = System.currentTimeMillis();
		String country = "'"+country_code+"'";
		String[] fields1 = new String[] { "id", "country", "name", "type", "type_en" };
		String[] fields2 = new String[] { "id", "parent_id", "country", "name", "type", "type_en" };

		Statement s = conn.createStatement(ResultSet.TYPE_FORWARD_ONLY, ResultSet.CONCUR_READ_ONLY);
		StringBuilder q = new StringBuilder();
		q.append("SELECT ");
		for (int level = 1; level <= 5; level++) {
			if (level > 1) q.append(',');
			q.append("`ID_").append(level).append("`,`NAME_").append(level).append("`,`ENGTYPE_").append(level).append("`,`TYPE_").append(level);
			if (level < 5) q.append("`,`VALIDTO_").append(level);
			q.append('`');
		}
		q.append(" FROM gadm2");
		// only this country
		q.append(" WHERE `ID_0`=").append(gadmid);
		// only valid entries
		q.append(" AND (");
		for (int level = 1; level <= 4; ++level) {
			if (level > 1) q.append(" OR ");
			q.append("`VALIDTO_").append(level).append("`='Present'"); 
		}
		q.append("OR `ID_5` IS NOT NULL)");
		s.execute(q.toString());
		ResultSet r = s.getResultSet();
		if (!r.next()) {
			// nothing
			r.close();
			s.close();
			return;
		}
		Map<Integer,Long> ids1 = new HashMap<Integer,Long>();
		Map<Integer,Map<Integer,Long>> ids2 = new HashMap<Integer,Map<Integer,Long>>();
		Map<Integer,Map<Integer,Map<Integer,Long>>> ids3 = new HashMap<Integer,Map<Integer,Map<Integer,Long>>>();
		Map<Integer,Map<Integer,Map<Integer,Map<Integer,Long>>>> ids4 = new HashMap<Integer,Map<Integer,Map<Integer,Map<Integer,Long>>>>();
		String[] values;
		String type, type_en;
		Long[] id = new Long[5];
		Integer[] gid = new Integer[5];
		boolean[] valid = new boolean[5];
		int max_level = 0;
		do {
			gid[1] = r.getInt("ID_1");
			if (gid[1] == null || gid[1] == 0) continue;
			if (max_level == 0) max_level = 1;
			id[1] = ids1.get(gid[1]);
			if (id[1] == null) {
				// this is a new level
				List<String> names = GADM.getNames(r.getString("NAME_1"));
				if (names.isEmpty()) {
					System.err.println("NO NAME for country "+gadmid+" level 1 id "+gid[1]);
					names.add("");
					//continue;
				}
				type = r.getString("TYPE_1");
				type_en = r.getString("ENGTYPE_1");
				if (type != null && type.length() == 0) type = null;
				if (type_en != null && type_en.length() == 0) type_en = null;
				if (type == null && type_en != null) { type = type_en; type_en = null; }
				if (type != null && type.equals(type_en)) type_en = null;
				if (type == null) type = "";
				synchronized (Adm.class) {
					id[1] = adm_id[0]++;
				}
				values = new String[] { Long.toString(id[1]), country, SQLProcessor.escape(names.get(0)), SQLProcessor.escape(type), SQLProcessor.escape(type_en) };
				SQLProcessor.insert_delay("`geography`.`adm1`", fields1, values);
				ids1.put(gid[1], id[1]);
				ids2.put(gid[1], new HashMap<Integer,Long>());
				ids3.put(gid[1], new HashMap<Integer,Map<Integer,Long>>());
				ids4.put(gid[1], new HashMap<Integer,Map<Integer,Map<Integer,Long>>>());
			}
			Integer gid5 = r.getInt("ID_5");
			if (gid5 != null && gid5 == 0) gid5 = null;
			for (int i = 2; i <= 4; ++i)
				valid[i] = "Present".equals(r.getString("VALIDTO_"+i));
			for (int level = 2; level < 5; ++level) {
				gid[level] = r.getInt("ID_"+level);
				if (gid[level] != null && gid[level] == 0) gid[level] = null;
				if (gid[level] == null) break;
				switch (level) {
				case 2: id[level] = ids2.get(gid[1]).get(gid[2]); break;
				case 3: id[level] = ids3.get(gid[1]).get(gid[2]).get(gid[3]); break;
				case 4: id[level] = ids4.get(gid[1]).get(gid[2]).get(gid[3]).get(gid[4]); break;
				}
				
				if (id[level] == null) {
					if (gid5 == null) {
						boolean all = true;
						for (int i = level; i <= 4; ++i)
							if (valid[i]) { all = false; break; }
						if (all) break; // no valid entry anymore
					}
					if (max_level < level) max_level = level;
					List<String> names = GADM.getNames(r.getString("NAME_"+level));
					if (names.isEmpty()) {
						System.err.println("NO NAME for country "+gadmid+" level "+level+" id "+gid[level]);
						continue;
					}
					type = r.getString("TYPE_"+level);
					type_en = r.getString("ENGTYPE_"+level);
					if (type != null && type.length() == 0) type = null;
					if (type_en != null && type_en.length() == 0) type_en = null;
					if (type == null && type_en != null) { type = type_en; type_en = null; }
					if (type != null && type.equals(type_en)) type_en = null;
					if (type == null) type = "";
					synchronized (Adm.class) {
						id[level] = adm_id[level-1]++;
					}
					values = new String[] { Long.toString(id[level]), Long.toString(id[level-1]), country, SQLProcessor.escape(names.get(0)), SQLProcessor.escape(type), SQLProcessor.escape(type_en) };
					SQLProcessor.insert_delay("`geography`.`adm"+level+"`", fields2, values);
					switch (level) {
					case 2:
						ids2.get(gid[1]).put(gid[2], id[2]);
						ids3.get(gid[1]).put(gid[2], new HashMap<Integer,Long>());
						ids4.get(gid[1]).put(gid[2], new HashMap<Integer,Map<Integer,Long>>());
						break;
					case 3:
						ids3.get(gid[1]).get(gid[2]).put(gid[3], id[3]);
						ids4.get(gid[1]).get(gid[2]).put(gid[3], new HashMap<Integer,Long>());
						break;
					case 4:
						ids4.get(gid[1]).get(gid[2]).get(gid[3]).put(gid[4], id[4]);
						break;
					}
				}
			}
			if (gid5 != null) {
				max_level = 5;
				List<String> names = GADM.getNames(r.getString("NAME_5"));
				if (names.isEmpty()) {
					System.err.println("NO NAME for country "+gadmid+" level 5 id "+gid5);
					continue;
				}
				type = r.getString("TYPE_5");
				type_en = r.getString("ENGTYPE_5");
				if (type != null && type.length() == 0) type = null;
				if (type_en != null && type_en.length() == 0) type_en = null;
				if (type == null && type_en != null) { type = type_en; type_en = null; }
				if (type != null && type.equals(type_en)) type_en = null;
				if (type == null) type = "";
				long id5;
				synchronized (Adm.class) {
					id5 = adm_id[4]++;
				}
				values = new String[] { Long.toString(id5), Long.toString(id[4]), country, SQLProcessor.escape(names.get(0)), SQLProcessor.escape(type), SQLProcessor.escape(type_en) };
				SQLProcessor.insert_delay("`geography`.`adm5`", fields2, values);
			}
		} while (r.next());		
		r.close();
		s.close();

		// update number of divisions
		SQLProcessor.add_query("`geography`.`country`", "UPDATE `geography`.`country` SET `divisions`="+max_level+" WHERE `code`="+country);

		System.out.println("Country "+country_code+" processed in "+((System.currentTimeMillis()-start)/1000)+"s.");
	}
	
}
