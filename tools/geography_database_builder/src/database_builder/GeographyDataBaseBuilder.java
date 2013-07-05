package database_builder;

import java.sql.Statement;
import java.sql.Connection;

import database_builder.gadm.GADM;
import database_builder.geonames.GenerateGeography;


public class GeographyDataBaseBuilder {

	public static class GeoNamesBuilder {
		public static void main(String[] args) {
			GenerateGeography.main(new String[0]);
			try {
				Connection db = DataBase.connect_db();
				Statement s = db.createStatement();
				s.execute("RENAME DATABASE `geography` TO `geonames`");
				s.close();
			} catch (Throwable t) {
				t.printStackTrace();
			}
		}
	}
	
	public static class GADMBuilder {
		public static void main(String[] args) {
			try {
				Connection db = DataBase.connect_db();
				Statement s = db.createStatement();
				s.execute("CREATE DATABASE `geography` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci");
				s.close();
			} catch (Throwable t) {
				t.printStackTrace();
			}
			GADM.main(new String[0]);
			try {
				Connection db = DataBase.connect_db();
				Statement s = db.createStatement();
				s.execute("RENAME DATABASE `geography` TO `gadm`");
				s.execute("CREATE DATABASE `geography` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci");
				s.execute("CREATE TABLE `geography`.`name` (`id` int(32) NOT NULL AUTO_INCREMENT,`name` varchar(200) NOT NULL,PRIMARY KEY (`id`)) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 AUTO_INCREMENT=1");
				s.execute("CREATE TABLE `geography`.`country` (`code` varchar(2) NOT NULL,`sovereign` varchar(2),`latitude` float NOT NULL,`longitude` float NOT NULL,`timezone` varchar(40) NOT NULL,PRIMARY KEY (`code`)) ENGINE=InnoDB DEFAULT CHARSET=latin1");
				s.execute("CREATE TABLE `geography`.`country_name` (`code` varchar(2) NOT NULL,`lang` varchar(2) NOT NULL,`name` int(32) NOT NULL,UNIQUE KEY `code_lang` (`code`,`lang`)) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
				s.execute("CREATE TABLE `geography`.`adm` (`id` int(32) NOT NULL AUTO_INCREMENT,`parent_id` int(32) DEFAULT NULL,`country` varchar(2) NOT NULL,`latitide` float NOT NULL, `longitude` float NOT NULL,PRIMARY KEY (`id`)) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1");
				s.execute("CREATE TABLE `geography`.`adm_name` (`id` int(32) NOT NULL,`lang` varchar(2) NOT NULL,`name` int(32) NOT NULL,UNIQUE KEY `id_lang` (`id`,`lang`)) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
				s.execute("CREATE TABLE `geography`.`country_division` (`country` varchar(2) NOT NULL,`level` int(3),`name` varchar(200) NOT NULL,`english_name` varchar(200)) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
				s.execute("CREATE INDEX `country` ON `geography`.`country_division` (`country`)");
				s.close();
			} catch (Throwable t) {
				t.printStackTrace();
			}
		}
	}
	
	public static class Final {
		public static void main(String[] args) {
			try {
				Connection db = DataBase.connect_db();
				Statement s = db.createStatement();
				s.execute("CREATE DATABASE `geography` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci");
				s.execute("CREATE TABLE `geography`.`name` (`id` int(32) NOT NULL AUTO_INCREMENT,`name` varchar(200) NOT NULL,PRIMARY KEY (`id`)) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 AUTO_INCREMENT=1");
				s.execute("CREATE TABLE `geography`.`country` (`code` varchar(2) NOT NULL,`sovereign` varchar(2),`latitude` float NOT NULL,`longitude` float NOT NULL,`timezone` varchar(40),PRIMARY KEY (`code`)) ENGINE=InnoDB DEFAULT CHARSET=latin1");
				s.execute("CREATE TABLE `geography`.`country_name` (`code` varchar(2) NOT NULL,`lang` varchar(2) NOT NULL,`name` int(32) NOT NULL,UNIQUE KEY `code_lang` (`code`,`lang`)) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
				s.execute("CREATE TABLE `geography`.`adm` (`id` int(32) NOT NULL AUTO_INCREMENT,`parent_id` int(32) DEFAULT NULL,`country` varchar(2) NOT NULL,`latitide` float NOT NULL, `longitude` float NOT NULL,PRIMARY KEY (`id`)) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1");
				s.execute("CREATE TABLE `geography`.`adm_name` (`id` int(32) NOT NULL,`lang` varchar(2) NOT NULL,`name` int(32) NOT NULL,UNIQUE KEY `id_lang` (`id`,`lang`)) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
				s.execute("CREATE TABLE `geography`.`country_division` (`country` varchar(2) NOT NULL,`level` int(3),`name` varchar(200) NOT NULL,`english_name` varchar(200)) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4");
				s.execute("CREATE INDEX `country` ON `geography`.`country_division` (`country`)");
				s.close();
			} catch (Throwable t) {
				t.printStackTrace();
			}
		}
	}
	
	
	/*
	 * keep simple:
	 * use only gadm:
	 *  a) build countries
	 *  b) build divisions from gadm, with default name only
	 *  => search on a country
	 *    if last division: ok
	 *    else: propose sub divisions...
	 * 
	 * 
	 * 
	 * 
	 * steps to build final database
	 * 1- build temporary db from geonames
	 * 2- build from gadm: consolidation between geonames and gadm
	 * 
	 * 
	 * 
	 * 2- keep the countries
	 * 3- take gadm: build administrative areas, by type (province, city) and country info
	 * 4- for each, look in geonames :
	 *   + find all names matching, in the country
	 *     + find the good one
	 *       - if its parent's names are matching also
	 *   Take:
	 *   - alternate names
	 *   - latitude/longitude
	 *   - schools...
	 * 
	 * 
	 * country:
	 *   - code
	 *   - latitude
	 *   - longitude
	 *   - timezone
	 *   - max_level (0 to 5)
	 * 
	 * 
	 */
	
}
