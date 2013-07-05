package sql;

import java.io.UnsupportedEncodingException;
import java.sql.BatchUpdateException;
import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import database_builder.DataBase;

public class SQLProcessor {

	public static String escape(String s) {
		if (s == null) return "NULL";
		boolean ok = true;
		for (int i = s.length()-1; i >= 0; --i) {
			char c = s.charAt(i);
			if (c < 32 || c > 127 || c == '\r' || c == '\n' || c == '\\' || c == '\'' || c == '\"') { ok = false; break; }
		}
		if (ok) return "'"+s+"'";
		try {
			return "X'"+encodeHexa(s.getBytes("UTF-8"))+"'";
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace(System.err);
			return "''";
		}
	}
	public static String escape_ms(String s) {
		return "'"+s.replace("'", "''")+"'";
	}
	public static String escape_ms_string(String s) {
		return s.replace("'", "''");
	}
	public static String escape_sql(java.sql.Connection link, String str) throws SQLException {
		if (str == null) {
			return null;
		}

		if (str.replaceAll(
				"[a-zA-Z0-9_!@#$%^&*()-=+~.;:,\\Q[\\E\\Q]\\E<>{}\\/? ]", "")
				.length() < 1) {
			return str;
		}

		String clean_string = str;
		clean_string = clean_string.replaceAll("\\\\", "\\\\\\\\");
		clean_string = clean_string.replaceAll("\\n", "\\\\n");
		clean_string = clean_string.replaceAll("\\r", "\\\\r");
		clean_string = clean_string.replaceAll("\\t", "\\\\t");
		clean_string = clean_string.replaceAll("\\00", "\\\\0");
		clean_string = clean_string.replaceAll("'", "\\\\'");
		clean_string = clean_string.replaceAll("\\\"", "\\\\\"");

		if (clean_string.replaceAll(
				"[a-zA-Z0-9_!@#$%^&*()-=+~.;:,\\Q[\\E\\Q]\\E<>{}\\/?\\\\\"' ]",
				"").length() < 1) {
			return clean_string;
		}

		java.sql.Statement stmt = link.createStatement();
		String qry = "SELECT QUOTE('" + clean_string + "')";

		stmt.executeQuery(qry);
		java.sql.ResultSet resultSet = stmt.getResultSet();
		resultSet.first();
		String r = resultSet.getString(1);
		return r.substring(1, r.length() - 1);
	}
	private static final char[] hexaChar = new char[] { '0', '1', '2', '3', '4',
		'5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	private static String encodeHexa(byte[] data) {
		return encodeHexa(data, 0, data.length);
	}
	
	private static String encodeHexa(byte[] data, int off, int len) {
		StringBuilder str = new StringBuilder();
		for (int i = 0; i < len; ++i)
			str.append(hexaChar[(data[i + off] >>> 4) & 0xF]).append(
					hexaChar[data[i + off] & 0xF]);
		return str.toString();
	}

	
	private static class Query {
		Query(String query, List<Runnable> calls) { this.query = query; this.calls = calls; }
		String query;
		List<Runnable> calls;
	}
	private static Map<String,ArrayList<Query>> queries_by_table = new HashMap<String,ArrayList<Query>>();
	private static final int BATCH_NB_QUERIES = 500;
	public static void add_query(String table, String sql) {
		add_query(table,sql,null);
	}
	public static void add_query(String table, String sql, List<Runnable> calls) {
		ArrayList<Query> list;
		synchronized (queries_by_table) {
			list = queries_by_table.get(table);
			if (list == null) {
				list = new ArrayList<Query>(BATCH_NB_QUERIES);
				queries_by_table.put(table, list);
			}
			list.add(new Query(sql,calls));
		}
		long max = Runtime.getRuntime().maxMemory();
		long free = Runtime.getRuntime().freeMemory() + (max-Runtime.getRuntime().totalMemory());
		if (free < max/4) {
			Runtime.getRuntime().gc();
			max = Runtime.getRuntime().maxMemory();
			free = Runtime.getRuntime().freeMemory() + (max-Runtime.getRuntime().totalMemory());
			if (free < max/4) {
				System.out.println(Thread.currentThread().getName()+": Critical memory: less than 25% remaining ("+free+"/"+max+"="+(free*100/max)+"%): process SQL requests and wait 10 seconds... ["+sql_to_process.size()+" batches waiting]");
				clear_queries();
				int nb = 0;
				do {
					try { Thread.sleep(10000); } catch (InterruptedException e) {}
					if (++nb == 10) { Runtime.getRuntime().gc(); nb = 0; }
					max = Runtime.getRuntime().maxMemory();
					free = Runtime.getRuntime().freeMemory() + (max-Runtime.getRuntime().totalMemory());
					System.out.println("  - new status: "+free+"/"+max+"="+(free*100/max)+"%["+sql_to_process.size()+" batches waiting]");
					if (free > max/3 || sql_to_process.isEmpty()) break;
				} while (true);
			}
		} else if (list.size() >= BATCH_NB_QUERIES) {
			if (sql_to_process.size() > 100000) {
				do {
					System.out.println(Thread.currentThread().getName()+": Too much SQL to process ("+sql_to_process.size()+" batches): wait... (Memory free: "+free+"/"+max+"="+(free*100/max)+"%)");
					try { Thread.sleep(10000); } catch (InterruptedException e) {}
				} while (sql_to_process.size() > 80000);
				System.out.println(Thread.currentThread().getName()+": Resume ("+sql_to_process.size()+" batches, Memory free: "+free+"/"+max+"="+(free*100/max)+"%)");
			}
			synchronized (queries_by_table) {
				process_sql(table, list);
				queries_by_table.remove(table);
			}
		}
	}
	
	public static void insert(String table, String fields, List<String[]> values) {
		StringBuilder sql = new StringBuilder();
		sql.append("INSERT INTO ").append(table).append(" (").append(fields).append(") VALUES ");
		boolean f = true;
		for (String[] value : values) {
			if (f) f = false; else sql.append(',');
			sql.append('(');
			boolean first = true;
			for (String v : value) {
				if (first) first = false; else sql.append(',');
				sql.append(v);
			}
			sql.append(')');
		}
		add_query(table, sql.toString());
	}
	
	private static class InsertDelay {
		String[] fields;
		List<String[]> values = new LinkedList<String[]>();
		List<Runnable> calls = new LinkedList<Runnable>();
	}
	private static Map<String,LinkedList<InsertDelay>> inserts = new HashMap<String,LinkedList<InsertDelay>>();
	private static final int INSERT_MAX_VALUES = 500;
	public static void insert_delay(String table, String[] fields, String[] values) {
		insert_delay(table, fields, values, null);
	}
	public static void insert_delay(String table, String[] fields, String[] values, Runnable call) {
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
		LinkedList<InsertDelay> l;
		synchronized (inserts) {
			l = inserts.get(table);
			if (l == null) {
				l = new LinkedList<InsertDelay>();
				inserts.put(table, l);
			}
		}
		synchronized (l) {
			for (Iterator<InsertDelay> it = l.iterator(); it.hasNext(); ) {
				InsertDelay id = it.next();
				if (id.fields.length != fields.length) continue;
				int match[] = new int[fields.length];
				boolean perfect = true;
				boolean matching = true;
				for (int i = 0; i < fields.length; ++i) {
					match[i] = -1;
					for (int j = 0; j < id.fields.length; ++j)
						if (id.fields[j].equals(fields[i])) {
							if (j != i) perfect = false;
							match[i] = j;
						}
					if (match[i] == -1) {
						matching = false;
						break;
					}
				}
				if (!matching) continue;
				if (perfect) {
					id.values.add(values);
					if (call != null && !id.calls.contains(call)) id.calls.add(call);
					if (id.values.size() >= INSERT_MAX_VALUES) {
						process_insert(table, id);
						it.remove();
					}
					return;
				}
				String[] v = new String[values.length];
				for (int i = 0; i < v.length; ++i)
					v[match[i]] = values[i];
				id.values.add(v);
				if (call != null && !id.calls.contains(call)) id.calls.add(call);
				if (id.values.size() >= INSERT_MAX_VALUES) {
					process_insert(table, id);
					it.remove();
				}
				return;
			}
			InsertDelay i = new InsertDelay();
			i.fields = fields;
			i.values.add(values);
			if (call != null) i.calls.add(call);
			l.add(i);
		}
	}
	private static void process_insert(String table, InsertDelay id) {
		StringBuilder s = new StringBuilder();
		s.append("INSERT INTO ").append(table).append(" (");
		boolean first = true;
		for (String field : id.fields) {
			if (first) first = false; else s.append(',');
			s.append('`').append(field).append('`');
		}
		s.append(") VALUES ");
		first = true;
		for (String[] values: id.values) {
			if (first) first = false; else s.append(',');
			s.append('(');
			for (int i = 0; i < values.length; ++i) {
				if (i > 0) s.append(',');
				s.append(values[i]);
			}
			s.append(')');
		}
		add_query(table, s.toString(), id.calls);
	}
	
	public static void clear_queries() {
		synchronized (inserts) {
			for (Map.Entry<String,LinkedList<InsertDelay>> e : inserts.entrySet()) {
				for (InsertDelay i : e.getValue())
					process_insert(e.getKey(), i);
			}
			inserts.clear();
		}
		synchronized (queries_by_table) {
			for (Map.Entry<String,ArrayList<Query>> e : queries_by_table.entrySet())
				process_sql(e.getKey(), e.getValue());
			queries_by_table.clear();
		}
	}
	
	
	public static int getPendingBatches() { return sql_to_process.size(); }
	
	private static class SQLToProcess {
		String table;
		ArrayList<Query> queries;
	}
	private static LinkedList<SQLToProcess> sql_to_process = new LinkedList<SQLToProcess>();
	private static void process_sql(String table, ArrayList<Query> queries) {
		SQLToProcess t = new SQLToProcess();
		t.table = table;
		t.queries = queries;
		synchronized (sql_to_process) {
			sql_to_process.add(t);
			sql_to_process.notify();
		}
	}
	private static LinkedList<String> busy_tables = new LinkedList<String>();
	private static class SQLThread extends Thread {
		public SQLThread(int num) {
			super("SQLProcessor "+num);
			this.num = num;
		}
		private int num;
		private boolean stop = false, stopped = false;
		private Object stop_synch = new Object();
		private Connection db;
		public void end() {
			stop = true;
			synchronized (sql_to_process) {
				sql_to_process.notifyAll();
			}
			synchronized (stop_synch) {
				if (stopped) return;
				try { stop_synch.wait(); } catch (InterruptedException e) { return; }
			}
		}
		private int nb_queries = 0, nb_batch = 0;
		private long time = 0;
		int status = 0; // 0=stop, 1=look for requests, 2=pause, 3=processing, 4=stopping
		@Override
		public void run() {
			System.out.println("   SQL Thread "+num+": connecting to database...");
			try {
				db = DataBase.connect_db();
			} catch (SQLException e) {
				e.printStackTrace(System.err);
				return;
			}
			System.out.println("   SQL Thread "+num+": connected.");
			SQLToProcess t = null;
			int tried_requests = 0;
			int total_requests;
			boolean busy;
			status = 1;
			do {
				t = null;
				synchronized (sql_to_process) {
					if (sql_to_process.isEmpty()) {
						if (stop) break;
						// nothing to process ? let's see
						t = null;
						synchronized (queries_by_table) {
							for (Map.Entry<String, ArrayList<Query>> e : queries_by_table.entrySet())
								if (e.getValue().size() > 10) {
									boolean tbusy;
									synchronized (busy_tables) { tbusy = busy_tables.contains(e.getKey()); }
									if (!tbusy) {
										// let's take this one !
										t = new SQLToProcess();
										t.table = e.getKey();
										t.queries = e.getValue();
										queries_by_table.remove(e.getKey());
										break;
									}
								}
						}
						if (t == null) {
							// really nothing to do.. let's sleep
							status = 2;
							try { sql_to_process.wait(); } catch (InterruptedException e) { break; }
							status = 1;
							continue;
						}
					}
					total_requests = sql_to_process.size();
					if (t == null)
						t = sql_to_process.removeFirst();
					else
						tried_requests = total_requests;
				}
				busy = false;
				synchronized (busy_tables) {
					if (busy_tables.contains(t.table)) {
						tried_requests++;
						busy = true;
					} else {
						tried_requests = 0;
						busy_tables.add(t.table);
					}
				}
				if (busy) {
					synchronized (sql_to_process) {
						sql_to_process.add(t);
						//sql_to_process.notify();
					}
					if (tried_requests >= total_requests) {
						status = 2;
						if (total_requests > 1000)
							try { Thread.sleep(5000); } catch (InterruptedException e) { break; }
						else
							try { Thread.sleep(1000); } catch (InterruptedException e) { break; }
						status = 1;
						tried_requests = 0;
					}
					continue;
				}
				status = 3;
				nb_queries += t.queries.size();
				nb_batch++;
				long start = System.nanoTime();
				execute_queries(t.table, t.queries);
				time += System.nanoTime()-start;
				synchronized (busy_tables) {
					busy_tables.remove(t.table);
				}
				status = 1;
			} while (true);
			status = 4;
			try {
				db.close();
			} catch (SQLException e) {
				e.printStackTrace(System.err);
			}
			if (nb_queries > 0)
				System.out.println("   SQL thread "+num+": "+nb_queries+" queries processed in "+nb_batch+" batches: "+(time/1000000000)+"s. = "+(time/nb_batch/1000000)+"ms./batch = "+(time/nb_queries/1000000)+"ms./query");
			synchronized (stop_synch) {
				stopped = true;
				stop_synch.notify();
			}
			status = 0;
		}
		private void execute_queries(String table, List<Query> queries) {
			if (queries.isEmpty()) return;
			int original_nb = queries.size();
			Statement s;
			try {
				s = db.createStatement();
			} catch (SQLException e) {
				e.printStackTrace(System.err);
				return;
			}
			try {
				int nb_retry = 0;
				do {
					s.addBatch("START TRANSACTION");
					for (Query q : queries) {
						//System.out.println(q.query);
						s.addBatch(q.query);
					}
					s.addBatch("COMMIT");
					try {
						s.executeBatch();
						for (Query q : queries)
							call(q.calls);
						if (nb_retry > 0)
							System.out.println("    - lock resolved for table "+table+": batch processed successfully");
						break; // success
					} catch (BatchUpdateException ex) {
						int[] res = ex.getUpdateCounts();
						if ((ex.getErrorCode() == 1213 || ex.getErrorCode() == 1205) && nb_retry < 100) {
							// deadlock: we should retry
							if (nb_retry == 0) {
								if (ex.getErrorCode() == 1213)
									System.out.println("   Warning: deadlock situation for table "+table+", we will retry up to 100 times: "+queries.size()+" remaining on "+original_nb);
								else
									System.out.println("   Warning: lock timeout for table "+table+", we will retry up to 100 times: "+queries.size()+" remaining on "+original_nb);
							} else if ((nb_retry % 10) == 0)
								System.out.println("    - lock status for table "+table+": retry "+nb_retry+": "+queries.size()+" remaining on "+original_nb);
							LinkedList<Query> to_retry = new LinkedList<Query>();
							for (int i = 1; i < res.length-1; ++i)
								if (res[i] == Statement.EXECUTE_FAILED)
									to_retry.add(queries.get(i-1));
								else
									call(queries.get(i-1).calls);
							int old_nb = queries.size();
							queries = to_retry;
							s.close();
							s = db.createStatement();
							if (queries.size() < old_nb) {
								// we progress
								nb_retry = 0;
								continue;
							} else {
								nb_retry++;
								try { Thread.sleep(100*nb_retry); } catch (InterruptedException ex2) { break; }
								continue;
							}
						} else {
							System.err.println("SQL Batch execution error:");
							for (int i = 1; i < res.length-1; ++i)
								if (res[i] == Statement.EXECUTE_FAILED)
									System.err.println("SQL Query failed: "+queries.get(i-1).query);
								else
									call(queries.get(i-1).calls);
							System.err.println(" ** errors are:");
							SQLException e = ex;
							while (e != null) {
								System.err.println("SQL Exception code "+ex.getErrorCode()+":");
								e.printStackTrace(System.err);
								e = e.getNextException();
							}
							break; // failure
						}
					}
				} while (true);
			} catch (SQLException e) {
				System.err.println("Error executing queries:");
				e.printStackTrace(System.err);
			}
			try { 
				s.close();
			} catch (SQLException e) {
				e.printStackTrace(System.err);
			}
		}
	}
	
	private static LinkedList<Runnable> to_call = new LinkedList<Runnable>();
	private static void call(List<Runnable> calls) {
		if (calls == null || calls.isEmpty()) return;
		synchronized (to_call) {
			to_call.addAll(calls);
			to_call.notifyAll();
		}
	}
	private static class CallThread extends Thread {
		public CallThread() {
			super("Call thread");
		}
		boolean stop = false, stopped = false;
		boolean sleeping = false;
		@Override
		public void run() {
			Runnable r;
			do {
				synchronized (to_call) {
					if (to_call.isEmpty()) {
						if (stop) break;
						sleeping = true;
						try { to_call.wait(); } catch (InterruptedException e) { break; }
						sleeping = false;
						continue;
					}
					r = to_call.removeFirst();
				}
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
				r.run();
			} while (true);
			synchronized (this) {
				stopped = true;
				this.notify();
			}
		}
		public void end() {
			stop = true;
			synchronized (to_call) { to_call.notifyAll(); }
			synchronized (this) {
				if (stopped) return;
				try { this.wait(); } catch (InterruptedException e) { return; }
			}
		}
	}


	private static SQLThread[] sql_thread;
	private static CallThread[] call_thread;

	public static void start_threads() {
		sql_thread = new SQLThread[Runtime.getRuntime().availableProcessors()];
		for (int i = 0; i < sql_thread.length; ++i) {
			sql_thread[i] = new SQLThread(i+1);
			sql_thread[i].start();
		}
		call_thread = new CallThread[Runtime.getRuntime().availableProcessors()];
		for (int i = 0; i < call_thread.length; ++i) {
			call_thread[i] = new CallThread();
			call_thread[i].start();
		}
	}

	public static void wait_all_processed() {
		System.out.println("Waiting for all SQL requests to be processed");
		long last_status = 0;
		int nb_requests = 0;
		do {
			synchronized (sql_to_process) {
				synchronized (to_call) {
					clear_queries();
					if (sql_to_process.isEmpty()) {
						boolean all_sleep = true;
						for (int i = 0; i < sql_thread.length; ++i)
							if (sql_thread[i].status != 2) { all_sleep = false; break; }
						if (all_sleep) {
							if (to_call.isEmpty()) {
								for (int i = 0; i < call_thread.length; ++i)
									if (!call_thread[i].sleeping) { all_sleep = false; break; }
								if (all_sleep) {
									System.out.println("All SQL requests processed.");
									return;
								}
							}
						}
					}					
				}
			}
			long now = System.currentTimeMillis();
			if (now-last_status > 15000) {
				int nb_tables;
				synchronized (sql_to_process) {
					nb_tables = sql_to_process.size();
					nb_requests = 0;
					for (SQLToProcess t : sql_to_process)
						nb_requests += t.queries.size();
				}
				String s = "    status is: "+nb_requests+" queries pending in "+nb_tables+" batches, "+busy_tables.size()+" tables are busy. Threads: ";
				// 0=stop, 1=look for requests, 2=pause, 3=processing, 4=stopping
				int[] status = new int[] { 0,0,0,0,0 };
				for (int i = 0; i < sql_thread.length; ++i)
					status[sql_thread[i].status]++;
				if (status[0] > 0)
					s += status[0]+" stopped ";
				if (status[1] > 0)
					s += status[1]+" looking for requests ";
				if (status[2] > 0)
					s += status[2]+" paused ";
				if (status[3] > 0)
					s += status[3]+" processing ";
				if (status[4] > 0)
					s += status[4]+" stopping ";
				System.out.println(s);
				last_status = now;
			}
			long sleep;
			if (sql_to_process.isEmpty())
				sleep = 1000;
			else {
				if (nb_requests <= 100)
					sleep = 1000;
				else if (nb_requests <= BATCH_NB_QUERIES)
					sleep = 5000;
				else if (nb_requests <= BATCH_NB_QUERIES*2)
					sleep = 10000;
				else if (nb_requests <= BATCH_NB_QUERIES*5)
					sleep = 30000;
				else
					sleep = 60000;
			}
			try { Thread.sleep(sleep); } catch (InterruptedException e) {}
		} while (true);
	}

	public static void end_threads() {
		wait_all_processed();
		for (int i = 0; i < sql_thread.length; ++i)
			sql_thread[i].end();
		sql_thread = null;
		for (int i = 0; i < call_thread.length; ++i)
			call_thread[i].end();
		call_thread = null;
	}

}
