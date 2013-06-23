package database_builder.geonames;


import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;

import sql.SQLProcessor;

import net.lecousin.framework.io.BufferedInputStream_Threaded;
import database_builder.DataBase;
import database_builder.geonames.Entities.Entity;

public class GenerateGeography {

	public static void main(String[] args) {
		try {
			System.out.println("Initialization...");
			dir = new File("D:\\Code\\content\\geography\\geonames.org");
			System.out.println("Reset Database...");
			reset_db();
			System.out.println("Processing data...");
			long start = System.currentTimeMillis();
			parseAllCountries();
			System.out.println("Process done in "+((System.currentTimeMillis()-start)/1000)+"s.");
		} catch (Throwable t) {
			t.printStackTrace(System.err);
		}
	}
	
	static File dir;
	
	private static void reset_db() throws SQLException {
		System.out.println(" + Connecting to Database...");
		Connection db = DataBase.connect_db();
		System.out.println(" + Create database and tables...");

		Statement s;
		// initialize database
		s = db.createStatement();
		s.execute("DROP DATABASE IF EXISTS `geography`");
		s.close();
		s = db.createStatement();
		s.execute("CREATE DATABASE `geography` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci");
		s.close();
		
		// countries
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`country_temp` (`code` varchar(2) NOT NULL,`latitude` float NOT NULL,`longitude` float NOT NULL,`timezone` varchar(40) NOT NULL,PRIMARY KEY (`code`)) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`country_temp_names` (`code` varchar(2) NOT NULL,`lang` varchar(2) NOT NULL,`name` varchar(200) NOT NULL,UNIQUE KEY `code_lang` (`code`,`lang`)) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
		s.close();
		
		// temporary tables for administrative area and cities 
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`adm1_temp` (`country` varchar(2) NOT NULL,`id` INT(32) NOT NULL,`adm1` varchar(20),`latitude` float NOT NULL,`longitude` float NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`adm1_temp_names` (`country` varchar(2) NOT NULL,`id` INT(32) NOT NULL,`lang` varchar(2) NOT NULL,`name` varchar(200) NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`adm2_temp` (`country` varchar(2) NOT NULL,`id` INT(32) NOT NULL,`adm1` varchar(20),`adm2` varchar(80),`latitude` float NOT NULL,`longitude` float NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`adm2_temp_names` (`country` varchar(2) NOT NULL,`id` INT(32) NOT NULL,`lang` varchar(2) NOT NULL,`name` varchar(200) NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`adm3_temp` (`country` varchar(2) NOT NULL,`id` INT(32) NOT NULL,`adm1` varchar(20),`adm2` varchar(80),`adm3` varchar(20),`latitude` float NOT NULL,`longitude` float NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`adm3_temp_names` (`country` varchar(2) NOT NULL,`id` INT(32) NOT NULL,`lang` varchar(2) NOT NULL,`name` varchar(200) NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`adm4_temp` (`country` varchar(2) NOT NULL,`id` INT(32) NOT NULL,`adm1` varchar(20),`adm2` varchar(80),`adm3` varchar(20),`adm4` varchar(20),`latitude` float NOT NULL,`longitude` float NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`adm4_temp_names` (`country` varchar(2) NOT NULL,`id` INT(32) NOT NULL,`lang` varchar(2) NOT NULL,`name` varchar(200) NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`ppl_temp` (`country` varchar(2) NOT NULL,`id` INT(32) NOT NULL,`adm1` varchar(20),`adm2` varchar(80),`adm3` varchar(20),`adm4` varchar(20),`latitude` float NOT NULL,`longitude` float NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
		s.close();
		s = db.createStatement();
		s.execute("CREATE TABLE `geography`.`ppl_temp_names` (`country` varchar(2) NOT NULL,`id` INT(32) NOT NULL,`lang` varchar(2) NOT NULL,`name` varchar(200) NOT NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
		s.close();
		db.close();
	}
	
	static AlternateNamesProcessor an_processor;
	
	
	private static void parseAllCountries() throws IOException {
		// start processors
		System.out.println("Starting SQL processors");
		SQLProcessor.start_threads();
		System.out.println("Starting alternate names processor");
		an_processor = new AlternateNamesProcessor();
		an_processor.start();
		System.out.println("Starting entity processors");
		Entities.startProcessors();
		
		System.out.println("Opening file");
		// open file
		File file = new File(dir, "allCountries.txt");
		long size = file.length();
		FileInputStream fin = new FileInputStream(file);
		fin.skip(2); // skip the unicode marker
		BufferedInputStream_Threaded bin = new BufferedInputStream_Threaded(fin, 8192, 8192); // = 64M
		//InputStreamReader f = new InputStreamReader(bin, "UTF-16LE");
//		System.out.println("Waiting processors to be ready");
//		boolean ready;
//		do {
//			ready = true;
//			synchronized (Entities.entities) {
//				for (int i = 0; i < processors.length; ++i)
//					if (!processors[i].ready) {
//						try { Entities.entities.wait(); } catch (InterruptedException ex) { return; }
//						ready = false;
//						break;
//					}
//			}
//		} while (!ready);
		
//		System.out.println("Wait alternate names process to move on...");
//		while (AlternateNamesProcessor.lines < 1000000)
//			try { Thread.sleep(1000); } catch (InterruptedException ex) {}
		System.out.println("Parsing allCountries...");
		// go through each line
		long pos = 2;
		int field = 0;
		long line = 0;
		long start = System.currentTimeMillis();
		Entity e = Entities.get_entity();
		do {
			//int i = f.read();
			int i = bin.readUTF16LE();
			pos+=2;
			if (i == -1 || i == 9 || i == 10) {
				field++;
				if (i == -1 || i == 10) {
					// end of line
					if (field == 19) {
						Entities.to_process(e);
						e = Entities.get_entity();
					} else {
						// e will be reused
						e.init_buf();
					}
					if (i == -1) break; // end of file
					field = 0;
					line++;
					if ((line % 500000) == 0) {
						long now = System.currentTimeMillis();
						System.out.println("Lines processed: "+line+" ("+(pos*100/size)+"%) in "+((now-start)/1000)+"s. ("+(((now-start)/1000)*(size-pos)/pos)+"s. remaining)");
						long free = Runtime.getRuntime().freeMemory();
						long total = Runtime.getRuntime().totalMemory();
						long max = Runtime.getRuntime().maxMemory();
						System.out.println("Memory: max="+max+", allocated="+total+", free="+free+"; remaining = "+(max-total+free)+" ("+((max-total+free)*100/max)+"%)");
						//System.out.println("Processes: allCountries waited "+(Entities.waited_for_entities/1000000000)+"s. for entities, SQL blocked processes for "+(SQLProcessor.waited_pending/1000000000)+"s.");
						//if (line == 2000000) break;
					}
				}
			} else if (field < 19) {
				try {
					if (e.buf[field] != null)
						e.buf[field][e.buf_pos[field]++] = (char)i;
				} catch (ArrayIndexOutOfBoundsException ex) {
					System.err.println("Field "+field+" too long in line "+line+" (exceed "+e.buf[field].length+" characters)");
				}
			} else {
				System.err.println("More than 19 fields on line "+line+": "+field);
			}
		} while (true);
		long now = System.currentTimeMillis();
		System.out.println("Lines processed: "+line+" in "+((now-start)/1000)+"s., waited "+(Entities.waited_for_entities/1000000000)+"s. for available entities");
		//f.close();
		bin.close();
		System.out.println("allCountries reader waited "+(bin.getWaitedNanosecondsForDisk()/1000000)+"ms. for the disk");
		
		for (int i = 0; i < Entities.processors.length; ++i)
			Entities.processors[i].stop = true;
		int stopped;
		do {
			stopped = 0;
			for (int i = 0; i < Entities.processors.length; ++i)
				if (Entities.processors[i].stopped)
					stopped++;
				else {
					synchronized (Entities.entities_to_process) {
						Entities.entities_to_process.notifyAll();
					}
					try { Thread.sleep(100); } catch (InterruptedException ex) { break; }
				}
		} while (stopped < Entities.processors.length);
		AlternateNamesProcessor.stop = true;
		synchronized (AlternateNamesProcessor.to_process) {
			AlternateNamesProcessor.to_process.notifyAll();
		}
		while (!AlternateNamesProcessor.stopped)
			try { Thread.sleep(500); } catch (InterruptedException ex) { break; }
		SQLProcessor.end_threads();
	}
	
}
