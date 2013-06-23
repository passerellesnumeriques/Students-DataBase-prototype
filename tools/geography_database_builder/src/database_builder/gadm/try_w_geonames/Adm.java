package database_builder.gadm.try_w_geonames;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

public class Adm {

	public static void process(Map<String,String> countries_gadmid) throws SQLException {
		optimize_names();
		process_countries(countries_gadmid);
	}
	
	private static void optimize_names() throws SQLException {
		System.out.println("Optimizing geonames names tables");
		Statement s;
		long start;
		s = GADM.db.createStatement();
		
		start = System.currentTimeMillis();
		try { s.execute("CREATE INDEX country ON `geography`.`adm1_temp_names` (country)"); }
		catch (SQLException e) { if (e.getErrorCode()!=1061) { System.err.println(e.getErrorCode()); e.printStackTrace(System.err); } }
		System.out.println(" + adm1 names optimized in "+(System.currentTimeMillis()-start)/1000+"s.");
		start = System.currentTimeMillis();
		try { s.execute("CREATE INDEX country ON `geography`.`adm2_temp_names` (country)"); }
		catch (SQLException e) { if (e.getErrorCode()!=1061) { System.err.println(e.getErrorCode()); e.printStackTrace(System.err); } }
		System.out.println(" + adm2 names optimized in "+(System.currentTimeMillis()-start)/1000+"s.");
		start = System.currentTimeMillis();
		try { s.execute("CREATE INDEX country ON `geography`.`adm3_temp_names` (country)"); }
		catch (SQLException e) { if (e.getErrorCode()!=1061) { System.err.println(e.getErrorCode()); e.printStackTrace(System.err); } }
		System.out.println(" + adm3 names optimized in "+(System.currentTimeMillis()-start)/1000+"s.");
		start = System.currentTimeMillis();
		try { s.execute("CREATE INDEX country ON `geography`.`adm4_temp_names` (country)"); }
		catch (SQLException e) { if (e.getErrorCode()!=1061) { System.err.println(e.getErrorCode()); e.printStackTrace(System.err); } }
		System.out.println(" + adm4 names optimized in "+(System.currentTimeMillis()-start)/1000+"s.");
		start = System.currentTimeMillis();
		try { s.execute("CREATE INDEX country ON `geography`.`ppl_temp_names` (country)"); }
		catch (SQLException e) { if (e.getErrorCode()!=1061) { System.err.println(e.getErrorCode()); e.printStackTrace(System.err); } }
		System.out.println(" + ppl names optimized in "+(System.currentTimeMillis()-start)/1000+"s.");

		s.close();
	}
	
	private static void process_countries(Map<String,String> countries_gadmid) throws SQLException {
		System.out.println("Processing countries' administrative divisions");
		Statement s = GADM.db.createStatement();
		s.execute("SELECT `code` FROM `geography`.`country`");
		ResultSet rs = s.getResultSet();
		if (rs.next()) {
			do {
				String country_code = rs.getString(1);
				if (!country_code.equals("KH")) continue;
				String gadmid = countries_gadmid.get(country_code);
				if (gadmid == null) continue;
				process_country(country_code, gadmid);
			} while (rs.next());
		}
		rs.close();
		s.close();
	}
	
	private static void process_country(String country_code, String gadmid) throws SQLException {
		System.out.println(" + Processing country "+country_code+" (GADMID "+gadmid+")");
		long start = System.currentTimeMillis();
		
		int max_level = 0;
		int new_level = process_level(country_code, gadmid, 1, 1);
		if (new_level > max_level) {
			max_level = new_level;
			// TODO next levels
		}
		System.out.println("  => country "+country_code+" contains "+max_level+" levels of administrative divisions. Processed in "+((System.currentTimeMillis()-start)/1000)+"s.");
	}
	
	static class GADMEntity {
		long id;
		String type;
		List<String> def_names;
		List<String> local_names;
		long geoname_id;
	}
	static class GeoName {
		String lang;
		String name;
	}
	static class GeoNameEntity {
		long id;
		List<GeoName> names = new LinkedList<GeoName>();
	}
	private static int process_level(String country_code, String gadmid, int gadm_level, int geonames_level) throws SQLException {
		System.out.println("   + Processing GADM level "+gadm_level);

		// get the level 1
		Statement s = GADM.gadm.createStatement();
		StringBuilder q = new StringBuilder();
		q.append("SELECT DISTINCT ");
		q.append("`ID_").append(gadm_level).append("`,");
		q.append("`NAME_").append(gadm_level).append("`,");
		q.append("`VARNAME_").append(gadm_level).append("`,");
		q.append("`TYPE_").append(gadm_level).append("`,");
		q.append("`ENGTYPE_").append(gadm_level).append("`");
		q.append(" FROM `gadm2` WHERE ");
		q.append("`ID_0`=").append(gadmid); // restrict to country
		q.append(" AND `ID_").append(gadm_level).append("` IS NOT NULL");
		q.append(" AND `ENGTYPE_").append(gadm_level).append("` IS NOT NULL");
		q.append(" AND `ENGTYPE_").append(gadm_level).append("`<>'Water Body'");
		q.append(" AND `ENGTYPE_").append(gadm_level).append("`<>'Lake'");
		q.append(" AND `ENGTYPE_").append(gadm_level).append("`<>'Waterbody'");
		q.append(" AND `VALIDTO_").append(gadm_level).append("` IN ('Present','Unknown',NULL,'')");
		s.execute(q.toString());
		ResultSet rs = s.getResultSet();
		if (!rs.next()) {
			// no info at this level
			rs.close();
			s.close();
			return gadm_level-1;
		}
		LinkedList<GADMEntity> entities = new LinkedList<GADMEntity>();
		Map<Long,GADMEntity> match_founds = new HashMap<Long,GADMEntity>();
		do {
			GADMEntity e = new GADMEntity();
			e.id = ((Number)rs.getObject(1)).longValue();
			e.type = rs.getString(5);
			String name = rs.getString(2);
			e.def_names = GADM.getNames(name);
			String varname = rs.getString(3);
			e.local_names = GADM.getNames(varname);
			if (e.def_names.isEmpty() && e.local_names.isEmpty()) {
				System.out.println("NO NAME for level "+gadm_level+" ID "+rs.getObject(1)+" in country "+country_code+" ("+gadmid+")");
				continue;
			}
			entities.add(e);
			e.geoname_id = FindNames.find_name(country_code, e.def_names, e.local_names, new List[0], geonames_level);
			if (e.geoname_id != -1) {
				GADMEntity found = match_founds.get(e.geoname_id);
				if (found != null) {
					System.err.println("WARNING: two GADM match with the same geoname:");
					System.err.println(" - "+e.id);
					System.err.println(" - "+found.id);
					System.err.println(" - match with geoname "+e.geoname_id);
					System.err.println(" => cancel "+e.id);
					e.geoname_id = -1;
				} else {
					match_founds.put(e.geoname_id, e);
				}
			} else {
				System.out.println("NOT FOUND: "+name+" // "+varname);
			}
		} while (rs.next());
		
		System.out.println("     => "+entities.size()+" level "+gadm_level+" found, "+match_founds.size()+" are matching with geonames level "+geonames_level);
		if (match_founds.size() < entities.size()) {
			// some are missing
			if (match_founds.size() > 60*entities.size()/100) {
				// more than 60% found, most likely we are on the good level
				// how much do we have from geonames at this level, which didn't match ?
				StringBuilder sql = new StringBuilder();
				sql.append("SELECT `id` FROM `geography`.`");
				if (geonames_level < 5) sql.append("adm").append(geonames_level); else sql.append("ppl");
				sql.append("_temp` WHERE `country`='").append(country_code).append("' AND `id` NOT IN (");
				boolean first = true;
				for (Long id : match_founds.keySet()) {
					if (first) first = false; else sql.append(',');
					sql.append(id);
				}
				sql.append(')');
				Statement s2 = GADM.db.createStatement();
				s2.execute(sql.toString());
				ResultSet rs2 = s2.getResultSet();
				LinkedList<Long> remaining = new LinkedList<Long>();
				if (rs2.next())
					do {
						remaining.add(((Number)rs2.getObject(1)).longValue());
					} while (rs2.next());
				rs2.close();
				System.out.println("Not found="+(entities.size()-match_founds.size())+", remaining from geonames="+remaining.size());
				if (!remaining.isEmpty()) {
					// retrieve names from remaining
					LinkedList<GeoNameEntity> geonames = new LinkedList<GeoNameEntity>();
					for (Long rem_id : remaining) {
						sql = new StringBuilder();
						sql.append("SELECT `lang`,`name` FROM `geography`.`");
						if (geonames_level < 5) sql.append("adm").append(geonames_level); else sql.append("ppl");
						sql.append("_temp_names` WHERE `country`='").append(country_code).append("' AND `id`=").append(rem_id);
						s2.execute(sql.toString());
						rs2 = s2.getResultSet();
						GeoNameEntity e = new GeoNameEntity();
						e.id = rem_id;
						if (rs2.next())
							do {
								GeoName n = new GeoName();
								n.lang = rs2.getString(1);
								n.name = rs2.getString(2);
								e.names.add(n);
							} while (rs2.next());
						geonames.add(e);
						rs2.close();
					}
					// try to match remaining geonames with remaining gadm
					for (GADMEntity gadm : entities) {
						if (gadm.geoname_id != -1) continue; // already matched
						GeoNameEntity m = FindNames.find_best(gadm, geonames);
						if (m != null) {
							gadm.geoname_id = m.id;
							System.out.println("GADM finally matched:");
							System.out.println(" - default:");
							for (String n : gadm.def_names) System.out.println("   - "+n);
							System.out.println(" - local:");
							for (String n : gadm.local_names) System.out.println("   - "+n);
							System.out.println("MATCHED WITH");
							for (GeoName n : m.names)
								System.out.println(" - "+n.lang+"="+n.name);
							match_founds.put(m.id, gadm);
						} else {
							System.out.println("GADM remains unmatched:");
							System.out.println(" - default:");
							for (String n : gadm.def_names) System.out.println("   - "+n);
							System.out.println(" - local:");
							for (String n : gadm.local_names) System.out.println("   - "+n);
						}
					}
					if (!geonames.isEmpty()) {
						System.out.println("Remaining Geonames are:");
						for (GeoNameEntity geo : geonames) {
							System.out.println(" *");
							for (GeoName n : geo.names)
								System.out.println(" - "+n.lang+"="+n.name);
						}
					}
				}
				s2.close();
			}
		}			
		System.out.println("     => "+entities.size()+" level "+gadm_level+" found, "+match_founds.size()+" are matching with geonames level "+geonames_level);
		
		
		
		
		
		
//		int nb = 0;
//		int[] nb_found = new int[5-geonames_level+1];
//		List<Long> founds = new LinkedList<Long>();
//		List<String> missing_ids = new LinkedList<String>();
//		List<List<String>> missing_names = new LinkedList<List<String>>();
//		do {
//			String type = rs.getString(5);
//			String name = rs.getString(2);
//			String varname = rs.getString(3);
//			List<String> def_names = GADM.getNames(name);
//			List<String> local_names = GADM.getNames(varname);
//			if (def_names.isEmpty() && local_names.isEmpty()) {
//				System.out.println("NO NAME for level "+gadm_level+" ID "+rs.getObject(1)+" in country "+country_code+" ("+gadmid+")");
//				continue;
//			}
//			nb++;
//			long geoname_id = FindNames.find_name(country_code, def_names, local_names, new List[0], geonames_level);
//			if (geoname_id != -1) {
//				nb_found[0]++;
//				if (founds.contains(geoname_id))
//					System.out.println("WARNING: same match already found!!! "+geoname_id+" for "+name+" // "+varname);
//				else
//					founds.add(geoname_id);
//			} else {
//				System.out.println("NOT FOUND: "+name+" // "+varname);
//				missing_ids.add(rs.getString(1));
//				def_names.addAll(local_names);
//				missing_names.add(def_names);
//			}
//		} while (rs.next());
		
//		System.out.println("     => "+nb+" level "+gadm_level+" found, "+nb_found[0]+" are matching with geonames level "+geonames_level);
//		if (nb_found[0] < nb) {
//			// some are missing
//			if (nb_found[0] > 60*nb/100) {
//				// more than 60% found, most likely we are on the good level
//				// how much do we have from geonames at this level, which didn't match ?
//				StringBuilder sql = new StringBuilder();
//				sql.append("SELECT `id` FROM `geography`.`");
//				if (geonames_level < 5) sql.append("adm").append(geonames_level); else sql.append("ppl");
//				sql.append("_temp` WHERE `country`='").append(country_code).append("' AND `id` NOT IN (");
//				boolean first = true;
//				for (Long id : founds) {
//					if (first) first = false; else sql.append(',');
//					sql.append(id);
//				}
//				sql.append(')');
//				Statement s2 = GADM.db.createStatement();
//				s2.execute(sql.toString());
//				ResultSet rs2 = s2.getResultSet();
//				LinkedList<Long> remaining = new LinkedList<Long>();
//				if (rs2.next())
//					do {
//						remaining.add(((Number)rs2.getObject(1)).longValue());
//					} while (rs2.next());
//				rs2.close();
//				System.out.println("Not found="+(nb-nb_found[0])+", remaining from geonames="+remaining.size());
//				if (!remaining.isEmpty()) {
//					// try to match
//					for (Long rem_id : remaining) {
//						sql = new StringBuilder();
//						sql.append("SELECT `lang`,`name` FROM `geography`.`");
//						if (geonames_level < 5) sql.append("adm").append(geonames_level); else sql.append("ppl");
//						sql.append("_temp_names` WHERE `country`='").append(country_code).append("' AND `id`=").append(rem_id);
//						s2.execute(sql.toString());
//						rs2 = s2.getResultSet();
//						if (rs2.next())
//							do {
//								String geo_name = rs2.getString(2);
//								for (List<String> names : missing_names) {
//									for (String n : names) {
//										System.out.println(n+" // "+geo_name);
//									}
//								}
//							} while (rs2.next());
//						rs2.close();
//					}
//				}
//				s2.close();
//			}
//		}			
			
//		if (nb_found[0] < nb) {
//			
//			if (geonames_level < 5) {
//				s.execute(q.toString());
//				rs = s.getResultSet();
//				rs.next();
//				do {
//					String name = rs.getString(2);
//					String varname = rs.getString(3);
//					List<String> def_names = GADM.getNames(name);
//					List<String> local_names = GADM.getNames(varname);
//					if (def_names.isEmpty() && local_names.isEmpty()) {
//						continue;
//					}
//					for (int i = geonames_level+1; i <= 5; ++i) {
//						long geoname_id = FindNames.find_name(country_code, def_names, local_names, new List[0], i);
//						if (geoname_id != -1) {
//							nb_found[i-geonames_level]++;
//						} else {
//							//System.out.println("NOT FOUND: "+name+" // "+varname);
//						}
//					}
//				} while (rs.next());
//			}			
//			for (int i = geonames_level+1; i <= 5; ++i)
//				System.out.println("     => "+nb_found[i-geonames_level]+" are matching with geonames level "+i);
//		}
		rs.close();
		s.close();
		return gadm_level+1;
	}
	
}
