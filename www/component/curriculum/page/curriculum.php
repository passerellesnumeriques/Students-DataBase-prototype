<?php
$this->add_stylesheet("/static/curriculum/curriculum.css");
$this->add_javascript("/static/common/js/drawing.js");
$this->add_javascript("/static/common/js/popup_window/popup_window.js");
$this->add_stylesheet("/static/common/js/popup_window/popup_window.css");
$this->add_stylesheet("/static/common/js/splitter_vertical/splitter_vertical.css");
$this->add_javascript("/static/common/js/splitter_vertical/splitter_vertical.js");

global $edit, $id;
$id = $_GET["id"];
$edit = isset($_GET["edit"]) && $_GET["edit"] == "1";

// check user rights
$can_edit = PNApplication::$instance->user_management->has_right("edit_curricula");
$edit &= $can_edit;

// lock the curriculum if in edit mode
require_once("common/DataBaseLock.inc");
$locked_by = null;
if ($edit)
	$lock_id = DataBaseLock::lock("Curriculum", array("id"=>$id), $locked_by);

// get the name of the curriculum
require_once("common/SQLQuery.inc");
$name = SQLQuery::create()->select("Curriculum")->field('name')->where("id",$id)->execute_single_value();

// create page header with title and tool bar
require_once("component/application/SubPageHeader.inc");
$header = new SubPageHeader($this,'/static/curriculum/curriculum_32.png', get_locale("Curriculum").": <span style='font-family:Courrier New;font-weight:bold;font-style:italic'>".$name."</span>");
if ($locked_by <> null) {
	$header->add_header("<img src='/static/common/images/lock.png'/> ".get_locale("common","This page is already locked by")." ".$locked_by);
	$header->generate();
	return;
}
if (!$edit && $can_edit)
	$header->add_header("<div class='button' onclick=\"var u=new window.URL(location.href);u.params['edit']=1;location.href=u.toString();\"><img src='/static/common/images/edit.png'/> ".get_locale("common", "Edit")."</div>");
else if ($edit)
	$header->add_header("<div class='button' onclick=\"var u=new window.URL(location.href);u.params['edit']=0;location.href=u.toString();\"><img src='/static/common/images/no_edit.png'/> ".get_locale("common", "Stop Edit")."</div>");

if ($can_edit) {
	$header->add_header("<div class='button' onclick=\"curriculum_edit_class_types();\"><img src='/static/curriculum/class_type_16.png'/> ".get_locale("Edit Class Types")."</div>");
}

// retrieve class types
global $class_types;
$class_types = SQLQuery::create()->select("CurriculumClassType")->where("curriculum",$id)->execute();

// retrieve subjects categories
global $subjects_categories;
$subjects_categories = SQLQuery::create()->select("CurriculumSubjectCategory")->where("curriculum",$id)->order_by("name")->execute();
foreach ($subjects_categories as &$cat) {
	$cat["subjects"] = SQLQuery::create()->select("CurriculumSubject")->where("category",$cat["id"])->order_by("code")->execute();
	if ($cat["subjects"] == false) $cat["subjects"] = array();
	foreach ($cat["subjects"] as &$subject) {
		$subject["hours"] = array();
		foreach ($class_types as $ct) {
			$hours = SQLQuery::create()->select("SubjectTotalHours")->where("subject",$subject["id"])->where("class_type",$ct["id"])->field("total_hours")->execute_single_value();
			$subject["hours"][$ct["id"]] = $hours;
		}
	}
}

function &get_subject($id) {
	global $subjects_categories;
	foreach ($subjects_categories as &$cat)
		foreach ($cat["subjects"] as &$s)
			if ($s["id"] == $id)
				return $s;
}

// retrieve all branches, and periods of each branch
$list = SQLQuery::create()->select("CurriculumBranch")->where("curriculum",$id)->execute();
foreach ($list as &$branch) {
	$periods = SQLQuery::create()->select("CurriculumPeriod")->where("branch",$branch["id"])->execute();
	$branch["periods"] = array();
	$previous = 0;
	while (count($periods) > 0) {
		for ($i = 0; $i < count($periods); $i++) {
			if ($periods[$i]["previous"] == $previous || ($previous == 0 && $periods[$i]["previous"] == null)) {
				array_push($branch["periods"], $periods[$i]);
				$previous = $periods[$i]["id"];
				array_splice($periods, $i, 1);
				break;
			}
		}
	}
	// get subjects for each period
	foreach ($branch["periods"] as &$period) {
		$period["subjects"] = array();
		$hours = SQLQuery::create()->select("PeriodSubjects")->where("period",$period["id"])->execute();
		foreach ($hours as $h) {
			if (!isset($period["subjects"][$h["subject"]]))
				$period["subjects"][$h["subject"]] = array();
			$period["subjects"][$h["subject"]][$h["class_type"]] = $h["hours"];
		}
	}
}
// convert the flat list of branches from database, into a tree of branches
$branches = array();
function add_branch(&$branches, &$branch) {
	$found = false;
	foreach ($branches as &$b) {
		if ($b["id"] == $branch["previous"]) {
			array_push($b["sub_branches"], $branch);
			$found = true;
			break;
		}
	}
	if (!$found) {
		foreach ($branches as &$b) {
			if (count($b["sub_branches"]) > 0)
				if (add_branch($b["sub_branches"], $branch)) {
					$found = true;
					break;
				}
		}
	}
	return $found;
}
while (count($list) > 0) {
	$rem = array();
	foreach ($list as &$branch) {
		$branch["sub_branches"] = array();
		if ($branch["previous"] == null || $branch["previous"] == 0)
			array_push($branches, $branch);
		else
			if (!add_branch($branches, $branch))
				array_push($rem, $branch);
	}
	$list = $rem;
};

if (count($branches) == 0 && $edit)
	$header->add_header("<div class='button' onclick=\"curriculum_add_branch(0);\"><img src='/static/curriculum/branch_16.png'/> ".get_locale("Create root branch")."</div>");

$header->add_header("<div class='button' onclick=\"curriculum_toggle_subjects_list(this);\" title=\"".get_locale("Hide subjects")."\"><img src='/static/common/images/layout_remove_right_column_24.png'/></div>");

$header->generate();
//$header->start_scrollable_content();
$header->start_fill_content();

// split the screen into 2: the curriculum and the subjects list
echo "<div id='curriculum_subjects_splitter' style='width:100%;height:100%'>";
echo "<div style='overflow:auto;position:relative'>";

global $connectors;
$connectors = array();

/** generate the table corresponding to a period, then generate for next period... */
function generate_period($branch, $previous) {
	global $edit, $class_types;
	$period = null;
	foreach ($branch["periods"] as &$p) if ($p["previous"] == $previous) { $period = $p; break; }
	if ($period == null) return; // should never happen
	if ($previous <> 0) {
		// add a connection with the previous
		echo "<td width='15px' valign=middle>";
		echo "<div style='width:15px;height:1px;background-color:black'></div>";
		echo "</td>";
	}
	if ($edit) {
		echo "<td valign=middle width='10px'>";
		echo "<img src='/static/common/images/add_10.gif' style='cursor:pointer' onclick=\"curriculum_add_period(".$branch["id"].",".$previous.");\" title=\"".get_locale("Add a period")."\"/>";
		echo "</td>";
	}
	echo "<td valign=middle>";
	echo "<table class='curriculum_period' name='".$period["id"]."'><tbody>";
	$nb_types = count($class_types);
	echo "<tr><th colspan=".($nb_types==0?3:2+$nb_types)." class='curriculum_period_title'>".$period["name"]."</th></tr>";
	echo "<tr>";
		echo "<th colspan=2>".get_locale("Subject")."</th>";
		echo "<th colspan=".($nb_types == 0 ? 1 : $nb_types)." rowspan=".($nb_types == 0 ? 2 : 1).">".get_locale("Hours")."</th>";
	echo "</tr>";
	echo "<tr>";
		echo "<th>".get_locale("Code")."</th><th>".get_locale("common","Name")."</th>";
		foreach ($class_types as $ct)
			echo "<th>".$ct["name"]."</th>";
	echo "</tr>";
	foreach ($period["subjects"] as $subject_id=>$subject_hours) {
		echo "<tr>";
		$subject = get_subject($subject_id);
		echo "<td nowrap='nowrap'>".$subject["code"]."</td>";
		echo "<td nowrap='nowrap'>".$subject["name"]."</td>";
		foreach ($class_types as $ct) {
			echo "<td align=right>";
			foreach ($subject_hours as $ct_id=>$nb_hours)
				if ($ct["id"] == $ct_id) { echo $nb_hours; break; }
			echo "</td>";
		}
		echo "</tr>";
	}
	echo "</tbody></table>";
	echo "</td>";
	if ($period["next"] <> 0)
		generate_period($branch, $period["id"]);
	else if ($edit) {
		echo "<td valign=middle width='10px'>";
		echo "<img src='/static/common/images/add_10.gif' style='cursor:pointer' onclick=\"curriculum_add_period(".$branch["id"].",".$period["id"].");\" title=\"".get_locale("Add a period")."\"/>";
		echo "</td>";
	}
}

/** generate the table containing a branch */
function generate_branch($branch) {
	global $edit;
	echo "<table class='curriculum_branch'>";
	echo "<tr><th class='curriculum_branch_title'>";
	if ($edit)
		echo "<span style='cursor:pointer' onclick=\"curriculum_rename_branch(".$branch["id"].",'".$branch["name"]."');\">";
	echo $branch["name"];
	if ($edit)
		echo "</span>";
	echo "</th>";
	$cols = 1;
	if ($edit) {
		if (count($branch["sub_branches"]) == 0) {
			echo "<td width='18px'><div class='button' onclick=\"curriculum_remove_branch(".$branch["id"].",'".$branch["name"]."');\"><img src='/static/common/images/remove.png'/></div></td>";
			$cols++;
		}
		echo "<td width='18px'><div class='button' onclick=\"curriculum_add_branch(".$branch["id"].");\"><img src='/static/curriculum/branch_connector.png'/></div></td>";
		$cols++;
	}
	echo "</tr>";
	echo "<tr><td colspan=".$cols.">";
	if (count($branch["periods"]) > 0) {
		echo "<table class='curriculum_periods_container'><tobdy><tr>";
		generate_period($branch, 0);
		echo "</tr><tbody></table>";
	}
	if ($edit && count($branch["periods"])==0)
		echo "<div class='button' onclick=\"curriculum_add_period(".$branch["id"].",0);\"><img src='/static/curriculum/period.png'/>".get_locale("Create Period")."</div>";
	echo "</td></tr>";
	echo "</table>";
}

/** generate the global table, containing all the curriculum */
function generate_branch_table($branch) {
	global $connectors;
	echo "<table style='height:100%'><tr>";
	echo "<td rowspan=".(count($branch["sub_branches"]) > 1 ? count($branch["sub_branches"]) : 1)." id='branch_".$branch['id']."'>";
	generate_branch($branch);
	echo "</td>";
	if (count($branch["sub_branches"]) > 0) {
		echo "<td rowspan=".count($branch["sub_branches"])." width='30px'>";
		echo "</td>";
	}
	for ($i = 0; $i < count($branch["sub_branches"]); $i++) {
		if ($i > 0) { echo "</tr><tr>"; }
		echo "<td>";
		generate_branch_table($branch["sub_branches"][$i]);
		echo "</td>";
		array_push($connectors, array($branch["id"],$branch["sub_branches"][$i]["id"]));
	}
	echo "</tr></table>";
}
if (count($branches) > 0)
	generate_branch_table($branches[0]);

echo "</div>";


// the subjects list
?>
<div style='overflow:auto'>
<div class='curriculum_subjects_list_header'>
<img src='/static/curriculum/subjects_24.png'/>
<?php locale("Subjects")?>
</div>
<table rules='all' style='border:1px solid black'>
<tr>
	<th nowrap="nowrap"><?php locale("common","Name")?></th>
	<th nowrap="nowrap"><?php locale("Code")?></th>
	<?php foreach ($class_types as $ct) echo "<th nowrap=\"nowrap\">".$ct["name"]."</th>";?>
	<?php if ($edit) { ?><th nowrap="nowrap"></th><?php }?>
</tr>
<?php
$subjects_td_nb = 0;
foreach ($subjects_categories as &$cat) {
	echo "<tr>";
	echo "<td nowrap=\"nowrap\" colspan=".(count($class_types)+2)."><img src='/static/curriculum/subjects_16.png' style='vertical-align:bottom'/>";
	echo " <b>".$cat["name"]."</b>";
	echo "</td>";
	if ($edit) {
		echo "<td nowrap=\"nowrap\">";
		echo "<div class='button' onclick=\"curriculum_add_subject(".$cat["id"].")\" title=\"".get_locale("Add a subject")."\"><img src='/static/common/icon.php?main=component/curriculum/static/subject_16.png&small=add&where=right_bottom'/></div>";
		echo "<div class='button' onclick=\"curriculum_edit_subject_category(".$cat["id"].",'".$cat["name"]."')\" title=\"".get_locale("Edit category")."\"><img src='/static/common/images/edit.png'/></div>";
		echo "<div class='button' onclick=\"curriculum_remove_subject_category(".$cat["id"].",'".$cat["name"]."')\" title=\"".get_locale("Remove this category")."\"><img src='/static/common/images/remove.png' style='cursor:pointer'/></div>";
		echo "</td>";
	}
	echo "</tr>";
	foreach ($cat["subjects"] as &$subject) {
		echo "<tr>";
		echo "<td id='subject_td_".($subjects_td_nb++)."' name='".$subject["id"]."' nowrap=\"nowrap\" style='padding-left:10px;cursor:default'><img src='/static/curriculum/subject_16.png' style='vertical-align:bottom'/>".$subject["name"]."</td>";
		echo "<td nowrap=\"nowrap\">".$subject["code"]."</td>";
		foreach ($class_types as $ct) {
			$hours = $subject["hours"][$ct["id"]];
			if ($hours == null) $hours = 0;
			echo "<td align=right>".$hours."</td>";
		}
		if ($edit) {
			echo "<td nowrap=\"nowrap\">";
			echo "<div class='button' onclick=\"curriculum_edit_subject(".$subject["id"].",'".$subject["code"]."','".$subject["name"]."'";
			foreach ($class_types as $ct) {
				$hours = $subject["hours"][$ct["id"]];
				if ($hours == null) $hours = 0;
				echo ",".$hours;
			}
			echo ")\" title=\"".get_locale("Edit subject")."\"><img src='/static/common/images/edit.png'/></div>";
			echo "<div class='button' onclick=\"curriculum_remove_subject(".$subject["id"].")\" title=\"".get_locale("Remove this subject")."\"><img src='/static/common/images/remove.png' style='cursor:pointer'/></div>";
			echo "</td>";
		}
		echo "</tr>";
	}
}

if ($edit) {
?>
<tr><td colspan=<?php echo (count($class_types)+3);?>>
<div class='button' onclick='curriculum_add_subject_category()'>
	<img src='/static/common/images/add.png'/>
	<?php locale("Add new category")?>
</div>
</td></tr>
<?php }?>

</table>
</div>
<?php


// end of the screen
echo "</div>";

//$header->end_scrollable_content();
$header->end_fill_content();
$this->onload("new splitter_vertical('curriculum_subjects_splitter',location.hash.length > 1 ? location.hash.substr(1) : 0.75).positionChanged.add_listener(function(splitter){var u = new window.URL(location.href);u.hash=splitter.position;location.href=u.toString();});");


if ($edit) {
$this->add_javascript("/static/common/js/dragndrop.js");
?>
<script type='text/javascript'>
<?php
for ($i = 0; $i < $subjects_td_nb; $i++)
	echo "dnd.configure_drag_element(document.getElementById('subject_td_".$i."'),true,null,function(td){return 'curriculum_subject_'+td.attributes['name'].value;});";
?>
var periods = document.getElementsByClassName('curriculum_period');
for (var i = 0; i < periods.length; ++i)
	dnd.configure_drop_area(periods[i],function(data){
		if (data && data.substring(0,19) == 'curriculum_subject_') return common_images_url+"add.png";
		return null;
	},function(data,x,y,period){
		var screen_lock = lock_screen(null, "<img src='/static/common/images/loading.gif'/>");
		ajax.post_parse_result("/dynamic/curriculum/service/add_period_subject",{curriculum:<?php echo $id?>,period:period.attributes['name'].value,subject:data.substring(19)},function(result) {
			if (!result)
				unlock_screen(screen_lock);
			else
				location.reload();
		},true);
	});

function curriculum_add_branch(previous_id) {
	input_dialog('/static/curriculum/branch_16.png',"<?php locale("Create branch")?>","<?php locale("common","Name")?>","",100,function(name){
		if (name.length == 0)
			return "<?php locale("common","Cannot be empty")?>";
		return null;
	},function(name){
		if (name == null) return;
		var screen_lock = lock_screen(null, "<img src='/static/common/images/loading.gif'/>");
		ajax.post_parse_result("/dynamic/curriculum/service/create_branch",{curriculum:<?php echo $id?>,name:name,previous:previous_id},function(result) {
			if (!result || !result.id)
				unlock_screen(screen_lock);
			else
				location.reload();
		},true);
	});
}
function curriculum_rename_branch(id,current_name) {
	input_dialog('/static/curriculum/branch_16.png',"<?php locale("Rename Branch")?>","<?php locale("common","Name")?>",current_name,100,function(name){
		if (name.length == 0)
			return "<?php locale("common","Cannot be empty")?>";
		return null;
	},function(name){
		if (name == null) return;
		if (name == current_name) return;
		var screen_lock = lock_screen(null, "<img src='/static/common/images/loading.gif'/>");
		ajax.post_parse_result("/dynamic/curriculum/service/rename_branch",{id:id,name:name,curriculum:<?php echo $id?>},function(result) {
			if (!result)
				unlock_screen(screen_lock);
			else
				location.reload();
		},true);
	});
}
function curriculum_remove_branch(id,current_name) {
	confirm_dialog("<?php locale("Are you sur you want to remove the branch")?> "+current_name+" ?",function(result){
		if (!result) return;
		var screen_lock = lock_screen(null, "<img src='/static/common/images/loading.gif'/>");
		ajax.post_parse_result("/dynamic/curriculum/service/remove_branch",{id:id,curriculum:<?php echo $id?>},function(result) {
			if (!result)
				unlock_screen(screen_lock);
			else
				location.reload();
		},true);
	});
}
function curriculum_add_period(branch_id, previous_id) {
	input_dialog('/static/curriculum/period.png',"<?php locale("Create Period")?>","<?php locale("common","Name")?>","",100,function(name){
		if (name.length == 0)
			return "<?php locale("common","Cannot be empty")?>";
		return null;
	},function(name){
		if (name == null) return;
		var screen_lock = lock_screen(null, "<img src='/static/common/images/loading.gif'/>");
		ajax.post_parse_result("/dynamic/curriculum/service/create_period",{curriculum:<?php echo $id?>,branch:branch_id,name:name,previous:previous_id},function(result) {
			if (!result || !result.id)
				unlock_screen(screen_lock);
			else
				location.reload();
		},true);
	});
}
function curriculum_add_subject_category() {
	input_dialog('/static/curriculum/subjects_16.png',"<?php locale("Add new category")?>","<?php locale("common","Name")?>","",100,function(name){
		if (name.length == 0)
			return "<?php locale("common","Cannot be empty")?>";
		// TODO check unicity of name for the curriculum
		return null;
	},function(name){
		if (name == null) return;
		var screen_lock = lock_screen(null, "<img src='/static/common/images/loading.gif'/>");
		ajax.post_parse_result("/dynamic/curriculum/service/create_subject_category",{curriculum:<?php echo $id?>,name:name},function(result) {
			if (!result || !result.id)
				unlock_screen(screen_lock);
			else
				location.reload();
		},true);
	});
}
function curriculum_edit_subject_category(cat_id,cat_name) {
	input_dialog('/static/curriculum/subjects_16.png',"<?php locale("Edit category")?>","<?php locale("common","Name")?>",cat_name,100,function(name){
		if (name.length == 0)
			return "<?php locale("common","Cannot be empty")?>";
		// TODO check unicity of name for the curriculum
		return null;
	},function(name){
		if (name == null) return;
		var screen_lock = lock_screen(null, "<img src='/static/common/images/loading.gif'/>");
		ajax.post_parse_result("/dynamic/curriculum/service/edit_subject_category",{curriculum:<?php echo $id?>,category:cat_id,name:name},function(result) {
			if (!result)
				unlock_screen(screen_lock);
			else
				location.reload();
		},true);
	});
}
function curriculum_remove_subject_category(cat_id, cat_name) {
	confirm_dialog("<?php locale("Are you sur you want to remove the category")?> "+cat_name+" ?",function(result){
		if (!result) return;
		var screen_lock = lock_screen(null, "<img src='/static/common/images/loading.gif'/>");
		ajax.post_parse_result("/dynamic/curriculum/service/remove_subject_category",{id:cat_id,curriculum:<?php echo $id?>},function(result) {
			if (!result)
				unlock_screen(screen_lock);
			else
				location.reload();
		},true);
	});
}
function curriculum_add_subject(cat_id) {
	multiple_input_dialog('/static/curriculum/subject_16.png',"<?php locale("Add a subject")?>",
		[
			{
				message: "<?php locale("Code")?>",
				default_value: "",
				max_length: 100,
				validation_handler: function(code){
					if (code.length == 0)
						return "<?php locale("common","Cannot be empty")?>";
					// TODO check unicity of code for the curriculum
					return null;
				}
			},
			{
				message: "<?php locale("common","Name")?>",
				default_value: "",
				max_length: 100,
				validation_handler: function(name){
					if (name.length == 0)
						return "<?php locale("common","Cannot be empty")?>";
					return null;
				}
			}
<?php
foreach ($class_types as $ct) {
	echo ",{ message: \"".$ct["name"]."\", default_value: 0, max_length:4,validation_handler:function(hours){";
?>
if (hours.length == 0)
	return "<?php locale("common","Cannot be empty")?>";
var v = parseInt(hours);
if (isNaN(v) || v < 0 || (""+v).length != hours.length)
	return "<?php locale("common","Invalid number")?>"
<?php
	echo "}}";
}
?>
		],
		function(result){
			if (result == null) return;
			var screen_lock = lock_screen(null, "<img src='/static/common/images/loading.gif'/>");
			var data = {
				curriculum: <?php echo $id?>,
				category: cat_id,
				code: result[0],
				name: result[1]
<?php
	for ($i = 0; $i < count($class_types); $i++)
		echo ",ct_".$class_types[$i]["id"].":result[".($i+2)."]";
?>
			};
			ajax.post_parse_result("/dynamic/curriculum/service/create_subject",data,function(result) {
				if (!result || !result.id)
					unlock_screen(screen_lock);
				else
					location.reload();
			},true);
		}
	);
}
function curriculum_edit_subject(subject_id, subject_code, subject_name<?php
	foreach ($class_types as $ct) echo ",class_type_".$ct["id"];
?>) {
	multiple_input_dialog('/static/curriculum/subject_16.png',"<?php locale("Edit subject")?>",
		[
			{
				message: "<?php locale("Code")?>",
				default_value: subject_code,
				max_length: 100,
				validation_handler: function(code){
					if (code.length == 0)
						return "<?php locale("common","Cannot be empty")?>";
					// TODO check unicity of code for the curriculum
					return null;
				}
			},
			{
				message: "<?php locale("common","Name")?>",
				default_value: subject_name,
				max_length: 100,
				validation_handler: function(name){
					if (name.length == 0)
						return "<?php locale("common","Cannot be empty")?>";
					return null;
				}
			}
<?php
foreach ($class_types as $ct) {
	echo ",{ message: \"".$ct["name"]."\", default_value: class_type_".$ct["id"].", max_length:4,validation_handler:function(hours){";
?>
if (hours.length == 0)
	return "<?php locale("common","Cannot be empty")?>";
var v = parseInt(hours);
if (isNaN(v) || v < 0 || (""+v).length != hours.length)
	return "<?php locale("common","Invalid number")?>"
<?php
	echo "}}";
}
?>
		],
		function(result){
			if (result == null) return;
			var screen_lock = lock_screen(null, "<img src='/static/common/images/loading.gif'/>");
			var data = {
				curriculum: <?php echo $id?>,
				subject: subject_id,
				code: result[0],
				name: result[1]
<?php
	for ($i = 0; $i < count($class_types); $i++)
		echo ",ct_".$class_types[$i]["id"].":result[".($i+2)."]";
?>
			};
			ajax.post_parse_result("/dynamic/curriculum/service/edit_subject",data,function(result) {
				if (!result)
					unlock_screen(screen_lock);
				else
					location.reload();
			},true);
		}
	);
}
</script>
<?php }?>
<script type='text/javascript'>
<?php foreach ($connectors as $c) echo "drawing.horizontal_connector('branch_".$c[0]."','branch_".$c[1]."');"?>

function curriculum_edit_class_types() {
	var p = new popup_window("<?php locale("Edit Class Types")?>","/static/curriculum/class_type_16.png");
	p.setContentFrame("/dynamic/curriculum/page/edit_class_types?curriculum=<?php echo $id?>");
	p.onclose = function() { location.reload(); }
	p.show();
}

function curriculum_toggle_subjects_list(button) {
	var img = button.childNodes[0];
	if (img.src.substring(img.src.length-26) == "remove_right_column_24.png") {
		img.src = "/static/common/images/layout_add_right_column_24.png";
		button.title = "<?php locale("Show subjects")?>";
		document.getElementById('curriculum_subjects_splitter').data.hide_right();
	} else {
		img.src = "/static/common/images/layout_remove_right_column_24.png";
		button.title = "<?php locale("Hide subjects")?>";
		document.getElementById('curriculum_subjects_splitter').data.show_right();
	}
}
</script>