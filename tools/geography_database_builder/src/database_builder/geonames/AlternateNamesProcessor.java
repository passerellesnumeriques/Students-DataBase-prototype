package database_builder.geonames;


import java.io.File;
import java.io.FileInputStream;
import java.util.Iterator;
import java.util.LinkedList;

import net.lecousin.framework.collections.SelfMap;
import net.lecousin.framework.collections.SelfMapUniqueLongDoubleLinked;
import net.lecousin.framework.io.BufferedInputStream_Threaded;

import sql.SQLProcessor;

public class AlternateNamesProcessor extends Thread {

	static void process_names(long id, String default_name, String table, String[] prefill_fields, String[] prefill_values) {
		// try to find it in the parsed ones
		EntityNames en = null;
		synchronized (names) {
			if (id < last_id_found) {
				en = names.removeKey(id);
				if (en == null) return; // nothing will be found
			}
		}
		if (en != null) {
			processed_directly++;
			process_database(en, default_name, table, prefill_fields, prefill_values);
			return;
		}
		// not yet done
		Todo todo = new Todo();
		todo.id = id;
		todo.default_name = default_name;
		todo.table = table;
		todo.prefill_fields = prefill_fields;
		todo.prefill_values = prefill_values;
		synchronized (to_process) {
			to_process.add(todo);
			to_process.notify();
		}
	}
	static void forget(long id) {
		synchronized (names) {
			if (id <= last_id_found)
				names.removeKey(id);
		}
	}
	static boolean stop = false;
	static boolean stopped = false;
	static LinkedList<Todo> to_process = new LinkedList<Todo>();
	static int lines = 0;
	private static int processed_directly = 0;
	private static class Todo {
		long id;
		String default_name;
		String table;
		String[] prefill_fields;
		String[] prefill_values;
	}
	private static long last_id_found = -1;

	
	private static class Name {
		byte lang[] = new byte[2];
		String name;
		boolean preferred;
	}
	private static class EntityNames implements SelfMap.Entry<Long> {
		long id;
//		boolean done = false;
		LinkedList<Name> names = new LinkedList<Name>();
		@Override
		public Long getHashObject() { return id; }
	}
	private static SelfMapUniqueLongDoubleLinked<EntityNames> names = new SelfMapUniqueLongDoubleLinked<>(100000);
	
	public AlternateNamesProcessor() {
		super("Alternate Names");
	}
	@Override
	public void run() {
		try {
			System.out.println("Alternate Names: parsing file...");
			File file = new File(GenerateGeography.dir, "alternateNames.txt");
			FileInputStream fin = new FileInputStream(file);
			BufferedInputStream_Threaded bin = new BufferedInputStream_Threaded(fin, 8192, 8192); // = 64M
			boolean eof = false;
			long id = 0;
			byte[] lang = new byte[2];
			int lang_pos = 0;
			byte[] buf = new byte[1200];
			int buf_pos = 0;
			boolean flags[] = new boolean[4];
			EntityNames en = null;
			long start = System.currentTimeMillis();
			do {
//				if (last_id_found > Entities.last_id_seen+1000000) {
//					// we are too much in advance, let's wait
//					System.out.println("  Alternate names: waiting for allCountries");
//					long start_wait = System.currentTimeMillis();
//					do {
//						try { Thread.sleep(1000); } catch (InterruptedException ex) { break; }
//					} while (last_id_found > Entities.last_id_seen+500000);
//					long end_wait = System.currentTimeMillis();
//					start += end_wait-start_wait;
//					System.out.println("  Alternate names: resume after waiting for "+(end_wait-start_wait)+"ms.");
//				}
				boolean valid = true;
				boolean eol = false;
				// 1-alternate name id => skip
				do {
					int c = bin.read();
					if (c == '\t') break;
					if (c == -1) { eof = true; break; }
					if (c == '\n') { valid = false; eol = true; break; }
				} while (true);
				if (eof) break;
				if (!eol) {
					// 2-geo id
					id = 0;
					do {
						int c = bin.read();
						if (c == '\t') break;
						if (c == '\n') { valid = false; eol = true; break; }
						if (c == -1) { eof = true; break; }
						if (c >= '0' && c <= '9') {
							id = id*10+(c-'0');
						} else {
							valid = false;
						}
					} while (true);
					if (!eol) {
						// 3-language
						lang_pos = 0;
						do {
							int c = bin.read();
							if (c == '\t') break;
							if (c == '\n') { valid = false; eol = true; break; }
							if (c == -1) { eof = true; break; }
							if (lang_pos < 2)
								lang[lang_pos++] = (byte)c;
							else
								valid = false;
						} while (true);
						if (!eol) {
							// 4- alternate name (UTF-8 format)
							buf_pos = 0;
							do {
								int c1 = bin.read();
								if (c1 == -1) { eof = true; break; }
								if ((c1 & 0x80) == 0) {
									// only one byte
									if (c1 == '\t') break;
									if (c1 == '\n') { eol = true; break; }
									buf[buf_pos++] = (byte)c1;
								} else {
									buf[buf_pos++] = (byte)c1;
									if ((c1 & 0xFE) == 0xFC) {
										// 6 bytes
										buf[buf_pos++] = (byte)bin.read();
										buf[buf_pos++] = (byte)bin.read();
										buf[buf_pos++] = (byte)bin.read();
										buf[buf_pos++] = (byte)bin.read();
										buf[buf_pos++] = (byte)bin.read();
									} else if ((c1 & 0xFC) == 0xF8) {
										// 5 bytes
										buf[buf_pos++] = (byte)bin.read();
										buf[buf_pos++] = (byte)bin.read();
										buf[buf_pos++] = (byte)bin.read();
										buf[buf_pos++] = (byte)bin.read();
									} else if ((c1 & 0xF8) == 0xF0) {
										// 4 bytes
										buf[buf_pos++] = (byte)bin.read();
										buf[buf_pos++] = (byte)bin.read();
										buf[buf_pos++] = (byte)bin.read();
									} else if ((c1 & 0xF0) == 0xE0) {
										// 3 bytes
										buf[buf_pos++] = (byte)bin.read();
										buf[buf_pos++] = (byte)bin.read();
									} else if ((c1 & 0xE0) == 0xC0) {
										// 2 bytes
										buf[buf_pos++] = (byte)bin.read();
									}
								}
							} while (true);
							flags[0] = flags[1] = flags[2] = flags[3] = false;
							for (int i = 0; i < 4; ++i) {
								if (!eol && !eof) {
									do {
										int c = bin.read();
										if (c == '\t') break;
										if (c == -1) { eof = true; break; }
										if (c == '\n') { eol = true; break; }
										if (c == '1') flags[i] = true;
									} while (true);
								}
							}
						}
					}
				}
				if (valid && !flags[1] && !flags[2] && !flags[3] && lang_pos == 2 && buf_pos > 0) {
					Name name = new Name();
					name.lang[0] = lang[0]; name.lang[1] = lang[1];
					name.preferred = flags[0];
					name.name = new String(buf, 0, buf_pos, "UTF-8");
					if (en != null && en.id == id)
						en.names.add(name);
					else {
//						if (en != null) {
//							en.done = true;
//						}
						en = new EntityNames();
						en.id = id;
						en.names.add(name);
						synchronized (names) {
							names.putLast(en);
							last_id_found = id;
						}
					}
				} else if (id != 0) {
					synchronized (names) {
						last_id_found = id;
					}
				}
				lines++;
				if ((lines % 1000000) == 0) {
					long now = System.currentTimeMillis();
					System.out.println("Alternate names: "+lines+" lines processed in "+((now-start)/1000)+"s.");
				}
				if ((lines % 100000) == 0) {
					long max = Runtime.getRuntime().maxMemory();
					long free = Runtime.getRuntime().freeMemory() + (max-Runtime.getRuntime().totalMemory());
					if (free < max/4) {
						System.out.println(Thread.currentThread().getName()+": Critical memory: less than 25% remaining ("+free+"/"+max+"="+(free*100/max)+"%): wait...");
						do {
							try { Thread.sleep(10000); } catch (InterruptedException e) {}
							max = Runtime.getRuntime().maxMemory();
							free = Runtime.getRuntime().freeMemory() + (max-Runtime.getRuntime().totalMemory());
							if (free > max/5) break;
						} while (true);
						System.out.println(Thread.currentThread().getName()+": Resume as we have now more than 20%");
					}
				}
			} while (!eof);
//			if (en != null) en.done = true;
			long end = System.currentTimeMillis();
			System.out.println("Alternate names: "+lines+" processed in "+((end-start)/1000)+"s.");
			bin.close();
			System.out.println("Alternate names reader waited "+(bin.getWaitedNanosecondsForDisk()/1000000)+"ms. for the disk");
		} catch (Exception ex) {
			ex.printStackTrace(System.err);
		}
		// start processing remaining requests
		long processed = 0;
		long waited = 0;
		long started = System.currentTimeMillis();
		do {
			Todo t;
			synchronized (to_process) {
				if (to_process.isEmpty()) {
					if (stop) break;
					long start = System.nanoTime();
					try { to_process.wait(); } catch (InterruptedException ex) { break; }
					waited += System.nanoTime()-start;
					continue;
				}
				t = to_process.removeFirst();
			}
			processed++;
			EntityNames en = names.removeKey(t.id);
			if (en == null) continue;
			process_database(en, t.default_name, t.table, t.prefill_fields, t.prefill_values);
		} while (true);
		stopped = true;
		long ended = System.currentTimeMillis();
		System.out.println("Alternate Names processor: "+processed+" entities processed in "+((ended-started)/1000)+"s., waited "+(waited/1000000000)+"s.");
		System.out.println("Alternate Names processor: "+processed_directly+" entities processed directly from map by the caller");
	}	
	
	private static void process_database(EntityNames en, String default_name, String table, String[] prefill_fields, String[] prefill_values) {
		StringBuilder sql = new StringBuilder("INSERT INTO `geography`.`").append(table).append("` (");
		if (prefill_fields != null)
			for (String field : prefill_fields)
				sql.append(field).append(",");
		sql.append("`lang`,`name`) VALUES ");
		boolean first = true;
		while (!en.names.isEmpty()) {
			Name n = en.names.removeFirst();
			for (Iterator<Name> it = en.names.iterator(); it.hasNext(); ) {
				Name n2 = it.next();
				if (n2.lang[0] == n.lang[0] && n2.lang[1] == n.lang[1]) {
					// same language
					if (!n.preferred && n2.preferred)
						n = n2;
					it.remove();
				}
			}
			if (n.name.equals(default_name)) continue;
			if (first) first = false; else sql.append(',');
			sql.append('(');
			if (prefill_fields != null)
				for (String value : prefill_values)
					sql.append(value).append(",");
			sql.append('\'').append((char)(n.lang[0] & 0xFF)).append((char)(n.lang[1] & 0xFF)).append("',").append(SQLProcessor.escape(n.name)).append(')');
		}
		if (!first)
			SQLProcessor.add_query("`geography`.`"+table+"`", sql.toString());
	}
	
}
