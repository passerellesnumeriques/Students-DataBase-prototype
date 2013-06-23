package database_builder.geonames;

import java.io.UnsupportedEncodingException;
import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.LinkedList;
import java.util.List;

import database_builder.DataBase;


public class SQLProcessor extends Thread {

	static void query(String query) {
		do {
			synchronized (to_process_synch) {
				if (to_process_query.size() < MAX_PENDING_QUERY) {
					to_process_query.add(query);
					to_process_synch.notify();
					break;
				}
			}
			synchronized (too_much_synch_query) {
				long start = System.nanoTime();
				try { too_much_synch_query.wait(); } catch (InterruptedException e) { return; }
				waited_pending += System.nanoTime()-start;
			}
		} while (true);
	}
	static void conditional_query(String query, String condition, boolean has_result) {
		do {
			synchronized (to_process_synch) {
				if (to_process_conditional.size() < MAX_PENDING_CONDITIONAL) {
					to_process_conditional.add(new Object[] { query, condition, has_result });
					to_process_synch.notify();
					break;
				}
			}
			synchronized (too_much_synch_conditional) {
				long start = System.nanoTime();
				try { too_much_synch_conditional.wait(); } catch (InterruptedException e) { return; }
				waited_pending += System.nanoTime()-start;
			}
		} while (true);
	}
	static void insert(String table, String fields, List<String[]> values) {
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
		query(sql.toString());
	}
	private static Object too_much_synch_query = new Object();
	private static Object too_much_synch_conditional = new Object();
	private static final int MAX_PENDING_QUERY = 200000;
	private static final int PENDING_QUERY_RESTART = 180000;
	private static final int MAX_PENDING_CONDITIONAL = 100000;
	private static final int PENDING_CONDITIONAL_RESTART = 90000;
	private static final int MAX_BATCH_QUERIES = 1000;
	private static final int MAX_CONDITIONALS_DONE_ONCE = 250;
	static long waited_pending = 0;
	
	static boolean stop = false;
	static LinkedList<String> to_process_query = new LinkedList<String>();
	static LinkedList<Object> to_process_conditional = new LinkedList<Object>();
	static Object to_process_synch = new Object();
	
	public SQLProcessor(int num) {
		this.num = num;
	}
	private int num;
	private boolean stopped = false;
	private Object stop_synch = new Object();
	
	@Override
	public void run() {
		System.out.println("SQL Processor "+(num+1)+": connecting to database...");
		Connection db;
		try {
			db = DataBase.connect_db();
		} catch (SQLException e) {
			e.printStackTrace(System.err);
			return;
		}
		System.out.println("SQL Processor "+(num+1)+": connected, start processing requests");
		long nb = 0;
		long waited = 0;
		long time = 0;
		long start;
		Object[] queries = new Object[MAX_CONDITIONALS_DONE_ONCE];
		int nb_queries;
		Statement s = null;
		boolean ok;
		String request;
		String condition;
		Boolean has_result;
		int nb_batch;
		int i;
		do {
			nb_batch = 0;
			nb_queries = 0;
			//StringBuilder batch = new StringBuilder();
			try {
				synchronized (to_process_synch) {
					// prioritize single queries that can be done in a single batch, except if just few or not much conditional
					if (to_process_query.size() > MAX_BATCH_QUERIES/10 || to_process_conditional.size() < MAX_PENDING_CONDITIONAL/10)
						for (i = 0; i < MAX_BATCH_QUERIES && !to_process_query.isEmpty(); ++i) {
							nb_batch++;
							//String ss = to_process_query.removeFirst();
							//s.addBatch(ss);
							//batch.append(ss).append("\r\n");
							if (nb_batch == 1) {
								s = db.createStatement();
								s.addBatch("START TRANSACTION");
							}
							s.addBatch(to_process_query.removeFirst());
						}
					if (nb_batch == 0) {
						if (to_process_conditional.isEmpty()) {
							if (stop) break;
							start = System.nanoTime();
							try { to_process_synch.wait(); } catch (InterruptedException ex) { break; }
							waited += System.nanoTime()-start;
							continue;
						}
						for (nb_queries = 0; nb_queries < MAX_CONDITIONALS_DONE_ONCE && !to_process_conditional.isEmpty(); ++nb_queries)
							queries[nb_queries] = to_process_conditional.removeFirst();
					}
				}
				if (nb_batch > 0) {
					if (to_process_query.size() < PENDING_QUERY_RESTART)
						synchronized (too_much_synch_query) { too_much_synch_query.notifyAll(); }
					s.addBatch("COMMIT");
					nb += nb_batch;
					start = System.nanoTime();
					try { s.executeBatch(); }
					catch (SQLException ex) {
						while (ex != null) {
							System.err.println("SQL Query Errors:");
							ex.printStackTrace(System.err);
							ex = ex.getNextException();
						}
					}
					s.close();
					time += System.nanoTime()-start;
					continue;
				}
			} catch (SQLException ex) {
				System.err.println("SQL Error:");
				//System.err.println(batch.toString());
				ex.printStackTrace(System.err);
				continue;
			}
			if (to_process_conditional.size() < PENDING_CONDITIONAL_RESTART)
				synchronized (too_much_synch_conditional) { too_much_synch_conditional.notifyAll(); }
			
			for (i = 0; i < nb_queries; ++i) {
				try {
					nb++;
					condition = (String)((Object[])queries[i])[1];
					has_result = (Boolean)((Object[])queries[i])[2];
					start = System.nanoTime();
					s = db.createStatement();
					ok = s.execute(condition);
					s.close();
					time += System.nanoTime()-start;
					if (has_result.equals(ok)) {
						// put the request in the list, so it can be processed in a batch
						request = (String)((Object[])queries[i])[0];
						synchronized (to_process_synch) {
							to_process_query.add(request);
						}
					}
				} catch (SQLException ex) {
					System.err.println("SQL Error:");
					ex.printStackTrace(System.err);
				}
			}
		} while (true);
		System.out.println("SQL Processor "+(num+1)+": "+nb+" queries processed, waited "+(waited/1000000000)+"s.");
		System.out.println("SQL Processor "+(num+1)+": average time of request = "+(time/nb)+"ns.");
		synchronized (stop_synch) { stopped = true; stop_synch.notifyAll(); }
		try {
			db.close();
		} catch (SQLException ex) {
			ex.printStackTrace(System.err);
		}
	}
	public void wait_end() {
		synchronized (stop_synch) {
			if (stopped) return;
			try { stop_synch.wait(); } catch (InterruptedException e) { return; }
		}
	}

	
}
