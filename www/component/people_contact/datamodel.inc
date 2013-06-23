<?php
/* @var $model DataModel */
$model->addTable("PeopleContact")
	->addForeignKey("people", "People")
	->addForeignKey("contact", "Contact")
	;
$model->addTable("PeopleAddress")
	->addForeignKey("people", "People")
	->addForeignKey("address", "PostalAddress")
	;
?>