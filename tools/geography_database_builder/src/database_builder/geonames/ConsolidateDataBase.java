package database_builder.geonames;


import java.sql.Connection;
import java.sql.Driver;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;

import sql.SQLProcessor;

public class ConsolidateDataBase {

	public static void main(String[] args) {
		try {
			System.out.println("Connection to Database...");
			connect_db("localhost", "root", "");
			System.out.println("Optimize Database for consolidation...");
			optimize_db_for_consolidation();
			System.out.println("Reset Database...");
			reset_db();
			System.out.println("Consolidating data...");
			consolidate();
			System.out.println("Remove temporary tables...");
			remove_temp_tables();
			System.out.println("Optimize Database for use...");
			optimize_db_for_use();
			db.close();
		} catch (Throwable t) {
			t.printStackTrace(System.err);
		}
	}

	static Connection db;
	
	private static void connect_db(String server, String username, String password) throws SQLException, ClassNotFoundException {
		// connect to db
		Class<?> clazz = Class.forName("com.mysql.jdbc.Driver");
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
		db = driver.connect("jdbc:mysql://"+server, props);
	}
	
	private static void optimize_db_for_consolidation() throws SQLException {
		Statement s;
		long start;
		
//		start = System.currentTimeMillis();
//		s = db.createStatement();
//		s.execute("CREATE INDEX adm1 ON `geography`.`adm1_temp` (country,adm1)");
//		s.close();
//		System.out.println(" + adm1 optimized in "+(System.currentTimeMillis()-start)/1000+"s.");
//
		start = System.currentTimeMillis();
//		s = db.createStatement();
//		s.execute("CREATE INDEX adm1 ON `geography`.`adm2_temp` (country,adm1)");
//		s.close();
//		s = db.createStatement();
//		s.execute("CREATE INDEX adm2 ON `geography`.`adm2_temp` (country,adm2)");
//		s.close();
		s = db.createStatement();
		s.execute("CREATE INDEX country ON `geography`.`adm2_temp` (country)");
		s.close();
		System.out.println(" + adm2 optimized in "+(System.currentTimeMillis()-start)/1000+"s.");
//
		start = System.currentTimeMillis();
//		s = db.createStatement();
//		s.execute("CREATE INDEX adm1 ON `geography`.`adm3_temp` (country,adm1)");
//		s.close();
//		s = db.createStatement();
//		s.execute("CREATE INDEX adm2 ON `geography`.`adm3_temp` (country,adm2)");
//		s.close();
//		s = db.createStatement();
//		s.execute("CREATE INDEX adm3 ON `geography`.`adm3_temp` (adm3)");
//		s.close();
		s = db.createStatement();
		s.execute("CREATE INDEX country ON `geography`.`adm3_temp` (country)");
		s.close();
		System.out.println(" + adm3 optimized in "+(System.currentTimeMillis()-start)/1000+"s.");
//
		start = System.currentTimeMillis();
//		s = db.createStatement();
//		s.execute("CREATE INDEX adm1 ON `geography`.`adm4_temp` (country,adm1)");
//		s.close();
//		s = db.createStatement();
//		s.execute("CREATE INDEX adm2 ON `geography`.`adm4_temp` (country,adm2)");
//		s.close();
//		s = db.createStatement();
//		s.execute("CREATE INDEX adm3 ON `geography`.`adm4_temp` (country,adm3)");
//		s.close();
//		s = db.createStatement();
//		s.execute("CREATE INDEX adm4 ON `geography`.`adm4_temp` (country,adm4)");
//		s.close();
		s = db.createStatement();
		s.execute("CREATE INDEX country ON `geography`.`adm4_temp` (country)");
		s.close();
		System.out.println(" + adm4 optimized in "+(System.currentTimeMillis()-start)/1000+"s.");
//
		start = System.currentTimeMillis();
//		s = db.createStatement();
//		s.execute("CREATE INDEX adm1 ON `geography`.`ppl_temp` (country,adm1)");
//		s.close();
//		s = db.createStatement();
//		s.execute("CREATE INDEX adm2 ON `geography`.`ppl_temp` (country,adm2)");
//		s.close();
//		s = db.createStatement();
//		s.execute("CREATE INDEX adm3 ON `geography`.`ppl_temp` (country,adm3)");
//		s.close();
//		s = db.createStatement();
//		s.execute("CREATE INDEX adm4 ON `geography`.`ppl_temp` (country,adm4)");
//		s.close();
		s = db.createStatement();
		s.execute("CREATE INDEX country ON `geography`.`ppl_temp` (country)");
		s.close();
		System.out.println(" + ppl optimized in "+(System.currentTimeMillis()-start)/1000+"s.");
		
		start = System.currentTimeMillis();
		s = db.createStatement();
		s.execute("CREATE INDEX `id` ON `geography`.`adm1_temp_names` (`id`)");
		s.close();
		s = db.createStatement();
		s.execute("CREATE INDEX `id` ON `geography`.`adm2_temp_names` (`id`)");
		s.close();
		s = db.createStatement();
		s.execute("CREATE INDEX `id` ON `geography`.`adm3_temp_names` (`id`)");
		s.close();
		s = db.createStatement();
		s.execute("CREATE INDEX `id` ON `geography`.`adm4_temp_names` (`id`)");
		s.close();
		s = db.createStatement();
		s.execute("CREATE INDEX `id` ON `geography`.`ppl_temp_names` (`id`)");
		s.close();
		System.out.println(" + names optimized in "+(System.currentTimeMillis()-start)/1000+"s.");
		
	}
	
	private static void reset_db() throws SQLException {
		Statement s;
		// alternate names
		s = db.createStatement();
		s.execute("DROP TABLE IF EXISTS `geography`.`names`");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`names` (`id` INT(32) NOT NULL,`name` varchar(200) NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
		s.close();
		s = db.createStatement();
		s.execute("CREATE INDEX names ON `geography`.`names` (`name`)");
		s.close();
		// administrative areas
		s = db.createStatement();
		s.execute("DROP TABLE IF EXISTS `geography`.`AdministrativeArea1`");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`AdministrativeArea1` (`id` INT(32) NOT NULL,`country` varchar(2) NOT NULL,`latitude` float NOT NULL,`longitude` float NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=latin1");
		s.close();
		s = db.createStatement();
		s.execute("DROP TABLE IF EXISTS `geography`.`AdministrativeArea1_names`");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`AdministrativeArea1_names` (`id` INT(32) NOT NULL,`lang` varchar(2) NOT NULL,`name_id` int(32) NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=latin1");
		s.close();

		s = db.createStatement();
		s.execute("DROP TABLE IF EXISTS `geography`.`AdministrativeArea2`");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`AdministrativeArea2` (`id` INT(32) NOT NULL,`country` varchar(2) NOT NULL,`adm1` int(32) NOT NULL,`latitude` float NOT NULL,`longitude` float NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=latin1");
		s.close();
		s = db.createStatement();
		s.execute("DROP TABLE IF EXISTS `geography`.`AdministrativeArea2_names`");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`AdministrativeArea2_names` (`id` INT(32) NOT NULL,`lang` varchar(2) NOT NULL,`name_id` int(32) NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=latin1");
		s.close();
		
		s = db.createStatement();
		s.execute("DROP TABLE IF EXISTS `geography`.`AdministrativeArea3`");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`AdministrativeArea3` (`id` INT(32) NOT NULL,`country` varchar(2) NOT NULL,`adm1` int(32) NOT NULL,`adm2` int(32) NOT NULL,`latitude` float NOT NULL,`longitude` float NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=latin1");
		s.close();
		s = db.createStatement();
		s.execute("DROP TABLE IF EXISTS `geography`.`AdministrativeArea3_names`");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`AdministrativeArea3_names` (`id` INT(32) NOT NULL,`lang` varchar(2) NOT NULL,`name_id` int(32) NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=latin1");
		s.close();
		
		s = db.createStatement();
		s.execute("DROP TABLE IF EXISTS `geography`.`AdministrativeArea4`");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`AdministrativeArea4` (`id` INT(32) NOT NULL,`country` varchar(2) NOT NULL,`adm1` int(32) NOT NULL,`adm2` int(32) NOT NULL,`adm3` int(32) NOT NULL,`latitude` float NOT NULL,`longitude` float NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=latin1");
		s.close();
		s = db.createStatement();
		s.execute("DROP TABLE IF EXISTS `geography`.`AdministrativeArea4_names`");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`AdministrativeArea4_names` (`id` INT(32) NOT NULL,`lang` varchar(2) NOT NULL,`name_id` int(32) NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=latin1");
		s.close();
		
		s = db.createStatement();
		s.execute("DROP TABLE IF EXISTS `geography`.`Cities`");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`Cities` (`id` INT(32) NOT NULL,`country` varchar(2) NOT NULL,`adm1` int(32),`adm2` int(32),`adm3` int(32),`adm4` int(32),`latitude` float NOT NULL,`longitude` float NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=latin1");
		s.close();
		s = db.createStatement();
		s.execute("DROP TABLE IF EXISTS `geography`.`Cities_names`");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`Cities_names` (`id` INT(32) NOT NULL,`lang` varchar(2) NOT NULL,`name_id` int(32) NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=latin1");
		s.close();
	}
	private static void remove_temp_tables() throws SQLException {
		Statement s;
		s = db.createStatement();
		s.execute("DROP TABLE `geography`.`adm1_temp`");
		s.close();
		s = db.createStatement();
		s.execute("DROP TABLE `geography`.`adm1_temp_names`");
		s.close();
		s = db.createStatement();
		s.execute("DROP TABLE `geography`.`adm2_temp`");
		s.close();
		s = db.createStatement();
		s.execute("DROP TABLE `geography`.`adm2_temp_names`");
		s.close();
		s = db.createStatement();
		s.execute("DROP TABLE `geography`.`adm3_temp`");
		s.close();
		s = db.createStatement();
		s.execute("DROP TABLE `geography`.`adm3_temp_names`");
		s.close();
		s = db.createStatement();
		s.execute("DROP TABLE `geography`.`adm4_temp`");
		s.close();
		s = db.createStatement();
		s.execute("DROP TABLE `geography`.`adm4_temp_names`");
		s.close();
		s = db.createStatement();
		s.execute("DROP TABLE `geography`.`ppl_temp`");
		s.close();
		s = db.createStatement();
		s.execute("DROP TABLE `geography`.`ppl_temp_names`");
		s.close();
	}
	private static void optimize_db_for_use() throws SQLException {
		Statement s;
		long start;
		
		start = System.currentTimeMillis();
		s = db.createStatement();
		s.execute("ALTER IGNORE TABLE `geography`.`names` ADD PRIMARY KEY (`id`)");
		s.execute("ALTER TABLE `geography`.`names` MODIFY `id` int(32) NOT NULL AUTO_INCREMENT");
		s.execute("ALTER TABLE `geography`.`names` AUTO_INCREMENT="+names_auto_id);
		s.close();
		s = db.createStatement();
		s.execute("DROP INDEX names ON `geography`.`names`");
		s.close();
		s = db.createStatement();
		s.execute("OPTIMIZE TABLE `geography`.`names`");
		s.close();
		System.out.println(" + names optimized in "+(System.currentTimeMillis()-start)/1000+"s.");
		
		start = System.currentTimeMillis();
		s = db.createStatement();
		s.execute("ALTER IGNORE TABLE `geography`.`AdministrativeArea1` ADD PRIMARY KEY (`id`)");
		s.execute("ALTER TABLE `geography`.`AdministrativeArea1` MODIFY `id` int(32) NOT NULL AUTO_INCREMENT");
		s.execute("ALTER TABLE `geography`.`AdministrativeArea1` AUTO_INCREMENT="+adm_auto_id[0]);
		s.execute("CREATE INDEX `country` ON `geography`.`AdministrativeArea1` (`country`)");
		s.close();
		s = db.createStatement();
		s.execute("OPTIMIZE TABLE `geography`.`AdministrativeArea1`");
		s.close();
		s = db.createStatement();
		s.execute("ALTER IGNORE TABLE `geography`.`AdministrativeArea1_names` ADD UNIQUE KEY `id_lang` (`id`,`lang`)");
		s.close();
		s = db.createStatement();
		s.execute("OPTIMIZE TABLE `geography`.`AdministrativeArea1_names`");
		s.close();
		System.out.println(" + adm1 optimized in "+(System.currentTimeMillis()-start)/1000+"s.");
		
		start = System.currentTimeMillis();
		s = db.createStatement();
		s.execute("ALTER IGNORE TABLE `geography`.`AdministrativeArea2` ADD PRIMARY KEY (`id`)");
		s.execute("ALTER TABLE `geography`.`AdministrativeArea2` MODIFY `id` int(32) NOT NULL AUTO_INCREMENT");
		s.execute("ALTER TABLE `geography`.`AdministrativeArea2` AUTO_INCREMENT="+adm_auto_id[1]);
		s.execute("CREATE INDEX `adm1` ON `geography`.`AdministrativeArea2` (`adm1`)");
		s.close();
		s = db.createStatement();
		s.execute("OPTIMIZE TABLE `geography`.`AdministrativeArea2`");
		s.close();
		s = db.createStatement();
		s.execute("ALTER IGNORE TABLE `geography`.`AdministrativeArea2_names` ADD UNIQUE KEY `id_lang` (`id`,`lang`)");
		s.close();
		s = db.createStatement();
		s.execute("OPTIMIZE TABLE `geography`.`AdministrativeArea2_names`");
		s.close();
		System.out.println(" + adm2 optimized in "+(System.currentTimeMillis()-start)/1000+"s.");
		
		start = System.currentTimeMillis();
		s = db.createStatement();
		s.execute("ALTER IGNORE TABLE `geography`.`AdministrativeArea3` ADD PRIMARY KEY (`id`)");
		s.execute("ALTER TABLE `geography`.`AdministrativeArea3` MODIFY `id` int(32) NOT NULL AUTO_INCREMENT");
		s.execute("ALTER TABLE `geography`.`AdministrativeArea3` AUTO_INCREMENT="+adm_auto_id[2]);
		s.execute("CREATE INDEX `adm2` ON `geography`.`AdministrativeArea3` (`adm2`)");
		s.close();
		s = db.createStatement();
		s.execute("OPTIMIZE TABLE `geography`.`AdministrativeArea3`");
		s.close();
		s = db.createStatement();
		s.execute("ALTER IGNORE TABLE `geography`.`AdministrativeArea3_names` ADD UNIQUE KEY `id_lang` (`id`,`lang`)");
		s.close();
		s = db.createStatement();
		s.execute("OPTIMIZE TABLE `geography`.`AdministrativeArea3_names`");
		s.close();
		System.out.println(" + adm3 optimized in "+(System.currentTimeMillis()-start)/1000+"s.");
		
		start = System.currentTimeMillis();
		s = db.createStatement();
		s.execute("ALTER IGNORE TABLE `geography`.`AdministrativeArea4` ADD PRIMARY KEY (`id`)");
		s.execute("ALTER TABLE `geography`.`AdministrativeArea4` MODIFY `id` int(32) NOT NULL AUTO_INCREMENT");
		s.execute("ALTER TABLE `geography`.`AdministrativeArea4` AUTO_INCREMENT="+adm_auto_id[3]);
		s.execute("CREATE INDEX `adm3` ON `geography`.`AdministrativeArea4` (`adm3`)");
		s.close();
		s = db.createStatement();
		s.execute("OPTIMIZE TABLE `geography`.`AdministrativeArea4`");
		s.close();
		s = db.createStatement();
		s.execute("ALTER IGNORE TABLE `geography`.`AdministrativeArea4_names` ADD UNIQUE KEY `id_lang` (`id`,`lang`)");
		s.close();
		s = db.createStatement();
		s.execute("OPTIMIZE TABLE `geography`.`AdministrativeArea4_names`");
		s.close();
		System.out.println(" + adm4 optimized in "+(System.currentTimeMillis()-start)/1000+"s.");
		
		start = System.currentTimeMillis();
		s = db.createStatement();
		s.execute("ALTER IGNORE TABLE `geography`.`Cities` ADD PRIMARY KEY (`id`)");
		s.execute("ALTER TABLE `geography`.`Cities` MODIFY `id` int(32) NOT NULL AUTO_INCREMENT");
		s.execute("ALTER TABLE `geography`.`Cities` AUTO_INCREMENT="+adm_auto_id[4]);
		s.execute("CREATE INDEX `country` ON `geography`.`Cities` (`country`)");
		s.execute("CREATE INDEX `adm1` ON `geography`.`Cities` (`adm1`)");
		s.execute("CREATE INDEX `adm2` ON `geography`.`Cities` (`adm2`)");
		s.execute("CREATE INDEX `adm3` ON `geography`.`Cities` (`adm3`)");
		s.execute("CREATE INDEX `adm4` ON `geography`.`Cities` (`adm4`)");
		s.close();
		s = db.createStatement();
		s.execute("OPTIMIZE TABLE `geography`.`Cities`");
		s.close();
		s = db.createStatement();
		s.execute("ALTER IGNORE TABLE `geography`.`Cities_names` ADD UNIQUE KEY `id_lang` (`id`,`lang`)");
		s.close();
		s = db.createStatement();
		s.execute("OPTIMIZE TABLE `geography`.`Cities_names`");
		s.close();
		System.out.println(" + cities optimized in "+(System.currentTimeMillis()-start)/1000+"s.");
	}
	
	private static long names_auto_id = 1;
	private static long[] adm_auto_id = { 1, 1, 1, 1, 1 };
	
	private static void consolidate() {
		Object[] res;
		System.out.println(" + Consolidating Administrative Areas 1");
		SQLProcessor.start_threads();
		res = consolidate_adm(1);
		System.out.println("   + Wait for SQL requests to finish");
		SQLProcessor.end_threads();
		System.out.println("   => "+res[0]+" entries consolidated in "+((System.currentTimeMillis()-(long)res[1])/1000)+"s.");
		System.out.println(" + Consolidating Administrative Areas 2");
		SQLProcessor.start_threads();
		res = consolidate_adm(2);
		System.out.println("   + Wait for SQL requests to finish");
		SQLProcessor.end_threads();
		System.out.println("   => "+res[0]+" entries consolidated in "+((System.currentTimeMillis()-(long)res[1])/1000)+"s.");
		System.out.println(" + Consolidating Administrative Areas 3");
		SQLProcessor.start_threads();
		res = consolidate_adm(3);
		System.out.println("   + Wait for SQL requests to finish");
		SQLProcessor.end_threads();
		System.out.println("   => "+res[0]+" entries consolidated in "+((System.currentTimeMillis()-(long)res[1])/1000)+"s.");
		System.out.println(" + Consolidating Administrative Areas 4");
		SQLProcessor.start_threads();
		res = consolidate_adm(4);
		System.out.println("   + Wait for SQL requests to finish");
		SQLProcessor.end_threads();
		System.out.println("   => "+res[0]+" entries consolidated in "+((System.currentTimeMillis()-(long)res[1])/1000)+"s.");
		System.out.println(" + Consolidating Cities");
		SQLProcessor.start_threads();
		res = consolidate_adm(5); // cities
		System.out.println("   + Wait for SQL requests to finish");
		SQLProcessor.end_threads();
		System.out.println("   => "+res[0]+" entries consolidated in "+((System.currentTimeMillis()-(long)res[1])/1000)+"s.");
	}
	private static final int NB_INSERT_ONCE = 1000;
	private static final String[][] update_tables = {
		{
			"`geography`.`adm2_temp`",
			"`geography`.`adm3_temp`",
			"`geography`.`adm4_temp`",
			"`geography`.`ppl_temp`",
		},
		{
			"`geography`.`adm3_temp`",
			"`geography`.`adm4_temp`",
			"`geography`.`ppl_temp`",
		},
		{
			"`geography`.`adm4_temp`",
			"`geography`.`ppl_temp`",
		},
		{
			"`geography`.`ppl_temp`",
		},
		{
		},
	};
	private static Object[] consolidate_adm(int num) {
		long start = System.currentTimeMillis();
		long last_minute = start;
		int nb = 0;
		@SuppressWarnings("unchecked")
		Map<String,Long>[] created_adm = new Map[num-1];
		for (int i = 0; i < created_adm.length; ++i) created_adm[i] = new HashMap<String,Long>(100);
		String table_insert = "`geography`.`"+(num<5 ? "AdministrativeArea"+num : "Cities")+"`";
		try {
			Statement s = db.createStatement();
			s.execute("SELECT * FROM `geography`.`"+(num == 5 ? "ppl_temp" : "adm"+num+"_temp")+"`");
			ResultSet rs = s.getResultSet();
			if (rs.first()) {
				long ids[] = new long[NB_INSERT_ONCE];
				long new_ids[] = new long[NB_INSERT_ONCE];
				String[] countries = new String[NB_INSERT_ONCE];
				float latitude;
				float longitude;
				String[] adm = new String[NB_INSERT_ONCE];
				String[][] prev_adm = new String[NB_INSERT_ONCE][num-1];
				long[] prev_adm_id = new long[3];
				StringBuilder insert = new StringBuilder();
				insert.append("INSERT INTO ").append(table_insert);
				insert.append(" (`id`,`country`,");
				for (int i = 1; i < num; ++i)
					insert.append("`adm").append(i).append("`,");
				insert.append("`latitude`,`longitude`) VALUES ");
				int nb_insert = 0;
				long now;
				do {
					now = System.currentTimeMillis();
					if (now-last_minute >= 60000) {
						System.out.println("   After "+((now-start)/60000)+"m: "+nb+" entries processed, "+SQLProcessor.getPendingBatches()+" batches of SQL requests waiting");
						last_minute += 60000;
					}
					nb++;
					countries[nb_insert] = rs.getString(1);
					ids[nb_insert] = rs.getLong(2);
					int i = 3;
					for (int j = 1; j < num; j++)
						prev_adm[nb_insert][j-1] = rs.getString(i++);
					if (num < 5)
						adm[nb_insert] = rs.getString(i++);
					latitude = rs.getFloat(i++);
					longitude = rs.getFloat(i++);
					if (nb_insert++ > 0) insert.append(',');
					insert.append('(');
					insert.append(new_ids[nb_insert-1] = adm_auto_id[num-1]++).append(',');
					insert.append('\'').append(countries[nb_insert-1]).append("',");
					for (i = 1; i < num; ++i) {
						if (num == 5 && (prev_adm[nb_insert-1][i-1] == null || prev_adm[nb_insert-1][i-1].length() == 0)) {
							// no more adm, set all to null
							for (int j = i; j < num; ++j) {
								insert.append("NULL,");
								prev_adm[nb_insert-1][j-1] = null;
							}
							break;
						}
						if ((num < 5 && (prev_adm[nb_insert-1][i-1] == null || prev_adm[nb_insert-1][i-1].length() == 0 || prev_adm[nb_insert-1][i-1].charAt(0) != '_')) ||
							(num == 5 && prev_adm[nb_insert-1][i-1].charAt(0) != '_')) {
							// adm1 does not exist => create it
							boolean already = false;
							if (prev_adm[nb_insert-1][i-1] != null && prev_adm[nb_insert-1][i-1].length() > 0) {
								Long l = created_adm[i-1].get(prev_adm[nb_insert-1][i-1]);
								if (l != null) {
									already = true;
									prev_adm_id[i-1] = l;
								}
							}
							if (!already) {
								StringBuilder q = new StringBuilder();
								q.append("INSERT INTO `geography`.`AdministrativeArea").append(i).append("` (`id`,`country`,");
								for (int j = 1; j < i; ++j)
									q.append("`adm").append(j).append("`,");
								prev_adm_id[i-1] = adm_auto_id[i-1]++;
								q.append("`latitude`,`longitude`) VALUES (").append(prev_adm_id[i-1]).append(",'").append(countries[nb_insert-1]).append("',");
								for (int j = 1; j < i; ++j)
									q.append(prev_adm_id[j-1]).append(',');
								q.append("0,0)");
								SQLProcessor.add_query("`geography`.`AdministrativeArea"+i+"`", q.toString());
								// update references to the same missing
								if (prev_adm[nb_insert-1][i-1] != null && prev_adm[nb_insert-1][i-1].length() > 0) {
									for (int j = 0; j < update_tables[i-1].length; ++j)
										SQLProcessor.add_query(update_tables[i-1][j], "UPDATE "+update_tables[i-1][j]+" SET `adm"+i+"`='_"+prev_adm_id[i-1]+"' WHERE `adm"+i+"`="+SQLProcessor.escape(prev_adm[nb_insert-1][i-1])+" AND `country`='"+countries[nb_insert-1]+"'");
									created_adm[i-1].put(prev_adm[nb_insert-1][i-1], prev_adm_id[i-1]);
								}
							}
						} else
							prev_adm_id[i-1] = Long.parseLong(prev_adm[nb_insert-1][i-1].substring(1));
						insert.append(prev_adm_id[i-1]).append(',');
					}
					insert.append(latitude).append(',').append(longitude);
					insert.append(')');
					if (nb_insert == NB_INSERT_ONCE) {
						do_insert(table_insert, insert.toString(), ids, new_ids, nb_insert, num < 5 ? "`geography`.`adm"+num+"_temp_names`" : "`geography`.`ppl_temp_names`", num < 5 ? "`geography`.`AdministrativeArea"+num+"_names`" : "`geography`.`Cities_names`", num < 5 ? "`adm"+num+"`" : null, adm, update_tables[num], countries, prev_adm);
						nb_insert = 0;
						insert = new StringBuilder();
						insert.append("INSERT INTO ").append(table_insert);
						insert.append(" (`id`,`country`,");
						for (i = 1; i < num; ++i)
							insert.append("`adm").append(i).append("`,");
						insert.append("`latitude`,`longitude`) VALUES ");
					}
				} while (rs.next());
				if (nb_insert > 0)
					do_insert(table_insert, insert.toString(), ids, new_ids, nb_insert, num < 5 ? "`geography`.`adm"+num+"_temp_names`" : "`geography`.`ppl_temp_names`", num < 5 ? "`geography`.`AdministrativeArea"+num+"_names`" : "`geography`.`Cities_names`", num < 5 ? "`adm"+num+"`" : null, adm, update_tables[num], countries, prev_adm);
			}
			rs.close();
			s.close();
			SQLProcessor.clear_queries();
		} catch (SQLException e) {
			e.printStackTrace(System.err);
		}
		System.out.println("   "+nb+" entries processed, "+SQLProcessor.getPendingBatches()+" batches of SQL requests waiting");
		return new Object[] { nb, start };
	}
	
	private static class ToUpdate {
		String[] keys;
		StringBuilder req;
		StringBuilder where;
	}
	private static void do_insert(String table, String sql, long[] original_ids, long[] new_ids, int nb, String old_names_table, String new_names_table, String geocode_column, String[] geocodes, String[] geocodes_tables, String[] countries, String[][] prev_adm) throws SQLException {
		// insert in table
		SQLProcessor.add_query(table, sql);
		// move alternate names
		StringBuilder insert_names = new StringBuilder();
		insert_names.append("INSERT INTO ").append(new_names_table).append(" (`id`,`lang`,`name_id`) VALUES ");
		boolean first_name = true;
		Statement s = db.createStatement();
		StringBuilder q = new StringBuilder();
		q.append("SELECT ").
			append(old_names_table).append(".`id` AS `old_id`,").
			append(old_names_table).append(".`lang` AS `old_lang`,").
			append(old_names_table).append(".`name` AS `old_name`,").
			append("`new_names`.`id` AS `new_id`").
			append(" FROM ").append(old_names_table).
			append(" LEFT JOIN `geography`.`names` AS `new_names` ON `new_names`.`name`=").append(old_names_table).append(".`name`").
			append(" WHERE ").append(old_names_table).append(".`id` IN (");
		for (int i = 0; i < nb; ++i) {
			if (i > 0) q.append(',');
			q.append(original_ids[i]);
		}
		q.append(')');
		s.execute(q.toString());
		Map<String,Long> new_names = new HashMap<String,Long>(50);
		Statement s_new_names = db.createStatement();
		s_new_names.addBatch("START TRANSACTION");
		ResultSet rs = s.getResultSet();
		if (rs.first()) {
			long id;
			String lang;
			String name;
			long name_id;
			Number n;
			do {
				id = rs.getLong("old_id");
				lang = rs.getString("old_lang");
				name = rs.getString("old_name");
				n = (Number)rs.getObject("new_id");
				if (n == null) {
					Long nn = new_names.get(name);
					if (nn == null) {
						name_id = add_new_name(name, s_new_names);
						new_names.put(name, name_id);
					} else
						name_id = nn;
				} else
					name_id = n.longValue();
				long new_id = 0;
				for (int i = 0; i < nb; ++i)
					if (id == original_ids[i]) { new_id = new_ids[i]; break; }
				if (first_name) first_name = false; else insert_names.append(',');
				insert_names.append('(').append(new_id).append(",'").append(lang).append("',").append(name_id).append(')');
			} while (rs.next());
		}
		rs.close();
		s.close();
		// insert the new names
		if (!new_names.isEmpty()) {
			s_new_names.addBatch("COMMIT");
			s_new_names.executeBatch();
		}
		s_new_names.close();
		SQLProcessor.add_query(new_names_table, insert_names.toString());
		// remove old names from database
		q = new StringBuilder();
		q.append("DELETE FROM ").append(old_names_table).append(" WHERE `id` IN (");
		for (int i = 0; i < nb; ++i) {
			if (i > 0) q.append(',');
			q.append(original_ids[i]);
		}
		q.append(')');
		SQLProcessor.add_query(old_names_table, q.toString());
		// update geocodes with ids in other tables
		if (geocode_column != null) {
			for (int j = 0; j < geocodes_tables.length; ++j) {
				Map<String,ToUpdate> by_key = new HashMap<String,ToUpdate>();
				for (int i = 0; i < nb; ++i) {
					String key = countries[i];
					for (int k = 0; k < prev_adm[i].length; ++k)
						key += "-"+prev_adm[i][k];
					ToUpdate t = by_key.get(key);
					if (t == null) {
						t = new ToUpdate();
						t.keys = new String[1+prev_adm[i].length];
						t.keys[0] = countries[i];
						for (int k = 0; k < prev_adm[i].length; ++k)
							t.keys[k+1] = prev_adm[i][k];
						t.req = new StringBuilder();
						t.req.append("UPDATE ").append(geocodes_tables[j]).append(" SET ").append(geocode_column).append("= CASE ").append(geocode_column);
						t.where = new StringBuilder();
						by_key.put(key, t);
					}
					String g = SQLProcessor.escape(geocodes[i]);
					t.req.append(" WHEN ").append(g);
					t.req.append(" THEN ").append("'_").append(new_ids[i]).append('\'');
					if (t.where.length() > 0) t.where.append(',');
					t.where.append(g);
				}
				for (Map.Entry<String, ToUpdate> e : by_key.entrySet()) {
					ToUpdate t = e.getValue();
					t.req.append(" END");
					t.req.append(" WHERE ").append(geocode_column).append(" IN (");
					t.req.append(t.where);
					t.req.append(") AND `country`='").append(t.keys[0]).append("'");
					for (int i = 1; i < t.keys.length; ++ i) {
						t.req.append(" AND `adm").append(i).append("`=").append(SQLProcessor.escape(t.keys[i]));
					}
					SQLProcessor.add_query(geocodes_tables[j], t.req.toString());
				}
			}
		}
//		if (geocode_column != null) {
//			for (int i = 0; i < nb; ++i) {
//				for (int j = 0; j < geocodes_tables.length; ++j) {
//					add_query(geocodes_tables[j], "UPDATE "+geocodes_tables[j]+" SET "+geocode_column+"='_"+new_ids[i]+"' WHERE "+geocode_column+"="+SQLProcessor.escape(geocodes[i])+" AND `country`='"+countries[i]+"'");
//				}
//			}
//		}
	}
	

	private static long add_new_name(String name, Statement s) throws SQLException {
		long id = names_auto_id++;
		s.addBatch("INSERT INTO `geography`.`names` (`id`,`name`) VALUE ("+id+","+SQLProcessor.escape(name)+")");
		return id;
	}
	
}
