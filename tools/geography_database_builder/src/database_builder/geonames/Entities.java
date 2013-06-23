package database_builder.geonames;


import java.util.ArrayList;
import sql.SQLProcessor;

public class Entities {

	static class Entity {
		public Entity() {
			buf[0] = new char[32];
			buf[1] = new char[201];
			buf[2] = null;
			buf[3] = null;
			buf[4] = new char[32];
			buf[5] = new char[32];
			buf[6] = new char[2];
			buf[7] = new char[11];
			buf[8] = new char[3];
			buf[9] = null;
			buf[10] = new char[21];
			buf[11] = new char[81];
			buf[12] = new char[21];
			buf[13] = new char[21];
			buf[14] = null;
			buf[15] = null;
			buf[16] = null;
			buf[17] = new char[41];
			buf[18] = null;
		}
		char[][] buf = new char[19][];
		int[] buf_pos = new int[19];
		static int[] buf_pos_0 = new int[19];
		public void init_buf() {
			System.arraycopy(buf_pos_0, 0, buf_pos, 0, 19);  
		}
		
		long id;
		float latitude;
		float longitude;
		String default_name;
		char feature_class;
		String feature_code;
		String country_code;
		String adm1, adm2, adm3, adm4;
		String timezone;
		
		@Override
		public String toString() {
			return ""+id+"\t"+latitude+"\t"+longitude+"\t"+default_name+"\t"+feature_class+"\t"+feature_code+"\t"+country_code+"\t"+adm1;
		}
	}
	private static final int MAX_ENTITIES = 200000;
	static Entity[] entities = new Entity[MAX_ENTITIES];
	static int nb_entities = 0;
	static long waited_for_entities = 0;
	static Entity get_entity() {
		do {
			synchronized (entities) {
				if (nb_entities > 0)
					return entities[--nb_entities];
			}
			// no more entities => take from released
			for (int i = 0; i < released_entities.length; ++i) {
				synchronized (released_entities[i]) {
					int nb = released_nb_entities[i];
					if (nb == 0) continue;
					synchronized (entities) {
						if (nb + nb_entities > MAX_ENTITIES) nb = MAX_ENTITIES-nb_entities;
						System.arraycopy(released_entities[i], released_nb_entities[i]-nb, entities, nb_entities, nb);
						nb_entities += nb;
					}
					released_nb_entities[i] -= nb;
				}
			}
			// try again
			synchronized (entities) {
				if (nb_entities > 0)
					return entities[--nb_entities];
				long start = System.nanoTime();
				try { entities.wait(); } catch (InterruptedException ex) { return null; }
				long end = System.nanoTime();
				waited_for_entities += end-start;
			}
		} while (true);
	}
	static void put_entity(Entity e, int proc) {
		synchronized (released_entities[proc]) {
			if (released_nb_entities[proc] == MAX_RELEASED_ENTITIES) {
				synchronized (entities) {
					if (nb_entities == MAX_ENTITIES) return;
					entities[nb_entities++] = e;
					if (nb_entities == 1)
						entities.notifyAll();
				}
			} else {
				released_entities[proc][released_nb_entities[proc]++] = e;
				synchronized(entities) { entities.notifyAll(); }
			}
		}
	}
	
	static EntityProcessor[] processors;
	static Entity[][] released_entities;
	static int[] released_nb_entities;
	static final int MAX_RELEASED_ENTITIES = 10000;
	static void startProcessors() {
		processors = new EntityProcessor[Runtime.getRuntime().availableProcessors()+1];
		released_entities = new Entity[processors.length][MAX_RELEASED_ENTITIES];
		released_nb_entities = new int[processors.length];
		entities_to_process = new Entity[MAX_ENTITIES+MAX_RELEASED_ENTITIES*processors.length];
		for (int i = 0; i < processors.length; ++i) {
			processors[i] = new EntityProcessor(i+1);
			processors[i].start();
		}
	}
	
	static Entity[] entities_to_process;
	static int nb_entities_to_process = 0;
	static long last_id_seen = -1;
	static void to_process(Entity e) {
		last_id_seen = e.id;
		do {
			synchronized (entities_to_process) {
				if (nb_entities_to_process == entities_to_process.length) {
					System.out.println("WARNING: too much entities to process");
				} else {
					entities_to_process[Entities.nb_entities_to_process++] = e;
					entities_to_process.notify();
					return;
				}
			}
			try { Thread.sleep(100); } catch (InterruptedException ex) { return; }
		} while (true);
	}
	
	static class EntityProcessor extends Thread {
		public EntityProcessor(int num) {
			this.num = num;
		}
		int num;
		boolean stop = false;
		boolean ready = false;
		boolean stopped = false;
		public void run() {
			fill_entities();
			process_entities();
			stopped = true;
		}
		private void fill_entities() {
			// fill entities
			int created = 0;
			Entity[] e = new Entity[1000];
			do {
				for (int i = 0; i < e.length; ++i) e[i] = new Entity();
				synchronized (entities) {
					for (int i = 0; i < e.length; ++i) {
						if (nb_entities == MAX_ENTITIES) break;
						entities[nb_entities++] = e[i];
						created++;
					}
					if (nb_entities == MAX_ENTITIES) break;
				}
			} while (true);
			ready = true;
			synchronized (entities) { entities.notifyAll(); }
			System.out.println("Processor "+num+": "+created+" entities created");
		}
		private static final int BATCH_INSERT = 100;
		private void process_entities() {
			int processed = 0;
			long waited = 0;
			ArrayList<String[]> adm1_insert = new ArrayList<String[]>(BATCH_INSERT);
			ArrayList<String[]> adm1_names_insert = new ArrayList<String[]>(BATCH_INSERT);
			ArrayList<String[]> adm2_insert = new ArrayList<String[]>(BATCH_INSERT);
			ArrayList<String[]> adm2_names_insert = new ArrayList<String[]>(BATCH_INSERT);
			ArrayList<String[]> adm3_insert = new ArrayList<String[]>(BATCH_INSERT);
			ArrayList<String[]> adm3_names_insert = new ArrayList<String[]>(BATCH_INSERT);
			ArrayList<String[]> adm4_insert = new ArrayList<String[]>(BATCH_INSERT);
			ArrayList<String[]> adm4_names_insert = new ArrayList<String[]>(BATCH_INSERT);
			ArrayList<String[]> ppl_insert = new ArrayList<String[]>(BATCH_INSERT);
			ArrayList<String[]> ppl_names_insert = new ArrayList<String[]>(BATCH_INSERT);
			// process entities
			do {
				Entity e = null;
				do {
					synchronized (entities_to_process) {
						if (nb_entities_to_process == 0) {
							if (stop) break;
							long start = System.nanoTime();
							try { entities_to_process.wait(); } catch (InterruptedException ex) { break; }
							waited += System.nanoTime()-start;
							continue;
						}
						e = entities_to_process[--nb_entities_to_process];
						break;
					}
				} while (true);
				if (e == null) break; // stop and no more to process
				processed++;
				
				// parsing data
				boolean valid = true;
				boolean names_needed = false;
				try { e.id = parseLong(e.buf[0], e.buf_pos[0]); } catch (NumberFormatException ex) { valid = false; }
				if (valid) try { e.latitude = Float.parseFloat(new String(e.buf[4], 0, e.buf_pos[4])); } catch (NumberFormatException ex) { valid = false; }
				if (valid) try { e.longitude = Float.parseFloat(new String(e.buf[5], 0, e.buf_pos[5])); } catch (NumberFormatException ex) { valid = false; }
				if (valid) {
					e.feature_class = e.buf[6][0];
					if (e.feature_class == 'A' || e.feature_class == 'P') {
						e.default_name = new String(e.buf[1], 0, e.buf_pos[1]);
						e.feature_code = new String(e.buf[7], 0, e.buf_pos[7]);
						e.country_code = new String(e.buf[8], 0, e.buf_pos[8]);
						e.adm1 = e.adm2 = e.adm3 = e.adm4 = null;
						if (e.buf_pos[10] > 0) {
							e.adm1 = new String(e.buf[10], 0, e.buf_pos[10]);
							if (e.buf_pos[11] > 0) {
								e.adm2 = new String(e.buf[11], 0, e.buf_pos[11]);
								if (e.buf_pos[12] > 0) {
									e.adm3 = new String(e.buf[12], 0, e.buf_pos[12]);
									if (e.buf_pos[13] > 0) {
										e.adm4 = new String(e.buf[13], 0, e.buf_pos[13]);
									}
								}
							}
						}
						e.timezone = new String(e.buf[17], 0, e.buf_pos[17]);
						
						if (e.feature_class == 'A') {
							if (e.feature_code.startsWith("PCL")) {
								// this is a country
								if (e.feature_code.equals("PCLH")) {
									// historical: skip
									//System.out.println("Skip historical country: "+e.toString());
								} else {
									SQLProcessor.add_query("`geography`.`country_temp`","INSERT INTO `geography`.`country_temp` (`code`,`latitude`,`longitude`,`timezone`) VALUES ('"+e.country_code+"','"+Float.toString(e.latitude)+"','"+Float.toString(e.longitude)+"',"+SQLProcessor.escape(e.timezone)+")");
									SQLProcessor.add_query("`geography`.`country_temp_names`","INSERT INTO `geography`.`country_temp_names` (`code`,`lang`,`name`) VALUES ('"+e.country_code+"','__',"+SQLProcessor.escape(e.default_name)+")");
									AlternateNamesProcessor.process_names(e.id, e.default_name, "country_temp_names", new String[] {"`code`"}, new String[] {"'"+e.country_code+"'"});
									names_needed = true;
								}
							} else if (e.feature_code.startsWith("ADM")) {
								if (e.feature_code.equals("ADM1")) {
									adm1_insert.add(new String[] { "'"+e.country_code+"'",Long.toString(e.id),SQLProcessor.escape(e.adm1),"'"+Float.toString(e.latitude)+"'","'"+Float.toString(e.longitude)+"'"});
									adm1_names_insert.add(new String[] { "'"+e.country_code+"'",Long.toString(e.id),"'__'",SQLProcessor.escape(e.default_name) });
									if (adm1_insert.size() == BATCH_INSERT) {
										SQLProcessor.insert("`geography`.`adm1_temp`","`country`,`id`,`adm1`,`latitude`,`longitude`",adm1_insert);
										adm1_insert.clear();
										SQLProcessor.insert("`geography`.`adm1_temp_names`","`country`,`id`,`lang`,`name`",adm1_names_insert);
										adm1_names_insert.clear();
									}
									AlternateNamesProcessor.process_names(e.id, e.default_name, "adm1_temp_names", new String[] {"`country`","`id`"}, new String[] {"'"+e.country_code+"'",Long.toString(e.id)});
									names_needed = true;
								} else if (e.feature_code.equals("ADM2")) {
									adm2_insert.add(new String[] { "'"+e.country_code+"'",Long.toString(e.id),SQLProcessor.escape(e.adm1),SQLProcessor.escape(e.adm2),"'"+Float.toString(e.latitude)+"'","'"+Float.toString(e.longitude)+"'"});
									adm2_names_insert.add(new String[] { "'"+e.country_code+"'",Long.toString(e.id),"'__'",SQLProcessor.escape(e.default_name) });
									if (adm2_insert.size() == BATCH_INSERT) {
										SQLProcessor.insert("`geography`.`adm2_temp`","`country`,`id`,`adm1`,`adm2`,`latitude`,`longitude`",adm2_insert);
										adm2_insert.clear();
										SQLProcessor.insert("`geography`.`adm2_temp_names`","`country`,`id`,`lang`,`name`",adm2_names_insert);
										adm2_names_insert.clear();
									}
									AlternateNamesProcessor.process_names(e.id, e.default_name, "adm2_temp_names", new String[] {"`country`","`id`"}, new String[] {"'"+e.country_code+"'",Long.toString(e.id)});
									names_needed = true;
								} else if (e.feature_code.equals("ADM3")) {
									adm3_insert.add(new String[] { "'"+e.country_code+"'",Long.toString(e.id),SQLProcessor.escape(e.adm1),SQLProcessor.escape(e.adm2),SQLProcessor.escape(e.adm3),"'"+Float.toString(e.latitude)+"'","'"+Float.toString(e.longitude)+"'"});
									adm3_names_insert.add(new String[] { "'"+e.country_code+"'",Long.toString(e.id),"'__'",SQLProcessor.escape(e.default_name) });
									if (adm3_insert.size() == BATCH_INSERT) {
										SQLProcessor.insert("`geography`.`adm3_temp`","`country`,`id`,`adm1`,`adm2`,`adm3`,`latitude`,`longitude`",adm3_insert);
										adm3_insert.clear();
										SQLProcessor.insert("`geography`.`adm3_temp_names`","`country`,`id`,`lang`,`name`",adm3_names_insert);
										adm3_names_insert.clear();
									}
									AlternateNamesProcessor.process_names(e.id, e.default_name, "adm3_temp_names", new String[] {"`country`","`id`"}, new String[] {"'"+e.country_code+"'",Long.toString(e.id)});
									names_needed = true;
								} else if (e.feature_code.equals("ADM4")) {
									adm4_insert.add(new String[] { "'"+e.country_code+"'",Long.toString(e.id),SQLProcessor.escape(e.adm1),SQLProcessor.escape(e.adm2),SQLProcessor.escape(e.adm3),SQLProcessor.escape(e.adm4),"'"+Float.toString(e.latitude)+"'","'"+Float.toString(e.longitude)+"'"});
									adm4_names_insert.add(new String[] { "'"+e.country_code+"'",Long.toString(e.id),"'__'",SQLProcessor.escape(e.default_name) });
									if (adm4_insert.size() == BATCH_INSERT) {
										SQLProcessor.insert("`geography`.`adm4_temp`","`country`,`id`,`adm1`,`adm2`,`adm3`,`adm4`,`latitude`,`longitude`",adm4_insert);
										adm4_insert.clear();
										SQLProcessor.insert("`geography`.`adm4_temp_names`","`country`,`id`,`lang`,`name`",adm4_names_insert);
										adm4_names_insert.clear();
									}
									AlternateNamesProcessor.process_names(e.id, e.default_name, "adm4_temp_names", new String[] {"`country`","`id`"}, new String[] {"'"+e.country_code+"'",Long.toString(e.id)});
									names_needed = true;
								}
							}
						} else if (e.feature_class == 'P') {
							if (
									!e.feature_code.equals("PPLH") && // historical
									!e.feature_code.equals("PPLQ") && // abandoned
									!e.feature_code.equals("PPLR") && // religious
									!e.feature_code.equals("PPLW") // destroyed
								) {
								ppl_insert.add(new String[] { "'"+e.country_code+"'",Long.toString(e.id),SQLProcessor.escape(e.adm1),SQLProcessor.escape(e.adm2),SQLProcessor.escape(e.adm3),SQLProcessor.escape(e.adm4),"'"+Float.toString(e.latitude)+"'","'"+Float.toString(e.longitude)+"'"});
								ppl_names_insert.add(new String[] { "'"+e.country_code+"'",Long.toString(e.id),"'__'",SQLProcessor.escape(e.default_name) });
								if (ppl_insert.size() == BATCH_INSERT) {
									SQLProcessor.insert("`geography`.`ppl_temp`","`country`,`id`,`adm1`,`adm2`,`adm3`,`adm4`,`latitude`,`longitude`",ppl_insert);
									ppl_insert.clear();
									SQLProcessor.insert("`geography`.`ppl_temp_names`","`country`,`id`,`lang`,`name`",ppl_names_insert);
									ppl_names_insert.clear();
								}
								AlternateNamesProcessor.process_names(e.id, e.default_name, "ppl_temp_names", new String[] {"`country`","`id`"}, new String[] {"'"+e.country_code+"'",Long.toString(e.id)});
								names_needed = true;
							}
						}
					}
				}
				
				if (!names_needed)
					AlternateNamesProcessor.forget(e.id);
				
				// reset entity and put back to the list
				e.init_buf();
				put_entity(e, num-1);
			} while (true);
			if (!adm1_insert.isEmpty()) SQLProcessor.insert("`geography`.`adm1_temp`","`country`,`id`,`adm1`,`latitude`,`longitude`",adm1_insert);
			if (!adm1_names_insert.isEmpty()) SQLProcessor.insert("`geography`.`adm1_temp_names`","`country`,`id`,`lang`,`name`",adm1_names_insert);
			if (!adm2_insert.isEmpty()) SQLProcessor.insert("`geography`.`adm2_temp`","`country`,`id`,`adm1`,`adm2`,`latitude`,`longitude`",adm2_insert);
			if (!adm2_names_insert.isEmpty()) SQLProcessor.insert("`geography`.`adm2_temp_names`","`country`,`id`,`lang`,`name`",adm2_names_insert);
			if (!adm3_insert.isEmpty()) SQLProcessor.insert("`geography`.`adm3_temp`","`country`,`id`,`adm1`,`adm2`,`adm3`,`latitude`,`longitude`",adm3_insert);
			if (!adm3_names_insert.isEmpty()) SQLProcessor.insert("`geography`.`adm3_temp_names`","`country`,`id`,`lang`,`name`",adm3_names_insert);
			if (!adm4_insert.isEmpty()) SQLProcessor.insert("`geography`.`adm4_temp`","`country`,`id`,`adm1`,`adm2`,`adm3`,`adm4`,`latitude`,`longitude`",adm4_insert);
			if (!adm4_names_insert.isEmpty()) SQLProcessor.insert("`geography`.`adm4_temp_names`","`country`,`id`,`lang`,`name`",adm4_names_insert);
			if (!ppl_insert.isEmpty()) SQLProcessor.insert("`geography`.`ppl_temp`","`country`,`id`,`adm1`,`adm2`,`adm3`,`adm4`,`latitude`,`longitude`",ppl_insert);
			if (!ppl_names_insert.isEmpty()) SQLProcessor.insert("`geography`.`ppl_temp_names`","`country`,`id`,`lang`,`name`",ppl_names_insert);
			System.out.println("Processor "+num+": "+processed+" entities processed, waited "+(waited/1000000000)+"s.");
		}
	}
	
	private static long parseLong(char[] buf, int nb_chars) throws NumberFormatException {
		long value = 0;
		long mul = 1;
		while (nb_chars > 0) {
			long v = buf[--nb_chars] - '0';
			if (v < 0 || v > 9) throw new NumberFormatException();
			value += v*mul;
			mul *= 10;
		}
		return value;
	}
	
}
