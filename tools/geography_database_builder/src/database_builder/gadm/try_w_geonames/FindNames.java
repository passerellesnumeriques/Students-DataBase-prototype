package database_builder.gadm.try_w_geonames;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import sql.SQLProcessor;
import database_builder.gadm.try_w_geonames.Adm.GADMEntity;
import database_builder.gadm.try_w_geonames.Adm.GeoName;
import database_builder.gadm.try_w_geonames.Adm.GeoNameEntity;

public class FindNames {

	private static class Name {
		long id;
		// TODO
		LinkedList<String> names = new LinkedList<String>();
		double match = 0;
	}

	private static class Names {
		LinkedList<Name> names = new LinkedList<Name>();

		void add(long id, String lang, String name, List<String> def_names, List<String> local_names, double points) {
			Name n = null;
			for (Name e : names)
				if (e.id == id) {
					n = e;
					break;
				}
			if (n == null) {
				n = new Name();
				n.id = id;
				names.add(n);
			}
			n.names.add(name);
			if (lang.equals("__") || lang.equals("en")) {
				// this is the default name or the english name
				// 1- try to find a perfect match
				boolean found = false;
				for (String dn : def_names)
					if (dn.toLowerCase().equals(name.toLowerCase())) {
						// perfect match
						n.match += points;
						found = true;
						break;
					}
				if (!found) {
					for (String ln : local_names)
						if (ln.toLowerCase().equals(name.toLowerCase())) {
							// perfect match
							n.match += points * 0.9;
							found = true;
							break;
						}
				}
				// 2- try to find a match with the start
				if (!found) {
					for (String dn : def_names)
						if (dn.toLowerCase().startsWith(name.toLowerCase())	|| name.toLowerCase().startsWith(dn.toLowerCase())) {
							n.match += points * 0.5;
							found = true;
							break;
						}
				}
				if (!found) {
					for (String ln : local_names)
						if (ln.toLowerCase().startsWith(name.toLowerCase())	|| name.toLowerCase().startsWith(ln.toLowerCase())) {
							n.match += points * 0.3;
							found = true;
							break;
						}
				}
				// 3- try to find a match with the end
				if (!found) {
					for (String dn : def_names)
						if (dn.toLowerCase().endsWith(name.toLowerCase()) || name.toLowerCase().endsWith(dn.toLowerCase())) {
							n.match += points * 0.4;
							found = true;
							break;
						}
				}
				if (!found) {
					for (String ln : local_names)
						if (ln.toLowerCase().endsWith(name.toLowerCase()) || name.toLowerCase().endsWith(ln.toLowerCase())) {
							n.match += points * 0.2;
							found = true;
							break;
						}
				}
				// 4- how much words are there
				if (!found) {
					for (String dn : def_names)
						if (letters_match(dn, name)) {
							n.match += points * 0.2;
							found = true;
							break;
						}
				}
				if (!found) {
					for (String ln : local_names)
						if (letters_match(ln, name)) {
							n.match += points * 0.1;
							found = true;
							break;
						}
				}
				
				// 5- finally assign a low score
				if (!found)
					n.match += points * 0.05;
			} else {
				// this is a local name
				// 1- try to find a perfect match
				boolean found = false;
				for (String ln : local_names)
					if (ln.toLowerCase().equals(name.toLowerCase())) {
						// perfect match
						n.match += points * 0.5;
						found = true;
						break;
					}
				if (!found) {
					for (String dn : def_names)
						if (dn.toLowerCase().equals(name.toLowerCase())) {
							// perfect match
							n.match += points * 0.4;
							found = true;
							break;
						}
				}
				// 2- try to find a match with the start
				if (!found) {
					for (String ln : local_names)
						if (ln.toLowerCase().startsWith(name.toLowerCase()) || name.toLowerCase().startsWith(ln.toLowerCase())) {
							n.match += points * 0.1;
							found = true;
							break;
						}
				}
				if (!found) {
					for (String dn : def_names)
						if (dn.toLowerCase().startsWith(name.toLowerCase())	|| name.toLowerCase().startsWith(dn.toLowerCase())) {
							n.match += points * 0.08;
							found = true;
							break;
						}
				}
				// 2- try to find a match with the end
				if (!found) {
					for (String ln : local_names)
						if (ln.toLowerCase().endsWith(name.toLowerCase()) || name.toLowerCase().endsWith(ln.toLowerCase())) {
							n.match += points * 0.08;
							found = true;
							break;
						}
				}
				if (!found) {
					for (String dn : def_names)
						if (dn.toLowerCase().endsWith(name.toLowerCase()) || name.toLowerCase().endsWith(dn.toLowerCase())) {
							n.match += points * 0.06;
							found = true;
							break;
						}
				}
				// 4- how much words are there
				if (!found) {
					for (String ln : local_names)
						if (letters_match(ln, name)) {
							n.match += points * 0.06;
							found = true;
							break;
						}
				}
				if (!found) {
					for (String dn : def_names)
						if (letters_match(dn, name)) {
							n.match += points * 0.05;
							found = true;
							break;
						}
				}
				// 5- finally assign a low score
				if (!found)
					n.match += points * 0.001;
			}
		}
		private boolean letters_match(String n1, String n2) {
			int nb = Math.min(n1.length(), n2.length());
			n1 = n1.toLowerCase(); n2 = n2.toLowerCase();
			int nb_letters = 0;
			for (int i = 0; i < nb; ++i) {
				char c1 = n1.charAt(i);
				char c2 = n2.charAt(i);
				if (c1 >= 'a' && c1 <= 'z') {
					if (c1 != c2) return false;
					nb_letters++;
				} else if (c2 >= 'a' && c2 <= 'z')
					return false;
			}
			if (nb_letters > nb/2) return true;
			return false;
		}

		void printResult(String country_code, List<String> def_names, List<String> local_names) {
			System.out.println("Find name:");
			System.out.println(" - country_code = " + country_code);
			System.out.println(" - default names:");
			for (String n : def_names)
				System.out.println("   - " + n);
			System.out.println(" - local names:");
			for (String n : local_names)
				System.out.println("   - " + n);
			if (names.isEmpty()) {
				System.err.println("NO MATCH!");
				return;
			}
			System.out.println(" - matching names:");
			for (Name n : names) {
				System.out.println("   - id " + n.id + " match " + n.match + " names:");
				for (String s : n.names)
					System.out.println("     - " + s);
			}
			System.out.println("--------");
		}
		
		Name getBest() {
			if (names.isEmpty()) return null;
			Iterator<Name> it = names.iterator();
			Name best = it.next();
			int nb = 1;
			while (it.hasNext()) {
				Name n = it.next();
				if (n.match > best.match) { best = n; nb = 1; }
				else if (n.match == best.match) nb++;
			}
			if (nb > 1) {
				System.out.println("WARNING: match not good ! several having the same score");
				System.out.println(" - matching names:");
				for (Name n : names) {
					System.out.println("   - id " + n.id + " match " + n.match + " names:");
					for (String s : n.names)
						System.out.println("     - " + s);
				}
				System.out.println("--------");
			}
			return best;
		}
	}

	public static long find_name(String country_code, List<String> def_names, List<String> local_names, List<String>[] prev_names, int level)
	throws SQLException {
		// find the name from geonames
		StringBuilder q = new StringBuilder();
		q.append("SELECT * FROM `geography`.`");
		if (level < 5)
			q.append("adm").append(level);
		else
			q.append("ppl");
		q.append("_temp_names` WHERE `country`='").append(country_code).append("' AND (");
		boolean first = true;
		for (String n : def_names) {
			if (first)
				first = false;
			else
				q.append(" OR ");
			q.append("name LIKE '%").append(SQLProcessor.escape_sql(GADM.db, n)).append("%'");
			StringBuilder qq = new StringBuilder();
			boolean has_others = false;
			boolean last_letter = false;
			for (int i = 0; i < n.length(); ++i) {
				char c = n.charAt(i);
				if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
					qq.append(c); 
					last_letter = true;
				} else { 
					if (last_letter)
						qq.append('%');
					has_others = true;
					last_letter = false;
				}
			}
			if (has_others)
				q.append(" OR name LIKE '").append(qq).append("'");
		}
		for (String n : local_names) {
			if (first)
				first = false;
			else
				q.append(" OR ");
			q.append("name LIKE '%").append(SQLProcessor.escape_sql(GADM.db, n)).append("%'");
			StringBuilder qq = new StringBuilder();
			boolean has_others = false;
			boolean last_letter = false;
			for (int i = 0; i < n.length(); ++i) {
				char c = n.charAt(i);
				if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
					qq.append(c); 
					last_letter = true;
				} else { 
					if (last_letter)
						qq.append('%');
					has_others = true;
					last_letter = false;
				}
			}
			if (has_others)
				q.append(" OR name LIKE '").append(qq).append("'");
		}
		q.append(")");
		Statement s = GADM.db.createStatement();
		s.execute(q.toString());
		//System.out.println(q.toString());
		ResultSet rs = s.getResultSet();
		Names names = new Names();
		if (rs.next()) {
			//System.out.println(" - "+rs.getString(3)+": "+rs.getString(4));
			do {
				names.add(((Number) rs.getObject(2)).longValue(), rs.getString(3), rs.getString(4), def_names, local_names, 1);
			} while (rs.next());
		}
		//names.printResult(country_code, def_names, local_names);
		Name n = names.getBest();
		if (n == null) return -1;
		if ((names.names.size() > 1 && n.match < 0.1) || n.match < 0.01) {
			System.out.println("WARNING: Dangerous match "+n.match+" ("+names.names.size()+" possibilities) for "+q.toString());
			names.printResult(country_code, def_names, local_names);
		}
		return n.id;
	}

	static GeoNameEntity find_best(GADMEntity gadm, List<GeoNameEntity> geonames) {
		List<Double> scores = new ArrayList<Double>(geonames.size());
		List<String> gadm_names = new LinkedList<String>();
		gadm_names.addAll(gadm.def_names);
		gadm_names.addAll(gadm.local_names);
		for (GeoNameEntity e : geonames) {
			List<String> names = new LinkedList<String>();
			for (GeoName n : e.names)
				names.add(n.name);
			scores.add(score(gadm_names, names));
		}
		int best_i = 0;
		double best_score = scores.get(0);
		double best_score2 = -1;
		//System.out.println("SCORE="+best_score);
		for (int i = 1; i < scores.size(); ++i) {
			double s = scores.get(i);
			//System.out.println("SCORE="+s);
			if (s > best_score) {
				best_score2 = best_score;
				best_score = s;
				best_i = i;
			}
		}
		if (best_score >= 1) {
			if (best_score2 <= 0 || best_score2<best_score/2)
				return geonames.remove(best_i);
		}
		return null;
	}
	
	private static double score(List<String> n1, List<String> n2) {
		double score = 0;
		for (String s1 : n1)
			for (String s2 : n2)
				score += score(s1, s2);
		return score;
	}
	private static double score(String s1, String s2) {
		String[] words1 = s1.split(" ");
		String[] words2 = s2.split(" ");
		double score = 0;
		for (String w1 : words1) {
			if (w1.length() < 3) continue;
			w1 = w1.toLowerCase();
			for (String w2 : words2) {
				if (w2.length() < 3) continue;
				w2 = w2.toLowerCase();
				double s = score_word(w1, w2);
				if (s > 0) { score += s; continue; }
				String ww1 = java.text.Normalizer.normalize(w1, java.text.Normalizer.Form.NFD).replaceAll("[^\\p{ASCII}]", "");
				String ww2 = java.text.Normalizer.normalize(w2, java.text.Normalizer.Form.NFD).replaceAll("[^\\p{ASCII}]", "");
				//System.out.println(w1+" = "+ww1);
				//System.out.println(w2+" = "+ww2);
				score += score_word(ww1, ww2);
			}
		}
		return score;
	}
	private static double score_word(String w1, String w2) {
		if (w1.equals(w2)) return 10;
		double score = 0;
		// try to remove 1 letter
		for (int i = 0; i < w1.length(); ++i) {
			String w1_ = w1.substring(0,i)+w1.substring(i+1);
			if (w1_.equals(w2)) return 5;
			for (int j = 0; j < w2.length(); ++j) {
				String w2_ = w2.substring(0,j)+w2.substring(j+1);
				if (w2_.equals(w1)) return 5;
				if (w1_.equals(w2_)) score ++;
			}
		}
		// TODO better ?
		return score;
	}
	
}
