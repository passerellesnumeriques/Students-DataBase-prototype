package database_builder;


public class GeographyDataBaseBuilder {

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
