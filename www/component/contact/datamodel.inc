<?php
/* @var $model DataModel */
$model->addTable("Contact")
	->addPrimaryKey("id")
	->addEnum("type", array("EMail","Phone","IM"), true, false)
	->addString("sub_type", 200, 0, true, false)
	->addString("contact",  2000, 0, false, false)
	->allowReadAccess()
	;
$model->addTable("PostalAddress")
	->addPrimaryKey("id")
	->addString("country",2,2,false,false)
	->addInteger("adm1", 32, null, null, true, false)
	->addInteger("adm2", 32, null, null, true, false)
	->addInteger("adm3", 32, null, null, true, false)
	->addInteger("adm4", 32, null, null, true, false)
	->addInteger("adm5", 32, null, null, true, false)
	->addString("street", 1000, 0, true, false)
	->addString("street_number", 50, 0, true, false)
	->addString("building", 100, 0, true, false)
	->addString("unit", 100, 0, true, false)
	->addString("name", 200, 0, true, false)
	->addString("address_type", 200, 0, true, false)
	->addString("additional",2000,0,true,false)
	->allowReadAccess()
	;
$model->addTable("ContactEntityType")
	->addPrimaryKey("id")
	->addString("name", 200, 1, false, true)
	->allowReadAccess()
	;
$model->addTable("ContactEntity")
	->addPrimaryKey("id")
	->addString("name", 1000, 1, false, false)
	->allowReadAccess()
	;
$model->addTable("ContactEntityTypes")
	->addForeignKey("entity", "ContactEntity")
	->addForeignKey("type", "ContactEntityType")
	->allowReadAccess()
	;
$model->addTable("ContactEntityAddress")
	->addForeignKey("entity", "ContactEntity")
	->addForeignKey("address", "PostalAddress")
	->allowReadAccess()
	;
$model->addTable("EntityContacts")
	->addForeignKey("entity", "ContactEntity")
	->addForeignKey("contact", "Contact")
	->allowReadAccess()
	;
?>