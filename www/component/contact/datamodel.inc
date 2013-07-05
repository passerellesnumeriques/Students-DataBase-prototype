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
	->addInteger("adm", 32, null, null, true, false)
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
$model->addTable("ContactNote")
	->addForeignKey("entity", "ContactEntity")
	->addDate("date")
	->addString("note", 5000)
	;
$model->addTable("ContactGroup")
	->addPrimaryKey("id")
	->addString("name", 200, 1)
	;
$model->addTable("ContactGroupEntities")
	->addForeignKey("group", "ContactGroup")
	->addForeignKey("entity", "ContactEntity")
	;
$model->addTable("ContactGroupPeople")
	->addForeignKey("group", "ContactGroup")
	->addForeignKey("people", "People")
	;
$model->addDataCategoryLink("Contact", "/dynamic/people/page/profile?plugin=contact&people=%People.id%");
$model->addTable("PeopleContact")
	->addForeignKey("people", "People")
	->addForeignKey("contact", "Contact")
	->restrictReadAccess("consult_people_contacts",true)
	->allowSpecificReadAccess("id",function() { return PNApplication::$instance->uer_people->user_people_id; })
	->restrictWriteAccess("edit_people_contacts",true);
	;
$model->addTable("PeopleAddress")
	->addForeignKey("people", "People")
	->addForeignKey("address", "PostalAddress")
	;
$model->addTable("ContactPoint")
	->addForeignKey("entity", "ContactEntity")
	->addForeignKey("people", "People")
	->addString("designation", 200)
	;
?>