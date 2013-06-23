package database_builder.gadm;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import sql.SQLProcessor;

public class Countries {

	public static Map<String,String> process() throws SQLException {
		System.out.println("Building list of countries");
		System.out.println(" + Reset database");
		reset_db();
		System.out.println(" + Import from GADM");
		long start = System.currentTimeMillis();
		Map<String,String> mapping = build_countries();
		System.out.println("   done in "+(System.currentTimeMillis()-start)+"ms.");
		SQLProcessor.wait_all_processed();
		System.out.println(" + Optimize country tables for usage");
		start = System.currentTimeMillis();
		optimize_db();
		System.out.println("   done in "+(System.currentTimeMillis()-start)+"ms.");
		return mapping;
	}
	
	private static void reset_db() throws SQLException {
		Statement s = GADM.db.createStatement();
		s.execute("DROP TABLE IF EXISTS `geography`.`country`");
		s.execute("CREATE TABLE `geography`.`country` (`code` varchar(2) NOT NULL,`sovereign` varchar(2),`name` varchar(200) NOT NULL,`divisions` int(3) NOT NULL DEFAULT 0) ENGINE=InnoDB");
		s.close();
	}
	
	private static void optimize_db() throws SQLException {
		Statement s = GADM.db.createStatement();
		s.execute("ALTER TABLE `geography`.`country` ADD PRIMARY KEY (`code`)");
		s.execute("OPTIMIZE TABLE `geography`.`country`");
		s.close();
	}
	
	private static Map<String,String> build_countries() throws SQLException {
		Map<String,String> gadm_mapping = new HashMap<String,String>();
		String[] fields = new String[] { "code", "sovereign", "name" };
		Statement s = GADM.gadm.createStatement();
		s.execute("SELECT `GADMID`,`SOVEREIGN`,`ISO2`,`NAME_ENGLISH`,`NAME_LOCAL`,`NAME_VARIANTS` FROM `att_0` WHERE `ISO2` IS NOT NULL");
		ResultSet r = s.getResultSet();
		if (r.next())
			do {
				String country_gadmid = r.getString(1);
				String sov = (String)r.getObject(2);
				String country_code = r.getString(3);
				List<String> names = GADM.getNames(r.getString(4));
				gadm_mapping.put(country_code, country_gadmid);
				if (sov != null) {
					if (sov.length() == 0)
						sov = null;
					else if (!names.contains(sov) &&
							!GADM.getNames(r.getString(5)).contains(sov) &&
							!GADM.getNames(r.getString(6)).contains(sov)
						) {
						Statement s2 = GADM.gadm.createStatement();
						s2.execute("SELECT * FROM `att_0` WHERE `NAME_ENGLISH` LIKE '%"+SQLProcessor.escape_ms_string(sov)+"%' OR `NAME_LOCAL` LIKE '%"+SQLProcessor.escape_ms_string(sov)+"%' OR `NAME_VARIANTS` LIKE '%"+SQLProcessor.escape_ms_string(sov)+"%'");
						ResultSet r2 = s2.getResultSet();
						if (!r2.next()) {
							System.err.println("Cannot find sovereign country '"+sov+"' for sub-country code "+country_code);
							sov = null;
						} else {
							if (!GADM.getNames(r2.getString("NAME_ENGLISH")).contains(sov) &&
								!GADM.getNames(r2.getString("NAME_LOCAL")).contains(sov) &&
								!GADM.getNames(r2.getString("NAME_VARIANTS")).contains(sov)
							) {
								System.err.println("Cannot find sovereign country '"+sov+"' for sub-country code "+country_code);
								sov = null;
							} else {
								sov = r2.getString("ISO2");
							}
						}
						r2.close();
						s2.close();
					} else
						sov = null;
				}
				SQLProcessor.insert_delay("`geography`.`country`", fields, new String[] { "'"+country_code+"'", sov != null ? "'"+sov+"'" : "NULL", SQLProcessor.escape(names.get(0)) });
			} while (r.next());
		r.close();
		s.close();
		return gadm_mapping;
	}
	
}
