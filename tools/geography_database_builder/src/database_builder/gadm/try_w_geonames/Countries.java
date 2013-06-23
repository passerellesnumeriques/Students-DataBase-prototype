package database_builder.gadm.try_w_geonames;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.HashMap;
import java.util.Map;

import sql.SQLProcessor;

public class Countries {

	public static Map<String,String> process() throws SQLException {
		System.out.println("Building final list of countries");
		System.out.println(" + Reset database");
		reset_db();
		System.out.println(" + Consolidate geonames info with gadm");
		long start = System.currentTimeMillis();
		Map<String,String> mapping = build_countries();
		System.out.println("   done in "+(System.currentTimeMillis()-start)+"ms.");
		System.out.println(" + Optimize country tables for usage");
		start = System.currentTimeMillis();
		optimize_db();
		System.out.println("   done in "+(System.currentTimeMillis()-start)+"ms.");
		return mapping;
	}
	
	private static void reset_db() throws SQLException {
		Statement s = GADM.db.createStatement();
		s.execute("DROP TABLE IF EXISTS `geography`.`country`");
		s.execute("CREATE TABLE `geography`.`country` (`code` varchar(2) NOT NULL,`sovereign` varchar(2),`latitude` FLOAT NOT NULL,`longitude` FLOAT NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=latin1");
		s.execute("DROP TABLE IF EXISTS `geography`.`country_names`");
		s.execute("CREATE TABLE `geography`.`country_names` (`code` varchar(2) NOT NULL,`lang` varchar(2) NOT NULL,`name_id` int(32) NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=latin1");
		s.close();
	}
	
	private static void optimize_db() throws SQLException {
		SQLProcessor.wait_all_processed();
		Statement s = GADM.db.createStatement();
		s.execute("ALTER TABLE `geography`.`country` ADD PRIMARY KEY (`code`)");
		s.execute("ALTER IGNORE TABLE `geography`.`country_names` ADD UNIQUE KEY `code_lang` (`code`,`lang`)");
		s.execute("OPTIMIZE TABLE `geography`.`country`");
		s.execute("OPTIMIZE TABLE `geography`.`country_names`");
		s.close();
	}
	
	private static Map<String,String> build_countries() throws SQLException {
		System.out.println("   + Prepare the list of countries, match with GADM, and move the names");
		Map<String,String> gadm_mapping = new HashMap<String,String>();
		Map<String,String> sovereign = new HashMap<String,String>();
		Statement s = GADM.db.createStatement();
		s.execute("SELECT * FROM `geography`.`country_temp`");
		ResultSet rs = s.getResultSet();
		String[] country_fields = new String[] { "code","latitude","longitude"};
		final String[] names_fields = new String[] { "code","lang","name_id"};
		if (rs.next()) {
			do {
				String country_code = rs.getString(1);
				float latitude = rs.getFloat(2);
				float longitude = rs.getFloat(3);
				
				// look for the GADM ID of the country
				Statement s2 = GADM.gadm.createStatement();
				s2.execute("SELECT `GADMID`,`SOVEREIGN`,`NAME_ENGLISH`,`NAME_LOCAL`,`NAME_VARIANTS` FROM `att_0` WHERE `ISO2`='"+country_code+"'");
				ResultSet r = s2.getResultSet();
				if (!r.next()) {
					System.err.println("Country code "+country_code+" not found in GADM !");
				} else {
					String country_gadmid = r.getString(1);
					String sov = (String)r.getObject(2);
					gadm_mapping.put(country_code, country_gadmid);
					if (sov != null && sov.length() > 0) {
						if (!GADM.getNames(r.getString(3)).contains(sov) &&
							!GADM.getNames(r.getString(4)).contains(sov) &&
							!GADM.getNames(r.getString(5)).contains(sov)
						)
							sovereign.put(country_code, sov);
					}
				}
				r.close();
				s2.close();

				// insert country
				SQLProcessor.insert_delay("`geography`.`country`", country_fields, new String[] { "'"+country_code+"'", Float.toString(latitude), Float.toString(longitude) });
				// move names
				s2 = GADM.db.createStatement();
				s2.execute("SELECT * FROM `geography`.`country_temp_names` WHERE `code`='"+country_code+"'");
				r = s2.getResultSet();
				if (r.next()) {
					do {
						String lang = r.getString(2);
						String name = r.getString(3);
						Names.get(name, new Names.Call2("'"+country_code+"'","'"+lang+"'") {
							@Override
							public void call(long name_id) {
								SQLProcessor.insert_delay("`geography`.`country_names`", names_fields, new String[] { (String)o1, (String)o2, Long.toString(name_id) });
							}
						});
					} while (r.next());
				}
			} while (rs.next());
		}
		rs.close();
		s.close();
		SQLProcessor.wait_all_processed();
		for (Map.Entry<String, String> e : sovereign.entrySet()) {
			String country_code = e.getKey();
			String sov = e.getValue();
			Statement s2 = GADM.gadm.createStatement();
			s2.execute("SELECT * FROM `att_0` WHERE `NAME_ENGLISH` LIKE '%"+SQLProcessor.escape_ms_string(sov)+"%' OR `NAME_LOCAL` LIKE '%"+SQLProcessor.escape_ms_string(sov)+"%' OR `NAME_VARIANTS` LIKE '%"+SQLProcessor.escape_ms_string(sov)+"%'");
			ResultSet r = s2.getResultSet();
			if (!r.next()) {
				System.err.println("Cannot find sovereign country '"+sov+"' for sub-country code "+country_code);
			} else {
				if (!GADM.getNames(r.getString("NAME_ENGLISH")).contains(sov) &&
					!GADM.getNames(r.getString("NAME_LOCAL")).contains(sov) &&
					!GADM.getNames(r.getString("NAME_VARIANTS")).contains(sov)
				) {
					System.err.println("Cannot find sovereign country '"+sov+"' for sub-country code "+country_code);
				} else {
					String gadmid = r.getString("GADMID");
					boolean found = false;
					for (Map.Entry<String,String> ee : gadm_mapping.entrySet())
						if (ee.getValue().equals(gadmid)) {
							found = true;
							SQLProcessor.add_query("`geography`.`country`", "UPDATE `geography`.`country` SET `sovereign`='"+ee.getKey()+"' WHERE `code`='"+country_code+"'");
							break;
						}
					if (!found)
						System.err.println("Cannot find GADMID "+gadmid+" to match with sovereign country "+sov);
				}
			}
			r.close();
			s2.close();
		}
		return gadm_mapping;
	}
	
}
