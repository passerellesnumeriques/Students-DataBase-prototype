-- phpMyAdmin SQL Dump
-- version 3.5.2.2
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Jun 02, 2013 at 02:16 AM
-- Server version: 5.5.23
-- PHP Version: 5.3.9

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `students_test`
--

--
-- Dumping data for table `curriculum`
--

INSERT INTO `curriculum` (`id`, `name`) VALUES
(1, 'CCT');

--
-- Dumping data for table `curriculumbranch`
--

INSERT INTO `curriculumbranch` (`id`, `name`, `curriculum`, `previous`) VALUES
(1, 'Common', 1, 0),
(2, 'SNA', 1, 1),
(3, 'SD', 1, 1);

--
-- Dumping data for table `curriculumclasstype`
--

INSERT INTO `curriculumclasstype` (`id`, `curriculum`, `name`) VALUES
(1, 1, 'Lecture'),
(2, 1, 'Laboratory');

--
-- Dumping data for table `curriculumperiod`
--

INSERT INTO `curriculumperiod` (`id`, `name`, `weeks`, `curriculum`, `branch`, `next`, `previous`) VALUES
(1, 'd', NULL, 1, 1, 0, 0),
(2, 'Semester 2', NULL, 1, 2, 3, 0),
(3, 'Summer Break', NULL, 1, 2, 4, 2),
(4, 'Semester 3', NULL, 1, 2, 0, 3),
(5, 'Semester 2', NULL, 1, 3, 6, 0),
(6, 'Summer Break', NULL, 1, 3, 7, 5),
(7, 'Semester 3', NULL, 1, 3, 0, 6);

--
-- Dumping data for table `curriculumsubject`
--

INSERT INTO `curriculumsubject` (`id`, `category`, `code`, `name`) VALUES
(1, 1, 'SNA 112', 'Hardware'),
(2, 1, 'SNA 111', 'Basic Programming'),
(3, 1, 'SNA 114', 'CCNA 1'),
(4, 1, 'SNA 124', 'CCNA 2'),
(5, 2, 'ENGL 1PN', 'English'),
(6, 2, 'ENGL 2PN', 'English'),
(7, 2, 'ENGL 3PN', 'English'),
(8, 2, 'ENGL 4PN', 'English');

--
-- Dumping data for table `curriculumsubjectcategory`
--

INSERT INTO `curriculumsubjectcategory` (`id`, `curriculum`, `name`) VALUES
(1, 1, 'IT'),
(2, 1, 'General');

--
-- Dumping data for table `periodsubjects`
--

INSERT INTO `periodsubjects` (`period`, `subject`, `class_type`, `hours`) VALUES
(1, 2, 1, 54),
(1, 2, 2, 54),
(1, 1, 1, 54),
(1, 1, 2, 54),
(1, 3, 1, 54),
(1, 3, 2, 54),
(2, 4, 1, 54),
(2, 4, 2, 54),
(1, 5, 1, 54),
(1, 5, 2, 0),
(2, 6, 1, 54),
(2, 6, 2, 0),
(5, 6, 1, 54),
(5, 6, 2, 0),
(4, 7, 1, 54),
(4, 7, 2, 0),
(7, 7, 1, 54),
(7, 7, 2, 0),
(4, 8, 1, 54),
(4, 8, 2, 0),
(7, 8, 1, 54),
(7, 8, 2, 0);

--
-- Dumping data for table `subjecttotalhours`
--

INSERT INTO `subjecttotalhours` (`subject`, `class_type`, `total_hours`) VALUES
(2, 1, 54),
(2, 2, 54),
(1, 1, 54),
(1, 2, 54),
(3, 1, 54),
(3, 2, 54),
(4, 1, 54),
(4, 2, 54),
(5, 1, 54),
(5, 2, 0),
(6, 1, 54),
(6, 2, 0),
(8, 1, 54),
(8, 2, 0),
(7, 1, 54),
(7, 2, 0);

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
